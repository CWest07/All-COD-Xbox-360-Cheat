#include "stdafx.h"
#include "Utilities.h"
#include "launch.h"
#include "Globals.h"

static const char* launchModule = "launch.xex";

typedef enum {
	DL_ORDINALS_LDAT = 1,
	DL_ORDINALS_STARTSYSMOD,
	DL_ORDINALS_SHUTDOWN,
	DL_ORDINALS_FORCEINILOAD,
	DL_ORDINALS_GETNUMOPTS,
	DL_ORDINALS_GETOPTINFO,
	DL_ORDINALS_GETOPTVAL,
	DL_ORDINALS_SETOPTVAL,
	DL_ORDINALS_GETOPTVALBYNAME,
	DL_ORDINALS_SETOPTVALBYNAME,
	DL_ORDINALS_GETDRIVELIST,
	DL_ORDINALS_GETDRIVEINFO,
	DL_ORDINALS_BOOTPARSEBUTTONS,
	DL_ORDINALS_PLUGINPATH,
} DL_ORDINALS;

typedef int(*DLAUNCHGETNUMOPTS)(int* totalOpts);
typedef int(*DLAUNCHGETOPTINFO)(int opt, PDWORD optType, PCHAR outStr, PDWORD currVal, PDWORD defValue, PDWORD optCategory);
typedef BOOL(*DLAUNCHGETOPTVAL)(int opt, PDWORD val);
typedef BOOL(*DLAUNCHSETOPTVAL)(int opt, PDWORD val);
typedef BOOL(*DLAUNCHGETOPTVALBYNAME)(char* optName, PDWORD val);
typedef BOOL(*DLAUNCHSETOPTVALBYNAME)(char* optName, PDWORD val);
typedef VOID(*DLAUNCHFORCEINILOAD)(PCHAR path);
typedef DWORD(*DLAUNCHSTARTSYSMODULE)(char* modPath);
typedef VOID(*DLAUNCHSHUTDOWN)(VOID);
typedef DWORD(*DLAUNCHGETDRIVELIST)(DWORD dev, PCHAR devDest, PCHAR mountName, PCHAR friendlyName);
typedef DWORD(*DLAUNCHGETDRIVEINFO)(PDWORD maxIniDrives, PDWORD maxDevLen);

namespace launch {
	static bool initialized = false;

	static pldata ldat;
	static DLAUNCHSTARTSYSMODULE dlaunchStartSysModule;
	static DLAUNCHSHUTDOWN dlaunchShutdown;
	static DLAUNCHFORCEINILOAD dlaunchForceIniLoad;
	static DLAUNCHGETNUMOPTS dlaunchGetNumOpts;
	static DLAUNCHGETOPTINFO dlaunchGetOptInfo;
	static DLAUNCHGETOPTVAL dlaunchGetOptVal;
	static DLAUNCHSETOPTVAL dlaunchSetOptVal;
	static DLAUNCHGETOPTVALBYNAME dlaunchGetOptValByName;
	static DLAUNCHSETOPTVALBYNAME dlaunchSetOptValByName;
	static DLAUNCHGETDRIVELIST dlaunchGetDriveList;
	static DLAUNCHGETDRIVEINFO dlaunchGetDriveInfo;
	static PPLUGIN_LOAD_PATH dlaunchPluginPath = 0;

	static HRESULT checkInitialization() {
		if (isDevkit) return ERROR_BAD_DEV_TYPE;
		if (!initialized) return ERROR_INVALID_STATE;
		return S_OK;
	}

