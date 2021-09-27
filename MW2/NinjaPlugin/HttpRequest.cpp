#include "HttpRequest.h"


VOID HttpRequest::ResetRequest()
{
	isRequestBusy = FALSE;
	hSession = NULL;
	hConnect = NULL;
	hRequest = NULL;
	dwBytesRead = 0;
	dwContentLength = 0;
	dwBufferLength = 4;
	hasContentLengthHeader = FALSE;
	dwState = HTTP_STATE_IDLE;
	dwHttpStatusCode = 0;
	receiveBuffer = NULL;
}

HttpRequest::HttpRequest()
{
	ResetRequest();
}

void HttpRequest::MoveToState(DWORD state)
{
	//printf("Moving to state 0x%X\n", state);
	dwState = state;
}

VOID CALLBACK HttpRequest::StatusCallback(HINTERNET hInternet,
	DWORD_PTR dwpContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD dwStatusInformationSize)
{
	HttpRequest *request = (HttpRequest*)dwpContext;

	//printf("XHttp callback dwInternetStatus: 0x%X\n", dwInternetStatus);
	switch (dwInternetStatus)
	{
	case XHTTP_CALLBACK_STATUS_HANDLE_CLOSING: //This handle value has been terminated.The lpvStatusInformation parameter contains a pointer to the HINTERNET handle.There will be no more callbacks for this handle.
	case XHTTP_CALLBACK_STATUS_REDIRECT: //An HTTP request is about to automatically redirect the request.The lpvStatusInformation parameter indicates the new URL.At this point, the application can read any data returned by the server with the redirect response and can query the response headers.It can also cancel the operation by closing the handle.
	case XHTTP_CALLBACK_STATUS_INTERMEDIATE_RESPONSE: //Received an intermediate(100 level) status code message from the server.lpvStatusInformation indicates the status code.
	case XHTTP_CALLBACK_STATUS_WRITE_COMPLETE: //Data was successfully written to the server.The lpvStatusInformation parameter is a pointer to a DWORD that indicates the number of bytes written.
	case XHTTP_CALLBACK_STATUS_REQUEST_ERROR: //An error occurred while sending an HTTP request.The lpvStatusInformation parameter contains a pointer to a XHTTP_ASYNC_RESULT structure, of which the dwResult member indicates the return value of the function and any error codes that apply.
		request->MoveToState(HTTP_STATE_ERROR);
		break;
	case XHTTP_CALLBACK_STATUS_READ_COMPLETE: //Data was successfully read from the server.The lpvStatusInformation parameter contains a pointer to the buffer specified in the call to XHttpReadData.The dwStatusInformationLength parameter contains the number of bytes read.
		request->dwBytesRead = dwStatusInformationSize;
		request->MoveToState(HTTP_STATE_DATA_RECEIEVD);
		break;
	case XHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE: //The response header has been received and is available with XHttpQueryHeaders.The lpvStatusInformation parameter is NULL.
		request->MoveToState(HTTP_STATE_RESPONSE_RECEIVED);
		break;
	case XHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE: 
		request->MoveToState(HTTP_STATE_RECEIVE_RESPONSE);
		break;
	}
}

DWORD HttpRequest::GetHTTPStatusCode()
{
	return dwHttpStatusCode;
}

HRESULT HttpRequest::GetResponseStatusCode(
	HINTERNET hRequest,
	DWORD * pdwStatusCode
)
{
	HRESULT hr = S_OK;
	DWORD dwBufferLength = sizeof(DWORD);
	hr = GetResponseHeader(hRequest,
		(XHTTP_QUERY_STATUS_CODE | XHTTP_QUERY_FLAG_NUMBER),
		XHTTP_HEADER_NAME_BY_INDEX,
		pdwStatusCode,
		&dwBufferLength);
	return hr;
}

HRESULT HttpRequest::GetResponseETagHeader(
	HINTERNET hRequest
)
{
	HRESULT hr = S_OK;
	DWORD dwBufferLength = MAX_HEADER_LENGTH;
	PCHAR ETag = new CHAR[MAX_HEADER_LENGTH];
	hr = GetResponseHeader(hRequest,
		XHTTP_QUERY_CUSTOM,
		"ETag",
		ETag,
		&dwBufferLength);
	delete[] ETag;
	return hr;
}

