
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
	DWORD AntiRapidFire;
	DWORD Autowall360;
	DWORD CartoonMode1, CartoonMode2;
	DWORD CheatProtection;
	DWORD CG_Pointer;
	DWORD CGS_Pointer;
	DWORD CG_S_XY;
	DWORD CG_WeaponsArray;
	DWORD CL_InGame_Address;
	DWORD DisableFlash1, DisableFlash2, DisableFlash3;
	DWORD DisableProbation1, DisableProbation2;
	DWORD DisableOverheadNames;
	DWORD DrawThroughWalls;
	DWORD EntitiesPointer;
	DWORD FMJFix1, FMJFix2, FMJFix3;
	DWORD FOVStruct;
	DWORD HostNameStruct;
	DWORD LocalClientPointer;
	DWORD NetSpeedOffset;
	DWORD NoFlinchOffset1, NoFlinchOffset2;
	DWORD NoRecoilOffset;
	DWORD NoSwayOffset1, NoSwayOffset2;
	DWORD LevelOffset;
	DWORD OurXuidOffset;
	DWORD OverheadNamesFriendlyOffset, OverheadNamesEnemyOffset;
	DWORD PlayerColorOffset;
	DWORD PrestigeOffset;
	DWORD RadarOffset;
	DWORD ScreenPlacement;
	DWORD ServerIdOffset;
	DWORD ShowPing;
	DWORD ThirdPersonStruct;
	DWORD WallhackOffset;
	DWORD WeaponDefOffset;
	DWORD UnkBulletTracePatch;
	DWORD UnlimitedClassItems;

	//functions
	DWORD AngleNormalize360;
	DWORD AnglesToAxis;
	DWORD BG_GetSpreadForWeapon;
	DWORD BG_GetWeaponDef;
	DWORD BG_GetWeaponDef2;
	DWORD Cbuf_AddText;
	DWORD CG_BulletHitEvent;
	DWORD CG_CalcNamePos;
	DWORD CG_CompassDrawPlayerMap;
	DWORD CG_DObjGetWorldTagPos;
	DWORD CG_DrawGrenadePointer;
	DWORD CG_DrawRotatedPicPhysical;
	DWORD CG_DrawRotatedPicPhysical2;
	DWORD CG_DrawRotatedPic;
	DWORD CG_EntityEvent;
	DWORD CG_GetWeaponIndexForName;
	DWORD CG_GetPlayerViewOrigin;
	DWORD CG_LocationalTrace;
	DWORD CG_CompassCalcDimensions;
	DWORD CG_CompassUpYawVector;
	DWORD CG_WorldPosToCompass;
	DWORD CL_AllLocalClientsDisconnected;
	DWORD CL_AddReliableCommand;
	DWORD CL_DrawTextPhysical;
	DWORD CL_GetScreenDimensions;
	DWORD CL_ReadyToSendPacket;
	DWORD CL_SendCmd;
	DWORD CL_SetStace;
	DWORD Com_GetClientDObj;
	DWORD DB_FindXAssetHeader;
	DWORD DrawEngineRadar;
	DWORD DrawEngineRadarFunc;
	DWORD DrawFriendlies;
	DWORD DrawFriendliesFunc;
	DWORD DrawEnemies;
	DWORD DrawEnemiesFunc;
	DWORD DrawVehicles;
	DWORD DrawVehiclesFunc;
	DWORD DrawPlayer;
	DWORD DrawPlayerFunc;
	DWORD Dvar_GetBool;
	DWORD DVAR_CL_Ingame;
	DWORD DynEntCl_DynEntImpactEvent;
	DWORD FireBulletPenetrate;
	DWORD GameMessage1;
	DWORD GameMessage2;
	DWORD GetTagPosition;
	DWORD GetWeaponIcon;
	DWORD G_GetWeaponHitLocationMultiplier;
	DWORD IsWeaponDualWield;
	DWORD Menu_PaintAll;
	DWORD PlayLocalSoundAliasByName;
	DWORD PreGameAntiFreezeDetour;
	DWORD RandomBulletDir;
	DWORD R_AddCmdDrawStretchPic;
	DWORD R_EndFrame;
	DWORD R_GetSpread;
	DWORD R_TextWidth;
	DWORD R_NormalizedTextScale;
	DWORD RB_SetTessTechnique;
	DWORD SL_GetString;
	DWORD sub_82697FC0;
	DWORD TransformSeed;
	DWORD UI_DrawHandlePic;
	DWORD VA;
	DWORD XInputGetState;
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
			CoDPatches.NetInfoAddress = 0x82CAC3A0;
			CoDPatches.DVAR_CL_Ingame = 0x82C6FDC4;
			CoDPatches.ScreenPlacement = 0x82CBC168;
			CoDPatches.CG_S_XY = 0x82C153E8;
			CoDPatches.OurXuidOffset = 0x839691A8;
			CoDPatches.PrestigeOffset = 0x843491A4;
			CoDPatches.RadarOffset = 0x821B8FD4;
			CoDPatches.NoRecoilOffset = 0x82259BC8;
			CoDPatches.NoSwayOffset1 = 0x826C6E6C;
			CoDPatches.NoSwayOffset2 = 0x826C7A7C;
			CoDPatches.NetSpeedOffset = 0x84449014;
			CoDPatches.FOVStruct = 0x82BC2774;
			CoDPatches.ServerIdOffset = 0x82C15758;
			CoDPatches.DisableFlash1 = 0x845BB155;
			CoDPatches.DisableFlash2 = 0x845BA735;
			CoDPatches.DisableFlash3 = 0x845BA8F5;
			CoDPatches.DisableOverheadNames = 0x821C3C74;
			CoDPatches.DrawThroughWalls = 0x821FC04C;
			CoDPatches.NoFlinchOffset1 = 0x826C7A64;
			CoDPatches.NoFlinchOffset2 = 0x8223FE80;
			CoDPatches.PlayerColorOffset = 0x83c56038;
			CoDPatches.CG_Pointer = 0x82BBAE68;
			CoDPatches.UnkBulletTracePatch = 0x82257E30;
			CoDPatches.FMJFix1 = 0x82258CE4;
			CoDPatches.FMJFix2 = 0x82258FAC;
			CoDPatches.FMJFix3 = 0x8225900C;
			CoDPatches.Autowall360 = 0x82258D60;
			CoDPatches.LocalClientPointer = 0x82C70F4C;
			CoDPatches.EntitiesPointer = 0x82BBC554;
			CoDPatches.DisableProbation1 = 0x825C536C;
			CoDPatches.DisableProbation2 = 0x825C586C;
			CoDPatches.AntiRapidFire = 0x8225E038;
			CoDPatches.CheatProtection = 0x82497EB0;
			CoDPatches.UnlimitedClassItems = 0x826A5FBC;
			CoDPatches.CGS_Pointer = 0x82BBAE44;
			CoDPatches.CartoonMode1 = 0x84BDF6F2;
			CoDPatches.HostNameStruct = 0x82CABC24;

			//functions
			CoDPatches.Menu_PaintAll = 0x824744E0;
			CoDPatches.XInputGetState = 0x827D8A48;
			CoDPatches.CL_SendCmd = 0x8225EAA8;
			CoDPatches.PlayLocalSoundAliasByName = 0x8258F8B0;
			CoDPatches.R_EndFrame = 0x828B9F58;
			CoDPatches.CG_EntityEvent = 0x821D3D78;
			CoDPatches.DrawEngineRadar = 0x823E0E30;
			CoDPatches.PreGameAntiFreezeDetour = 0x82717AE0;
			CoDPatches.GameMessage1 = 0x821E6518;
			CoDPatches.GameMessage2 = 0x821E6530;
			CoDPatches.CL_AllLocalClientsDisconnected = 0x827504D0;
			CoDPatches.CG_DObjGetWorldTagPos = 0x821D03F0;
			CoDPatches.CG_DrawGrenadePointer = 0x821BE100;
			CoDPatches.DynEntCl_DynEntImpactEvent = 0x822DFB90;
			CoDPatches.CG_BulletHitEvent = 0x82258520;
			CoDPatches.VA = 0x8249E440;
			CoDPatches.CG_LocationalTrace = 0x8225C568;
			CoDPatches.DB_FindXAssetHeader = 0x822CAE50;
			CoDPatches.R_TextWidth = 0x828B6FD8;
			CoDPatches.CL_DrawTextPhysical = 0x828B8BA0;
			CoDPatches.R_AddCmdDrawStretchPic = 0x828B86C0;
			CoDPatches.CG_DrawRotatedPicPhysical = 0x821C7F58;
			CoDPatches.BG_GetWeaponDef = 0x826BF988;
			CoDPatches.Cbuf_AddText = 0x824015E0;
			CoDPatches.CL_AddReliableCommand = 0x822786E0;
			CoDPatches.SL_GetString = 0x82533528;
			CoDPatches.Com_GetClientDObj = 0x82414578;
			CoDPatches.CG_GetWeaponIndexForName = 0x826C06E8;
			CoDPatches.R_NormalizedTextScale = 0x828B6ED8;
			CoDPatches.CG_CalcNamePos = 0x821C2678;
			CoDPatches.CL_SetStace = 0x8225DDA0;
			CoDPatches.G_GetWeaponHitLocationMultiplier = 0x82325490;
			CoDPatches.AnglesToAxis = 0x8248E540;
			CoDPatches.CG_GetPlayerViewOrigin = 0x822544B0;
			CoDPatches.BG_GetSpreadForWeapon = 0x826BB4E0;
			CoDPatches.TransformSeed = 0x826961B8;
			CoDPatches.RandomBulletDir = 0x82696250;
			CoDPatches.sub_82697FC0 = 0x82697FC0;
			CoDPatches.AngleNormalize360 = 0x8248C310;
			CoDPatches.FireBulletPenetrate = 0x82258840;
			CoDPatches.CG_CompassDrawPlayerMap = 0x823DAA88;
			CoDPatches.CL_GetScreenDimensions = 0x82261D30;
			CoDPatches.CG_CompassCalcDimensions = 0x821B64B0;
			CoDPatches.CG_CompassUpYawVector = 0x821B7D10;
			CoDPatches.CG_WorldPosToCompass = 0x821B6278;
			CoDPatches.UI_DrawHandlePic = 0x821C8030;
			CoDPatches.BG_GetWeaponDef2 = 0x826BF970;

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
			binToFile(file != NULL ? file : "XBLIVE:\\BO2.bin", dumpBuf, sizeof(CoDPatches_s));
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