	static HRESULT initializeExports() {
		if (isDevkit)
			return S_OK;

		if ((ldat = (pldata)resolveFunct((PCHAR)launchModule, DL_ORDINALS_LDAT)) == NULL) {
			DbgPrint("failed to resolve ldat");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchStartSysModule = (DLAUNCHSTARTSYSMODULE)resolveFunct((PCHAR)launchModule, DL_ORDINALS_STARTSYSMOD)) == NULL) {
			DbgPrint("failed to resolve dlaunchStartSysModule");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchShutdown = (DLAUNCHSHUTDOWN)resolveFunct((PCHAR)launchModule, DL_ORDINALS_SHUTDOWN)) == NULL) {
			DbgPrint("failed to resolve dlaunchShutdown");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchForceIniLoad = (DLAUNCHFORCEINILOAD)resolveFunct((PCHAR)launchModule, DL_ORDINALS_FORCEINILOAD)) == NULL) {
			DbgPrint("failed to resolve dlaunchForceIniLoad");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchGetNumOpts = (DLAUNCHGETNUMOPTS)resolveFunct((PCHAR)launchModule, DL_ORDINALS_GETNUMOPTS)) == NULL) {
			DbgPrint("failed to resolve dlaunchGetNumOpts");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchGetOptInfo = (DLAUNCHGETOPTINFO)resolveFunct((PCHAR)launchModule, DL_ORDINALS_GETOPTINFO)) == NULL) {
			DbgPrint("failed to resolve dlaunchGetOptInfo");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchGetOptVal = (DLAUNCHGETOPTVAL)resolveFunct((PCHAR)launchModule, DL_ORDINALS_GETOPTVAL)) == NULL) {
			DbgPrint("failed to resolve dlaunchGetOptVal");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchSetOptVal = (DLAUNCHSETOPTVAL)resolveFunct((PCHAR)launchModule, DL_ORDINALS_SETOPTVAL)) == NULL) {
			DbgPrint("failed to resolve dlaunchSetOptVal");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchGetOptValByName = (DLAUNCHGETOPTVALBYNAME)resolveFunct((PCHAR)launchModule, DL_ORDINALS_GETOPTVALBYNAME)) == NULL) {
			DbgPrint("failed to resolve dlaunchGetOptValByName");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchSetOptValByName = (DLAUNCHSETOPTVALBYNAME)resolveFunct((PCHAR)launchModule, DL_ORDINALS_SETOPTVALBYNAME)) == NULL) {
			DbgPrint("failed to resolve dlaunchSetOptValByName");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchGetDriveList = (DLAUNCHGETDRIVELIST)resolveFunct((PCHAR)launchModule, DL_ORDINALS_GETDRIVELIST)) == NULL) {
			DbgPrint("failed to resolve dlaunchGetDriveList");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchGetDriveInfo = (DLAUNCHGETDRIVEINFO)resolveFunct((PCHAR)launchModule, DL_ORDINALS_GETDRIVEINFO)) == NULL) {
			DbgPrint("failed to resolve dlaunchGetDriveInfo");
			return ERROR_INVALID_ORDINAL;
		}

		if ((dlaunchPluginPath = (PPLUGIN_LOAD_PATH)resolveFunct((PCHAR)launchModule, DL_ORDINALS_PLUGINPATH)) == NULL) {
			DbgPrint("failed to resolve dlaunchPluginPath");
			return ERROR_INVALID_ORDINAL;
		}

		return S_OK;
	}

	static HRESULT initializePluginLoadPath() {
		HRESULT ret;


		if (!isDevkit && dlaunchPluginPath->magic == PLUGIN_LOAD_PATH_MAGIC && dlaunchPluginPath->devicePath != NULL && dlaunchPluginPath->iniPath != NULL) {
			// copy the path information
			launchDevice.assign(dlaunchPluginPath->devicePath);
			launchFile.assign(dlaunchPluginPath->iniPath);
			haveLaunchPath = true;

			std::string link("\\System??\\" LAUNCH_MOUNT);
			std::string directory(launchDevice);
			directory.append(launchFile.substr(1, launchFile.find_last_of('\\')));
			size_t dirLength = (launchFile.substr(1, launchFile.find_last_of('\\')).length());
			launchXEX.assign(launchFile.substr(dirLength, launchFile.find_last_of("xex") + 3));
			DbgPrint("Mounting %s to %s", link.c_str(), directory.c_str());

			if (FAILED(ret = mountPath(directory, link))) {
				DbgPrint("mountPath failed (0x%X)!", ret);
				return ret;
			}
			launchPath.assign(launchDevice + launchFile.substr(1));
		}
		else
		{
			DbgPrint("launch - Trying to mount as Devkit");
			//Handle Devits
			if (isDevkit) {
				std::string link("\\System??\\" LAUNCH_MOUNT);
				std::string directory("\\Device\\Harddisk0\\Partition1\\DEVKIT\\NiNJA");

				if (FAILED(ret = mountPath(directory, link))) {
					DbgPrint("mountPath failed (0x%X)!", ret);
					return ret;
				}

				link = ("\\??\\" LAUNCH_MOUNT);
				directory = ("\\Device\\Harddisk0\\Partition1\\DEVKIT\\NiNJA");

				if (FAILED(mountPath(directory, link))) {
					DbgPrint("mountPath failed (0x%X)!", ret);
					//return ret;
				}
			}
		}

		return S_OK;
	}

	HRESULT initialize() {
		HRESULT ret;
		if (!isDevkit&&initialized) {
			std::string link("\\??\\" LAUNCH_MOUNT);
			std::string directory(launchDevice);
			directory.append(launchFile.substr(1, launchFile.find_last_of('\\')));
			size_t dirLength = (launchFile.substr(1, launchFile.find_last_of('\\')).length());
			launchXEX.assign(launchFile.substr(dirLength, launchFile.find_last_of("xex") + 3));
			DbgPrint("Mounting %s to %s", link.c_str(), directory.c_str());

			if (FAILED(ret = mountPath(directory, link))) {
				DbgPrint("mountPath failed (0x%X)!", ret);
				return ret;
			}
		}
		else if (!isDevkit&&!initialized) {
			if (FAILED(ret = initializeExports())) {
				DbgPrint("initializeExports failed (0x%X)!", ret);
			}

			if (FAILED(ret = initializePluginLoadPath())) {
				DbgPrint("initializePluginLoadPath failed (0x%X)!", ret);
			}
			initialized = true;
		}

		if (isDevkit) {
			if (FAILED(ret = initializePluginLoadPath())) {
				DbgPrint("initializePluginLoadPath failed (0x%X)!", ret);
			}
			initialized = false;
		}

		return ret;
	}

	HRESULT setLiveBlock(bool enabled) {
		HRESULT ret;

		// check initialization
		if ((ret = checkInitialization()) != S_OK) return ret;

		DWORD value = enabled ? 1 : 0;

		// set liveblock
		if (!dlaunchSetOptValByName("liveblock", &value)) {
			DbgPrint("setting liveblock to %i failed!", value);
			return E_FAIL;
		}

		// set stronglive
		if (!dlaunchSetOptValByName("livestrong", &value)) {
			DbgPrint("setting livestrong to %i failed!", value);
			return E_FAIL;
		}

		return S_OK;
	}
}