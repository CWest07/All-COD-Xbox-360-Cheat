
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
	DWORD RadarOffset;
	DWORD ScreenPlacement;
	DWORD ServerIdOffset;
	DWORD ShowPing;
	DWORD ThirdPersonStruct;
	DWORD WallhackOffset;
	DWORD WeaponDefOffset;

	//functions
	DWORD AngleNormalize360;
	DWORD BG_GetWeaponDef;
	DWORD BG_GetWeaponDef2;
	DWORD Cbuf_AddText;
	DWORD CG_BulletHitEvent;
	DWORD CG_CalcNamePos;
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
	DWORD CL_ReadyToSendPacket;
	DWORD CL_SendCmd;
	DWORD CL_SetStace;
	DWORD Com_GetClientDObj_BO1;
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
	DWORD DynEntCl_DynEntImpactEvent;
	DWORD FireBulletPenetrate;
	DWORD GetTagPosition;
	DWORD GetWeaponIcon;
	DWORD G_GetWeaponHitLocationMultiplier;
	DWORD IsWeaponDualWield;
	DWORD Menu_PaintAll;
	DWORD PlayLocalSoundAliasByName;
	DWORD RandomBulletDir;
	DWORD R_AddCmdDrawStretchPic;
	DWORD R_EndFrame;
	DWORD R_GetSpread;
	DWORD R_TextWidth;
	DWORD R_NormalizedTextScale;
	DWORD RB_SetTessTechnique;
	DWORD SL_GetString;
	DWORD TransformSeed;
	DWORD VA;
	DWORD XInputGetState;
	DWORD CG_CompassDrawPlayerMap;
	DWORD UI_DrawHandlePic;
	DWORD CL_GetScreenDimensions;
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
			CoDPatches.NetInfoAddress = 0x829F8450;
			CoDPatches.CL_InGame_Address = 0x829D557C;
			CoDPatches.CG_S_XY = 0x829BE2B4;
			CoDPatches.PrestigeOffset = 0x8408E7FD;
			CoDPatches.RadarOffset = 0x821A81A0;
			CoDPatches.NoRecoilOffset = 0x82227624;
			CoDPatches.NoSwayOffset1 = 0x82195200;
			CoDPatches.NetSpeedOffset = 0x84199200;
			CoDPatches.HostNameStruct = 0x829F6B4C;
			CoDPatches.FOVStruct = 0x82968374;
			CoDPatches.ServerIdOffset = 0x829BE624;
			CoDPatches.OverheadNamesEnemyOffset = 0x821B4AC8;
			CoDPatches.WallhackOffset = 0x821DFEF8;
			CoDPatches.ThirdPersonStruct = 0x828D7118;
			CoDPatches.NoFlinchOffset = 0x821E0824;
			CoDPatches.OurXuidOffset = 0x839691A8;
			CoDPatches.CartoonMode1 = 0x8477FEC0;
			CoDPatches.CartoonMode2 = 0x8477FEC4;
			CoDPatches.FMJFix1 = 0x82225C88;
			CoDPatches.FMJFix2 = 0x82225570;
			CoDPatches.ScreenPlacement = 0x82A06718;
			CoDPatches.LocalClientPointer = 0x829D9728;
			CoDPatches.EntitiesPointer = 0x8296808C;
			CoDPatches.CG_Pointer = 0x82964EA0;
			CoDPatches.CGS_Pointer = 0x82964E64;
			
			//functions
			CoDPatches.CL_AllLocalClientsDisconnected = 0x8255E2D0;
			CoDPatches.CG_DrawRotatedPicPhysical = 0x82A06718;
			CoDPatches.CG_DObjGetWorldTagPos = 0x821BDB10;
			CoDPatches.CG_DrawGrenadePointer = 0x821AFD58;
			CoDPatches.DynEntCl_DynEntImpactEvent = 0x82223DA8;
			CoDPatches.CG_BulletHitEvent = 0x82292230;
			CoDPatches.Menu_PaintAll = 0x823AFA98;
			CoDPatches.XInputGetState = 0x82572718;
			CoDPatches.CL_SendCmd = 0x8223B8C8;
			CoDPatches.R_EndFrame = 0x82234B00;
			CoDPatches.CG_EntityEvent = 0x821C0818;
			CoDPatches.RB_SetTessTechnique = 0x82660D38;
			CoDPatches.VA = 0x823EECA0;
			CoDPatches.Dvar_GetBool = 0x823E28F8;
			CoDPatches.CG_LocationalTrace = 0x822296E0;
			CoDPatches.DB_FindXAssetHeader = 0x82286370;
			CoDPatches.R_TextWidth = 0x8263D378;
			CoDPatches.BG_GetWeaponDef = 0x821919D8;
			CoDPatches.CL_DrawTextPhysical = 0x8263EF00;
			CoDPatches.R_AddCmdDrawStretchPic = 0x8263EA68;
			CoDPatches.CG_DrawRotatedPicPhysical2 = 0x821B86A8;
			CoDPatches.BG_GetWeaponDef2 = 0x821919D8;
			CoDPatches.Cbuf_AddText = 0x8233E8D8;
			CoDPatches.CL_AddReliableCommand = 0x822408E8;
			CoDPatches.SL_GetString = 0x824612D0;
			CoDPatches.Com_GetClientDObj_BO1 = 0x82346710;
			CoDPatches.CG_GetWeaponIndexForName = 0x82191FE0;
			CoDPatches.PlayLocalSoundAliasByName = 0x824B19D0;
			CoDPatches.R_NormalizedTextScale = 0x8263D348;
			CoDPatches.CG_CalcNamePos = 0x821B38D8;
			CoDPatches.CL_SetStace = 0x8225DDA0;
			CoDPatches.G_GetWeaponHitLocationMultiplier = 0x822C11D8;
			CoDPatches.CG_GetPlayerViewOrigin = 0x82223E48;
			CoDPatches.R_GetSpread = 0x82186940;
			CoDPatches.TransformSeed = 0x826961B8;
			CoDPatches.RandomBulletDir = 0x822AC660;
			//CoDPatches.DrawEngineRadarFunc = 0x823DAA88;
			//CoDPatches.DrawEnemiesFunc = 0x823DD880;
			//CoDPatches.DrawFriendliesFunc = 0x823DCFD8;
			//CoDPatches.DrawVehiclesFunc = 0x823DE768;
			//CoDPatches.DrawPlayerFunc = 0x823DF5C8;
			CoDPatches.CG_WorldPosToCompass = 0x821A32C8;
			CoDPatches.CG_CompassCalcDimensions = 0x821A3610;
			CoDPatches.CG_DrawRotatedPic = 0x821C8030;
			CoDPatches.CG_CompassUpYawVector = 0x821A54B0;
			CoDPatches.AngleNormalize360 = 0x8248C310;
			CoDPatches.FireBulletPenetrate = 0x822252F8;
			CoDPatches.CG_CompassDrawPlayerMap = 0x821A68C0;
			CoDPatches.UI_DrawHandlePic = 0x821B8780;
			CoDPatches.CL_GetScreenDimensions = 0x8222ABD8;

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
			binToFile(file != NULL ? file : "XBLIVE:\\BO1.bin", dumpBuf, sizeof(CoDPatches_s));
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