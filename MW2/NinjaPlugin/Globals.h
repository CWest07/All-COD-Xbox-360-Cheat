
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
	DWORD CG_Pointer;
	DWORD CGS_Pointer;
	DWORD CG_S_XY;
	DWORD CG_WeaponsArray;
	DWORD EntitiesPointer;
	DWORD FOVStruct;
	DWORD LocalClientPointer;
	DWORD NetSpeedOffset;
	DWORD NoFlinchOffset;
	DWORD NoRecoilOffset;
	DWORD NoSwayOffset1, NoSwayOffset2;
	DWORD LevelOffset;
	DWORD OurXuidOffset;
	DWORD OverheadNamesFriendlyOffset, OverheadNamesEnemyOffset;
	DWORD PrestigeOffset;
	DWORD DisableStatWipe;
	DWORD ActiveActionOffset;
	DWORD RadarOffset;
	DWORD ScreenPlacement;
	DWORD ServerIdOffset;
	DWORD ShowPing;
	DWORD ThirdPersonStruct;
	DWORD WallhackOffset;
	DWORD WeaponDefOffset;

	//functions
	DWORD CG_BulletHitEvent;
	DWORD CG_DObjGetWorldTagPos;
	DWORD CG_DrawGrenadePointer;
	DWORD CG_DrawRotatedPicPhysical;
	DWORD CG_EntityEvent;
	DWORD CL_AllLocalClientsDisconnected;
	DWORD CL_ReadyToSendPacket;
	DWORD CL_SendCmd;
	DWORD DrawEngineRadarNOP;
	DWORD DrawFriendliesNOP;
	DWORD DrawEnemiesNOP;
	DWORD DrawVehiclesNOP;
	DWORD DrawPlayerNOP;
	DWORD DrawIndexedPrimitive;
	DWORD DrawIndexedPrimitiveLR;
	DWORD DynEntCl_DynEntImpactEvent;
	DWORD GetTagPosition;
	DWORD GetWeaponIcon;
	DWORD IsWeaponDualWield;
	DWORD Menu_PaintAll;
	DWORD R_DrawIndexedPrimitive;
	DWORD R_EndFrame;
	DWORD RB_SetTessTechnique;
	DWORD XInputGetState;
	DWORD LiveStorage_SetStat;
	DWORD VA;
	DWORD Dvar_GetBool;
	DWORD CG_LocationalTrace;
	DWORD DB_FindXAssetHeader;
	DWORD R_TextWidth;
	DWORD CL_DrawTextPhysical;
	DWORD R_AddCmdDrawStretchPic;
	DWORD CG_DrawRotatedPicPhysicalAddr;
	DWORD UI_FillRectPhysical;
	DWORD Cbuf_AddText;
	DWORD SL_GetString;
	DWORD Com_GetClientDObj;
	DWORD CG_GetWeaponIndexForName;
	DWORD PlayLocalSoundAliasByName;
	DWORD CG_GetEntityBModelBounds;
	DWORD R_NormalizedTextScale;
	DWORD CG_CalcNamePosition;
	DWORD G_GetWeaponHitLocationMultiplier;
	DWORD CL_SetStance;
	DWORD AnglesToAxis;
	DWORD sub_82229BC0;
	DWORD BG_GetSpreadForWeapon;
	DWORD RandomBulletDir;
	DWORD BG_GetWeaponDef;
	DWORD CG_GetPlayerViewOrigin;
	DWORD CalculateRandomSeed;
	DWORD DrawEngineRadar;
	DWORD DrawEnemies;
	DWORD DrawFriendlies;
	DWORD DrawVehicles;
	DWORD DrawPlayer;
	DWORD CG_WorldPosToCompass;
	DWORD CG_CompassCalcDimensions;
	DWORD CG_DrawRotatedPic;
	DWORD CG_CompassUpYawVector;
	DWORD CG_GetCompassSize;
	DWORD sub_8228C068;
	DWORD FireBulletPenetrate;
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
			CoDPatches.NetInfoAddress = 0x8268EEC2;
			CoDPatches.Autowall360 = 0x8213492C;
			CoDPatches.CG_WeaponsArray = 0x825B8524;
			CoDPatches.CG_S_XY = 0x825E3068;
			CoDPatches.OurXuidOffset = 0x838BA820;
			CoDPatches.PrestigeOffset = 0x831A0DD4;
			CoDPatches.DisableStatWipe = 0x8222AE8C;
			CoDPatches.ActiveActionOffset = 0x825E21AC;
			CoDPatches.RadarOffset = 0x820EF0C4;
			CoDPatches.NoRecoilOffset = 0x82135BFC;
			CoDPatches.NoSwayOffset1 = 0x820E5B38;
			CoDPatches.NoSwayOffset2 = 0x820E657C;
			CoDPatches.ShowPing = 0x82116CBC;
			CoDPatches.NetSpeedOffset = 0x838BBB10;
			CoDPatches.FOVStruct = 0x825BE8C8;
			CoDPatches.ServerIdOffset = 0x826237E0;
			CoDPatches.OverheadNamesEnemyOffset = 0x820F30A0;
			CoDPatches.OverheadNamesFriendlyOffset = 0x820F32C8;
			CoDPatches.LevelOffset = 0x831A0DCC;
			CoDPatches.NoFlinchOffset = 0x821131A4;
			CoDPatches.CG_Pointer = 0x825B8638;
			CoDPatches.LocalClientPointer = 0x82682354;
			CoDPatches.EntitiesPointer = 0x825BE150;
			CoDPatches.CGS_Pointer = 0x825B861C;
			
			//functions
			CoDPatches.CG_DrawRotatedPicPhysical = 0x82690F48;
			CoDPatches.GetWeaponIcon = 0x820E22D8;
			CoDPatches.GetTagPosition = 0x820C5830;
			CoDPatches.CG_DrawGrenadePointer = 0x820F17F0;
			CoDPatches.IsWeaponDualWield = 0x820DAF58;
			CoDPatches.DynEntCl_DynEntImpactEvent = 0x821A4A58;
			CoDPatches.CG_BulletHitEvent = 0x82134510;
			CoDPatches.DrawEngineRadarNOP = 0x821102A8;
			CoDPatches.DrawFriendliesNOP = 0x8211036C;
			CoDPatches.DrawEnemiesNOP = 0x82110388;
			CoDPatches.DrawVehiclesNOP = 0x821103DC;
			CoDPatches.DrawPlayerNOP = 0x821103A8;
			CoDPatches.Menu_PaintAll = 0x82285F08;
			CoDPatches.XInputGetState = 0x82331DF8;
			CoDPatches.CL_SendCmd = 0x82140728;
			CoDPatches.R_EndFrame = 0x82351728;
			CoDPatches.CG_EntityEvent = 0x820FD778;
			CoDPatches.RB_SetTessTechnique = 0x82373AF8;
			CoDPatches.LiveStorage_SetStat = 0x8211D2A8;
			CoDPatches.VA = 0x822A7008;
			CoDPatches.Dvar_GetBool = 0x8229EF58;
			CoDPatches.CG_LocationalTrace = 0x82136E88;
			CoDPatches.DB_FindXAssetHeader = 0x8219BBD8;
			CoDPatches.R_TextWidth = 0x8234DD00;
			CoDPatches.CL_DrawTextPhysical = 0x82350258;
			CoDPatches.R_AddCmdDrawStretchPic = 0x8234F998;
			CoDPatches.CG_DrawRotatedPicPhysicalAddr = 0x820F7900;
			CoDPatches.UI_FillRectPhysical = 0x82262308;
			CoDPatches.Cbuf_AddText = 0x82224A28;
			CoDPatches.SL_GetString = 0x822422D8;
			CoDPatches.Com_GetClientDObj = 0x8222FD58;
			CoDPatches.CG_GetWeaponIndexForName = 0x820E2308;
			CoDPatches.PlayLocalSoundAliasByName = 0x8232B640;
			CoDPatches.CG_GetEntityBModelBounds = 0x82135D10;
			CoDPatches.R_NormalizedTextScale = 0x8234DCA0;
			CoDPatches.CG_CalcNamePosition = 0x820F2348;
			CoDPatches.G_GetWeaponHitLocationMultiplier = 0x821DD100;
			CoDPatches.CL_SetStance = 0x8213E640;
			CoDPatches.AnglesToAxis = 0x8228EEF8;
			CoDPatches.sub_82229BC0 = 0x82229BC0;
			CoDPatches.BG_GetSpreadForWeapon = 0x820E33F0;
			CoDPatches.RandomBulletDir = 0x8212EF78;
			CoDPatches.BG_GetWeaponDef = 0x820E22C0;
			CoDPatches.CG_GetPlayerViewOrigin = 0x8212E670;
			CoDPatches.CalculateRandomSeed = 0x820D94B8;
			CoDPatches.DrawEngineRadar = 0x820EC0C8;
			CoDPatches.DrawEnemies = 0x820EEFA8;
			CoDPatches.DrawFriendlies = 0x820EE938;
			CoDPatches.DrawVehicles = 0x820EFEA0;
			CoDPatches.DrawPlayer = 0x820EC7A0;
			CoDPatches.CG_WorldPosToCompass = 0x820EB0A0;
			CoDPatches.CG_CompassCalcDimensions = 0x820EB278;
			CoDPatches.CG_DrawRotatedPic = 0x820F7AB0;
			CoDPatches.CG_CompassUpYawVector = 0x820ECD00;
			CoDPatches.CG_GetCompassSize = 0x820ED420;
			CoDPatches.sub_8228C068 = 0x8228C068;
			CoDPatches.FireBulletPenetrate = 0x82134648;
			CoDPatches.DrawIndexedPrimitive = 0x82388028;
			CoDPatches.R_DrawIndexedPrimitive = 0x820AF1D0;
			CoDPatches.DrawIndexedPrimitiveLR = 0x82398344;

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
			binToFile(file != NULL ? file : "XBLIVE:\\MW2.bin", dumpBuf, sizeof(CoDPatches_s));

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