HRESULT HttpRequest::GetResponseContentLength(
	HINTERNET hRequest,
	DWORD * pdwContentLength
)
{
	HRESULT hr = S_OK;
	DWORD dwBufferLength = sizeof(DWORD);
	hr = GetResponseHeader(hRequest,
		(XHTTP_QUERY_CONTENT_LENGTH | XHTTP_QUERY_FLAG_NUMBER),
		XHTTP_HEADER_NAME_BY_INDEX,
		pdwContentLength,
		&dwBufferLength);
	return hr;
}


HRESULT HttpRequest::GetResponseHeader(
	HINTERNET hRequest,
	DWORD dwInfoLevel,
	CONST CHAR * pszHeader,
	VOID * pvBuffer,
	DWORD * pdwBufferLength
)
{
	HRESULT hr = S_OK;
	if (!XHttpQueryHeaders(hRequest,
		dwInfoLevel,
		pszHeader,
		pvBuffer,
		pdwBufferLength,
		XHTTP_NO_HEADER_INDEX))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
	}
	return hr;
}

string FAIL_STR = "";
string HttpRequest::SendRequest(CHAR *ServerName, string Path, BOOL https, BOOL *success, DWORD port, DWORD dataLength)
{
	if (!isRequestBusy)
	{
		if (dataLength > RECEIVE_BUFFER_SIZE)
		{
			DbgPrint("Error: Receive buffer size must not be greater than 0x%X bytes!\n", RECEIVE_BUFFER_SIZE);
			*success = FALSE;
			return FAIL_STR;
		}
		XHttpStartup(0, NULL);
		ResetRequest();
		isRequestBusy = TRUE;
		BOOL ret = FALSE;
		hSession = XHttpOpen("NiNJA Engine", XHTTP_ACCESS_TYPE_DEFAULT_PROXY, XHTTP_NO_PROXY_NAME, XHTTP_NO_PROXY_BYPASS, XHTTP_FLAG_ASYNC);
		if (hSession)
		{
			//printf("XHttpOpen successful\n");
			DWORD portToUse = port == -1 ? INTERNET_DEFAULT_PORT : port;
			hConnect = XHttpConnect(hSession, ServerName, https ? INTERNET_DEFAULT_HTTPS_PORT : portToUse, https ? XHTTP_FLAG_SECURE : 0);
		}
		if (hConnect)
		{
			//printf("XHttpConnect successful\n");
			hRequest = XHttpOpenRequest(hConnect, "GET", Path.c_str(), NULL, XHTTP_NO_REFERRER, NULL, 0);
		}
		if (hRequest)
		{
			//printf("XHttpOpenRequest successful\n");
			XHTTP_STATUS_CALLBACK callback = XHttpSetStatusCallback(hRequest, StatusCallback, XHTTP_CALLBACK_FLAG_ALL_COMPLETIONS, NULL);
			ret = XHttpSendRequest(hRequest, XHTTP_NO_ADDITIONAL_HEADERS, NULL, XHTTP_NO_REQUEST_DATA, NULL, NULL, (DWORD_PTR)this);
		}
		if (ret)
		{
			//printf("XHttpSendRequest successful\n");
			MoveToState(HTTP_STATE_REQUEST_SENT);
			HRESULT hr;
			while (true)
			{
				switch (dwState)
				{
				case HTTP_STATE_REQUEST_SENT:
					//waiting for callback, do nothing
					break;
				case HTTP_STATE_RECEIVE_RESPONSE:
					ret = XHttpReceiveResponse(hRequest, NULL);
					if (!ret)
						MoveToState(HTTP_STATE_ERROR);
					else
						MoveToState(HTTP_STATE_WAITING_FOR_RESPONSE);
					break;
				case HTTP_STATE_WAITING_FOR_RESPONSE:
					//waiting for callback, do nothing
					break;
				case HTTP_STATE_RESPONSE_RECEIVED:
					/*hr = GetResponseETagHeader(hRequest);
					if (hr != HRESULT_FROM_WIN32(ERROR_XHTTP_HEADER_NOT_FOUND) && FAILED(hr))
					{
						printf("HeaderNotFound, moving to state Error Encountered\n");
						MoveToState(HTTP_STATE_ERROR);
						break;
					}*/
					hr = GetResponseStatusCode(hRequest, &dwHttpStatusCode);
					if (FAILED(hr))
					{
						//printf("GetResponseStatusCode failed, moving to state Error Encountered\n");
						MoveToState(HTTP_STATE_ERROR);
						break;
					}
					else if (dwHttpStatusCode >= HTTP_STATUS_BAD_REQUEST)
					{
						//printf("StatusCodeBadRequest, moving to state Error Encountered\n");
						MoveToState(HTTP_STATE_ERROR);
						break;
					}
					else // NOTE: if we get a 403/404 we do still have data that maybe helpful
					{
						if (XHttpQueryHeaders(hRequest, XHTTP_QUERY_CONTENT_LENGTH | XHTTP_QUERY_FLAG_NUMBER, //query content length
							XHTTP_HEADER_NAME_BY_INDEX, &dwContentLength, &dwBufferLength, XHTTP_NO_HEADER_INDEX))
						{
							if (dwContentLength > 0)
							{
								hasContentLengthHeader = TRUE;
								MoveToState(HTTP_STATE_WAITING_FOR_DATA);
							}
							else
							{
								//printf("Content length was less than or equal to 0, moving to state Response Received.\n");
								MoveToState(HTTP_STATE_DATA_RECEIEVD);
								break;
							}
						}//
						else if (GetLastError() == ERROR_XHTTP_HEADER_NOT_FOUND)
						{
							//printf("Exception HttpHeaderNotFound encountered, using default buffer length.\n");
							dwContentLength = dataLength;
							hasContentLengthHeader = FALSE;
						}
						else
						{
							//printf("GetResponseContentLength failed, moving to state Error Encountered\n");
							MoveToState(HTTP_STATE_ERROR);
							break;
						}
					}

					if (dwContentLength > RECEIVE_BUFFER_SIZE)
					{
						DbgPrint("Error: Content length must not be greater than 0x%X bytes!\n", RECEIVE_BUFFER_SIZE);
						*success = FALSE;
						return FAIL_STR;
					}

					mManager->alloc(dwContentLength, &receiveBuffer);
					//receiveBuffer = new BYTE[dwContentLength]; //allocate memory for our read operation
					if (receiveBuffer == NULL) //check if we allocated memory successfully
					{
						MoveToState(HTTP_STATE_ERROR);
						break;
					}
					ret = XHttpReadData(hRequest, receiveBuffer, dwContentLength, NULL);

					if ((dwBytesRead > 0 && !hasContentLengthHeader) || (dwBytesRead == dwContentLength && hasContentLengthHeader))
						MoveToState(HTTP_STATE_DATA_RECEIEVD); //we received the response instantly
					else
						MoveToState(HTTP_STATE_WAITING_FOR_DATA);
					break;
				case HTTP_STATE_WAITING_FOR_DATA:
					//waiting for callback
					if ((dwBytesRead > 0 && !hasContentLengthHeader) || (dwBytesRead == dwContentLength && hasContentLengthHeader))
						MoveToState(HTTP_STATE_DATA_RECEIEVD);
					break;
				case HTTP_STATE_DATA_RECEIEVD:
					MoveToState(HTTP_STATE_COMPLETED);
					break;
				case HTTP_STATE_COMPLETED:
				case HTTP_STATE_ERROR:
					goto exitLoop;
				}
				if (!XHttpDoWork(hSession, 0))
					MoveToState(HTTP_STATE_ERROR);
				Sleep(33);
			}
		}
	exitLoop:
		if (dwState == HTTP_STATE_COMPLETED && receiveBuffer != NULL)
		{
			DbgPrint("Http request succeeded with status code %i", dwHttpStatusCode);//
			receiveBuffer[dwBytesRead] = 0;
		}
		else
		{
			DbgPrint("Http request failed with status code %i", dwHttpStatusCode);
		}

		if (hRequest) XHttpCloseHandle(hRequest);
		if (hConnect) XHttpCloseHandle(hConnect);
		if (hSession) XHttpCloseHandle(hSession);
		
		XHttpShutdown();

		isRequestBusy = FALSE;

		if (dwState == HTTP_STATE_COMPLETED && receiveBuffer != NULL)
		{
			string ret((PCHAR)receiveBuffer, dwContentLength); //package up our returned bytes as a string
			DbgPrint("Ret: %s", ret.c_str());
			*success = TRUE;
			mManager->free(receiveBuffer);
			return ret;
		}
	}
	*success = FALSE;
	return FAIL_STR;
}