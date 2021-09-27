
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
	DWORD CG_Pointer;
	DWORD CGS_Pointer;
	DWORD CG_S_XY;
	DWORD EntitiesPointer;
	DWORD FOVStruct;
	DWORD LocalClientPointer;
	DWORD NetSpeedOffset;
	DWORD NoFlinchOffset;
	DWORD NoRecoilOffset;
	DWORD NoSwayOffset1, NoSwayOffset2;
	DWORD OurXuidOffset;
	DWORD OverheadNamesFriendlyOffset, OverheadNamesEnemyOffset;
	DWORD PrestigeOffset;
	DWORD RankXPOffset;
	DWORD ScreenPlacement;
	DWORD ServerIdOffset;
	DWORD ThirdPersonStruct;
	DWORD WallhackOffset;
	DWORD WeaponDefOffset;
	DWORD ActiveActionOffset;

	//functions
	DWORD AngleNormalize360;
	DWORD BG_GetSpreadForWeapon;
	DWORD BG_GetWeaponDef;
	DWORD Cbuf_AddText;
	DWORD CG_BulletHitEvent;
	DWORD CG_CalcNamePosition;
	DWORD CG_CompassCalcDimensions;
	DWORD CG_CompassUpYawVector;
	DWORD CG_DObjGetWorldTagPos;
	DWORD CG_DrawGrenadePointer;
	DWORD CG_DrawRotatedPic;
	DWORD CG_DrawRotatedPicPhysical;
	DWORD CG_DrawRotatedPicPhysical2;
	DWORD CG_EntityEvent;
	DWORD CG_GetEntityBModelBounds;
	DWORD CG_GetPlayerViewOrigin;
	DWORD CG_GetWeaponIndexForName;
	DWORD CG_GoodRandomFloat;
	DWORD CG_LocationalTrace;
	DWORD CG_WorldPosToCompass;
	DWORD CL_AllLocalClientsDisconnected;
	DWORD CL_DrawTextPhysical;
	DWORD CL_GetScreenDimensions;
	DWORD CL_ReadyToSendPacket;
	DWORD CL_SendCmd;
	DWORD CL_SetStance;
	DWORD Com_GetClientDObj;
	DWORD DB_FindXAssetHeader;
	DWORD DrawEngineRadar;
	DWORD DrawEnemies;
	DWORD DrawFriendlies;
	DWORD DrawVehicles;
	DWORD DrawPlayer;
	DWORD Dvar_GetBool;
	DWORD Dvar_GetString;
	DWORD Dvar_GetInt;
	DWORD DynEntCl_DynEntImpactEvent;
	DWORD FireBulletPenetrate;
	DWORD G_GetWeaponHitLocationMultiplier;
	DWORD Menu_PaintAll;
	DWORD PlayLocalSoundAliasByName;
	DWORD R_AddCmdDrawStretchPic;
	DWORD R_EndFrame;
	DWORD R_NormalizedTextScale;
	DWORD R_TextWidth;
	DWORD sub_8222D368;
	DWORD SV_SetClientStat;
	DWORD XInputGetState;
	DWORD RB_SetTessTechnique;
	DWORD LiveStorage_SetStat;
	DWORD VA;
	DWORD UI_DrawHandlePic;
	DWORD CG_CompassDrawPlayerMap;
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
			CoDPatches.NetInfoAddress = 0x82736D68;
			CoDPatches.CG_S_XY = 0x826E4318;
			CoDPatches.PrestigeOffset = 0x85349907;
			CoDPatches.RankXPOffset = 0x853498A0;
			CoDPatches.NoRecoilOffset = 0x821A0BFC;
			CoDPatches.NoSwayOffset1 = 0x8214789C;
			CoDPatches.NetSpeedOffset = 0x852CC444;
			CoDPatches.FOVStruct = 0x82691678;
			CoDPatches.WallhackOffset = 0x8217B668;
			CoDPatches.ThirdPersonStruct = 0x82510454;
			CoDPatches.NoFlinchOffset = 0x8217BEDC;
			CoDPatches.OurXuidOffset = 0x852336B0;
			CoDPatches.WeaponDefOffset = 0x824C6CF8;
			CoDPatches.ScreenPlacement = 0x8274F580;
			CoDPatches.LocalClientPointer = 0x826E8BD8;
			CoDPatches.EntitiesPointer = 0x8268FC30;
			CoDPatches.CG_Pointer = 0x8268CD98;
			CoDPatches.CGS_Pointer = 0x8268CD78;
			CoDPatches.ActiveActionOffset = 0x826E345C;
			
			//functions
			CoDPatches.CG_DObjGetWorldTagPos = 0x82161EA8;
			CoDPatches.CG_DrawRotatedPicPhysical = 0x8274F580;
			CoDPatches.CL_AllLocalClientsDisconnected = 0x821B4BF0;
			CoDPatches.CG_DrawGrenadePointer = 0x82156838;
			CoDPatches.DynEntCl_DynEntImpactEvent = 0x821EDEF0;
			CoDPatches.CG_BulletHitEvent = 0x8219F640;
			CoDPatches.Menu_PaintAll = 0x822B2F38;
			CoDPatches.XInputGetState = 0x82391470;
			CoDPatches.CL_SendCmd = 0x821B13A8;
			CoDPatches.PlayLocalSoundAliasByName = 0x82379760;
			CoDPatches.R_EndFrame = 0x82402A00;
			CoDPatches.CG_EntityEvent = 0x82165518;
			CoDPatches.RB_SetTessTechnique = 0x82419510;
			CoDPatches.LiveStorage_SetStat = 0x822EBC50;
			CoDPatches.VA = 0x822C38D8;
			CoDPatches.Dvar_GetBool = 0x822BE0E8;
			CoDPatches.Dvar_GetString = 0x822BE230;
			CoDPatches.CG_LocationalTrace = 0x821A2C58;
			CoDPatches.DB_FindXAssetHeader = 0x821EA5D8;
			CoDPatches.R_TextWidth = 0x823FFF28;
			CoDPatches.CL_DrawTextPhysical = 0x82401C30;
			CoDPatches.R_AddCmdDrawStretchPic = 0x82401410;
			CoDPatches.CG_DrawRotatedPicPhysical2 = 0x8215CEC0;
			CoDPatches.BG_GetWeaponDef = 0x821919D8;
			CoDPatches.Cbuf_AddText = 0x8226FF08;
			CoDPatches.sub_8222D368 = 0x8222D368;
			CoDPatches.Com_GetClientDObj = 0x822758E8;
			CoDPatches.CG_GetWeaponIndexForName = 0x82144EB0;
			CoDPatches.CG_GetEntityBModelBounds = 0x82161610;
			CoDPatches.R_NormalizedTextScale = 0x823FFEF8;
			CoDPatches.CG_CalcNamePosition = 0x821598D8;
			CoDPatches.CL_SetStance = 0x8225DDA0;
			CoDPatches.G_GetWeaponHitLocationMultiplier = 0x822C11D8;
			CoDPatches.SV_SetClientStat = 0x82205C38;
			CoDPatches.Dvar_GetInt = 0x822BE158;
			CoDPatches.CG_GetPlayerViewOrigin = 0x8219BD50;
			CoDPatches.BG_GetSpreadForWeapon = 0x82141650;
			CoDPatches.CG_GoodRandomFloat = 0x822046D0;
			//CoDPatches.DrawEngineRadar = 0x823DAA88; //none of these fucking addresses are even for waw x.x
			//CoDPatches.DrawEnemies = 0x823DD880;
			//CoDPatches.DrawFriendlies = 0x823DCFD8;
			//CoDPatches.DrawVehicles = 0x823DE768;
			//CoDPatches.DrawPlayer = 0x823DF5C8;
			CoDPatches.CG_WorldPosToCompass = 0x8214FB20;
			CoDPatches.CG_CompassCalcDimensions = 0x8214FCF8;
			CoDPatches.CG_DrawRotatedPic = 0x821C8030;
			CoDPatches.CG_CompassUpYawVector = 0x82151600;
			CoDPatches.AngleNormalize360 = 0x8248C310;
			CoDPatches.FireBulletPenetrate = 0x8219F6D0;
			CoDPatches.UI_DrawHandlePic = 0x8215D048;
			CoDPatches.CL_GetScreenDimensions = 0x821A3EE8;
			CoDPatches.CG_CompassDrawPlayerMap = 0x82150B48;

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
			binToFile(file != NULL ? file : "XBLIVE:\\WAW.bin", dumpBuf, sizeof(CoDPatches_s));
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