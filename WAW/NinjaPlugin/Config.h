#pragma once
#include "stdafx.h"
#include "PatchData.h"

extern BOOL CleanMode;
extern BOOL isDevkit;
extern BOOL MountInitialized;

// NOTE: Testing server port can be enabled by uncommenting this 
//#define TEST_MODULE

// NOTE: Enables debug print output without needing a debug build
//#define DBG_PRINT

// NOTE: Dumps encrypted patch offset binaries for server
//#define DUMP_BINS

// NOTE: Enables server mode for patches
//#define PATCH_SVR

// NOTE: Uncomment to enable anti debugging
#define ANTIHACK

// NOTE: Uncomment to enable encrypted strings
//#define CRYPTED_STRINGS

//prevent accidents
#ifdef PATCH_SVR
#undef DBG_PRINT
#undef DUMP_BINS
#endif

#define XSTL_SERVER_VER 104

// NOTE: Uncomment to open BO3 menu pre-game
//#define PREGAME_MENU

// Module names
#define NAME_KERNEL				    "xboxkrnl.exe"
#define NAME_XAM				    "xam.xex"
//Must match name in properties
#define NAME_XBLS				    "module.xex" 
#define NAME_XBDM                   "xbdm.xex"
#define NAME_LAUNCH                 "launch.xex"
#define NAME_XOSC                   "xosc.xex"

// File names
#define NAME_KXAM_PATCHES		    "\\kxam.patch"
#define NAME_LOG					"\\Engine.log"
#define NAME_INI					"\\Engine.ini"

// Devices, and device paths
#define XBLS_DEVICE_NAME_HDD		"\\Device\\Harddisk0\\Partition1"
#define XBLS_DRIVE_HDD			    "HDD:\\"


// File paths
#define LAUNCH_MOUNT                "AERO:"
#define PATH_KXAM_PATCHES		    LAUNCH_MOUNT NAME_KXAM_PATCHES
#define PATH_XBLS					LAUNCH_MOUNT NAME_XBLS
#define PATH_XOSC_DUMP_XEX			LAUNCH_MOUNT NAME_XOSC
#define PATH_LOG					LAUNCH_MOUNT NAME_LOG
#define PATH_INI					LAUNCH_MOUNT NAME_INI


// Hardcoded keys
const BYTE DeveloperKey19[0x10]		= { 0xDA, 0xB6, 0x9A, 0xD9, 0x8E, 0x28, 0x76, 0x4F, 0x97, 0x7E, 0xE2, 0x48, 0x7E, 0x4F, 0x3F, 0x68 };
const BYTE RetailKey19[0x10]		= { 0xE1, 0xBC, 0x15, 0x9C, 0x73, 0xB1, 0xEA, 0xE9, 0xAB, 0x31, 0x70, 0xF3, 0xAD, 0x47, 0xEB, 0xF3 };

