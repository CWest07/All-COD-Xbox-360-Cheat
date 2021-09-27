#include "GeoLocationParser.h"

namespace GeoIpParser
{
	//Example return from geo-ip api: {"as":"AS7922 Comcast Cable Communications, LLC", "city" : "Lisle", "country" : "United States", "countryCode" : "US", "isp" : "Comcast Cable", "lat" : 41.787, "lon" : -88.0833, 
	//"org" : "Comcast Cable", "query" : "13.37.13.37", "region" : "IL", "regionName" : "Illinois", "status" : "success", "timezone" : "America/Chicago", "zip" : "60532"}
	char IP[0x100];
	
	void GeoIpParser::ParseIntenal(string ip, GeoIpInfo *clientInfo)
	{
		sprintf(IP, "/json/%s", ip.c_str());
		BOOL httpSuccess = FALSE;
		HttpRequest req;
		string ipStr(IP);
		//printf("Attempting web request to ip-api.com%s\n", ipStr.c_str());
		string ret = req.SendRequest("ip-api.com", ipStr, FALSE, &httpSuccess);
		if (httpSuccess)
		{
			this->Parse(ret.c_str());
			isValid = !HasParseError();

			if (!isValid || this->IsNull())
			{
				DbgPrint("ERROR PARSING JSON!");
				return;
			}

			const rapidjson::Value& membersObject = this[0];
			if (membersObject.IsNull())
			{
				DbgPrint("Empty");
				return;
			}

			if (membersObject["status"].IsString())
			{
				if (Compare((CHAR*)membersObject["status"].GetString(), "success"))
				{
					if (membersObject["city"].IsString())
						memcpy(clientInfo->city, membersObject["city"].GetString(), sizeof(clientInfo->city));

					if (membersObject["region"].IsString())
						memcpy(clientInfo->state, membersObject["region"].GetString(), sizeof(clientInfo->state));

					if (membersObject["regionName"].IsString())
						memcpy(clientInfo->stateFull, membersObject["regionName"].GetString(), sizeof(clientInfo->stateFull));

					if (membersObject["country"].IsString())
						memcpy(clientInfo->country, membersObject["country"].GetString(), sizeof(clientInfo->country));

					if (membersObject["isp"].IsString())
						memcpy(clientInfo->isp, membersObject["isp"].GetString(), sizeof(clientInfo->isp));

					if (membersObject["zip"].IsString())
						memcpy(clientInfo->zip, membersObject["zip"].GetString(), sizeof(clientInfo->zip));

					success = true;
					return;
				}
			}

		}
		success = false;
	}

	GeoIpParser::GeoIpParser(string ip, GeoIpInfo *clientInfo)
	{
		ParseIntenal(ip, clientInfo);
	}

	GeoIpParser::GeoIpParser(IN_ADDR ip, GeoIpInfo *clientInfo)
	{
		sprintf(IP, "%d.%d.%d.%d", ip.S_un.S_un_b.s_b1, ip.S_un.S_un_b.s_b2, ip.S_un.S_un_b.s_b3, ip.S_un.S_un_b.s_b4);
		ParseIntenal(IP, clientInfo);
	}

	GeoIpParser::GeoIpParser(DWORD ip, GeoIpInfo *clientInfo)
	{
		IN_ADDR ipT;
		ipT.S_un.S_addr = ip;
		sprintf(IP, "%d.%d.%d.%d", ipT.S_un.S_un_b.s_b1, ipT.S_un.S_un_b.s_b2, ipT.S_un.S_un_b.s_b3, ipT.S_un.S_un_b.s_b4);
		ParseIntenal(IP, clientInfo);
	}
};