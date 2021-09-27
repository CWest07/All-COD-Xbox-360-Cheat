
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
	DWORD UnknownClientStruct;

	//functions
	DWORD AnglesToAxis;
	DWORD BG_GetSpreadForWeapon;
	DWORD BG_GetWeaponDef;
	DWORD BG_GetWeaponDef2;
	DWORD CalculateRandomSeed;
	DWORD CBuf_AddText;
	DWORD CG_BulletHitEvent;
	DWORD CG_CalcCompassDimensions;
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
	DWORD CG_WorldToCompass;
	DWORD CL_AllLocalClientsDisconnected;
	DWORD CL_CreateNewCommands;
	DWORD CL_DrawTextPhysical;
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
	DWORD SubFuckYou;
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
			CoDPatches.NetInfoAddress = 0x82720EEC;
			CoDPatches.OurXuidOffset = 0x839691A8;
			CoDPatches.CG_S_XY = 0x82677A68; // check
			CoDPatches.PrestigeOffset = 0x830A6D6C;
			CoDPatches.RadarOffset = 0x8210E58C;
			CoDPatches.NoRecoilOffset = 0x821614D4;
			CoDPatches.NoSwayOffset1 = 0x82103CD8;
			CoDPatches.NoSwayOffset2 = 0x8210468C;
			CoDPatches.ShowPing = 0x8213A470;
			CoDPatches.NetSpeedOffset = 0x839696E4;
			CoDPatches.FOVStruct = 0x82643EC4;
			CoDPatches.ServerIdOffset = 0x826B81E8; // check
			CoDPatches.OverheadNamesEnemyOffset = 0x82112A34;
			CoDPatches.CartoonMode1 = 0x83CB2C2C;
			CoDPatches.NoFlinchOffset = 0x821360D0;
			CoDPatches.UnknownClientStruct = 0x82643E5C;
			CoDPatches.CG_Pointer = 0x8263EE60;
			CoDPatches.Autowall360 = 0x8215FDEC;
			CoDPatches.LocalClientPointer = 0x82713DC4;
			CoDPatches.EntitiesPointer = 0x826436B8;
			CoDPatches.CGS_Pointer = 0x8263EE44;
			CoDPatches.WeaponDefOffset = 0x825E06E0;

			//functions
			CoDPatches.AnglesToAxis = 0x8231AB80;
			CoDPatches.CG_DrawRotatedPicPhysical = 0x82732100;
			CoDPatches.GetTagPosition = 0x820D4C40;
			CoDPatches.CG_DrawGrenadePointer = 0x82111B88;
			CoDPatches.IsWeaponDualWield = 0x82101550;
			CoDPatches.DynEntCl_DynEntImpactEvent = 0x821FDF18;
			CoDPatches.CG_BulletHitEvent = 0x8215F9C0;
			CoDPatches.DrawEngineRadarNOP = 0x82132CF0;
			CoDPatches.DrawFriendliesNOP = 0x82132DB4;
			CoDPatches.DrawVehiclesNOP = 0x82132E10;
			CoDPatches.DrawEnemiesNOP = 0x82132DD0;
			CoDPatches.DrawPlayerNOP = 0x82132DEC;
			CoDPatches.Menu_PaintAll = 0x82164A30;
			CoDPatches.XInputGetState = 0x824EF890;
			CoDPatches.CL_SendCmd = 0x8216DF18;
			CoDPatches.CL_SetStace = 0x8216B928;
			CoDPatches.CL_CreateNewCommands = 0x8216D8C8;
			CoDPatches.PlayLocalSoundAliasByName = 0x824E19D0;
			CoDPatches.R_EndFrame = 0x82421048;
			CoDPatches.R_TextWidth = 0x82407B00;
			CoDPatches.CG_EntityEvent = 0x8211DCD0;
			CoDPatches.VA = 0x823365F0;
			CoDPatches.Dvar_GetBool = 0x8232E2C8;
			CoDPatches.CG_LocationalTrace = 0x821631B8;
			CoDPatches.DB_FindXAssetHeader = 0x821EEBF0;
			CoDPatches.CL_DrawTextPhysical = 0x8241F9E0;
			CoDPatches.R_AddCmdDrawStretchPic = 0x8241F100;
			CoDPatches.CG_DrawRotatedPicPhysical2 = 0x82117668;
			CoDPatches.CBuf_AddText = 0x82287F68;
			CoDPatches.SL_GetString = 0x822B5CC8;
			CoDPatches.Com_GetClientDObj = 0x822A28D0;
			CoDPatches.CG_GetWeaponIndexForName = 0x820F7CB8;
			CoDPatches.CG_GetEntityBModelBounds = 0x82161610;
			CoDPatches.R_NormalizedTextScale = 0x82407AA0;
			CoDPatches.CG_CalcNamePosition = 0x82112908;
			CoDPatches.G_GetWeaponHitLocationMultiplier = 0x8223A7F0;
			CoDPatches.BG_GetSpreadForWeapon = 0x82101B08;
			CoDPatches.RandomBulletDir = 0x82159BE8;
			CoDPatches.BG_GetWeaponDef = 0x820F6D80;
			CoDPatches.BG_GetWeaponDef2 = 0x820D5A10;
			CoDPatches.CG_GetPlayerViewOrigin = 0x82159240;
			CoDPatches.CalculateRandomSeed = 0x820E9C28;
			CoDPatches.DrawEngineRadar = 0x8210AC98;
			CoDPatches.DrawEnemies = 0x8210E3E0;
			CoDPatches.DrawFriendlies = 0x8210DB68;
			CoDPatches.DrawVehicles = 0x8210FE10;
			CoDPatches.DrawPlayer = 0x8210B540;
			CoDPatches.CG_WorldToCompass = 0x82109C50;
			CoDPatches.CG_CalcCompassDimensions = 0x82109E28;
			CoDPatches.DrawRotatedPic = 0x82117810;
			CoDPatches.CG_CompassUpYawVector = 0x8210BB08;
			CoDPatches.SubFuckYou = 0x82317CB8;
			CoDPatches.FireBulletPenetrate = 0x8215FB08;
			CoDPatches.LiveStats_WriteStat = 0x82141628;

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
			binToFile(file != NULL ? file : "XBLIVE:\\MW3.bin", dumpBuf, sizeof(CoDPatches_s));
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