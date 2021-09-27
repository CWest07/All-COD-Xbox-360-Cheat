
#include "stdafx.h"
#include "Config.h"
#include "Utilities.h"
#include "XBLSMsgr.h"
#include "MemoryManager.h"

#pragma once

#pragma warning (disable: 4244)
#pragma warning (disable: 4826)

#define docryptoBypass(a,b)  ((QWORD)((QWORD)( ((QWORD)0x710e7a17d2b42090) * ((QWORD)(b)) ) ^ (~(QWORD)(a))))

#if defined(PATCH_SVR)
#define getCryptedCoD(a) ((DWORD)(docryptoBypass(a, (QWORD)((((DWORD*)&a - (DWORD*)&CheatPatches->CoDPatches.NetInfoAddress))))))
#define getCryptedCoDQWORD(a) ((QWORD)(docryptoBypass(a, (QWORD)((((DWORD*)&a - (DWORD*)&CheatPatches->CoDPatches.NetInfoAddress))))))
#else
#define getCryptedCoD(a) (a)
#define getCryptedCoDQWORD(a) (a)
#endif

struct CoDPatches_s
{
	//structs
	DWORD NetInfoAddress;
	DWORD Autowall360;
	DWORD CartoonMode1, CartoonMode2;
	DWORD CG_Pointer;
	DWORD CGS_Pointer;
	DWORD CG_S_XY;
	DWORD CG_WeaponsArray;
	DWORD CL_InGame_Address;
	DWORD EntitiesPointer;
	DWORD FMJFix1, FMJFix2;
	DWORD FOVStruct;
	DWORD HostNameStruct;
	DWORD LocalClientPointer;
	DWORD NetSpeedOffset;
	DWORD NoFlinchOffset;
	DWORD NoRecoilOffset;
	DWORD NoSwayOffset1, NoSwayOffset2;
	DWORD LevelOffset;
	DWORD OurXuidOffset;
	DWORD OverheadNamesFriendlyOffset, OverheadNamesEnemyOffset;
	DWORD PrestigeOffset;
	DWORD RadarOffset1, RadarOffset2, RadarOffset3;
	DWORD ScreenPlacement;
	DWORD ServerIdOffset;
	DWORD SquadPointOffset;
	DWORD ShowPing;
	DWORD ThirdPersonStruct;
	DWORD WallhackOffset;
	DWORD WeaponDefOffset;
	DWORD UnknownClientStruct;
	DWORD UnlockAllOffset;

	//functions
	DWORD AnglesToAxis;
	DWORD BG_GetSpreadForWeapon;
	DWORD BG_GetWeaponDef;
	DWORD BG_GetWeaponDef2;
	DWORD CalculateRandomSeed;
	DWORD CBuf_AddText;
	DWORD CG_BulletHitEvent;
	DWORD CG_CompassCalcDimensions;
	DWORD CG_CompassDrawPlayerMap;
	DWORD CG_CalcNamePosition;
	DWORD CG_CompassUpYawVector;
	DWORD CG_DObjGetWorldTagPos;
	DWORD CG_DrawGrenadePointer;
	DWORD CG_DrawRotatedPicPhysical;
	DWORD CG_DrawRotatedPicPhysical2;
	DWORD CG_EntityEvent;
	DWORD CG_GetEntityBModelBounds;
	DWORD CG_GetPlayerViewOrigin;
	DWORD CG_GetWeaponIndexForName;
	DWORD CG_LocationalTrace;
	DWORD CG_WorldPosToCompass;
	DWORD CL_AllLocalClientsDisconnected;
	DWORD CL_CreateNewCommands;
	DWORD CL_DrawTextPhysical;
	DWORD CL_GetScreenDimensions;
	DWORD CL_ReadyToSendPacket;
	DWORD CL_SendCmd;
	DWORD CL_SetStace;
	DWORD Com_GetClientDObj;
	DWORD DB_FindXAssetHeader;
	DWORD DrawEngineRadar;
	DWORD DrawEngineRadarNOP;
	DWORD DrawFriendlies;
	DWORD DrawFriendliesNOP;
	DWORD DrawEnemies;
	DWORD DrawEnemiesNOP;
	DWORD DrawRotatedPic;
	DWORD DrawVehicles;
	DWORD DrawVehiclesNOP;
	DWORD DrawPlayer;
	DWORD DrawPlayerNOP;
	DWORD Dvar_GetBool;
	DWORD DynEntCl_DynEntImpactEvent;
	DWORD FireBulletPenetrate;
	DWORD G_GetWeaponHitLocationMultiplier;
	DWORD GetWeaponHudName;
	DWORD GetTagPosition;
	DWORD GetWeaponIcon;
	DWORD IsWeaponDualWield;
	DWORD LiveStats_WriteStat;
	DWORD Menu_PaintAll;
	DWORD PlayLocalSoundAliasByName;
	DWORD RandomBulletDir;
	DWORD R_AddCmdDrawStretchPic;
	DWORD R_EndFrame;
	DWORD R_NormalizedTextScale;
	DWORD R_TextWidth;
	DWORD RB_SetTessTechnique;
	DWORD SL_GetString;
	DWORD TransformSeed;
	DWORD SubFuckYou;
	DWORD SomeRandomFuckingShit;
	DWORD UI_DrawHandlePic;
	DWORD UI_FillRect;
	DWORD UI_FillRectPhysical;
	DWORD XInputGetState;
	DWORD VA;
};

