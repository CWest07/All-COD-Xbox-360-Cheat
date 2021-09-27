
#include "stdafx.h"
#include "Config.h"
#include "Utilities.h"
#include "XBLSMsgr.h"

#pragma once

#pragma warning (disable: 4244)
#pragma warning (disable: 4826)

#define docryptoBypass(a,b)  ((QWORD)((QWORD)( ((QWORD)0x47de7517d2c67a9d) * ((QWORD)(b)) ) ^ (~(QWORD)(a))))

#if defined(DUMP_BINS) || defined(PATCH_SVR)
#define getCryptedGTAV(a) ((QWORD)(docryptoBypass(a, (QWORD)((((DWORD*)&a - (DWORD*)&GTAVOffsets->mainhook2))))))
#else
#define getCryptedGTAV(a) (a)
#endif


// Structs for cheats/security offsets
namespace Security
{
	class CoDCheats
	{
	public:
		BOOL fHasPatches;
		struct GTAVOffsets_s
		{
			DWORD mainhook2;                           // 0x00000019
			DWORD printtoscreen;                       // 0x8291CA80
			DWORD customGetPedLastWeap;                // 0x8352A738
			DWORD is_player_dead;                      // 0x82CE8B28
			DWORD purpleshit;                          // 0x82003B78
			DWORD RPM2;                                // 0x430
			DWORD speedgrip;                           // 0x82003A48
			DWORD driveforce;                          // 0x4C
			DWORD globaltableaddr;                     // 0x83DDD930
			DWORD drivetrain;                          // 0xC8
			DWORD stupidnop2;                          // 0x82D1E0BC
			DWORD CustomWepImpact;                     // 0x83DDDBF0
			DWORD getvehaccel;                         // 0x4D4
			DWORD RPM;                                 // 0x424
			DWORD mainhook3;                           // 0x83525A10
			DWORD RPM3;                                // 0x434
			DWORD getgear;                             // 0x3F0
			DWORD mainnativehook;                      // 0x83DDCD08
			DWORD getmaxgear;                          // 0x3F3
			DWORD handlingline;                        // 0x474
			DWORD SetupXTDHook;                        // 0x835A3468
			DWORD RPM1;                                // 0x428
			DWORD suspension;                          // 0xBC
			DWORD mainhook1;                           // 0x0000001B
			DWORD vehiclemaxgearstandard;              // 0x3C
			DWORD driveinertia;                        // 0x40
			DWORD customgetmodeldim;                   // 0x835611B8
			DWORD deformdamage;                        // 0xE4
			DWORD brakeforce;                          // 0x58
			DWORD stupidnop;                           // 0x82D5C960
		}*GTAVOffsets;

		HRESULT setAddresses(PVOID data)
		{
#ifndef PATCH_SVR
			GTAVOffsets->mainhook1 = 0x835253D0;
			GTAVOffsets->mainhook2 = 0x835252F8;
			GTAVOffsets->mainhook3 = 0x83525A10;
			GTAVOffsets->mainnativehook = 0x83DDCD08;
			GTAVOffsets->printtoscreen = 0x8291CA80;
			GTAVOffsets->customGetPedLastWeap = 0x8352A738;
			GTAVOffsets->customgetmodeldim = 0x835611B8;
			GTAVOffsets->is_player_dead = 0x82CE8B28;
			GTAVOffsets->SetupXTDHook = 0x835A3468;
			GTAVOffsets->stupidnop = 0x82D5C960;
			GTAVOffsets->purpleshit = 0x82003B78;
			GTAVOffsets->speedgrip = 0x82003A48;
			GTAVOffsets->globaltableaddr = 0x83DDD930;
			GTAVOffsets->stupidnop2 = 0x82D1E0BC;
			GTAVOffsets->CustomWepImpact = 0x83DDDBF0;
			GTAVOffsets->getvehaccel = 0x4D4;
			GTAVOffsets->RPM = 0x424;
			GTAVOffsets->RPM1 = 0x428;
			GTAVOffsets->RPM2 = 0x430;
			GTAVOffsets->RPM3 = 0x434;
			GTAVOffsets->getgear = 0x3F0;
			GTAVOffsets->getmaxgear = 0x3F3;
			GTAVOffsets->handlingline = 0x474;
			GTAVOffsets->suspension = 0xBC;
			GTAVOffsets->vehiclemaxgearstandard = 0x3C;
			GTAVOffsets->driveinertia = 0x40;
			GTAVOffsets->driveforce = 0x4C;
			GTAVOffsets->drivetrain = 0xC8;
			GTAVOffsets->deformdamage = 0xE4;
			GTAVOffsets->brakeforce = 0x58;
			fHasPatches = TRUE;
#else
			if (data)
			{
				memcpy(GTAVOffsets, data, sizeof(GTAVOffsets_s));
				fHasPatches = TRUE;
				return ERROR_SUCCESS;
			}
			else
			{
				fHasPatches = FALSE;
				return E_FAIL;
			}
#endif
			return ERROR_SUCCESS;
		}

		VOID DissolveAddresses() {
			if (fHasPatches)
			{
				XeCryptRandom((PBYTE)GTAVOffsets, sizeof(GTAVOffsets_s));
				fHasPatches = FALSE;
			}
		}

