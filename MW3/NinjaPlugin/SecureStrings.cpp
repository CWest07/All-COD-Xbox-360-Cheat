#include "SecureStrings.h"

vector<TimedString> DecryptedStrings;
MemoryManager *sManager;
char *XOR_Alloc(char *data)
{
	SENSITIVE_FUNCTION_START;
	int data_len = strlen(data);
	PCHAR newMem;
	if (sManager->alloc(data_len + 1, &newMem))
	{
		memcpy(newMem, data, data_len);
		for (int i = 0; i < data_len; i++)
			newMem[i] ^= xorKey[i % 0x10];
		newMem[data_len] = 0x0;
		//TimedString str;
		//str.InitTime = GetTickCount();
		//str.Lifetime = 0;
		//str.Ptr = (DWORD)newMem;
		//DecryptedStrings.push_back(str);
		return newMem;
	}
	return "";
}

vector<TimedString> DecryptedStringsTimed;
char *XOR_Timed(char *data)
{
	SENSITIVE_FUNCTION_START;
	int data_len = strlen(data);
	PCHAR newMem;
	if (sManager->alloc(data_len + 1, &newMem))
	{
		memcpy(newMem, data, data_len);
		for (int i = 0; i < data_len; i++)
			newMem[i] ^= xorKey[i % 0x10];
		newMem[data_len] = '\0';
		TimedString str;
		str.InitTime = GetTickCount();
		str.Lifetime = DECRYPTED_STRING_LIFETIME_MS;
		str.Ptr = (DWORD)newMem;
		DecryptedStringsTimed.push_back(str);
		return newMem;
	}
	return "";
}


char *XOR(char *data)
{
	SENSITIVE_FUNCTION_START;
	CHAR bigBuff[0x1000];
	memset(bigBuff, 0, 0x1000);
	int data_len = strlen(data);
	memcpy(bigBuff, data, data_len);
	for (int i = 0; i<data_len; i++)
		bigBuff[i] ^= xorKey[i % 0x10];
	bigBuff[data_len] = '\0';
	return bigBuff;
}