// Structs for cheats/security offsets
namespace Security
{
	class CoDCheats
	{
	public:
		BOOL fHasPatches;
		CoDPatches_s CoDPatches;

		HRESULT setAddresses(PVOID data)
		{
#ifndef PATCH_SVR
			//structs
			CoDPatches.NetInfoAddress = 0x82BBD634;
			CoDPatches.ScreenPlacement = 0x82bcdd10;
			CoDPatches.OurXuidOffset = 0x83F0A358;
			CoDPatches.PrestigeOffset = 0x835887F4;
			CoDPatches.RadarOffset1 = 0x822653A0;
			CoDPatches.RadarOffset2 = 0x82265794;
			CoDPatches.RadarOffset3 = 0x82265800;
			CoDPatches.NoRecoilOffset = 0x822C9344;
			CoDPatches.NoSwayOffset1 = 0x822583D0;
			CoDPatches.NoSwayOffset2 = 0x82258BEC;
			CoDPatches.NetSpeedOffset = 0x83F0A254;
			CoDPatches.FOVStruct = 0x82ACCADC;
			CoDPatches.ServerIdOffset = 0x82B55908;
			CoDPatches.OverheadNamesEnemyOffset = 0x8226BA18;
			CoDPatches.OverheadNamesFriendlyOffset = 0x8226BC0C;
			CoDPatches.ThirdPersonStruct = 0x82A90960;
			CoDPatches.NoFlinchOffset = 0x82292B64;
			CoDPatches.UnlockAllOffset = 0x83587BB0;
			CoDPatches.SquadPointOffset = 0x835884FC;
			CoDPatches.GetWeaponIcon = 0x82A96FD0;
			CoDPatches.Autowall360 = 0x822C784C;
			CoDPatches.LocalClientPointer = 0x82BAC404;
			CoDPatches.EntitiesPointer = 0x82AD11AC;
			CoDPatches.CG_Pointer = 0x82AD56FC;
			CoDPatches.CGS_Pointer = 0x82AD56F8;
			CoDPatches.CG_S_XY = 0x82B150E8;

			//functions
			CoDPatches.GetTagPosition = 0x821379E0;
			CoDPatches.CG_DrawGrenadePointer = 0x822679A8;
			CoDPatches.IsWeaponDualWield = 0x82253798;
			CoDPatches.CG_BulletHitEvent = 0x822C73C0;
			CoDPatches.DynEntCl_DynEntImpactEvent = 0x82385B38;
			CoDPatches.TransformSeed = 0x82234840;
			CoDPatches.Menu_PaintAll = 0x822CE318;
			CoDPatches.XInputGetState = 0x8272D6A8;
			CoDPatches.CL_SendCmd = 0x822D6CF8;
			CoDPatches.CL_CreateNewCommands = 0x822D67E8;
			CoDPatches.PlayLocalSoundAliasByName = 0x82726738;
			CoDPatches.R_EndFrame = 0x822E7298;
			CoDPatches.CG_EntityEvent = 0x822785C0;
			CoDPatches.LiveStats_WriteStat = 0x8229CBD8;
			CoDPatches.VA = 0x8256E970;
			CoDPatches.Dvar_GetBool = 0x825639B0;
			CoDPatches.CG_LocationalTrace = 0x822CB3B0;
			CoDPatches.DB_FindXAssetHeader = 0x82376680;
			CoDPatches.R_TextWidth = 0x8264DC78;
			CoDPatches.CL_DrawTextPhysical = 0x8266CEB8;
			CoDPatches.R_AddCmdDrawStretchPic = 0x8266AF28;
			CoDPatches.UI_FillRect = 0x82270138;
			CoDPatches.UI_FillRectPhysical = 0x82510E70;
			CoDPatches.CBuf_AddText = 0x8244C738;
			CoDPatches.SL_GetString = 0x824934A8;
			CoDPatches.CG_GetWeaponIndexForName = 0x8224A260;
			CoDPatches.CG_GetEntityBModelBounds = 0x822C9728;
			CoDPatches.R_NormalizedTextScale = 0x8264DC18;
			CoDPatches.CG_CalcNamePosition = 0x8226A098;
			CoDPatches.G_GetWeaponHitLocationMultiplier = 0x821DD100;
			CoDPatches.GetWeaponHudName = 0x822BE118;
			CoDPatches.BG_GetSpreadForWeapon = 0x82254700;
			CoDPatches.RandomBulletDir = 0x822C2558;
			CoDPatches.BG_GetWeaponDef = 0x820F6D80;
			CoDPatches.BG_GetWeaponDef2 = 0x8213EDE0;
			CoDPatches.CG_GetPlayerViewOrigin = 0x822BC078;
			CoDPatches.SomeRandomFuckingShit = 0x8222EC70;
			CoDPatches.FireBulletPenetrate = 0x822C7518;
			CoDPatches.CG_CompassCalcDimensions = 0x8225F3D8;
			CoDPatches.CG_WorldPosToCompass = 0x8225FD18;
			CoDPatches.UI_DrawHandlePic = 0x822702F0;
			CoDPatches.CL_GetScreenDimensions = 0x82315378;
			CoDPatches.CG_CompassDrawPlayerMap = 0x82260B50;
			CoDPatches.CG_CompassUpYawVector = 0x82261428;
			CoDPatches.CL_AllLocalClientsDisconnected = 0x82316CA0;
			

			DbgPrint("Addresses set!");

			fHasPatches = TRUE;
#else
			if (data)
			{
				memcpy(&CoDPatches, data, sizeof(CoDPatches_s));
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
				XeCryptRandom((PBYTE)&CoDPatches, sizeof(CoDPatches_s));
				fHasPatches = FALSE;
			}
		}

