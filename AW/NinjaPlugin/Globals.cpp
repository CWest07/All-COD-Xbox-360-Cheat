#pragma once
#include "stdafx.h"
#include "Globals.h"

// XBLS Server
WCHAR ServerUpdateNotifyMsg[400];

_Flags Flags;
_Handle Handle;
_CritSections CritSections;

Security::CoDCheats CheatPatches;

BOOL isDevkit = FALSE;
bool haveLaunchPath = false;
std::string launchDevice;
std::string launchFile;
std::string launchXEX;
std::string launchPath;

XBLSMsgr Messenger;

DWORD numSharedOnKV = 0;