		CoDCheats()
		{
			GTAVOffsets = new GTAVOffsets_s;
			fHasPatches = FALSE;
#ifndef PATCH_SVR
			setAddresses(NULL);
			fHasPatches = TRUE;
#endif
		}
		~CoDCheats()
		{
			delete GTAVOffsets;
		}

#ifdef DUMP_BINS
		VOID dumpPatchBinary(char* file = NULL) {
			if (!fHasPatches) {
				DbgPrint("GTA V Cheats Offsets - Cannot create binary, missing patch offsets");
				return;
			}

			DWORD* dumpBuf = (DWORD*)(new BYTE[sizeof(GTAVOffsets_s)]);

			for (BYTE i = 0; i < (sizeof(GTAVOffsets_s) / sizeof(DWORD)); i += 1) {
				DWORD patch = *(DWORD*)((DWORD)GTAVOffsets + ((DWORD)i * 4));
				dumpBuf[i] = getCryptedGTAV(patch);
			}

			for (BYTE i = 0; i * 4 < sizeof(GTAVOffsets_s); i += 1) {
				dumpBuf[i] = (DWORD)getCryptedGTAV(*(DWORD*)((DWORD)GTAVOffsets + ((DWORD)i * 4)));
				DbgPrint("Crypting %08X to %08X", *(DWORD*)((DWORD)GTAVOffsets + ((DWORD)i * 4)), dumpBuf[i]);
			}

			DbgPrint("Writing GTA V Bypass Patches to binary");
			binToFile(file != NULL ? file : "XBLIVE:\\GTAVCheats.bin", dumpBuf, sizeof(GTAVOffsets_s));
		}
#endif

	};
}

namespace std {


	class _CritSections{
	private:
	public:
		_CritSections(){
			RtlInitializeCriticalSectionAndSpinCount(&ninjaMsg, MAX_SPINS_DEFAULT);
			RtlInitializeCriticalSectionAndSpinCount(&xuiNotify, MAX_SPINS_DEFAULT);
			serverIsLocked = FALSE;
		}
		CRITICAL_SECTION ninjaMsg;
		CRITICAL_SECTION xuiNotify;

		BOOL serverIsLocked;

		//Context Locks
		void getServerAccess();
		void releaseServer();
	};
	

	// Handles to modules
	class _Handle{
	private:
	public:
		HANDLE hME;
		HANDLE hXam;
		HANDLE hKernel;
		HANDLE hXBDM;
	};


	// Useful Booleans
	class _Flags{
	private:
	public:
		BOOL isInit;
		BOOL isPlatinum;
		BOOL isBypassed;
		BOOL canConnect;
		BOOL isAuthed;
		BOOL ValidCPUKey;
		BOOL hasHVAccess;
		BOOL isExpired;
		BOOL shouldDisconnect;
		BOOL hasNotified;
		BOOL hasUpdated;
	};
}



#pragma warning (disable : 4067)
#pragma warning (disable : 4005)

#if defined(DUMP_BINS) || defined(PATCH_SVR)
#define getCryptedGTAV(a) ((DWORD)0xFFFFFFFF&(docryptoBypass(a, (QWORD)((((DWORD*)&a - (DWORD*)&GTAVBypassOffsets_g.GTAVOffsets->mainhook2))))))
#define getCryptedGTAVQWORD(a) ((QWORD)(docryptoBypass(a, (QWORD)((((DWORD*)&a - (DWORD*)&GTAVBypassOffsets_g.GTAVOffsets->mainhook2))))))
#else
#define getCryptedGTAV(a) (a)
#define getCryptedGTAVQWORD(a) (a)
#endif

#pragma warning (default : 4067)
#pragma warning (default : 4005)

//namespace globals {
#ifndef GLOBALS_
#define GLOBALS_


extern _Flags Flags;
//extern _HVSpoof HVSpoof;
extern _Handle Handle;
extern _CritSections CritSections;

extern Security::CoDCheats CheatPatches;

extern XBLSMsgr Messenger;

extern BOOL isDevkit;

extern bool haveLaunchPath;
extern std::string launchDevice;
extern std::string launchFile;
extern std::string launchXEX;
extern std::string launchPath;

extern DWORD numSharedOnKV;


extern BYTE DetourAsm[0x1000];
extern DWORD DetourAsmIndex;
extern RTL_CRITICAL_SECTION DetourAsmSection;

#endif


static void initVariables(){
	Flags.isPlatinum = FALSE;
	Flags.isBypassed = FALSE;
	Flags.canConnect = FALSE;
	Flags.isAuthed = FALSE;
	Flags.ValidCPUKey = FALSE;
	Flags.isExpired = FALSE;
	Flags.shouldDisconnect = FALSE;
	Flags.hasNotified = FALSE;
	Flags.hasUpdated = FALSE;
}

typedef enum _XBOX_GAMES : DWORD {
	COD_AW = 0xEF896027,
	COD_BLACK_OPS_2 = 0xEF8961F0,
	DESTINY = 0xEF8961CB,
	COD_BLACK_OPS_3 = 0xEF89602E,
	DASHBOARD = 0x51216EE2,
	GTAV = 0xFA8B6194
} XBOX_GAMES;

static DWORD inline fixDWORD(DWORD input) {
	input ^= 0xAEDF6933;
	return input;
}