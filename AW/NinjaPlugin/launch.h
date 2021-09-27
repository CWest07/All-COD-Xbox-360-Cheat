#pragma once
#include "stdafx.h"

namespace launch {
	typedef enum {
		// DWORD containing a value between 0 and 1
		DL_OPT_TYPE_BOOL = 0,
		// WORD containing a value between 0 and 0xFFFF
		DL_OPT_TYPE_WORD,
		DL_OPT_TYPE_WORDREGION, // 0-0x7FFF
		DL_OPT_TYPE_WORDPORT, // 1 - 0xFFFF
		// DWORD containing a value between 0 and 0xFFFFFFFF
		DL_OPT_TYPE_DWORD,
		DL_OPT_TYPE_DWORDTIME, // in seconds
		// anything >= this can't be changed easily as it is a pkey pointer
		DL_OPT_TYPE_MAX_ACCESS,
		// DWORD containing a memory address/pkeydata element
		DL_OPT_TYPE_PATH = DL_OPT_TYPE_MAX_ACCESS, // generic path
		DL_OPT_TYPE_PATHQLB, // quick launch button
		DL_OPT_TYPE_PATHPLUGIN,
	} DL_OPT_TYPES;

	typedef enum {
		OPT_MIN = 0,
		OPT_CAT_PATHS, // various paths including quick launch buttons
		OPT_CAT_BEHAVIOR, // Drawing that change xbox behavior
		OPT_CAT_NETWORK, // Drawing that impact network
		OPT_CAT_TIMERS, // Drawing for things that run on timers
		OPT_CAT_PLUGINS, // plugin paths
		OPT_CAT_EXTERNAL, // options for the configurator
		OPT_CAT_MAX // not a real category, just a placeholder for array stuff
	} DL_OPT_CATEGORIES;

	typedef struct {
		DWORD ID;
		DWORD ltype;
		char link[MAX_PATH];
		char dev[MAX_PATH];
		USHORT versionMaj;
		USHORT versionMin;
		USHORT targetKernel;
		USHORT svnVer;
		DWORD options; // for external apps that want to know what dash launch has set/parsed
		DWORD DebugRoutine; // for external apps that want to recursively hook and call the first/last chance exception trap on their own
		DWORD DebugStepPatch; // address to path single step exception to not be skipped (write 0x60000000/nop to this address to enable it)
		PBYTE tempData; // DL will monitor temps, a copy of the smc temp data is placed here, 0x10 bytes in len
		DWORD iniPathSel; // the path corresponding to this number can be gotten via dlaunchGetDriveList, 0xFF is none, 0xFE is forced
	} ldata, *pldata;

	typedef struct {
		char launchpath[MAX_PATH];
		DWORD flags;
		DWORD dev;
		DWORD rootDev;
	} keydata, *pkeydata;

#define PLUGIN_LOAD_PATH_MAGIC 0x504C5041

	typedef struct {
		DWORD       magic;
		const char* devicePath;
		const char* iniPath;
	} PLUGIN_LOAD_PATH, *PPLUGIN_LOAD_PATH;

	HRESULT initialize();
	HRESULT setLiveBlock(bool enabled);
}
