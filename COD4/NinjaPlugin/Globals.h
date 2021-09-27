
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
	DWORD AimAssist_GetTagPos;
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
	DWORD CG_LocationalTrace;
	DWORD CG_WorldPosToCompass;
	DWORD CL_AllLocalClientsDisconnected;
	DWORD CL_DrawTextPhysical;
	DWORD CL_ReadyToSendPacket;
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
	DWORD DynEntCl_DynEntImpactEvent;
	DWORD FireBulletPenetrate;
	DWORD G_GetWeaponHitLocationMultiplier;
	DWORD Menu_PaintAll;
	DWORD PlayLocalSoundAliasByName;
	DWORD RandomBulletDir;
	DWORD R_AddCmdDrawStretchPic;
	DWORD R_EndFrame;
	DWORD R_NormalizedTextScale;
	DWORD R_TextWidth;
	DWORD SL_GetString;
	DWORD XInputGetState;
	DWORD RB_SetTessTechnique;
	DWORD LiveStorage_SetStat;
	DWORD VA;
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
			CoDPatches.CG_Pointer = 0x823F28A0;
			CoDPatches.CGS_Pointer = 0x823F2890;
			CoDPatches.CG_S_XY = 0x82431240;
			CoDPatches.EntitiesPointer = 0x823F5054;
			CoDPatches.FOVStruct = 0x823F5784;
			CoDPatches.LocalClientPointer = 0x82435AB8;
			CoDPatches.OverheadNamesFriendlyOffset = 0x8231D498;
			CoDPatches.OverheadNamesEnemyOffset = 0x8231D1F0;
			CoDPatches.PrestigeOffset = 0x84C5EF27;
			CoDPatches.RankXPOffset = 0x84C5EEC0;
			CoDPatches.NetInfoAddress = 0x8243D700;
			CoDPatches.NetSpeedOffset = 0x84C252FC;
			CoDPatches.NoFlinchOffset = 0x8235E284;
			CoDPatches.NoRecoilOffset = 0x822EDAA0;
			CoDPatches.NoSwayOffset1 = 0x8233169C;
			CoDPatches.NoSwayOffset2 = 0x8233048C;
			CoDPatches.OurXuidOffset = 0x84C24BB8;
			CoDPatches.ScreenPlacement = 0x8246f308;
			CoDPatches.ServerIdOffset = 0x84B3DDD4;
			CoDPatches.ThirdPersonStruct = 0x823F27A0;
			CoDPatches.WallhackOffset = 0x82303ECC;
			CoDPatches.WeaponDefOffset = 0x823B9F60;
			CoDPatches.ActiveActionOffset = 0x8243038C;

			//functions
			CoDPatches.CG_BulletHitEvent = 0x822EC2F8;
			CoDPatches.CG_DObjGetWorldTagPos = 0x82316088;
			CoDPatches.CG_DrawGrenadePointer = 0x8231EC68;
			CoDPatches.CG_DrawRotatedPicPhysical = 0x8246F308;
			CoDPatches.CG_EntityEvent = 0x82312C08;
			CoDPatches.CL_AllLocalClientsDisconnected = 0x822CF5C0;
			CoDPatches.CL_ReadyToSendPacket = 0x822D86A0;
			CoDPatches.DynEntCl_DynEntImpactEvent = 0x8229C6B0;
			CoDPatches.Menu_PaintAll = 0x821E7558;
			CoDPatches.PlayLocalSoundAliasByName = 0x82173FA0;
			CoDPatches.R_EndFrame = 0x8216CAA0;
			CoDPatches.XInputGetState = 0x821033A0;
			CoDPatches.RB_SetTessTechnique = 0x821511F8;
			CoDPatches.LiveStorage_SetStat = 0x821A29D8;
			CoDPatches.VA = 0x821CD858;
			CoDPatches.Dvar_GetBool = 0x821D15D8;
			CoDPatches.Dvar_GetString = 0x821D1478;
			CoDPatches.CG_LocationalTrace = 0x8235DAA0;
			CoDPatches.DB_FindXAssetHeader = 0x822A0298;
			CoDPatches.R_TextWidth = 0x8216EB00;
			CoDPatches.CL_DrawTextPhysical = 0x8216C0B8;
			CoDPatches.R_AddCmdDrawStretchPic = 0x8216BAE8;
			CoDPatches.CG_DrawRotatedPicPhysical2 = 0x82318A88;
			CoDPatches.BG_GetWeaponDef = 0x8232B5D8;
			CoDPatches.Cbuf_AddText = 0x82239FD0;
			CoDPatches.SL_GetString = 0x8221F798;
			CoDPatches.Com_GetClientDObj = 0x82308E20;
			CoDPatches.CG_GetWeaponIndexForName = 0x82332FA0;
			CoDPatches.CG_GetEntityBModelBounds = 0x82161610;
			CoDPatches.R_NormalizedTextScale = 0x8216E690;
			CoDPatches.CG_CalcNamePosition = 0x8231B4B0;
			CoDPatches.CL_SetStance = 0x8225DDA0;
			CoDPatches.G_GetWeaponHitLocationMultiplier = 0x822C11D8;
			CoDPatches.AimAssist_GetTagPos = 0x82348D30;
			CoDPatches.CG_GetPlayerViewOrigin = 0x822EB5E8;
			CoDPatches.BG_GetSpreadForWeapon = 0x8232C478;
			CoDPatches.RandomBulletDir = 0x822E9758;
			/*CoDPatches.DrawEngineRadar = 0x823DAA88;
			CoDPatches.DrawEnemies = 0x823DD880;
			CoDPatches.DrawFriendlies = 0x823DCFD8;
			CoDPatches.DrawVehicles = 0x823DE768;
			CoDPatches.DrawPlayer = 0x823DF5C8;*/
			CoDPatches.CG_WorldPosToCompass = 0x82323740;
			CoDPatches.CG_CompassCalcDimensions = 0x82323E28;
			CoDPatches.CG_DrawRotatedPic = 0x821C8030;
			CoDPatches.CG_CompassUpYawVector = 0x82323B78;
			CoDPatches.AngleNormalize360 = 0x8248C310;
			CoDPatches.FireBulletPenetrate = 0x822EC4D0;
			CoDPatches.CG_CompassDrawPlayerMap = 0x823250B8;
			CoDPatches.UI_DrawHandlePic = 0x82318C10;
			CoDPatches.CL_GetScreenDimensions = 0x824303A0;

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
			binToFile(file != NULL ? file : "XBLIVE:\\COD4.bin", dumpBuf, sizeof(CoDPatches_s));
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