		CoDCheats()
		{
			fHasPatches = FALSE;
#ifndef PATCH_SVR
			setAddresses(NULL);
			fHasPatches = TRUE;
#endif
		}
		~CoDCheats()
		{
		}

#ifdef DUMP_BINS
		#define getCryptedCoDDump(a) ((DWORD)(docryptoBypass(a, (QWORD)((((DWORD*)&a - (DWORD*)&CoDPatches.NetInfoAddress))))))
		BYTE dumpBuf[sizeof(CoDPatches_s)];
		VOID dumpPatchBinary(char* file = NULL)
		{
			if (!fHasPatches)
				return;

			memset(dumpBuf, 0, sizeof(CoDPatches_s));

			DWORD *fileBuff = (DWORD*)&dumpBuf[0];
			for (BYTE i = 0; i * 4 < sizeof(CoDPatches_s); i += 1)
				fileBuff[i] = (DWORD)getCryptedCoDDump(*(DWORD*)((DWORD)&CoDPatches + ((DWORD)i * 4)));

			printf("Writing main patches to binary!\n");
			binToFile(file != NULL ? file : "XBLIVE:\\Ghosts.bin", dumpBuf, sizeof(CoDPatches_s));
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

extern Security::CoDCheats *CheatPatches;
extern MemoryManager *mManager;

extern XBLSMsgr Messenger;

extern BOOL isDevkit;

extern bool haveLaunchPath;
extern std::string launchDevice;
extern std::string launchFile;
extern std::string launchXEX;
extern std::string launchPath;

extern DWORD numSharedOnKV;



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
	COD_GHOSTS = 0xEF8961CF,
	DESTINY = 0xEF8961CB,
	COD_BLACK_OPS_3 = 0xEF89602E,
	DASHBOARD = 0x51216EE2,
	GTAV = 0xFA8B6194
} XBOX_GAMES;

static DWORD inline fixDWORD(DWORD input) {
	input ^= 0xAEDF6933;
	return input;
}