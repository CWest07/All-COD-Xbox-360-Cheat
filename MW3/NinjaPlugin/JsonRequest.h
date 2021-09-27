#pragma once
#ifndef JSON_REQUEST_H
#define JSON_REQUEST_H


#include "stdafx.h"
#include "HttpRequest.h"
#include "Utilities.h"
#include <string>
#include <vector>

#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"
#include "rapidjson\reader.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"


using namespace rapidjson;

class JsonRequest : public rapidjson::Document {
protected:
	//json flags
	BOOL isValid;
public:
	JsonRequest() {
		isValid = FALSE;
		success = FALSE;
		// make the document an object (root)
		//SetObject();
	}
	BOOL success;
	const rapidjson::Value& Get(string host, string path, BOOL isArray);
	const rapidjson::Value& Get(char *data, BOOL isArray);
};

#endif