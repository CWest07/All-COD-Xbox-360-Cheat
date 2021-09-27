#pragma once
#ifndef GEOPARSER
#define GEOPARSER


#include "stdafx.h"
#include "Globals.h"
#include "HttpRequest.h"

#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"
#include "rapidjson\reader.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"

using namespace rapidjson;

struct GeoIpInfo
{
	char city[40];
	char state[40];
	char stateFull[40];
	char country[40];
	char isp[40];
	char zip[40];
};

namespace GeoIpParser 
{

	class GeoIpParser : public rapidjson::Document {
	protected:
		//internal parsing function
		void ParseIntenal(string ip, GeoIpInfo *clientInfo);

		//json flags
		BOOL isValid;
	public:
		GeoIpParser() {
			isValid = FALSE;
			success = false;
			// make the document an object (root)
			//SetObject();
		}

		//constructors
		GeoIpParser(IN_ADDR ip, GeoIpInfo *clientInfo);
		GeoIpParser(DWORD ip, GeoIpInfo *clientInfo);
		GeoIpParser(string ip, GeoIpInfo *clientInfo);

		//public vars
		bool success;
	};

}

#endif