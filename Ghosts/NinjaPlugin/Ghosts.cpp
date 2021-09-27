#include "stdafx.h"
#include "Utilities.h"
#include "Config.h"
#include <time.h>
#include "Globals.h"
#include "Ghosts.h"
#include "Detour.h"
#include <d3d9.h>


BOOL DPads[4];
DWORD scroll = 0;
DWORD subScroll = 0;
BOOL AButton;
BOOL BButton;
BOOL menuOpen;
BOOL menuWait;
BOOL subMenu;
BOOL clientMenu;
DWORD selectedClient;
CONST DWORD MenuOptions = 7;
DWORD SubMenuOptions[MenuOptions];

DWORD OptionEnabled[18][MenuOptions];
DWORD ClientOptionEnabled[10][18];
PCHAR OptionStrings[18][MenuOptions];
PCHAR* OptionValues[18][MenuOptions];
int OptionArraySize[18][MenuOptions];
int OptionArrayCurrent[18][MenuOptions];

PCHAR SubMenuNames[18][MenuOptions];

pclientActive_t_Ghosts LocalClient_Ghosts;
pcentity_s_Ghosts Entities_Ghosts;
pcgs_t_Ghosts cgs_Ghosts;
pcg_s_Ghosts cg_Ghosts;
prefdef_s_Ghosts refdef_Ghosts;

ClientInformation_Ghosts PlayerInfo[18];

//__declspec(dllexport) BYTE freememory[0x1000];
Detour<int> *Menu_PaintAllDetour;
Detour<DWORD> *XInputGetStateDetour;
Detour<int> *CL_SendCmdDetour;
Detour<void> *CL_CreateNewCommandsDetour;
Detour<int> *PlayLocalSoundAliasByNameDetour;
Detour<void> *R_EndFrameDetour;
Detour<DWORD> *NetDll_XNetGetTitleXnAddrDetour;
Detour<void> *CG_EntityEventDetour;
Detour<void> *R_DrawXModelSkinnedCachedDetour;
Detour<HRESULT> *R_DrawIndexedPrimitiveDetour;
Detour<HRESULT> *DrawIndexedPrimitiveDetour;
Detour<void> *R_AddCmdDrawTextDetour;
Detour<void> *LiveStats_WriteStatDetour;
Detour<void> *EngineDrawRadarDetour;
BOOL menuReady = FALSE;
BOOL DroppedAllTargets = FALSE;
BOOL SpinbotFlip = FALSE;
BOOL SpinbotFlipX = FALSE;
BOOL WasReloading = FALSE;
BOOL YButtonPressed = FALSE;
BOOL FoundTarget = FALSE;
BOOL SetXUIDOnce = FALSE;
BOOL FlipRapidFire = FALSE;
BOOL ShouldSendPacket = TRUE;
BOOL LobbyInit = FALSE;
BOOL FiringDualWield = FALSE;
float rotation = 0;
float dx, dy, dz;
float SpinbotSpin = 5;
float SpinbotSpinFlip = 0;
float SpinbotSpinZ = 5;
float SpinbotSpinZR = 5;
int ClientName = 0;
int NameStealDelay = 0; 
int RainbowGTDelay = 0;
int PrestigeDiscoDelay = 0;
int UpdateIPLogDelay = 0;
int ClosestClient;
int FakeLagCount = 0;
int PacketSkipAmount = 26;
int HudSmallFont;
int BigDevFont;
int SmallFont;
int WhiteMaterial;
int ScavMaterial;
int CompassPingMaterial;
int GradientFadeIn;
int yyCount = 0;
int BlacklistCtr = 0;
int RapidFireCtr = 0;
int TriggerValue = 0;
int GClosestClient = -1;
int CrouchCounter = 0;
bool Crouching = false;
int OurHealth = 0;
short BoneIndexs[21];

bool flip;
bool flip2;

char RandomXUID[0xC];
char OriginalXUID[0xE];

wchar_t wcOldGT[0x30];
char OriginalGT[0x1F];

float menuWidth = 40;
float menuHeight = 40;

D3DTexture* OrangeTexture = NULL;
D3DTexture* LightBlueTexture = NULL;
MemoryBuffer* IPBuffer;
DWORD GReactionDelay = 0;
PCHAR AimTag;

CRectDef lastCompass;
DWORD LastCompassType = 0;
DWORD DrawCompassTimer = 0;
DWORD screenMaxX = NULL, screenMaxY = NULL;

CONST PCHAR GetMenuOptionString(eMenuNames SubMenu, PCHAR Name)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to get menu option string\n");
		return "null";
	}

	for (int i = 0; i < SubMenuOptions[Tabindex]; i++)
	{
		if (!strcmp(Name, SubMenuNames[i][Tabindex]) && (OptionValues[i][Tabindex] != NULL))
		{
			return OptionValues[i][Tabindex][OptionArrayCurrent[i][Tabindex]];
		}

		if (!strcmp(Name, SubMenuNames[i][Tabindex]))
		{
			return OptionStrings[i][Tabindex];
		}
	}
	return "null";
}

CONST PCHAR GetMenuOptionString(eMenuNames SubMenu, eMenuOptions Name)
{
	return GetMenuOptionString(SubMenu, MenuOptionNames[Name]);
}

int GetMenuOptionArrayIndex(eMenuNames SubMenu, eMenuOptions Name)
{
	if (!menuReady) return 0;
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to get menu option array index\n");
		return 0;
	}

	for (int i = 0; i < SubMenuOptions[Tabindex]; i++)
	{
		if (!strcmp(MenuOptionNames[Name], SubMenuNames[i][Tabindex]))
		{
			return OptionArrayCurrent[i][Tabindex];
		}
	}
	return 0;
}

int GetMenuOptionValue(eMenuNames SubMenu, eMenuOptions Name)
{
	if (!menuReady) return 0;
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to get menu option value\n");
		return 0;
	}

	for (int i = 0; i < SubMenuOptions[Tabindex]; i++)
	{
		if (!strcmp(MenuOptionNames[Name], SubMenuNames[i][Tabindex]))
		{
			return OptionEnabled[i][Tabindex];
		}
	}
	return 0;
}

void SetMenuOptionValue(eMenuNames SubMenu, eMenuOptions Name, DWORD ValueToSet)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to set menu option value\n");
		return;
	}

	for (int i = 0; i < SubMenuOptions[Tabindex]; i++)
	{
		if (!strcmp(MenuOptionNames[Name], SubMenuNames[i][Tabindex]))
		{
			OptionEnabled[i][Tabindex] = ValueToSet;
		}
	}
	return;
}

void SetMenuOptionString(eMenuNames SubMenu, eMenuOptions Name, DWORD ArrayNumber)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to set menu option value\n");
		return;
	}

	for (int i = 0; i < SubMenuOptions[Tabindex]; i++)
	{
		if (!strcmp(MenuOptionNames[Name], SubMenuNames[i][Tabindex]))
		{
			if (!strcmp(MenuOptionNames[Name], SubMenuNames[i][Tabindex]) && (OptionValues[i][Tabindex] != NULL))
			{
				OptionArrayCurrent[i][Tabindex] = ArrayNumber;
			}
		}
	}
	return;
}

void AddMenuOption(eMenuNames SubMenu, eMenuOptions Name, int index, BOOL defaultValue = FALSE, CONST PCHAR defaultString = "false", char** Array = NULL, int ArraySize = NULL, int ArrayCur = 0)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[SubMenu], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to add menu option\n");
		return;
	}

	OptionStrings[index][Tabindex] = defaultString;
	OptionEnabled[index][Tabindex] = defaultValue;
	OptionValues[index][Tabindex] = Array;
	OptionArraySize[index][Tabindex] = ArraySize;
	OptionArrayCurrent[index][Tabindex] = ArrayCur;
	SubMenuNames[index][Tabindex] = MenuOptionNames[Name];
	SubMenuOptions[Tabindex] += 1;

}

int GetNetInfoAddress(int index)
{
	return (getCryptedCoD(CheatPatches->CoDPatches.NetInfoAddress) + (index * 0x1C0));
}

PCHAR GetNameFromNetInfo(int index)
{
	return (char*)(GetNetInfoAddress(index));
}

BYTE* GetIPFromNetInfo(int index)
{
	return (BYTE*)(GetNetInfoAddress(index) + 0xE8); 
}

QWORD GetXUIDFromNetInfo(int index)
{
	return (GetNetInfoAddress(index) + 0x134);
}

QWORD GetMachineIDFromNetInfo(int index)
{
	return _byteswap_uint64(*(QWORD*)(GetNetInfoAddress(index) + 0xFC)); 
}

BOOL CheckInGame()
{
	return Dvar_GetBool_Ghosts("cl_ingame") && !((bool(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CL_AllLocalClientsDisconnected))();
}

void UpdateClientsNames()
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(MenuNames[MENU_CLIENTS], MenuNames[i]))
		{
			Tabindex = i;
			break;
		}
	}

	if (Tabindex == -1)
	{
		DbgPrint("Failed to update client names\n");
		return;
	}

	for (int iClients = 0; iClients < 18; iClients++)
	{
		char* Name = GetNameFromNetInfo(iClients);
		BYTE* PlayerXUID = (BYTE*)GetXUIDFromNetInfo(iClients);
		if (strcmp(Name, ""))
		{
			if (PlayerXUID[0] == 0x0 && PlayerXUID[1] == 0x09 && Name[0] != 0)
				SubMenuNames[iClients][Tabindex] = Name;
			else
				SubMenuNames[iClients][Tabindex] = MenuOptionNames[OPTION_NAMEDEFAULT];
		}
		else
			SubMenuNames[iClients][Tabindex] = MenuOptionNames[OPTION_NAMEDEFAULT];
	}
}

void GenerateRandomXUIDCharacter()
{
	char Characters[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

	for (int i = 0; i < 0xC; i++)
	{
		DWORD dwRandom = rand() % 9;

		RandomXUID[i] = Characters[dwRandom];
	}
}

void SetupMenuText()
{
	// NOTE: These menu names MUST be initialized in the same order as the enum corresponding to the MenuOptionNames/MenuNames vectors. 
	// Check against eMenuNames and eMenuOptions for the correct order!

	//Menu Names
	MenuNames.push_back(E_ALLOC("Main"));
	MenuNames.push_back(E_ALLOC("Ragebot"));
	MenuNames.push_back(E_ALLOC("Legitbot"));
	MenuNames.push_back(E_ALLOC("Spinbot"));
	MenuNames.push_back(E_ALLOC("Drawing"));
	MenuNames.push_back(E_ALLOC("Clients"));
	MenuNames.push_back(E_ALLOC("Settings"));

	//Main
	MenuOptionNames.push_back(E_ALLOC("Radar"));
	MenuOptionNames.push_back(E_ALLOC("No Recoil"));
	MenuOptionNames.push_back(E_ALLOC("No Sway"));
	MenuOptionNames.push_back(E_ALLOC("Change Team?"));
	MenuOptionNames.push_back(E_ALLOC("Steal GT's"));
	MenuOptionNames.push_back(E_ALLOC("Rainbow GT"));
	MenuOptionNames.push_back(E_ALLOC("Spoof GT?"));
	MenuOptionNames.push_back(E_ALLOC("Show Host"));
	MenuOptionNames.push_back(E_ALLOC("Show Ping"));
	MenuOptionNames.push_back(E_ALLOC("Fast Reload"));
	MenuOptionNames.push_back(E_ALLOC("Everyone Hears Me"));
	MenuOptionNames.push_back(E_ALLOC("Never Host"));
	MenuOptionNames.push_back(E_ALLOC("Field Of View:"));
	MenuOptionNames.push_back(E_ALLOC("Third Person"));
	MenuOptionNames.push_back(E_ALLOC("Fake Lag Amount:"));
	MenuOptionNames.push_back(E_ALLOC("Jugg Class?"));
	MenuOptionNames.push_back(E_ALLOC("End Game?"));
	MenuOptionNames.push_back(E_ALLOC("Leave Game?"));

	//Ragebot
	MenuOptionNames.push_back(E_ALLOC("Enable Ragebot"));
	MenuOptionNames.push_back(E_ALLOC("Ragebot Type:"));
	MenuOptionNames.push_back(E_ALLOC("Aim Bone:"));
	MenuOptionNames.push_back(E_ALLOC("Prediction Scale:"));
	MenuOptionNames.push_back(E_ALLOC("No Spread"));
	MenuOptionNames.push_back(E_ALLOC("No Flinch"));
	MenuOptionNames.push_back(E_ALLOC("Auto Wall"));
	MenuOptionNames.push_back(E_ALLOC("Auto Shoot"));
	MenuOptionNames.push_back(E_ALLOC("Auto Aim"));
	MenuOptionNames.push_back(E_ALLOC("Rapid Fire"));
	MenuOptionNames.push_back(E_ALLOC("Target Shields"));
	MenuOptionNames.push_back(E_ALLOC("Preferred Settings"));

	//Legitbot
	MenuOptionNames.push_back(E_ALLOC("Enable Legitbot"));
	MenuOptionNames.push_back(E_ALLOC("Aim Button:"));
	MenuOptionNames.push_back(E_ALLOC("Reaction Delay:"));
	MenuOptionNames.push_back(E_ALLOC("FOV Size:"));
	MenuOptionNames.push_back(E_ALLOC("Draw FOV Box"));
	MenuOptionNames.push_back(E_ALLOC("Smooth Scale:"));
	MenuOptionNames.push_back(E_ALLOC("Triggerbot"));

	//Spinbot
	MenuOptionNames.push_back(E_ALLOC("Enable Spinbot"));
	MenuOptionNames.push_back(E_ALLOC("X Spin Direction:"));
	MenuOptionNames.push_back(E_ALLOC("Y Spin Direction:"));
	MenuOptionNames.push_back(E_ALLOC("Y Spin Speed:"));
	MenuOptionNames.push_back(E_ALLOC("Z Spin Direction:"));
	MenuOptionNames.push_back(E_ALLOC("Z Spin Speed:"));
	MenuOptionNames.push_back(E_ALLOC("X Anti Aim Spinbot:"));
	MenuOptionNames.push_back(E_ALLOC("Y Anti Aim Spinbot:"));

	//Drawing
	MenuOptionNames.push_back(E_ALLOC("Draw Enemies"));
	MenuOptionNames.push_back(E_ALLOC("Draw Friendlies"));
	MenuOptionNames.push_back(E_ALLOC("ESP Box Type:"));
	MenuOptionNames.push_back(E_ALLOC("Draw Name"));
	MenuOptionNames.push_back(E_ALLOC("Draw Weapon"));
	MenuOptionNames.push_back(E_ALLOC("Draw Bones"));
	MenuOptionNames.push_back(E_ALLOC("Draw Explosives"));
	MenuOptionNames.push_back(E_ALLOC("Draw Snaplines:"));
	MenuOptionNames.push_back(E_ALLOC("Draw Killstreaks"));
	MenuOptionNames.push_back(E_ALLOC("Directional Arrows"));
	MenuOptionNames.push_back(E_ALLOC("Crosshair Type:"));
	MenuOptionNames.push_back(E_ALLOC("Draw Custom Radar"));
	MenuOptionNames.push_back(E_ALLOC("Draw Dropped Items"));
	MenuOptionNames.push_back(E_ALLOC("Draw Enemy Visible Text"));
	MenuOptionNames.push_back(E_ALLOC("Draw Proximity Warning"));
	MenuOptionNames.push_back(E_ALLOC("Cartoon Mode"));

	//Clients
	MenuOptionNames.push_back(E_ALLOC("Searching..."));

	//Settings
	MenuOptionNames.push_back(E_ALLOC("Disable Menu Sounds"));
	MenuOptionNames.push_back(E_ALLOC("Disable Menu Instructions"));
	MenuOptionNames.push_back(E_ALLOC("Disable NiNJA Engine Text"));
	MenuOptionNames.push_back(E_ALLOC("Disable Main Menu Music"));
	MenuOptionNames.push_back(E_ALLOC("Prestige Disco"));
	MenuOptionNames.push_back(E_ALLOC("Prestige:"));
	MenuOptionNames.push_back(E_ALLOC("Level 60?"));
	MenuOptionNames.push_back(E_ALLOC("1,000 Squad Points?"));
	MenuOptionNames.push_back(E_ALLOC("Unlock All?"));
	MenuOptionNames.push_back(E_ALLOC("Anti Derank"));
	MenuOptionNames.push_back(E_ALLOC("Invert Team Check"));
	MenuOptionNames.push_back(E_ALLOC("Log IP's"));
}

void SetupMenu()
{
	SetupMenuText();

	AddMenuOption(MENU_MAIN, OPTION_RADAR, 0, bRadar);
	AddMenuOption(MENU_MAIN, OPTION_NORECOIL, 1, bNoRecoil);
	AddMenuOption(MENU_MAIN, OPTION_NOSWAY, 2, bNoSway);
	//AddMenuOption(MENU_MAIN, "Change Team?", 3);
	//AddMenuOption(MENU_MAIN, "Change Class", 4);
	AddMenuOption(MENU_MAIN, OPTION_STEALGTS, 3, bStealGTs);
	AddMenuOption(MENU_MAIN, OPTION_RAINBOWGT, 4, bRainbowGT);
	AddMenuOption(MENU_MAIN, OPTION_SPOOFGT, 5);
	AddMenuOption(MENU_MAIN, OPTION_SHOWHOST, 6, bShowHost);
	AddMenuOption(MENU_MAIN, OPTION_SHOWPING, 7, bShowPing);
	AddMenuOption(MENU_MAIN, OPTION_FASTRELOAD, 8, bFastReload);
	AddMenuOption(MENU_MAIN, OPTION_EVERYONEHEARSME, 9, bEveryoneHearsMe);
	AddMenuOption(MENU_MAIN, OPTION_NEVERHOST, 10, bNeverHost);
	AddMenuOption(MENU_MAIN, OPTION_FOV, 11, iFieldOfViewIndex, "Normal", FOVS, FOVS_Size, FOVSizes_Start);
	AddMenuOption(MENU_MAIN, OPTION_THIRDPERSON, 12, bThirdPerson);
	AddMenuOption(MENU_MAIN, OPTION_FAKELAGAMOUNT, 13, iFakeLagAmount, "Off", FakeLagTypes, FakeLagTypes_Size, FakeLagTypes_Start);
	//AddMenuOption(MENU_MAIN, "Jugg Class?", 14);
	AddMenuOption(MENU_MAIN, OPTION_ENDGAME, 14);
	AddMenuOption(MENU_MAIN, OPTION_LEAVEGAME, 15);


	AddMenuOption(MENU_RAGEBOT, OPTION_ENABLERAGEBOT, 0, bRagebot);
	AddMenuOption(MENU_RAGEBOT, OPTION_RAGEBOTTYPE, 1, iRagebotType, "Silent", RagebotTypes, RagebotTypes_Size, RagebotTypes_Start);
	//AddMenuOption(MENU_RAGEBOT, "Ragebot Logic:", 2, 0, "Distance", RagebotLogics, RagebotLogics_Size, RagebotLogics_Start);
	AddMenuOption(MENU_RAGEBOT, OPTION_AIMBONE, 2, iRBAimBone, "Best", AimBones, AimBones_Size, AimBones_Start);
	AddMenuOption(MENU_RAGEBOT, OPTION_PREDICTION, 3, iRBPrediction, "Off", PredictionScales, PredictionScales_Size, PredictionScales_Start);
	AddMenuOption(MENU_RAGEBOT, OPTION_NOSPREAD, 4, bNoSpread);
	AddMenuOption(MENU_RAGEBOT, OPTION_NOFLINCH, 5, bNoFlinch);
	AddMenuOption(MENU_RAGEBOT, OPTION_AUTOWALL, 6, bAutoWall);
	AddMenuOption(MENU_RAGEBOT, OPTION_AUTOSHOOT, 7, bAutoShoot);
	AddMenuOption(MENU_RAGEBOT, OPTION_AUTOAIM, 8, bAutoAim);
	AddMenuOption(MENU_RAGEBOT, OPTION_RAPIDFIRE, 9, bRapidFire);
	AddMenuOption(MENU_RAGEBOT, OPTION_TARGETSHIELDS, 10, bTargetShields);
	//AddMenuOption(MENU_RAGEBOT, "Anti-Blacklist", 11);
	AddMenuOption(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS, 11, bRagebotPreferred);



	AddMenuOption(MENU_LEGITBOT, OPTION_ENABLELEGITBOT, 0, bLegitbot);
	//AddMenuOption(MENU_LEGITBOT, "Legitbot Logic:", 1, 0, "Distance");
	AddMenuOption(MENU_LEGITBOT, OPTION_AIMBONE, 1, iLBAimBone, "j_head", AimBonesLegit, AimBonesLegit_Size, AimBonesLegit_Start);
	AddMenuOption(MENU_LEGITBOT, OPTION_AIMKEY, 2, iLBAimButton, "Off", AimKeys, AimKeys_Size, AimKeys_Start);
	AddMenuOption(MENU_LEGITBOT, OPTION_AUTOSHOOT, 3, bLBAutoShoot);
	AddMenuOption(MENU_LEGITBOT, OPTION_AUTOAIM, 4, bLBAutoAim);
	AddMenuOption(MENU_LEGITBOT, OPTION_REACTIONDELAY, 5, iReactionDelay, "Off", ReactionDelays, ReactionDelays_Size, ReactionDelays_Start);
	AddMenuOption(MENU_LEGITBOT, OPTION_FOVSIZE, 6, iFOVSize, "80.0", FOVSizes, FOVSizes_Size, FOVSizes_Start);
	AddMenuOption(MENU_LEGITBOT, OPTION_DRAWFOVBOX, 7, bDrawFOVBox);
	AddMenuOption(MENU_LEGITBOT, OPTION_SMOOTHSCALE, 8, iSmoothScale, "10.0", SmoothScales, SmoothScales_Size, SmoothScales_Start);
	AddMenuOption(MENU_LEGITBOT, OPTION_TRIGGERBOT, 9, bTriggerbot);
	AddMenuOption(MENU_LEGITBOT, OPTION_RAPIDFIRE, 10, bLBRapidFire);
	AddMenuOption(MENU_LEGITBOT, OPTION_TARGETSHIELDS, 11, bLBTargetShields);
	AddMenuOption(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS, 12);


	AddMenuOption(MENU_SPINBOT, OPTION_ENABLESPINBOT, 0, bSpinbot);
	AddMenuOption(MENU_SPINBOT, OPTION_XSPINDIRECTION, 1, XSpinDirects_Start, "Off", XSpinDirects, XSpinDirects_Size, iXSpinDirection);
	AddMenuOption(MENU_SPINBOT, OPTION_YSPINDIRECTION, 2, YSpinDirects_Start, "Off", YSpinDirects, YSpinDirects_Size, iYSpinDirection);
	AddMenuOption(MENU_SPINBOT, OPTION_YSPINSPEED, 3, YSpinSpeeds_Start, "10.0", YSpinSpeeds, YSpinSpeeds_Size, iYSpinSpeed);
	AddMenuOption(MENU_SPINBOT, OPTION_ZSPINDIRECTION, 4, 0, "Off", ZSpinDirects, ZSpinDirects_Size, ZSpinDirects_Start);
	AddMenuOption(MENU_SPINBOT, OPTION_ZSPINSPEED, 5, 0, "10.0", ZSpinSpeeds, ZSpinSpeeds_Size, ZSpinSpeeds_Start);
	//AddMenuOption(MENU_SPINBOT, "Funny Spinbots:", 4, 0, "Off", FunnySpinbots, FunnySpinbots_Size, FunnySpinbots_Start);
	AddMenuOption(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 6, XAASpinbots_Start, "Off", XAASpinbots, XAASpinbots_Size, iXAntiAimSpinbot);
	AddMenuOption(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 7, YAASpinbots_Start, "Off", YAASpinbots, YAASpinbots_Size, iYAntiAimSpinbot);
	AddMenuOption(MENU_SPINBOT, OPTION_PREFERREDSETTINGS, 8, bSpinbotPreferred);


	AddMenuOption(MENU_DRAWING, OPTION_DRAWENEMIES, 0, bDrawEnemies);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWFRIENDLIES, 1, bDrawFriendlies);
	AddMenuOption(MENU_DRAWING, OPTION_ESPBOXTYPE, 2, ESPBoxes_Start, "Off", ESPBoxes, ESPBoxes_Size, iESPBoxType);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWNAME, 3, bDrawName);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWWEAPON, 4, bDrawWeapon);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWBONES, 5, bDrawBones);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWEXPLOSIVES, 6, bDrawExplosives);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWSNAPLINES, 7, SnaplinesTypes_Start, "Off", SnaplinesTypes, SnaplinesTypes_Size, iSnaplines);
	//AddMenuOption(MENU_DRAWING, "Draw Killstreaks", 9);
	AddMenuOption(MENU_DRAWING, OPTION_DIRECTIONALARROWS, 8, bDirectionalArrows);
	AddMenuOption(MENU_DRAWING, OPTION_CROSSHAIRTYPE, 9, CrosshairTypess_Start, "Off", CrosshairTypes, CrosshairTypes_Size, iCrosshairType);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR, 10, bDrawCustomRadar);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS, 11, bDrawDroppedItems);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWENEMYVISIBLE, 12, bDrawEnemyVis);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWPROXIMITYWARNING, 13, bDrawProximityWrn);
	AddMenuOption(MENU_DRAWING, OPTION_CARTOONMODE, 14, bCartoonMode);


	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 0);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 1);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 2);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 3);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 4);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 5);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 6);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 7);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 8);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 9);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 10);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 11);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 12);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 13);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 14);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 15);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 16);
	AddMenuOption(MENU_CLIENTS, OPTION_NAMEDEFAULT, 17);


	//AddMenuOption(MENU_SETTINGS, "Move Menu?", 0);
	//AddMenuOption(MENU_SETTINGS, "Menu Open:", 1, 0, "\x0016 & \x0012", MenuControls, MenuControls_Size, MenuControls_Start);
	AddMenuOption(MENU_SETTINGS, OPTION_DISABLEMENUSOUNDS, 0, bDisableMenuSounds);
	AddMenuOption(MENU_SETTINGS, OPTION_DISABLEMENUINSTRUCTIONS, 1, bDisableMenuInstructions);
	AddMenuOption(MENU_SETTINGS, OPTION_DISABLENINJATEXT, 2, bDisableNiNJAEngineText);
	//AddMenuOption(MENU_SETTINGS, "Disable Item Descriptions", 5);
	//AddMenuOption(MENU_SETTINGS, "Disable Main Menu Music", 3);
	AddMenuOption(MENU_SETTINGS, OPTION_PRESTIGEDISCO, 3, bPrestigeDisco);
	AddMenuOption(MENU_SETTINGS, OPTION_PRESTIGE, 4, 0, "N/A", Prestiges, Prestiges_Size, Prestiges_Start);
	AddMenuOption(MENU_SETTINGS, OPTION_LEVEL60, 5);
	AddMenuOption(MENU_SETTINGS, OPTION_SQUADPOINTS, 6);
	AddMenuOption(MENU_SETTINGS, OPTION_UNLOCKALL, 7);
	AddMenuOption(MENU_SETTINGS, OPTION_ANTIDERANK, 8);
	//AddMenuOption(MENU_SETTINGS, "Set Legit Stats?", 11);
	//AddMenuOption(MENU_SETTINGS, "Invert Team Check", 9);
	AddMenuOption(MENU_SETTINGS, OPTION_LOGIPS, 9);
	//AddMenuOption(MENU_SETTINGS, "Freeze All Consoles?", 9);

	HudSmallFont = DB_FindXAssetHeader_Ghosts(font_Ghosts, "fonts/hudsmallfont", 1);
	BigDevFont = DB_FindXAssetHeader_Ghosts(font_Ghosts, "fonts/bigDevFont", 1);
	SmallFont = DB_FindXAssetHeader_Ghosts(font_Ghosts, "fonts/smallfont", 1);
	WhiteMaterial = DB_FindXAssetHeader_Ghosts(material_Ghosts, "white", 1);
	ScavMaterial = DB_FindXAssetHeader_Ghosts(material_Ghosts, "scavenger_pickup", 1);
	CompassPingMaterial = DB_FindXAssetHeader_Ghosts(material_Ghosts, "compassping_player", 1);

	memcpy(OriginalGT, (PVOID)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1E);
	memcpy(OriginalXUID, (PVOID)(0x838BA853), 0xE);

	menuReady = TRUE;
}

void PlayMenuSound(char* SoundName)
{
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUSOUNDS))
		return;

	PlayLocalSoundAliasByName(0, SoundName, 0);
}

void MonitorButtons()
{
	XINPUT_STATE currentState;
	XInputGetState(0, &currentState);

	if ((currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) && DPads[0] == FALSE && menuOpen)
	{
		DPads[0] = TRUE;
		if (subMenu == FALSE)
			scroll--;
		else
			subScroll--;

		PlayMenuSound("ui_hilite");
	}
	if (!(currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) && DPads[0] == TRUE)
	{
		DPads[0] = FALSE;
	}

	if ((currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && DPads[1] == FALSE && menuOpen)
	{
		DPads[1] = TRUE;
		if (subMenu == FALSE)
			scroll++;
		else
			subScroll++;

		PlayMenuSound("ui_hilite");
	}
	if (!(currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && DPads[1] == TRUE)
	{
		DPads[1] = FALSE;
	}

	if ((currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) && (currentState.Gamepad.bLeftTrigger) && menuWait == FALSE)
	{
		menuWait = TRUE;
		menuOpen ^= 1;
		if (menuOpen == 0)
			shouldSaveMenuOptions = true;
		PlayMenuSound("ui_hilite");
	}
	else if (!(currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) && !(currentState.Gamepad.bLeftTrigger) && menuWait == TRUE)
	{
		menuWait = FALSE;
	}

	if (menuOpen && (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT))
	{
		subMenu = TRUE;
		subScroll = 0;
	}

	if (menuOpen && (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT))
	{
		subMenu = FALSE;
		subScroll = 0;
		clientMenu = FALSE;
	}


	if (menuOpen && subMenu && (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && AButton == FALSE)
	{
		AButton = TRUE;
		if(!clientMenu)
			OptionEnabled[subScroll][scroll] ^= 1;
		if(clientMenu)
			ClientOptionEnabled[subScroll][selectedClient] ^= 1;

		PlayMenuSound("ui_select_confirm");

		if (!clientMenu && OptionValues[subScroll][scroll] != NULL)
		{
			if (OptionArrayCurrent[subScroll][scroll] == (OptionArraySize[subScroll][scroll]) - 1)
				OptionArrayCurrent[subScroll][scroll] = 0;
			else
				OptionArrayCurrent[subScroll][scroll] = OptionArrayCurrent[subScroll][scroll] + 1;
		}
		if(scroll == 5 && subMenu && !clientMenu)
		{
			clientMenu = TRUE;
			selectedClient = subScroll;
			subScroll = 1;
		}
	}
	if (menuOpen && !(currentState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && AButton == TRUE)
	{
		AButton = FALSE;
	}

	if (menuOpen && subMenu && (currentState.Gamepad.wButtons & XINPUT_GAMEPAD_B) && BButton == FALSE)
	{
		BButton = TRUE;

		PlayMenuSound("ui_select_confirm");

		if (!clientMenu && OptionValues[subScroll][scroll] != NULL)
		{
			if (OptionArrayCurrent[subScroll][scroll] == 0)
				OptionArrayCurrent[subScroll][scroll] = (OptionArraySize[subScroll][scroll]) - 1;
			else
				OptionArrayCurrent[subScroll][scroll] = (OptionArrayCurrent[subScroll][scroll]) - 1;
		}
		if(scroll == 5 && subMenu && clientMenu)
		{
			clientMenu = FALSE;
			subScroll = 0;
		}
	}
	if (menuOpen && !(currentState.Gamepad.wButtons & XINPUT_GAMEPAD_B) && BButton == TRUE)
	{
		BButton = FALSE;
	}

	if (scroll == MenuOptions)
	{
		scroll = 0;
	}

	if (scroll == -1)
	{
		scroll = (MenuOptions - 1);
	}

	if (subScroll == SubMenuOptions[scroll])
	{
		subScroll = 0;
	}
	if (subScroll == -1)
	{
		subScroll = (SubMenuOptions[scroll] - 1);
	}

	if (clientMenu)
	{
		if (subScroll < 1)
		{
			subScroll = 8;
		}
		else if (subScroll > 8)
		{
			subScroll = 1;
		}
	}

}

void DrawBox_Ghosts(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color)
{
	R_AddCmdDrawStretchPic_Ghosts(X, Y, W, H, 4, 4, 1, 1, Color, WhiteMaterial);
}

void DrawGradient_Ghosts(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color1, Vector4& Color2, bool horizontal)
{
	float fAdd = (horizontal ? 0 : W / 2 - H / 2);
	R_AddCmdDrawStretchPic_Ghosts(X, Y, W, H, 4, 4, 1, 1, Color2, WhiteMaterial);
	R_AddCmdDrawStretchPic_Ghosts(X + fAdd, Y - fAdd, (horizontal ? W : H), (horizontal ? H : W), 4, 4, 1, 1, Color1, GradientFadeIn);
}

void DrawText_Ghosts(CONST PCHAR Text, FLOAT X, FLOAT Y, DWORD index, FLOAT addy, DWORD Font, Vector4& Color, float fontXSize = 0.7, float fontYSize = 0.7)
{
	CL_DrawTextPhysical_Ghosts(Text, 0x7FFFFFFF, Font, X, Y + (index * addy), fontXSize, fontYSize, 0, Color, 0);
}

bool WorldPosToScreenPos(float* In, Vector2* Out)
{
	Vector3 Position, Transform;

	VectorSubtract(In, cg_Ghosts->refdef.ViewOrigin, Position);

	Transform.x = DotProduct(Position, cg_Ghosts->refdef.ViewAxis[1]);
	Transform.y = DotProduct(Position, cg_Ghosts->refdef.ViewAxis[2]);
	Transform.z = DotProduct(Position, cg_Ghosts->refdef.ViewAxis[0]);

	if (Transform.z < 0.1f)
		return false;

	Vector2 Center; Vector2 OutPut;
	Center.x = cg_Ghosts->refdef.Width * 0.5f; Center.y = cg_Ghosts->refdef.Height * 0.5f;
	OutPut.x = Center.x * (1 - (Transform.x / cg_Ghosts->refdef.Fov.x / Transform.z)); OutPut.y = Center.y * (1 - (Transform.y / cg_Ghosts->refdef.Fov.y / Transform.z));
	*Out = OutPut;
	return true;
}

void DrawLine_Ghosts(FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, FLOAT Width, Vector4& Color)
{
	FLOAT X, Y, Angle, L1, L2, H1;
	H1 = Y2 - Y1;
	L1 = X2 - X1;
	L2 = sqrt(L1 * L1 + H1 * H1);
	X = X1 + ((L1 - L2) / 2);
	Y = Y1 + (H1 / 2);
	Angle = (FLOAT)atan(H1 / L1) * (180 / 3.14159265358979323846);
	CG_DrawRotatedPicPhysical_Ghosts(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), X, Y, L2, Width, Angle, Color, WhiteMaterial);
}

void MakeLines(Vector3 Origin, FLOAT X1, FLOAT Y1, FLOAT Z1, FLOAT X2, FLOAT Y2, FLOAT Z2, Vector4& Color, int Size = 1)
{
	Vector2 Screen1, Screen2;
	Vector3 Origin1(Origin.x + X1, Origin.y + Y1, (Origin.z + Z1) - 4);
	Vector3 Origin2(Origin.x + X2, Origin.y + Y2, (Origin.z + Z2) - 4);
	if (WorldPosToScreenPos((float*)&Origin1, &Screen1) && WorldPosToScreenPos((float*)&Origin2, &Screen2)) {
		DrawLine_Ghosts(Screen1.x, Screen1.y, Screen2.x, Screen2.y, Size, Color);
	}
}

void DrawCheckBox(FLOAT X, FLOAT Y, BOOL Checked = FALSE)
{
	Vector4 Color;
	if (Checked)
		Color = Vector4(0, 1, 0, 1);
	else
		Color = Vector4(1, 0, 0, 1);
	DrawLine_Ghosts(X - 7.5, Y - 7.5, X + 7.5, Y - 7.5, 2, Color);//Top
	DrawLine_Ghosts(X - 7.5, Y - 7.5, X - 7.5, Y + 7.5, 2, Color);//Left Side
	DrawLine_Ghosts(X + 7.5, Y - 7.5, X + 7.5, Y + 7.5, 2, Color);//Right Side
	DrawLine_Ghosts(X - 7.5, Y + 7.5, X + 7.5, Y + 7.5, 2, Color);//Bottom

	if (Checked)
	{
		DrawLine_Ghosts(X - 7.5, Y - 7.5, X + 7.5, Y + 7.5, 1.5, Color);//Check
		DrawLine_Ghosts(X - 7.5, Y + 7.5, X + 7.5, Y - 7.5, 1.5, Color);
	}
}

void DrawMenu_Ghosts()
{
	FLOAT X, Y;
	X = (FLOAT)*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY);//cgs_Ghosts->Width;
	Y = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY) + 0x4);//cgs_Ghosts->Height;
	Vector4 ActiveTextColor = GetColorCode(colorVioletFlower);
	Vector4 InActiveTextColor = Vector4(1, 1, 1, 0.9);

	//FLOAT XFix, YFix, W, H;

	//XFix = *(FLOAT*)((DWORD)freememory);
	//YFix = *(FLOAT*)((DWORD)freememory + 4);
	//W = *(FLOAT*)((DWORD)freememory + 8);
	//H = *(FLOAT*)((DWORD)freememory + 12);
	//int indexTest = *(DWORD*)((DWORD)freememory + 16);

	if (menuWidth != 420)
		menuWidth += 20;

	if (menuHeight != 400 && menuWidth == 420)
		menuHeight += 20;

	//Draws Menu Layout
	DrawBox_Ghosts(X * 0.331, Y * 0.25, menuWidth, menuHeight, Vector4(0, 0, 0, 0.420));
	DrawLine_Ghosts(X * 0.331, Y * 0.25, (X * 0.331) + menuWidth, Y * 0.25, 3.5, Vector4(1, 0, 0, 1));
	DrawLine_Ghosts(X * 0.331, Y * 0.25 + menuHeight, (X * 0.331) + menuWidth, (Y * 0.25) + menuHeight, 3.5, Vector4(1, 0, 0, 1));
	DrawLine_Ghosts((X * 0.331) + 115, (Y * 0.25), (X * 0.331) + 115, (Y * 0.25) + menuHeight, 3.5, Vector4(1, 0, 0, 1));

	// Wait for the animation to be done
	if (menuWidth == 420 && menuHeight == 400)
	{
		//Draw Menu Names
		for (int i = 0; i < MenuOptions; i++)
		{
			DrawText_Ghosts(MenuNames[i], X * 0.3328, Y * 0.2831 - 5.0f, i, 22, HudSmallFont, (scroll == i) ? ActiveTextColor : InActiveTextColor);
			DrawLine_Ghosts(X * 0.331, (Y * 0.28) + (i * 22), (X * 0.331) + 115, (Y * 0.28) + (22 * i), 3.5, Vector4(1, 0, 0, 1));
		}

		//Draw SubOptions
		for (int i = 0; i < SubMenuOptions[scroll]; i++)
		{
			if (subMenu == FALSE)
			{
				DrawText_Ghosts(SubMenuNames[i][scroll], X * 0.425, Y * 0.2861, i, 22, HudSmallFont, InActiveTextColor);
			}
			else if (subMenu == TRUE && clientMenu == FALSE)
			{
				DrawText_Ghosts(SubMenuNames[i][scroll], X * 0.425, Y * 0.2861, i, 22, HudSmallFont, (subScroll == i) ? ActiveTextColor : InActiveTextColor);
			}
			else if (subMenu == TRUE && clientMenu == TRUE && i == 0)
			{
				int Tabindex = -1;
				for (int i = 0; i < MenuOptions; i++)
				{
					if (!strcmp(MenuNames[MENU_CLIENTS], MenuNames[i]))
					{
						Tabindex = i;
						break;
					}
				}

				DrawText_Ghosts(SubMenuNames[selectedClient][Tabindex], X * 0.425, (Y * 0.2861) + (0 * 22), 0, 0, HudSmallFont, (subMenu == TRUE) ? ActiveTextColor : InActiveTextColor);
				DrawText_Ghosts("Prioritize", X * 0.425, (Y * 0.2861), 1, 22, HudSmallFont, (subMenu == TRUE && subScroll == 1) ? ActiveTextColor : InActiveTextColor);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (1 * 22), ClientOptionEnabled[1][selectedClient]);

				DrawText_Ghosts("Whitelist", X * 0.425, (Y * 0.2861), 2, 22, HudSmallFont, (subMenu == TRUE && subScroll == 2) ? ActiveTextColor : InActiveTextColor);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (2 * 22), ClientOptionEnabled[2][selectedClient]);

				DrawText_Ghosts("Invert Team", X * 0.425, (Y * 0.2861), 3, 22, HudSmallFont, (subMenu == TRUE && subScroll == 3) ? ActiveTextColor : InActiveTextColor);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (3 * 22), ClientOptionEnabled[3][selectedClient]);


				DrawText_Ghosts("Set GT To Player's IP?", X * 0.425, (Y * 0.2861), 4, 22, HudSmallFont, (subMenu == TRUE && subScroll == 4) ? ActiveTextColor : InActiveTextColor);

				DrawText_Ghosts("Player Has Anti-Aim", X * 0.425, (Y * 0.2861), 5, 22, HudSmallFont, (subMenu == TRUE && subScroll == 5) ? ActiveTextColor : InActiveTextColor);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (5 * 22), ClientOptionEnabled[5][selectedClient]);

				BYTE* IP = GetIPFromNetInfo(selectedClient);
				DrawText_Ghosts(va_Ghosts("IP: %i.%i.%i.%i", IP[0], IP[1], IP[2], IP[3]), X * 0.425, (Y * 0.2861), 6, 22, HudSmallFont, (subMenu == TRUE && subScroll == 6) ? ActiveTextColor : InActiveTextColor);
				QWORD MachineID = GetMachineIDFromNetInfo(selectedClient);
				DWORD MachineIDPT1 = *(DWORD*)(DWORD)&MachineID;
				DWORD MachineIDPT2 = *(DWORD*)((DWORD)&MachineID + 0x04);
				DrawText_Ghosts(va_Ghosts("Machine ID: %08X%08X", MachineIDPT1, MachineIDPT2), X * 0.425, Y * 0.2861, 7, 22, HudSmallFont, (subMenu == TRUE && subScroll == 7) ? ActiveTextColor : InActiveTextColor);
				BYTE* XUID = (BYTE*)GetXUIDFromNetInfo(selectedClient);

				DrawText_Ghosts(va_Ghosts("XUID: %08X%08X", *(DWORD*)(DWORD)&XUID[0], *(DWORD*)(DWORD)&XUID[4]), X * 0.425, Y * 0.2861, 8, 22, HudSmallFont, (subMenu == TRUE && subScroll == 8) ? ActiveTextColor : InActiveTextColor);

				IN_ADDR ip = *(IN_ADDR*)GetIPFromNetInfo(selectedClient);
				if (PlayerInfo[selectedClient].ip.S_un.S_addr != ip.S_un.S_addr && (ip.S_un.S_addr != 0))
				{
					PlayerInfo[selectedClient].GrabGeoIPInfo = TRUE;
					PlayerInfo[selectedClient].ip = ip;
				}

				DrawText_Ghosts(va_Ghosts(E("City: %s"), PlayerInfo[selectedClient].city), X * 0.425, Y * 0.2861, 9, 22, HudSmallFont, (subMenu == TRUE && subScroll == 9) ? ActiveTextColor : InActiveTextColor);
				DrawText_Ghosts(va_Ghosts(E("State: %s"), PlayerInfo[selectedClient].stateFull), X * 0.425, Y * 0.2861, 10, 22, HudSmallFont, (subMenu == TRUE && subScroll == 10) ? ActiveTextColor : InActiveTextColor);
				DrawText_Ghosts(va_Ghosts(E("Country: %s"), PlayerInfo[selectedClient].country), X * 0.425, Y * 0.2861, 11, 22, HudSmallFont, (subMenu == TRUE && subScroll == 11) ? ActiveTextColor : InActiveTextColor);
				DrawText_Ghosts(va_Ghosts(E("ISP: %s"), PlayerInfo[selectedClient].isp), X * 0.425, Y * 0.2861, 12, 22, HudSmallFont, (subMenu == TRUE && subScroll == 12) ? ActiveTextColor : InActiveTextColor);


			}

			if (clientMenu)
				continue;

			if (strstr(SubMenuNames[i][scroll], "?"))
				continue;

			if (!strstr(SubMenuNames[i][scroll], ":") && scroll != 5)
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (i * 22), OptionEnabled[i][scroll]);
			else if (strstr(SubMenuNames[i][scroll], ":"))
				DrawText_Ghosts(GetMenuOptionString((eMenuNames)scroll, SubMenuNames[i][scroll]), X * 0.58, (Y * 0.2841) + (i * 22), 0, 0, HudSmallFont, (subMenu == TRUE && subScroll == i) ? ActiveTextColor : InActiveTextColor);
		}
	}
}

float ToDegrees(float radian)
{
	return radian * (180 / 3.14159265359);
}

float ToRadians(float degrees)
{
	return degrees * (3.14159265359 / 180);
}

float Vec3DistanceSq(const float *p1, const float *p2)
{
	return ((p2[0] - p1[0]) * (p2[0] - p1[0]) +
		((p2[2] - p1[2]) * (p2[2] - p1[2]) +
		((p2[1] - p1[1]) * (p2[1] - p1[1]))));
}

void RotateXY(float cosAngle, float sinAngle, float pivotX, float pivotY, float x, float y, float *outX, float *outY)
{
	*outX = (float)((float)((float)(x - pivotX) * cosAngle) + pivotX) - (float)((float)(y - pivotY) * sinAngle);
	*outY = (float)((float)((float)(y - pivotY) * cosAngle) + pivotY) + (float)((float)(x - pivotX) * sinAngle);
}

void DrawAd()
{

	FLOAT X, Y;
	CONST PCHAR Text = "NiNJA Engine v1.0";
	X = (FLOAT)cgs_Ghosts->Width * 0.78125;//*(FLOAT*)((DWORD)freememory);
	Y = (FLOAT)cgs_Ghosts->Height * 0.13888888888;//*(FLOAT*)((DWORD)freememory + 4);
	//X2 = *(FLOAT*)((DWORD)freememory + 8);
	//Y2 = *(FLOAT*)((DWORD)freememory + 12);
	RotateXY(cos(ToRadians(rotation)), sin(ToRadians(rotation)), X + (FLOAT)((FLOAT)R_TextWidth_Ghosts(Text, 0x7FFFFFFF, BigDevFont) / 2), Y, X, Y, &X, &Y);

	rotation += 0.25;
	if (rotation >= 360.0)
		rotation -= 360.0;



	CL_DrawTextPhysical_Ghosts(Text, 0x7FFFFFFF, BigDevFont, X, Y, 1, 1, rotation, Vector4(1, 0, 0, 0.50), 0);
}

BOOL ShouldUpdateTagCache()
{
	if (CheckInGame() && !LobbyInit)
	{
		LobbyInit = TRUE;
		return TRUE;
	}
	else if (!CheckInGame() && LobbyInit)
	{
		LobbyInit = FALSE;
		return FALSE;
	}

	return FALSE;
}

short GetTagIndex(PCHAR Tag, BOOL ShouldCache)
{
	// Cache the tag index's
	if (ShouldCache)
	{
		// Cache our bone array index's
		for (int i = 0; i < ARRAYSIZE(BoneCache); i++)
		{
			BoneIndexs[i] = SL_GetString_Ghosts(BoneCache[i], 0);
		}

		DbgPrint("Precached Player Tags!");
	}

	short index = 0;
	for (index = 0; index < ARRAYSIZE(BoneCache); index++)
	{
		if (!strcmp(BoneCache[index], Tag))
			return BoneIndexs[index];

	}
	return BoneIndexs[index];
}

bool GetTagPosistion(pcentity_s_Ghosts centity_s, int ClientIndex, PCHAR SelectedTag, float* OutPos)
{
	char Temp[0x100];
	short TagIndex;
	//int Dobj;

	//if (ClientIndex < 18)
	//	return false;

	for (int i = 0; i <= strlen(SelectedTag); i++)
		Temp[i] = tolower(SelectedTag[i]);

	TagIndex = GetTagIndex(SelectedTag, ShouldUpdateTagCache()); //SL_GetString_Ghosts(Temp, 0);
	//Dobj = Com_GetClientDObj_Ghosts(ClientIndex, 0);

	//if (Dobj == 0)
	//	return false;

	return ((bool(*)(...))getCryptedCoD(CheatPatches->CoDPatches.GetTagPosition))(0, centity_s, TagIndex, OutPos);
}

short TagToShort(PCHAR Tag)
{
	char Temp[0x100];

	for (int i = 0; i <= strlen(Tag); i++)
		Temp[i] = tolower(Tag[i]);

	return SL_GetString_Ghosts(Temp, 0);
}

void DropTarget(ClientInformation_Ghosts PlayerInfo)
{
	PlayerInfo.Alive = FALSE;
	PlayerInfo.Distance = 900000;
	PlayerInfo.Name = NULL;
	PlayerInfo.OnTeam = FALSE;
	PlayerInfo.SelectedBone = "j_head";
	//DbgPrint("Selected Bone Test: %s", PlayerInfo.SelectedBone);
	PlayerInfo.TagPosistion = NULL;
	PlayerInfo.Team = 69;
	PlayerInfo.ThreatLevel = 0;
	PlayerInfo.Visible = FALSE;
	PlayerInfo.VisibleESP = FALSE;
	PlayerInfo.Wallbangable = FALSE;
	PlayerInfo.ClientNumber = 420;
	PlayerInfo.CurrentTarget = FALSE;
	PlayerInfo.GVisible = FALSE;
	PlayerInfo.GVisibleESP = FALSE;
	PlayerInfo.PrimaryShield = FALSE;
	PlayerInfo.AutowallPower = -0.0f;
}

void StealNames()
{
	char Output[0x200];
	int RandomClient = rand() % 11;
	if (CheckInGame() && (cg_Ghosts != NULL))
	{
		if (strcmp(cg_Ghosts->ClientName[RandomClient].Name, "") && (RandomClient != cg_Ghosts->ClientNumber) && (cg_Ghosts->ClientName[RandomClient].Name != NULL))
		{
			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", cg_Ghosts->ClientName[RandomClient].Name, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
			Cbuf_AddText_Ghosts(0, Output);
		}
	}
	else
	{
		if (strcmp(GetNameFromNetInfo(RandomClient), ""))
		{
			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", GetNameFromNetInfo(RandomClient), getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
			Cbuf_AddText_Ghosts(0, Output);
			strcpy_s((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1F, GetNameFromNetInfo(RandomClient));
		}
	}
}

void RainbowName()
{
	char Output[0x200], Color[10], PreGameOutput[0x50];
	int RandomColor = rand() % 9;
	sprintf(Color, "^%i", RandomColor);

	if (CheckInGame())
	{
		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", OriginalGT, Color, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
		Cbuf_AddText_Ghosts(0, Output);
	}
	else
	{
		sprintf(PreGameOutput, "^%i%s", RandomColor, OriginalGT);
		strcpy_s((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1F, PreGameOutput);
	}
}

void PrestigeDisco()
{
	int RandomPrestige = rand() % 11;
	if (!CheckInGame())
	{
		*(BYTE*)getCryptedCoD(CheatPatches->CoDPatches.PrestigeOffset) = RandomPrestige;
	}
	else
	{
		//sprintf(Output, "userinfo \"\\name\\%s\\xuid\\%s\\natType\\1\rank\\%i\\prestige\\%i\"", OriginalGT, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30, RandomPrestige, RandomPrestige);
		//Cbuf_AddText_Ghosts(0, Output);
	}
}

void SpoofGamertag()
{
	// Thanks James
	char Output[0x200];
	wchar_t wcGT[0x1F];
	XOVERLAPPED XOver;
	ZeroMemory(&XOver, sizeof(XOVERLAPPED));
	XShowKeyboardUI(0, VKBD_HIGHLIGHT_TEXT, wcOldGT, L"Spoof Gamertag", L"Type In New Gamertag!", wcGT, 0x1E, &XOver);

	while (!XHasOverlappedIoCompleted(&XOver)) Sleep(25);

	Sleep(2000);

	if (wcGT[0] != 0 && wcGT[1] != 0)
	{
		wcscpy_s(wcOldGT, wcGT);
		wcstombs(OriginalGT, wcGT, 0x1F);

		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", OriginalGT, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
		Cbuf_AddText_Ghosts(0, Output);
		strcpy_s((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1F, OriginalGT);
	}

	ExitThread(ERROR_SUCCESS);
}

int FindFOVSize()
{
	int FOVSize = 80;
	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "80.0")
		FOVSize = 80;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "90.0")
		FOVSize = 90;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "100.0")
		FOVSize = 100;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "120.0")
		FOVSize = 120;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "140.0")
		FOVSize = 140;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "180.0")
		FOVSize = 180;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE) == "220.0")
		FOVSize = 220;

	return FOVSize;
}

int FindFieldOfViewSize()
{
	int FieldOfViewSize = 65;
	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "Normal")
		FieldOfViewSize = 65;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "70.0")
		FieldOfViewSize = 70;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "75.0")
		FieldOfViewSize = 75;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "80.0")
		FieldOfViewSize = 80;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "90.0")
		FieldOfViewSize = 90;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "100.0")
		FieldOfViewSize = 100;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FOV) == "110.0")
		FieldOfViewSize = 110;

	return FieldOfViewSize;
}

int FindYSpinSpeed()
{
	int YSpinSpeed = 10;
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "10.0")
		YSpinSpeed = 10;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "20.0")
		YSpinSpeed = 20;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "30.0")
		YSpinSpeed = 30;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "40.0")
		YSpinSpeed = 40;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "50.0")
		YSpinSpeed = 50;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "60.0")
		YSpinSpeed = 60;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "70.0")
		YSpinSpeed = 70;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "Random")
		YSpinSpeed = rand() % 71;

	return YSpinSpeed;
}

int FindZSpinSpeed()
{
	int ZSpinSpeed = 10;
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "10.0")
		ZSpinSpeed = 10;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "20.0")
		ZSpinSpeed = 20;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "30.0")
		ZSpinSpeed = 30;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "40.0")
		ZSpinSpeed = 40;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "50.0")
		ZSpinSpeed = 50;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "60.0")
		ZSpinSpeed = 60;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "70.0")
		ZSpinSpeed = 70;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINSPEED) == "Random")
		ZSpinSpeed = rand() % 71;

	return ZSpinSpeed;
}

int FindFakeLagAmount()
{
	int FakeLagAmount = 5;
	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "5.0")
		FakeLagAmount = 5;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "10.0")
		FakeLagAmount = 10;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "15.0")
		FakeLagAmount = 15;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "20.0")
		FakeLagAmount = 20;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "25.0")
		FakeLagAmount = 25;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "30.0")
		FakeLagAmount = 30;

	if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) == "Random")
		FakeLagAmount = rand() % 25;

	return FakeLagAmount;
}

int FindReactionDelayAmount()
{
	int ReactionDelayAmount = 20;
	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "2.0")
		ReactionDelayAmount = 20;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "3.0")
		ReactionDelayAmount = 30;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "4.0")
		ReactionDelayAmount = 40;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "5.0")
		ReactionDelayAmount = 50;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "6.0")
		ReactionDelayAmount = 60;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "7.0")
		ReactionDelayAmount = 70;

	if (GetMenuOptionString(MENU_MAIN, OPTION_REACTIONDELAY) == "Random")
		ReactionDelayAmount = rand() % 80;

	return ReactionDelayAmount;
}

BYTE FindPrestigeValue()
{
	BYTE Prestige = *(BYTE*)getCryptedCoD(CheatPatches->CoDPatches.PrestigeOffset);

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "N/A")
		Prestige = 0;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "0th")
		Prestige = 0;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "1st")
		Prestige = 1;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "2nd")
		Prestige = 2;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "3rd")
		Prestige = 3;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "4th")
		Prestige = 4;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "5th")
		Prestige = 5;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "6th")
		Prestige = 6;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "7th")
		Prestige = 7;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "8th")
		Prestige = 8;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "9th")
		Prestige = 9;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "10th")
		Prestige = 10;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "11th")
		Prestige = 11;

	return Prestige;
}

int FindPredicationScale()
{
	int PredicationScale = 0;
	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) == "2.0")
		PredicationScale = 2;

	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) == "5.0")
		PredicationScale = 5;

	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) == "10.0")
		PredicationScale = 10;

	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) == "15.0")
		PredicationScale = 15;

	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) == "20.0")
		PredicationScale = 20;

	return PredicationScale;
}

void LogGamertagAndIP()
{

	//DbgPrint("Starting IP Log");
	if (!IPBuffer)
	{
		IPBuffer = new MemoryBuffer(0x8000);
		if (!IPBuffer)
		{
			DbgPrint("Failed to allocate IP buffer");
			return;
		}
	}

	memset(IPBuffer->GetData(), 0, 0x8000);
	IPBuffer->Rewind();

	BOOL exists = FileExists("XBLIVE:\\IPs.log");

	if (exists && !CReadFile("XBLIVE:\\IPs.log", *IPBuffer))
	{
		DbgPrint("Failed to read IP Log file");
		return;
	}

	for (int i = 0; i < 12; i++)
	{
		char buffer2[0x400];
		char* Name = GetNameFromNetInfo(i);
		BYTE* IPAddress = GetIPFromNetInfo(i);

		if (Name[0] == NULL || IPAddress[0] == 0)
			continue;

		if (exists && strstr((CONST PCHAR)IPBuffer->GetData(), Name))
			continue;

		sprintf(buffer2, "Gamertag: %s IP Address: %d.%d.%d.%d\r\n", Name, IPAddress[0], IPAddress[1], IPAddress[2], IPAddress[3]);
		IPBuffer->Add(buffer2, strlen(buffer2));
	}

	if (!exists)
	{
		BOOL ye = WriteFileAlt("XBLIVE:\\IPs.log", IPBuffer->GetData(), IPBuffer->GetDataLength()); // XBLIVE:\\IPs.log
		if (ye)
			DbgPrint("Writing New IP Log File");
		else
			DbgPrint("Failed To Write IP Log File");
		return;
	}

	WriteFileAlt("XBLIVE:\\IPs.log", IPBuffer->GetData(), IPBuffer->GetDataLength());
	//DbgPrint("IP Log Update Complete");
}

void HandleToggleOptions(pcgs_t_Ghosts cgs_Ghosts)
{
	// Make sure the menu is ready
	if (!menuReady) return;

	// Update Player Names
	UpdateClientsNames();

	// Handle UAV
	if (GetMenuOptionValue(MENU_MAIN, OPTION_RADAR))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.RadarOffset1) = 0x60000000;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.RadarOffset2) = 0x4800000C;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.RadarOffset3) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.RadarOffset1) = 0x419A0008;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.RadarOffset2) = 0x409A000C;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.RadarOffset3) = 0x419A0110;
	}

	// Handle No Recoil
	if (GetMenuOptionValue(MENU_MAIN, OPTION_NORECOIL))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoRecoilOffset) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoRecoilOffset) = 0x4BF8AE4D;
	}

	// Handle No Sway
	if (GetMenuOptionValue(MENU_MAIN, OPTION_NOSWAY))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset1) = 0x60000000;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset2) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset1) = 0x4BFFEAA1;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset2) = 0x4BFFFBA5;
	}

	// Handle Show Ping
	if (GetMenuOptionValue(MENU_MAIN, OPTION_SHOWPING))
	{
		if (CheckInGame() && (cg_Ghosts != NULL))
		{
			char* Ping = va_Ghosts("Ping: %i", cg_Ghosts->snapShot->Ping);
			DrawText_Ghosts(Ping, 10, 175, 0, 0, HudSmallFont, Vector4(1, 1, 1, 1));
		}
	}

	// Handle Hear All Players
	BOOL HearEveryoneValue = Dvar_GetBool_Ghosts("cg_everyoneHearsEveryone");
	if (GetMenuOptionValue(MENU_MAIN, OPTION_EVERYONEHEARSME) && (CheckInGame()))
	{
		if (!HearEveryoneValue)
			Cbuf_AddText_Ghosts(0, "cg_everyoneHearsEveryone 1");
	}
	else if(!GetMenuOptionValue(MENU_MAIN, OPTION_EVERYONEHEARSME) && (CheckInGame()))
	{
		if (HearEveryoneValue)
			Cbuf_AddText_Ghosts(0, "cg_everyoneHearsEveryone 0");
	}

	// Handle Never Host
	if (GetMenuOptionValue(MENU_MAIN, OPTION_NEVERHOST))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NetSpeedOffset) = 1000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NetSpeedOffset) = 10000000;
	}

	// Handle Show Host
	if (GetMenuOptionValue(MENU_MAIN, OPTION_SHOWHOST) && (CheckInGame()))
	{
		if (cgs_Ghosts == NULL)
			return;

		char* HostName = va_Ghosts("Host: %s", cgs_Ghosts->HostName);
		DrawText_Ghosts(HostName, 10, 190, 0, 0, HudSmallFont, Vector4(1, 1, 1, 1));
	}

	// Handle Custom Field Of View
	*(float*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.FOVStruct)) + 0xC) = FindFieldOfViewSize();

	// Handle End Game
	if (GetMenuOptionValue(MENU_MAIN, OPTION_ENDGAME) && (CheckInGame()))
	{
		char* EndGameString;
		EndGameString = va_Ghosts(E("cmd lui 9 1 %i"), *(int*)getCryptedCoD(CheatPatches->CoDPatches.ServerIdOffset));

		Cbuf_AddText_Ghosts(0, EndGameString);
		SetMenuOptionValue(MENU_MAIN, OPTION_ENDGAME, 0);
	}

	// Handle Leave Game
	if (GetMenuOptionValue(MENU_MAIN, OPTION_LEAVEGAME))
	{
		Cbuf_AddText_Ghosts(0, "disconnect");
		SetMenuOptionValue(MENU_MAIN, OPTION_LEAVEGAME, 0);
	}

	// Handle Ragebot Preferred Settings
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS))
	{
		// Enable these
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT, 1);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD, 1);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL, 1);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT, 1);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_NOFLINCH, 1);
		SetMenuOptionValue(MENU_MAIN, OPTION_NORECOIL, 1);
		SetMenuOptionValue(MENU_MAIN, OPTION_NOSWAY, 1);
		//SetMenuOptionString(MENU_RAGEBOT, "Ragebot Logic:", 1);

		// Disable These
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT, 0);
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_RAPIDFIRE, 0);
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_DRAWFOVBOX, 0);


		// Copy over menu settings before forcing these, and apply those back if this get disabled
	}

	// Handle Legitbot Preferred Settings
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS))
	{
		// Enable these
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT, 1);
		SetMenuOptionString(MENU_LEGITBOT, OPTION_FOVSIZE, 1);
		SetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE, 3);
		SetMenuOptionString(MENU_LEGITBOT, OPTION_AIMBONE, 4);

		// Disable These
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_NOFLINCH, 0);


		// Set FOV Size, Smooth Scale, Aim Delay, and Legitbot Logic
		// Copy over menu settings before forcing these, and apply those back if this get disabled
	}

	// Handle Custom Spinbot Settings
	if (GetMenuOptionValue(MENU_SPINBOT, OPTION_PREFERREDSETTINGS))
	{
		// Enable These
		SetMenuOptionValue(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 1);
		SetMenuOptionValue(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 1);

		// Disable These
		//SetMenuOptionValue(MENU_SPINBOT, "Funny Spinbots:", 0);
	}

	// Handle Ragebot Enabled
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT))
	{
		// Disable These
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS, 0);
		SetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT, 0);

		// Enable these
		SetMenuOptionValue(MENU_MAIN, OPTION_NORECOIL, 1);
		SetMenuOptionValue(MENU_MAIN, OPTION_NOSWAY, 1);
	}

	// Handle Legitbot Enabled
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT))
	{
		// Disable These
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL, 0);
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT, 0);
	}

	// Handle Name ESP
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWNAME))
	{
		// Disable over head names for enemies
		if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMIES))
		{
			*(int*)getCryptedCoD(CheatPatches->CoDPatches.OverheadNamesEnemyOffset) = 0x60000000; // Enemy Names
		}
		else
		{
			*(int*)getCryptedCoD(CheatPatches->CoDPatches.OverheadNamesEnemyOffset) = 0x4BFFE9B1; // Enemy Names
		}

		// Disable over head names for friendlies
		if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWFRIENDLIES))
		{
			*(int*)getCryptedCoD(CheatPatches->CoDPatches.OverheadNamesFriendlyOffset) = 0x60000000; // Friendly Names
		}
		else
		{
			*(int*)getCryptedCoD(CheatPatches->CoDPatches.OverheadNamesFriendlyOffset) = 0x4BFFE7BD; // Friendly Names
		}
	}

	// Handle Cartoon Mode
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_CARTOONMODE))
	{
		*(int*)0x8427192C = 0x00000002;
	}
	else
	{
		*(int*)0x8427192C = 0x00000007;
	}

	// Handle Steal Gamertags
	if (GetMenuOptionValue(MENU_MAIN, OPTION_STEALGTS) /*&& (CheckInGame())*/)
	{
		if (NameStealDelay > 40)
		{
			NameStealDelay = 0;
			StealNames();
		}
		else
		{
			NameStealDelay++;
		}
	}

	// Handle Rainbow Gamertag
	if (GetMenuOptionValue(MENU_MAIN, OPTION_RAINBOWGT))
	{
		if (RainbowGTDelay > 40)
		{
			RainbowGTDelay = 0;
			RainbowName();
		}
		else
		{
			RainbowGTDelay++;
		}
	}

	// Crosshair
	if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) != "Off" && (CheckInGame()))
	{
		if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) == "+")
		{
			// + styled crosshair
			DrawLine_Ghosts((cg_Ghosts->refdef.Width / 2) - 10, cg_Ghosts->refdef.Height / 2, (cg_Ghosts->refdef.Width / 2) + 10, cg_Ghosts->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine_Ghosts(cg_Ghosts->refdef.Width / 2, (cg_Ghosts->refdef.Height / 2) - 10, cg_Ghosts->refdef.Width / 2, (cg_Ghosts->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) == "x")
		{
			// X styled crosshair
			DrawLine_Ghosts((cg_Ghosts->refdef.Width / 2) - 8, (cg_Ghosts->refdef.Height / 2) - 8, (cg_Ghosts->refdef.Width / 2) + 8, (cg_Ghosts->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine_Ghosts((cg_Ghosts->refdef.Width / 2) - 8, (cg_Ghosts->refdef.Height / 2) + 8, (cg_Ghosts->refdef.Width / 2) + 8, (cg_Ghosts->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) == "o")
		{
			DrawLine_Ghosts((cg_Ghosts->refdef.Width / 2) - 10, cg_Ghosts->refdef.Height / 2, (cg_Ghosts->refdef.Width / 2) + 10, cg_Ghosts->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine_Ghosts(cg_Ghosts->refdef.Width / 2, (cg_Ghosts->refdef.Height / 2) - 10, cg_Ghosts->refdef.Width / 2, (cg_Ghosts->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));

			DrawLine_Ghosts((cg_Ghosts->refdef.Width / 2) - 8, (cg_Ghosts->refdef.Height / 2) - 8, (cg_Ghosts->refdef.Width / 2) + 8, (cg_Ghosts->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine_Ghosts((cg_Ghosts->refdef.Width / 2) - 8, (cg_Ghosts->refdef.Height / 2) + 8, (cg_Ghosts->refdef.Width / 2) + 8, (cg_Ghosts->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
		}
	}

	// Handle Prestige Disco
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_PRESTIGEDISCO))
	{
		if (PrestigeDiscoDelay > 40)
		{
			PrestigeDiscoDelay = 0;
			PrestigeDisco();
		}
		else
		{
			PrestigeDiscoDelay++;
		}
	}

	// Set level 60
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_LEVEL60))
	{
		// Set all squad members to level 60
		for (int i = 0; i < 10; i++)
		{
			*(BYTE*)(0x835838CC + (i * 0x564) + 0xFE8) = 0xFF;
		}

		SetMenuOptionValue(MENU_SETTINGS, OPTION_LEVEL60, 0);
	}

	// Spoof gamertag
	if (GetMenuOptionValue(MENU_MAIN, OPTION_SPOOFGT))
	{
		SetMenuOptionValue(MENU_MAIN, OPTION_SPOOFGT, 0);

		ExCreateThread(0, 0, 0, 0, (LPTHREAD_START_ROUTINE)SpoofGamertag, 0, 0);
	}

	// Handle Third Person
	if (GetMenuOptionValue(MENU_MAIN, OPTION_THIRDPERSON) && CheckInGame())
	{
		*(BYTE*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.ThirdPersonStruct)) + 0xC) = 1;
	}
	else
	{
		*(BYTE*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.ThirdPersonStruct)) + 0xC) = 0;
	}

	// Draw Legitbot FOV Box
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_DRAWFOVBOX) && CheckInGame() && !menuOpen && GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT))
	{
		int FOVSize = FindFOVSize();
		DrawBox_Ghosts((cg_Ghosts->refdef.Width / 2) - (FOVSize / 2), (cg_Ghosts->refdef.Height / 2) - (FOVSize / 2), FOVSize, FOVSize, Vector4(1, 1, 1, 0.50f));
	}

	// Handle No Flinch
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOFLINCH))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoFlinchOffset) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoFlinchOffset) = 0x4BFFF91D;
	}

	// Handle Anti-Blacklist
	/*if (GetMenuOptionValue(MENU_RAGEBOT, "Anti-Blacklist") && CheckInGame())
	{
		// Increment Counter
		BlacklistCtr++;

		// Set Random XUID
		if (BlacklistCtr > 500)
		{
			// Generate our random xuid as character
			GenerateRandomXUIDCharacter();

			// Set Random XUID
			memcpy((PVOID)0x838BA854, RandomXUID, 0xC);

			// Reset Counter
			BlacklistCtr = 0;

			// Set this back to false so we can set our original when we turn this off
			SetXUIDOnce = FALSE;
		}
	}
	else
	{
		// Set Our Original XUID Back
		if (!SetXUIDOnce)
		{
			memcpy((PVOID)0x838BA853, OriginalXUID, 0xE);
			SetXUIDOnce = TRUE;
		}
	}*/

	// Log IP's
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_LOGIPS))
	{
		if (UpdateIPLogDelay > 1000)
		{
			UpdateIPLogDelay = 0;
			LogGamertagAndIP();
		}
		else
		{
			UpdateIPLogDelay++;
		}
	}

	// Handle Set GT To Players IP
	if (ClientOptionEnabled[4][selectedClient])
	{
		char Output[0x200];
		BYTE* IP = GetIPFromNetInfo(selectedClient);
		char* ClientName = GetNameFromNetInfo(selectedClient);

		BYTE* PlayerXUID = (BYTE*)GetXUIDFromNetInfo(selectedClient);
		if (strcmp(ClientName, ""))
		{
			if (PlayerXUID[0] == 0x0 && PlayerXUID[1] == 0x09 && ClientName[0] != 0)
			{
				char* IPName = va_Ghosts("%s's IP: %i.%i.%i.%i", ClientName, IP[0], IP[1], IP[2], IP[3]);
				sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", IPName, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
				Cbuf_AddText_Ghosts(0, Output);
				//strcpy_s((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1F, IPName);
				strcpy((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), IPName);
			}
		}

		ClientOptionEnabled[4][selectedClient] = 0;
	}

	// Freeze All Consoles
	/*if (GetMenuOptionValue(MENU_SETTINGS, "Freeze All Consoles?"))
	{
		if (CheckInGame())
		{
			char Output[0x200];
			PCHAR Hax = "^\x0002NiNJA";

			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", cg_Ghosts->clientInfo[cg_Ghosts->ClientNumber].Name, Hax, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
			Cbuf_AddText_Ghosts(0, Output);
		}
		else
		{
			PCHAR Hax = "^\x0002NiNJA";
			memcpy((PVOID)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), Hax, sizeof(Hax));
		}

		SetMenuOptionValue(MENU_SETTINGS, "Freeze All Consoles?", 0);
	}*/

	// Handle Spinbot Preferred Settings
	if (GetMenuOptionValue(MENU_SPINBOT, OPTION_PREFERREDSETTINGS))
	{
		// Disable these
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);


		// Enable these
		SetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT, 1);
		SetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 1);
		SetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 1);
	}

	// Handle Set Prestige
	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) != "N/A")
	{
		BYTE Prestige = FindPrestigeValue();
		*(BYTE*)getCryptedCoD(CheatPatches->CoDPatches.PrestigeOffset) = Prestige;
	}

	// Handle Jugg Class
	/*if (GetMenuOptionValue(MENU_MAIN, "Jugg Class?"))
	{
		char* JuggClass = va_Ghosts("cmd lui 5 1;wait 20;cmd mr %i 10 custom%i", *(int*)getCryptedCoD(CheatPatches->CoDPatches.ServerIdOffset), *(int*)getCryptedCoD(CheatPatches->CoDPatches.ServerIdOffset), 1);

		Cbuf_AddText_Ghosts(0, JuggClass);

		SetMenuOptionValue(MENU_MAIN, "Jugg Class?", 0);
	}*/

	// Correct Some Settings
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD) && GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM))
	{
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM, 0);
	}

	// Unlock All
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_UNLOCKALL))
	{
		// Set Unlock All Data
		memset((LPVOID)getCryptedCoD(CheatPatches->CoDPatches.UnlockAllOffset), 0xFF, 0x356);

		// Disable the menu option
		SetMenuOptionValue(MENU_SETTINGS, OPTION_UNLOCKALL, 0);
	}

	// Give 1,000 Squad Points
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_SQUADPOINTS))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.SquadPointOffset) = _byteswap_ulong(_byteswap_ulong(*(int*)getCryptedCoD(CheatPatches->CoDPatches.SquadPointOffset)) + 1000);

		// Disable the menu option
		SetMenuOptionValue(MENU_SETTINGS, OPTION_SQUADPOINTS, 0);
	}
}

bool WorldToScreen(Vector3 World, Vector3 &Screen)
{//mpgh.net/forum/showthread.php?t=548848

	float viewaxis[3][3];
	float vieworg[3];
	float width;
	float height;
	float tanHalfFovX;
	float tanHalfFovY;

	//Get the enemy position
	float Position[3];
	float Transform[3];

	memcpy(viewaxis, cg_Ghosts->refdef.ViewAxis, sizeof(viewaxis));
	memcpy(vieworg, &cg_Ghosts->refdef.ViewOrigin, sizeof(vieworg));
	width = (float)cg_Ghosts->refdef.Width;
	height = (float)cg_Ghosts->refdef.Height;
	tanHalfFovX = cg_Ghosts->refdef.Fov.x;
	tanHalfFovY = cg_Ghosts->refdef.Fov.y;

	VectorSubtract(World, vieworg, Position);

	//Get the Dot Products from the View Angles of the player
	Transform[0] = DotProduct(Position, viewaxis[1]);
	Transform[1] = DotProduct(Position, viewaxis[2]);
	Transform[2] = DotProduct(Position, viewaxis[0]);

	//Make sure the enemy is in front of the player. If not, return.
	if (Transform[2] < 0.1f)
		return false;

	//Calculate the center of the screen
	float Center[2] = { width * 0.5f, height * 0.5f };

	//Calculates the screen coordinates
	Screen.x = Center[0] * (1 - (Transform[0] / tanHalfFovX / Transform[2]));
	Screen.y = Center[1] * (1 - (Transform[1] / tanHalfFovY / Transform[2]));

	return true;
}

float GetDistance(Vector3 c1, Vector3 c2)
{
	float dx = c2.x - c1.x;
	float dy = c2.y - c1.y;
	float dz = c2.z - c1.z;

	return sqrt((float)((dx * dx) + (dy * dy) + (dz * dz)));
}

float InverseNorm(float input)
{
	if (input < 0)
		return (360.0f + input);
	return input;
}

float GetBoxWidth(pcentity_s_Ghosts Entity) {
	if (Entity->Stance & 0x08)
		return 35;
	else return 17;
}

float GetBoxHeight(pcentity_s_Ghosts Entity) {
	if (Entity->Stance & 0x08)
		return 25;
	else if (Entity->Stance & 0x04)
		return 55;
	else return 70;
}

void getWeaponAspectRatio(DWORD killIconType, float *aspectWidth, float *aspectHeight)
{
	float width = 1.4f, height = 1.4f;
	if (killIconType == 0) //1:1
	{
		width = 1.4f;
		height = 1.4f;
	}
	else if (killIconType == 1) //2:1
	{
		width = 2.8f;
		height = 1.4f;
	}
	else if (killIconType == 2) //3:1
	{
		width = 2.8f;
		height = 0.7f;
	}
	width *= 20.0f;
	height *= 20.0f;
	*aspectWidth = width;
	*aspectHeight = height;
}

int GetWeaponIcon(int Weapon, float *aspectW, float *aspectH)
{
	DWORD weapDef = ((DWORD(*)(...))getCryptedCoD(CheatPatches->CoDPatches.BG_GetWeaponDef2))(Weapon);
	if (weapDef)
	{
		DWORD killIconType = *(DWORD*)(weapDef + 0x528);
		getWeaponAspectRatio(killIconType, aspectW, aspectH);
		return *(int *)(*(int *)(getCryptedCoD(CheatPatches->CoDPatches.GetWeaponIcon) + ((Weapon * 4) & 0x3FC)) + 0x84);
	}
	return 0;
}

void DrawSnapLine(Vector4 color, int Index, BOOL SameTeam)
{
	float xy2[2];
	Vector3 xy1, LinePos, LinePosOut;

	//Get center of screen coordinates
	xy2[0] = (float)cg_Ghosts->refdef.Width / 2.0f;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Bottom")
		xy2[1] = (float)cg_Ghosts->refdef.Height;
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Center")
		xy2[1] = (float)cg_Ghosts->refdef.Height / 2.0f;
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Top")
		xy2[1] = (float)cg_Ghosts->refdef.Height / 10.0f - 65.0f;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) != "Top")
		LinePos = Entities_Ghosts[Index].Origin;
	else
	{
		GetTagPosistion(&Entities_Ghosts[Index], Index, "j_helmet", (float*)&LinePos);
	}

	//Get coordinates of person
	if (WorldToScreen(LinePos, xy1) == TRUE)
	{
		DrawLine_Ghosts((int)xy2[0], (int)xy2[1], (int)xy1[0], (int)xy1[1], 1.0f, color);
	}
}

void DrawArrows(float* color, int Index)
{
	Vector3 Output;
	VectorSubtract(Entities_Ghosts[Index].Origin, Entities_Ghosts[cg_Ghosts->ClientNumber].Origin, Output);

	((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CG_DrawGrenadePointer))(0, cg_Ghosts->refdef.Width / 2.0f, cg_Ghosts->refdef.Height / 2.0f, &Output, color, 0.0f, 1.0f);
}

void DrawBones(Vector4 color, int Index)
{
	Vector3 TagPos[2], TagScreen[2];
	for (int b = 0; b < ARRAYSIZE(bonesESPMW2) - 1; b++)
	{
		if (!GetTagPosistion(&Entities_Ghosts[Index], Index, bonesESPMW2[b], (float*)&TagPos[0]) || !GetTagPosistion(&Entities_Ghosts[Index], Index, bonesESPMW2[b + 1], (float*)&TagPos[1]))
			return;

		if (WorldToScreen(TagPos[0], TagScreen[0]) && WorldToScreen(TagPos[1], TagScreen[1]))
			DrawLine_Ghosts(TagScreen[0].x, TagScreen[0].y, TagScreen[1].x, TagScreen[1].y,  2.0f, color);
	}
}

void CalcBoundingBox(int index, Vector4 color)
{
	float corner2X, corner2Y,
		corner3X, corner3Y,
		corner4X, corner4Y,
		corner5X, corner5Y,
		corner6X, corner6Y,
		corner7X, corner7Y,
		minsX, minsY,
		maxesX, maxesY;

	Vector3 Mins(999999.0f, 999999.0f, 999999.0f), Maxes(-999999.0f, -999999.0f, -999999.0f);

	Vector3 BonePos[14];
	for (int t = 0; t < ARRAYSIZE(ThreeDBoxesBones) - 1; t++)
	{
		Vector3* Bones;
		GetTagPosistion(&Entities_Ghosts[index], index, ThreeDBoxesBones[t], (float*)&BonePos[t]);
		Bones = &BonePos[t];
		if (Bones->x < Mins.x)
			Mins.x = Bones->x;
		if (Bones->y < Mins.y)
			Mins.y = Bones->y;
		if (Bones->z < Mins.z)
			Mins.z = Bones->z;
		if (Bones->x > Maxes.x)
			Maxes.x = Bones->x;
		if (Bones->y > Maxes.y)
			Maxes.y = Bones->y;
		if (Bones->z > Maxes.z)
			Maxes.z = Bones->z;
	}

	PlayerInfo[index].bBox.crnr2.setAngles(Maxes.x, Mins.y, Mins.z);
	PlayerInfo[index].bBox.crnr3.setAngles(Maxes.x, Mins.y, Maxes.z);
	PlayerInfo[index].bBox.crnr4.setAngles(Mins.x, Mins.y, Maxes.z);
	PlayerInfo[index].bBox.crnr5.setAngles(Mins.x, Maxes.y, Maxes.z);
	PlayerInfo[index].bBox.crnr6.setAngles(Mins.x, Maxes.y, Mins.z);
	PlayerInfo[index].bBox.crnr7.setAngles(Maxes.x, Maxes.y, Mins.z);
	PlayerInfo[index].bBox.mins.setAngles(Mins);
	PlayerInfo[index].bBox.maxes.setAngles(Maxes);

	int checksPassed = 0;
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr2, &corner2X, &corner2Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr3, &corner3X, &corner3Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr4, &corner4X, &corner4Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr5, &corner5X, &corner5Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr6, &corner6X, &corner6Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.crnr7, &corner7X, &corner7Y);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.mins, &minsX, &minsY);
	checksPassed += !CG_CalcNamePosition(0, &PlayerInfo[index].bBox.maxes, &maxesX, &maxesY);

	if (checksPassed == 0)
	{
		//mins to 2, 4, and 6
		DrawLine_Ghosts(minsX, minsY, corner2X, corner2Y, 1.0f, color);
		DrawLine_Ghosts(minsX, minsY, corner4X, corner4Y, 1.0f, color);
		DrawLine_Ghosts(minsX, minsY, corner6X, corner6Y, 1.0f, color);
		//maxes to 3, 5, and 7
		DrawLine_Ghosts(maxesX, maxesY, corner3X, corner3Y, 1.0f, color);
		DrawLine_Ghosts(maxesX, maxesY, corner5X, corner5Y, 1.0f, color);
		DrawLine_Ghosts(maxesX, maxesY, corner7X, corner7Y, 1.0f, color);
		//from 2 to 3 and 7
		DrawLine_Ghosts(corner2X, corner2Y, corner3X, corner3Y, 1.0f, color);
		DrawLine_Ghosts(corner2X, corner2Y, corner7X, corner7Y, 1.0f, color);
		//from 4 to 3 and 5
		DrawLine_Ghosts(corner4X, corner4Y, corner3X, corner3Y, 1.0f, color);
		DrawLine_Ghosts(corner4X, corner4Y, corner5X, corner5Y, 1.0f, color);
		//from 6 to 5 and 7
		DrawLine_Ghosts(corner6X, corner6Y, corner5X, corner5Y, 1.0f, color);
		DrawLine_Ghosts(corner6X, corner6Y, corner7X, corner7Y, 1.0f, color);
	}
}

void DrawESPBox(Vector4 color, int Index)
{
	float fHeight, fWidth;
	Vector3 HeadPos, FeetLocation, HeadLocation, MainRootPos;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "Corners")
	{
		if (!GetTagPosistion(&Entities_Ghosts[Index], Index, "j_head", (float*)&HeadPos))
			return;

		if (WorldToScreen(Entities_Ghosts[Index].Origin, FeetLocation) && WorldToScreen(HeadPos, HeadLocation))
		{
			fHeight = FeetLocation.y - HeadLocation.y;
			if (Entities_Ghosts[Index].Stance & 0x8)
				fWidth = fHeight / 0.9;
			else
				fWidth = fHeight / 2;

			DrawBox_Ghosts((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawBox_Ghosts((FeetLocation.x - (fWidth / 2)), FeetLocation.y, (fWidth / 4), 1, color);
			DrawBox_Ghosts((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawBox_Ghosts((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawBox_Ghosts((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawBox_Ghosts((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), FeetLocation.y, (fWidth / 4), 1, color);
			DrawBox_Ghosts((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);
			DrawBox_Ghosts((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);

		}
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "3D")
	{
		CalcBoundingBox(Index, color);
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "2D" || GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "2D Filled")
	{
	
		Vector3 MainRoot, MainRootOut, FeetOut, Feet;
		GetTagPosistion(&Entities_Ghosts[Index], Index, "j_mainroot", (float*)&MainRoot);
		MainRoot.z += 10;

		int BoxHeight = 55; // 8 or 0x0A = prone|| 4 or 6 = crouched||0 or 2 = standing
		if (Entities_Ghosts[Index].Stance & 0x8)
			BoxHeight = 17;
		else if (Entities_Ghosts[Index].Stance & 0x4)
			BoxHeight = 35;

		Feet.x = MainRoot.x; Feet.y = MainRoot.y; Feet.z = (MainRoot.z - -BoxHeight - 10);

		if (WorldToScreen(MainRoot, MainRootOut) == TRUE && WorldToScreen(Feet, FeetOut) == TRUE)
		{
			FLOAT Height = (MainRootOut.y - FeetOut.y);
			FLOAT Width = (Height / 3.0f);

			if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) != "2D Filled")
			{
				DrawLine_Ghosts((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), 1.0f, color); // Left side line
				DrawLine_Ghosts((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), 1.0f, color); // Bottom line
				DrawLine_Ghosts((int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Right side line
				DrawLine_Ghosts((int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Top line
			}
			else 
				R_AddCmdDrawStretchPic_Ghosts((MainRootOut.x - Width), (MainRootOut.y - (Height*0.5f)), Width*2.0f, Height*1.5f, 0.0f, 0.0f, 0.0f, 0.0f, color, WhiteMaterial);
		}
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "Pyramid")
	{
		Vector3 Center = Entities_Ghosts[Index].Origin;

		float W = GetBoxWidth(&Entities_Ghosts[Index]), H = GetBoxHeight(&Entities_Ghosts[Index]);

		W *= 1.2;
		H *= 1.0;

		//bottom
		MakeLines(Center, -W, -W, 0, W, -W, 0, color, 1);
		MakeLines(Center, -W, -W, 0, -W, W, 0, color, 1);
		MakeLines(Center, W, W, 0, W, -W, 0, color, 1);
		MakeLines(Center, W, W, 0, -W, W, 0, color, 1);
		//middle
		MakeLines(Center, -W, -W, 0, 0, 0, H, color, 1);
		MakeLines(Center, -W, W, 0, 0, 0, H, color, 1);
		MakeLines(Center, W, -W, 0, 0, 0, H, color, 1);
		MakeLines(Center, W, W, 0, 0, 0, H, color, 1);
	}
}

void DrawMenuControlsText()
{
	char Output[0x200];
	if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUINSTRUCTIONS))
	{
		if (!CheckInGame())
		{
			FLOAT X, Y;
			X = (FLOAT)*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY);
			Y = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY) + 0x4);
			sprintf(Output, "Press \x0016 & \x0012 To %s ^7NiNJA Engine", menuOpen ? "^1Close" : "^2Open");
			DrawText_Ghosts(Output, (X - 380), Y - 20, 0, 0, HudSmallFont, Vector4(1, 1, 1, 0.75f));
		}
		else
		{
			sprintf(Output, "Press \x0016 & \x0012 To %s ^7NiNJA Engine", menuOpen ? "^1Close" : "^2Open");
			DrawText_Ghosts(Output, (cg_Ghosts->refdef.Width - 1250), cg_Ghosts->refdef.Height - 70, 0, 0, HudSmallFont, Vector4(1, 1, 1, 0.75f));
		}
	}
}

double Radians(float Degrees)
{
	return  0.01745329238474369 * Degrees;
}

void GetSpread_Ghosts(int* seed, float* x, float* y)
{
	float x2, y2, min, max, spread;
	BG_GetSpreadForWeapon_Ghosts(cg_Ghosts, Entities_Ghosts[cg_Ghosts->ClientNumber].WeaponID, &min, &max);

	spread = (max - min) * (*(FLOAT*)((DWORD)cg_Ghosts + 0xC77E0) * 0.00392156862) + min;
	RandomBulletDir_Ghosts(seed, 0, 360, &x2, &y2);
	float HAX = sub_8222EC70(seed);

	*x = spread * x2 * HAX;
	*y = spread * y2 * HAX;
}

Vector3 SmoothAimbotAngles(Vector3 AimbotAngles, float SmoothValue)
{
	float FinishedAngles[3]; float smoothdiff[3];
	AimbotAngles[0] -= LocalClient_Ghosts->ViewAngles.x;
	AimbotAngles[1] -= LocalClient_Ghosts->ViewAngles.y;
	if (AimbotAngles[0] > 180)  AimbotAngles[0] -= 360;
	if (AimbotAngles[1] > 180)  AimbotAngles[1] -= 360;
	if (AimbotAngles[0] < -180) AimbotAngles[0] += 360;
	if (AimbotAngles[1] < -180) AimbotAngles[1] += 360;
	smoothdiff[0] = AimbotAngles[0] / SmoothValue;
	smoothdiff[1] = AimbotAngles[1] / SmoothValue;
	FinishedAngles[0] = LocalClient_Ghosts->ViewAngles.x + smoothdiff[0];
	FinishedAngles[1] = LocalClient_Ghosts->ViewAngles.y + smoothdiff[1];
	if (FinishedAngles[0] > 180)  FinishedAngles[0] -= 360;
	if (FinishedAngles[1] > 180)  FinishedAngles[1] -= 360;
	if (FinishedAngles[0] < -180) FinishedAngles[0] += 360;
	if (FinishedAngles[1] < -180) FinishedAngles[1] += 360;
	return FinishedAngles;
}

signed char ClampChar(int i)
{
	if (i < -128)
		return -128;

	if (i > 127)
		return 127;

	return i;
}

void FixTextForDistance(CONST PCHAR Text, float* x, float* y, DWORD Font, float scale, Vector3 WorldPos, float* OutDist)
{
	float distanceSq = cg_Ghosts->refdef.ViewOrigin.Distance(WorldPos);
	float distance = sqrt(distanceSq);
	float distFrac = (float)(distance - 64) / 448;
	float distScale = (float)(distFrac * scale) + (float)(1 - distFrac);
	*OutDist = distFrac;
	float x2 = (FLOAT)(floor(-((((FLOAT)R_TextWidth_Ghosts(Text, 0x7FFFFFFF, Font) * R_NormalizedTextScale_Ghosts(Font, scale * distScale)) * 0.4) - *x) + 0.4));

	__asm
	{
		frsp x2, x2
	}
	*x = x2;
	*y = (FLOAT)(floor(*y + 0.4));
}

bool IsWeaponDualWield()
{
	return ((bool(*)(...))getCryptedCoD(CheatPatches->CoDPatches.IsWeaponDualWield))(Entities_Ghosts[cg_Ghosts->ClientNumber].WeaponID);
}

bool Autowall(int Index, PCHAR TagToUse, BOOL UsingShield)
{
	Vector3 TagPos, Angles, dir, Hax[3], MyPos;

	if (!GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL))
		return false;

	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_TARGETSHIELDS) && UsingShield)
		TagToUse = "j_ankle_le";

	if (!GetTagPosistion(&Entities_Ghosts[Index], Index, TagToUse, (float*)&TagPos))
		return false;

	CG_GetPlayerViewOrigin_Ghosts(0, cg_Ghosts, MyPos);


	BulletFireParams bulletFireParams;
	bulletFireParams.skipNum = cg_Ghosts->ClientNumber;
	bulletFireParams.worldEntNum = 2046;
	bulletFireParams.power = 1.0f;
	bulletFireParams.bulletType = 2;
	bulletFireParams.noRicochet = 0;
	VectorCopy(MyPos, bulletFireParams.vieworg);
	VectorCopy(MyPos, bulletFireParams.start);
	VectorCopy(TagPos, bulletFireParams.end);
	VectorSubtract(TagPos, MyPos, dir);
	Angles = VectorToAngles(dir);
	
	AngleVectors(Angles, &Hax[0], NULL, NULL);
	VectorCopy(Hax[0], bulletFireParams.viewDirection);

	// Disable visual effects
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_BulletHitEvent) = 0x4E800020;
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.DynEntCl_DynEntImpactEvent) = 0x4E800020;

	// Allow 360 degree autowall checks
	int restoreVal = *(int*)getCryptedCoD(CheatPatches->CoDPatches.Autowall360);
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.Autowall360) = 0x38600001;

	// Disable the games FireBulletPenetrate call
	*(DWORD*)0x822C9380 = 0x60000000;

	FireBulletPenetrate_Ghosts(0, &bulletFireParams, Entities_Ghosts[cg_Ghosts->ClientNumber].WeaponID, 0, &Entities_Ghosts[cg_Ghosts->ClientNumber], 0xFFFFFFFF, MyPos, false);

	// Unpatch 360 degree autowall check
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.Autowall360) = restoreVal;

	// Restore visual effects
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_BulletHitEvent) = 0x7D8802A6;
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.DynEntCl_DynEntImpactEvent) = 0x7D8802A6;

	// Unpatch the games FireBulletPenetrate call
	*(DWORD*)0x82259B40 = 0x4BFFE9C9;

	return (bulletFireParams.skipNum == Index);
}

void CorrectMovement(usercmd_s_Ghosts *usercommand, Vector3 vec)
{
	Vector3 Fix = vec - (LocalClient_Ghosts->ViewAngles);

	int forward = usercommand->forwardmove;
	int side = usercommand->rightmove;

	if (forward >= 128)
	{
		forward = -(256 - forward);
	}

	if (side >= 128)
	{
		side = -(256 - side);
	}

	int fixedforward = (cos(Radians(Fix.y)) * forward) + (cos(Radians(Fix.y + 90)) * side);
	int fixedside = (sin(Radians(Fix.y)) * forward) + (sin(Radians(Fix.y + 90)) * side);

	usercommand->forwardmove = ClampChar(fixedforward);
	usercommand->rightmove = ClampChar(fixedside);
}

bool TriggerBot(int Index)
{
	Vector3 HeadOrigin, Hitboxes;
	Vector3 FeetOrigin = Entities_Ghosts[Index].Origin;
	GetTagPosistion(&Entities_Ghosts[Index], Index, "j_head", (float*)&HeadOrigin);
	FeetOrigin.z -= 5;
	HeadOrigin.z += 10;
	Vector2 FeetLocation, HeadLocation;
	if (WorldPosToScreenPos((float*)&FeetOrigin, &FeetLocation) && WorldPosToScreenPos((float*)&HeadOrigin, &HeadLocation))
	{
		float fHeight = FeetLocation.y - HeadLocation.y;
		float fWidth = fHeight / 1.5;
		Vector2 ScreenPos;
		for (int b = 0; b < 14; b++)
		{
			GetTagPosistion(&Entities_Ghosts[Index], Index, TriggerBotBones[b], (float*)&Hitboxes);
			if (WorldPosToScreenPos((float*)&Hitboxes, &ScreenPos))
			{
				if (ScreenPos.x - ((fWidth / 5) / 2) < (cg_Ghosts->refdef.Width / 2) &&
					ScreenPos.x + ((fWidth / 5) / 2) > (cg_Ghosts->refdef.Width / 2) &&
					ScreenPos.y - ((fWidth / 5) / 2) < (cg_Ghosts->refdef.Height / 2) &&
					ScreenPos.y + ((fWidth / 5) / 2) > (cg_Ghosts->refdef.Height / 2))
				{

					return true;
				}
			}
		}
	}

	return false;
}

float FindSmoothScale()
{
	float SmoothScale = 5.0f;
	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "5.0")
		SmoothScale = 5.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "10.0")
		SmoothScale = 10.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "15.0")
		SmoothScale = 15.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "20.0")
		SmoothScale = 20.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "25.0")
		SmoothScale = 25.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "30.0")
		SmoothScale = 30.0f;

	return SmoothScale;
}

bool FOVCheck(int index, PCHAR Tag)
{
	Vector2 ScreenPos; float Output[3];
	int FOVSize = FindFOVSize();
	GetTagPosistion(&Entities_Ghosts[index], index, Tag, Output);
	if (WorldPosToScreenPos(Output, &ScreenPos))
	{
		if (ScreenPos.x > ((cg_Ghosts->refdef.Width / 2) - FOVSize) &&
			ScreenPos.x < ((cg_Ghosts->refdef.Width / 2) + FOVSize) &&
			ScreenPos.y >((cg_Ghosts->refdef.Height / 2) - FOVSize) &&
			ScreenPos.y < ((cg_Ghosts->refdef.Height / 2) + FOVSize))
		{
			return true;
		}
	}

	return false;
}

BOOL IsPlayerEnemy(int ClientIndex, int myClientNum)
{
	// If whitelisted
	if (ClientOptionEnabled[2][ClientIndex])
		return FALSE;

	// Handle Normal / Inverted Team Check
	//if (!GetMenuOptionValue(MENU_SETTINGS, "Invert Team Check"))
	//{
		if ((cg_Ghosts->ClientTeam[myClientNum].Team == cg_Ghosts->ClientTeam[ClientIndex].Team) && (cg_Ghosts->ClientTeam[myClientNum].Team != 0))
		{
			// Invert individual player team check
			if (ClientOptionEnabled[3][ClientIndex])
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			// Invert individual player team check
			if (ClientOptionEnabled[3][ClientIndex])
				return FALSE;
			else
				return TRUE;
		}
	//}
	//else
	//{
	//	// Reverse team check
	//	if ((cg_Ghosts->ClientTeam[myClientNum].Team == cg_Ghosts->ClientTeam[ClientIndex].Team) && (cg_Ghosts->ClientTeam[myClientNum].Team != 0))
	//	{
	//		// Invert individual player team check
	//		if (ClientOptionEnabled[3][ClientIndex])
	//			return FALSE;
	//		else
	//			return TRUE;
	//	}
	//	else
	//	{
	//		// Invert individual player team check
	//		if (ClientOptionEnabled[3][ClientIndex])
	//			return TRUE;
	//		else
	//			return FALSE;
	//	}
	//}


	// We shouldn't get here
	return FALSE;
}

BOOL ValidPlayer(int ClientIndex, int myClientNum, BOOL UsingRagebot)
{
	// If it's us
	if (ClientIndex == myClientNum)
		return false;

	// If alive
	if (Entities_Ghosts[ClientIndex].Type != 1 || (*(int*)((DWORD)&Entities_Ghosts[ClientIndex] + 0xCC) & 0x20) || (*(int*)((DWORD)&Entities_Ghosts[ClientIndex] + 0x1C4) & 0x1) == 0)
		return false;

	// If enemy
	if (!IsPlayerEnemy(ClientIndex, myClientNum))
		return false;

	// If whitelisted
	if (ClientOptionEnabled[2][ClientIndex])
		return false;

	return true;
}

float ScaleRadarX(float X, float ScreenWidth)
{
	return (X * (640.0f / ScreenWidth));
}

float ScaleRadarY(float Y, float ScreenHeight)
{
	return (Y * (480.0f / ScreenHeight));
}

void WorldToCompass_Mw2(CRadarHud* radarHud, Vector2& size, Vector3& World, Vector2& Compass, Vector2& widthHeight)
{
	Vector4 RadarSize;
	CG_CompassCalcDimensions_Ghosts(0, cg_Ghosts, size, radarHud, &RadarSize.x, &RadarSize.y, &RadarSize.z, &RadarSize.a);

	float x2 = (RadarSize.z * 0.5) + RadarSize.x;
	float y2 = (RadarSize.a * 0.5) + RadarSize.y;

	Vector2 out, outClipped, Yaw;

	CG_CompassUpYawVector_Ghosts(cg_Ghosts, Yaw);

	CG_WorldPosToCompass_Ghosts(0, cg_Ghosts, RadarSize, Yaw, Entities_Ghosts[cg_Ghosts->ClientNumber].Origin, World, out, outClipped);

	Compass.x = -((widthHeight.x * 0.5) - x2) + outClipped.x;
	Compass.y = -((widthHeight.y * 0.5) - y2) + outClipped.y;
}

void DrawRadar_Ghosts()
{
	CRadarHud radarHud;
	Vector2 size;
	DWORD compassPingFriendly = *(DWORD*)0x82ACD048;
	size.x = 0;//ScaleRadarX(12, cg_Ghosts->refdef.Width);
	size.y = 0;//ScaleRadarY(12, cg_Ghosts->refdef.Height);
	radarHud.x = ScaleRadarX(12, cg_Ghosts->refdef.Width);
	radarHud.y = ScaleRadarY(12, cg_Ghosts->refdef.Height);
	radarHud.w = 178.667;
	radarHud.h = 93.3333;
	radarHud.unk = 0x0C0D0000;

	DrawEngineRadar_Ghosts(0, 0, size, &radarHud, WhiteMaterial, Vector4(1, 1, 1, 1));

	// Draw Dropped Items
	for (int i = 100; i < 800; i++)
	{
		if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS))
		{
			if (Entities_Ghosts[i].isAliveBYTE1 != 0 && Entities_Ghosts[i].Type == 3)
			{
				Vector2 Screen;
				Vector3 Origin = Entities_Ghosts[i].Origin;

				// Make sure it's not a scavenger pack
				if ((Entities_Ghosts[i].WeaponID != CG_GetWeaponIndexForName("scavenger_bag_mp")))
					continue;


				Vector2 Compass, widthHeight;
				widthHeight.x = 25;
				widthHeight.y = 15;
				WorldToCompass_Mw2(&radarHud, size, Entities_Ghosts[i].Origin, Compass, widthHeight);

				int horz = radarHud.unk >> 24;
				int vert = (radarHud.unk >> 16) & 0xFF;

				//float unk = sub_8228C068(*(FLOAT*)((DWORD)cg_Mw2 + 0x6EB8C) - *(FLOAT*)0x8259C334);

				CG_DrawRotatedPic_Ghosts(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), Compass.x, Compass.y, widthHeight.x, widthHeight.y, horz, vert, 0, Vector4(1, 1, 1, 0.75f), DB_FindXAssetHeader_Ghosts(material_Ghosts, "scavenger_pickup", 1));
			}
		}
	}

	// Draw Enemies
	for (int client = 0; client < 18; client++)
	{
		if (client == cg_Ghosts->ClientNumber || Entities_Ghosts[client].Type != 1 || (*(int*)((DWORD)&Entities_Ghosts[client] + 0xCC) & 0x20) || (*(int*)((DWORD)&Entities_Ghosts[client] + 0x1C4) & 0x1) == 0)
			continue;

		Vector2 Compass, widthHeight;
		widthHeight.x = 16;
		widthHeight.y = 16;
		WorldToCompass_Mw2(&radarHud, size, Entities_Ghosts[client].Origin, Compass, widthHeight);

		int horz = radarHud.unk >> 24;
		int vert = (radarHud.unk >> 16) & 0xFF;

		float angle = AngleNormalize360_Ghosts(*(FLOAT*)((DWORD)cg_Ghosts + 0x720BC) - Entities_Ghosts[client].Angles.y);
		CG_DrawRotatedPic_Ghosts(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), Compass.x, Compass.y, widthHeight.x, widthHeight.y, horz, vert, angle, !IsPlayerEnemy(client, cg_Ghosts->ClientNumber) ? Vector4(0, 1, 0, 0.75f) : Vector4(1, 0, 0, 0.75f), compassPingFriendly);
	}

	//DrawEnemies_Mw2(0, 0, size, &radarHud, Vector4(1,0,0,1));
	//DrawFriendlies_Ghosts(0, 0, size, &radarHud, Vector4(0, 1, 0, 0.75f));
	DrawVehicles_Ghosts(0, 0, size, &radarHud, Vector4(0, 0, 1, 0.75f));
	DrawPlayer_Ghosts(0, 0, size, &radarHud, Vector4(1, 1, 0, 1));
}

void CG_EntityEventHook(int localClient, int entityState_t, int Event, int eventParam)
{
	if (GetMenuOptionValue(MENU_MAIN, OPTION_FASTRELOAD))
	{
		if (Event == 22)
		{
			int ClientReloadIndex = *(int*)(entityState_t + 0xC4);
			if (ClientReloadIndex == cg_Ghosts->ClientNumber)
				WasReloading = TRUE;
		}
		else if (Event == 25)
		{
			int ClientSwitchIndex = *(int*)(entityState_t + 0xC4);
			if (WasReloading && yyCount == 1)
				YButtonPressed = FALSE;
		}
	}

	CG_EntityEventDetour->CallOriginal(localClient, entityState_t, Event, eventParam);
}

DWORD NetDll_XNetGetTitleXnAddrHook(XNCALLER_TYPE xnc, XNADDR *pxna)
{
	DWORD retVal = NetDll_XNetGetTitleXnAddrDetour->CallOriginal(xnc, pxna);

	pxna->wPortOnline = 6969;

	return retVal;
}

DWORD XInputGetStateHook(DWORD dwUserIndex, PXINPUT_STATE pState)
{
	if (dwUserIndex != 0)
		return XInputGetStateDetour->CallOriginal(dwUserIndex, pState);

	DWORD Result = XInputGetStateDetour->CallOriginal(dwUserIndex, pState);
	if (FAILED(Result) || !pState)
		return Result;

	// Handle Auto Reload Cancel
	if (GetMenuOptionValue(MENU_MAIN, OPTION_FASTRELOAD) && CheckInGame())
	{
		if (WasReloading)
		{
			if (!YButtonPressed)
			{
				if (!((pState->Gamepad.wButtons & XINPUT_GAMEPAD_Y) == XINPUT_GAMEPAD_Y))
					pState->Gamepad.wButtons |= XINPUT_GAMEPAD_Y;
				YButtonPressed = TRUE;
				yyCount++;
			}
			if (yyCount > 1)
			{
				WasReloading = FALSE;
				yyCount = 0;
				YButtonPressed = FALSE;
			}
		}
	}

	// Auto Aim
	if ((GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOAIM) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_TRIGGERBOT)) && (GClosestClient != -1))
		pState->Gamepad.bLeftTrigger = 100;

	// Remove game input when menu is open
	if (menuOpen)
		pState->Gamepad.wButtons &= ~(XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_RIGHT | XINPUT_GAMEPAD_A | XINPUT_GAMEPAD_B);

	// Rapid Fire
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_RAPIDFIRE) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_RAPIDFIRE))
	{
		if (pState->Gamepad.bRightTrigger > 0)
		{
			TriggerValue = pState->Gamepad.bRightTrigger;
			pState->Gamepad.bRightTrigger = 0;
		}
		else
		{
			TriggerValue = 0;
		}
	}

	return Result;
}

void GetRadarXY(float *x, float *y)
{
	*x = lastCompass.x;
	*y = lastCompass.y;
}

void GetRectDef(float radarX, float radarY, CRectDef *rectDef)
{
	if (LastCompassType == 0)
		rectDef->horizAlign = 0x0c0dea70;
	else
		rectDef->horizAlign = 0x0C0DEF20;

	rectDef->x = lastCompass.x;
	rectDef->y = lastCompass.y;
	rectDef->w = lastCompass.w;
	rectDef->h = lastCompass.h;
}

BOOL CalcCompassDimensions(DWORD cg_t, DWORD CompassType, Vector2& size, DWORD rectDef, Vector4& radarSize)
{
	return ((BOOL(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CG_CompassCalcDimensions))(CompassType, cg_t, &size, rectDef, &radarSize.x, &radarSize.y, &radarSize.z, &radarSize.a);
}
vec3 out;
BOOL WorldPosToCompass(DWORD cg_t, DWORD CompassType, Vector4& RadarSize, Vector2& Yaw, Vector3& refOrigin, vec3 *worldPos, Vector2& outClipped)
{
	return ((BOOL(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CG_WorldPosToCompass))(CompassType, cg_t, &RadarSize, &Yaw, &refOrigin, worldPos, &out, &outClipped);
}

void CompassUpYawVector(DWORD cg_t, Vector2& Yaw)
{
	((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CG_CompassUpYawVector))(cg_t, &Yaw);
}

void DrawRadarPic(float x, float y, float w, float h, float rotation, const float *color, DWORD shader)
{
	DWORD horizAlign = 0, vertAlign = 0;
	horizAlign = 0xc;
	vertAlign = 0xd;
	((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.UI_DrawHandlePic))(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), x, y, w, h, horizAlign, vertAlign, rotation, color, shader);
}

void WorldToCompass_Internal(DWORD cg_t, vec3 *World, Vector2& Compass)
{
	if (screenMaxX == NULL || screenMaxY == NULL)
	{
		float aspect;
		((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CL_GetScreenDimensions))(&screenMaxX, &screenMaxY, &aspect);
	}

	Vector4 RadarSize;
	float radarX = 0, radarY = 0;
	GetRadarXY(&radarX, &radarY);

	CRectDef rectDef;
	GetRectDef(radarX, radarY, &rectDef);

	Vector2 size(radarX, radarY);
	CalcCompassDimensions(cg_t, LastCompassType, size, (DWORD)&rectDef, RadarSize);

	float x2 = (RadarSize.z * 0.5) + RadarSize.x;
	float y2 = (RadarSize.a * 0.5) + RadarSize.y;

	Vector2 Yaw(0, 0);
	CompassUpYawVector(cg_t, Yaw);

	Vector2 outClipped(0, 0);
	//vec3 myOrigin = GameFuncs::getLocalViewOrigin(&myOrigin);

	Vector3 MyPos;
	CG_GetPlayerViewOrigin_Ghosts(0, (pcg_s_Ghosts)cg_t, MyPos);
	WorldPosToCompass(cg_t, LastCompassType, RadarSize, Yaw, MyPos, World, outClipped);

	Vector2 widthHeight(16, 16);
	Compass.x = -((widthHeight.x * 0.5) - x2) + outClipped.x;
	Compass.y = -((widthHeight.y * 0.5) - y2) + outClipped.y;

	if (LastCompassType == 0)
	{
		Compass.x -= 2.0f;
		Compass.y -= 3.0f;
	}
	else
	{
		Compass.x -= 462.0f;
		Compass.y -= 176.0f;
	}
}
void WorldToCompass(vec3 *World, Vector2& Compass)
{
	int cg_t = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	if (cg_t)
		WorldToCompass_Internal((DWORD)cg_t, World, Compass);
}

void WorldToCompass(Vector3& World, Vector2 &Compass)
{
	WorldToCompass((vec3*)&World.x, Compass);
}

DWORD getRefDef()
{
	int cg_t = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	if (cg_t)
		return cg_t + 0x6D53C;
	return 0;
}
int getViewAxis_ptr()
{
	DWORD refDef = getRefDef();
	if (refDef)
		return refDef + 0x24;
	return 0;
}

vec3 getRefDefViewAngles()
{
	int viewAxis_ptr = getViewAxis_ptr();
	if (viewAxis_ptr)
	{
		vec3_t axisAngles[3];
		//float axisAngles[9];
		for (int i = 0; i<3; i++)
		{
			axisAngles[0][i] = *(float*)(viewAxis_ptr + (i * 0x04));
			axisAngles[1][i] = *(float*)(viewAxis_ptr + 0xc + (i * 0x04));
			axisAngles[2][i] = *(float*)(viewAxis_ptr + 0x18 + (i * 0x04));
		}

		float newAngles[3];
		AxisToAngles((Vector3*)axisAngles, newAngles);
		return vec3(newAngles);
	}
	return vec3(0, 0, 0);
}

void DrawCustomRadar()
{
	// Loop for all ents (this 1st so our player ESP draws over it if they over lap)
	for (int i = 18; i < 0x7FF; i++)
	{
		// Draw Dropped Items
		if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS))
		{
			BOOL IsWeapon = TRUE;

			// Make sure it's a dropped item
			if ((*(int*)((DWORD)&Entities_Ghosts[i] + 0x1C4) & 0x1) == 1 && Entities_Ghosts[i].Type == 3)
			{
				// Make sure it's not a scavenger pack
				if ((Entities_Ghosts[i].WeaponID != CG_GetWeaponIndexForName("scavenger_bag_mp")))
					IsWeapon = FALSE;

				// Set color based on weapon/pack
				Vector4 color = !IsWeapon ? Vector4(1, 1, 1, 1) : Vector4(0.51f, 1, 1, 1);
				if (IsWeapon)
				{
					if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR) && DrawCompassTimer > GetTickCount())
					{
						Vector2 compass;
						WorldToCompass(Entities_Ghosts[i].Origin, compass);
						DrawRadarPic(compass.x, compass.y, 20.0f, 10.0f, 0.0f, (float*)&color, ScavMaterial);
					}
				}
			}
		}
	}


	if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR) && DrawCompassTimer > GetTickCount())
	{
		// Loop for all clients
		for (int i = 0; i < 18; i++)
		{
			// Make sure the client is not us
			if (Entities_Ghosts[i].ClientNum != cg_Ghosts->ClientNumber && cg_Ghosts->ClientNumber != i)
			{
				// Alive and is player
				if (Entities_Ghosts[i].Type != 1 || (*(int*)((DWORD)&Entities_Ghosts[i] + 0xCC) & 0x20) || (*(int*)((DWORD)&Entities_Ghosts[i] + 0x1C4) & 0x1) == 0)
				{
					// Invalid player
					continue;
				}

				//if (LastCompassType == 1)
				//	goto skip;

				int centity = *(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer);

				if (centity)
				{
					centity = centity + (i * 0x1ec);

					vec3 entityView = *(vec3*)(centity + 0x100);
					vec3 myView = getRefDefViewAngles();

					float yaw = entityView.y;

					float rotation = (360.0f - yaw);
					if (LastCompassType == 0)
						rotation += myView.y;
					//else if (currentTitle == MW3_TITLE_ID)
					//	rotation += 90.0f;

					float w = 20.0, h = 20.0;
					Vector2 Compass;
					WorldToCompass(Entities_Ghosts[i].Origin, Compass);
					DrawRadarPic(Compass.x, Compass.y, w, h, rotation, PlayerInfo[i].OnTeam ? (float*)&Vector4(0, 1, 0, 0.75f) : (float*)&Vector4(1, 0, 0, 0.75f), CompassPingMaterial);
				}
			}

			// Continue if the client mumber is ours
			else continue;
		}

	skip:
		if (LastCompassType == 0) //draw our own pointer
		{
			Vector2 Compass;
			DWORD cg_t = *(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
			if (cg_t)
			{
				Vector3 MyPos;
				CG_GetPlayerViewOrigin_Ghosts(0, (pcg_s_Ghosts)cg_t, MyPos);
				WorldToCompass(MyPos, Compass);
				float w = 20.0f, h = 20.0f;
				DrawRadarPic(Compass.x, Compass.y, w, h, 0.0f, (float*)&GetColorCode(colorWhite), CompassPingMaterial);
			}
		}
	}
}

void DrawESP()
{
	LocalClient_Ghosts = (pclientActive_t_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu17
	Entities_Ghosts = (pcentity_s_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu17
	cg_Ghosts = (pcg_s_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu17
	cgs_Ghosts = (pcgs_t_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu17

	// If in-game, start our ESP
	if (CheckInGame() && menuReady && (cgs_Ghosts != NULL))
	{
		// NiNJA Engine v1.0
		if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLENINJATEXT))
			DrawAd();

		// Loop for all ents (this 1st so our player ESP draws over it if they over lap)
		for (int i = 18; i < 0x7FF; i++)
		{
			// Draw Explosives
			if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWEXPLOSIVES))
			{
				// Make sure it's a explosive
				if (Entities_Ghosts[i].isAliveBYTE1 != 0 && Entities_Ghosts[i].Type == 4)
				{
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					CG_GetEntityBModelBounds(&Entities_Ghosts[i], (float*)&Mins, (float*)&Maxs);
					TopLocation = Entities_Ghosts[i].Origin; TopLocation.z -= Mins.z;
					if (WorldToScreen(TopLocation, TopLocationOut))
					{
						Vector4 color = Vector4(1, 1, 0, 0.50f);

						float aspectW, aspectH;
						int ExplosiveIcon = GetWeaponIcon(Entities_Ghosts[i].WeaponID, &aspectW, &aspectH);
						if (MmIsAddressValid((DWORD*)ExplosiveIcon))
							CG_DrawRotatedPicPhysical_Ghosts(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), TopLocationOut.x - (aspectW/2), TopLocationOut.y - (aspectH/2), aspectW, aspectH, 0.0, GetColorCode(colorYellow), ExplosiveIcon);
					}
				}
			}

			// Draw Dropped Items
			if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS))
			{
				BOOL IsWeapon = TRUE;

				// Make sure it's a dropped item
				if ((*(int*)((DWORD)&Entities_Ghosts[i] + 0x1C4) & 0x1) == 1 && Entities_Ghosts[i].Type == 3 && (GetDistance(cg_Ghosts->refdef.ViewOrigin, Entities_Ghosts[i].Origin) < 1500.0f))
				{
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					CG_GetEntityBModelBounds(&Entities_Ghosts[i], (float*)&Mins, (float*)&Maxs);
					TopLocation = Entities_Ghosts[i].Origin; TopLocation.z -= Mins.z;
					if (WorldToScreen(TopLocation, TopLocationOut))
					{
						// Make sure it's not a scavenger pack
						if ((Entities_Ghosts[i].WeaponID != CG_GetWeaponIndexForName("scavenger_bag_mp")))
							IsWeapon = FALSE;

						// Set color based on weapon/pack
						Vector4 color = !IsWeapon ? Vector4(1, 1, 1, 0.80f) : Vector4(0.51f, 1, 1, 0.80f);

						if (!IsWeapon)
						{
							float aspectW, aspectH;
							int DroppedIcon = GetWeaponIcon(Entities_Ghosts[i].WeaponID, &aspectW, &aspectH);
							if (MmIsAddressValid((DWORD*)DroppedIcon))
								CG_DrawRotatedPicPhysical_Ghosts(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), TopLocationOut.x - (aspectW/2), TopLocationOut.y - (aspectH/2), aspectW, aspectH, 0.0, color, DroppedIcon);
						}
						else
						{
							CG_DrawRotatedPicPhysical_Ghosts(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), TopLocationOut.x - 30, TopLocationOut.y - 10, 45, 30, 0.0, color, ScavMaterial);
						}
					}
				}
			}
		}

		// Loop for all clients
		for (int i = 0; i < cgs_Ghosts->MaxClients; i++)
		{
			// Make sure the client is not us
			if (Entities_Ghosts[i].ClientNum != cg_Ghosts->ClientNumber && cg_Ghosts->ClientNumber != i)
			{
				// Alive and is player
				if (Entities_Ghosts[i].Type != 1 || (*(int*)((DWORD)&Entities_Ghosts[i] + 0xCC) & 0x20) || (*(int*)((DWORD)&Entities_Ghosts[i] + 0x1C4) & 0x1) == 0)
				{
					// Invalid player
					continue;
				}

				// Set some of the players info in our struct
				PlayerInfo[i].Alive = TRUE;
				PlayerInfo[i].ClientNumber = Entities_Ghosts[i].ClientNum;
				PlayerInfo[i].Name = cg_Ghosts->ClientName[i].Name;

				// Team check
				//if (!GetMenuOptionValue(MENU_SETTINGS, "Invert Team Check"))
				//{
					if ((cg_Ghosts->ClientTeam[cg_Ghosts->ClientNumber].Team == cg_Ghosts->ClientTeam[i].Team) && (cg_Ghosts->ClientTeam[cg_Ghosts->ClientNumber].Team != 0))
					{
						// Save players team
						PlayerInfo[i].Team = cg_Ghosts->ClientTeam[i].Team;
						PlayerInfo[i].OnTeam = TRUE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;

						// If we want to draw friendlies
						if (!GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWFRIENDLIES))
							continue;
					}
					else
					{
						// If we want to draw enemies
						if (!GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMIES))
							continue;

						PlayerInfo[i].Team = cg_Ghosts->ClientTeam[i].Team;
						PlayerInfo[i].OnTeam = FALSE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
					}
				//}
				//else
				//{
				//	// Reverse team check
				//	if ((cg_Ghosts->ClientTeam[cg_Ghosts->ClientNumber].Team == cg_Ghosts->ClientTeam[i].Team) && (cg_Ghosts->ClientTeam[cg_Ghosts->ClientNumber].Team != 0))
				//	{
				//		// If we want to draw enemies
				//		if (!GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMIES))
				//			continue;

				//		PlayerInfo[i].Team = cg_Ghosts->ClientTeam[i].Team;
				//		PlayerInfo[i].OnTeam = FALSE;

				//		// Invert individual player team check
				//		if (ClientOptionEnabled[3][i])
				//			PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
				//	}
				//	else
				//	{
				//		// Save players team
				//		PlayerInfo[i].Team = cg_Ghosts->ClientTeam[i].Team;
				//		PlayerInfo[i].OnTeam = TRUE;

				//		// Invert individual player team check
				//		if (ClientOptionEnabled[3][i])
				//			PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;

				//		// If we want to draw friendlies
				//		if (!GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWFRIENDLIES))
				//			continue;
				//	}
				//}

				// Visible / Wallbang Check
				if (!PlayerInfo[i].OnTeam)
				{
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_Ghosts(0, &Entities_Ghosts[i], TagToShort("j_head"));

					// Get the players posistion for our visible check
					GetTagPosistion(&Entities_Ghosts[i], i, "j_head", (float*)&PlayerInfo[i].TagPosistion);

					// Run our trace
					trace_t Trace;
					CG_LocationalTraceGhosts(&Trace, &cg_Ghosts->refdef.ViewOrigin, &PlayerInfo[i].TagPosistion, i, 0x803003);

					// If the trace fraction is >= 0.97 let's say they are visible
					PlayerInfo[i].VisibleESP = ((Trace.Fraction >= 0.97f) ? TRUE : FALSE);
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_Ghosts(0, &Entities_Ghosts[i], TagToShort("j_head"));

					if (!GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL))
						PlayerInfo[i].Wallbangable = FALSE;
					//else
					//	PlayerInfo[i].Wallbangable = Autowall(i, "j_head");
					PlayerInfo[i].GVisibleESP = (PlayerInfo[i].Wallbangable || PlayerInfo[i].VisibleESP);
				}

				// If Whitelisted
				if (ClientOptionEnabled[2][i])
					PlayerInfo[i].OnTeam = TRUE;

				// Setup the colors for ESP
				Vector4 Color = PlayerInfo[i].OnTeam ? Vector4(0.51f, 1, 1, 0.50f) : GetColorCode(colorOrange); // Vector4(0.51f, 1, 1, 0.50f)

				// If Blacklisted
				if (ClientOptionEnabled[1][i])
					Color = Vector4(1, 1, 1, 0.50f);

				if (PlayerInfo[i].GVisibleESP)
				{
					if (!PlayerInfo[i].OnTeam)
					{
						if (!ClientOptionEnabled[1][i])
							Color = Vector4(1, 1, 0, 0.50f);

						if (PlayerInfo[i].Wallbangable && !PlayerInfo[i].VisibleESP)
							Color = Vector4(0.502f, 0, 0.502f, 0.50f);
					}
				}

				// ESP Snaplines
				if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) != "Off")
				{
					DrawSnapLine(Color, i, PlayerInfo[i].OnTeam);
				}

				// ESP Arrows
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DIRECTIONALARROWS))
				{
					DrawArrows((float*)&Color, i);
				}

				// ESP Bones
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWBONES))
				{
					DrawBones(Color, i);
				}

				// ESP Boxes
				if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) != "Off")
				{
					DrawESPBox(Color, i);
				}

				// ESP Weapon 
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWWEAPON))
				{
					Vector3 Pos[3], xy1[2], FeetPos, FeetPosOut;
					GetTagPosistion(&Entities_Ghosts[i], i, "j_helmet", (float*)&Pos[1]);
					FeetPos = Entities_Ghosts[i].Origin;

					//if (WorldToScreen(Pos[1], xy1[0]) && WorldToScreen(FeetPos, FeetPosOut))
					if (CG_CalcNamePosition(0, &Pos[1], &xy1[0].x, &xy1[0].y))
					{
						float aspectW, aspectH;
						int WeaponIcon = GetWeaponIcon(Entities_Ghosts[i].WeaponID, &aspectW, &aspectH);
						if (MmIsAddressValid((DWORD*)WeaponIcon))
							CG_DrawRotatedPicPhysical_Ghosts(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), xy1[0].x - (aspectW/2), xy1[0].y - (aspectH/2) - 10, aspectW, aspectH, 0.0, Color, WeaponIcon);

						//PCHAR WeaponName = GetWeaponHudName(Entities_Ghosts[i].CurrentWeapon);
						//if (MmIsAddressValid((PCHAR*)WeaponName))
						//	DrawText_Ghosts(WeaponName, (xy1[0].x - (R_TextWidth_Ghosts(WeaponName, 0x7FFFFFFF, SmallFont) / 2) * 0.5f), (xy1[0].y), 0, 0, SmallFont, Vector4(1, 1, 1, 0.50f), 0.5f, 0.5f);
					}
				}

				// ESP Name
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWNAME))
				{
					Vector3 HeadPos, HeadPosOut, FeetPos, FeetPosOut;
					GetTagPosistion(&Entities_Ghosts[i], i, "j_head", (float*)&HeadPos);
					FeetPos = Entities_Ghosts[i].Origin;

					//if (WorldToScreen(HeadPos, HeadPosOut) && WorldToScreen(FeetPos, FeetPosOut))
					if (CG_CalcNamePosition(0, &HeadPos, &HeadPosOut.x, &HeadPosOut.y) && CG_CalcNamePosition(0, &FeetPos, &FeetPosOut.x, &FeetPosOut.y))
					{
						if (MmIsAddressValid((PCHAR*)cg_Ghosts->ClientName[i].Name))
						{
							float DisanceOut;
							Vector3 FeetPosOutOG, HeadPosOutOG;
							VectorCopy(FeetPosOut, FeetPosOutOG);
							VectorCopy(HeadPosOut, HeadPosOutOG);

							FixTextForDistance(cg_Ghosts->ClientName[i].Name, &FeetPosOut.x, &FeetPosOut.y, SmallFont, 1, FeetPos, &DisanceOut);
							//FeetPosOut.y -= ((HeadPosOutOG.y - FeetPosOutOG.y) * 0.750);
							FeetPosOut.y += (((FeetPosOutOG.y - HeadPosOutOG.y) * (DisanceOut * 0.0001f) + 15));

							DrawText_Ghosts(cg_Ghosts->ClientName[i].Name, (FeetPosOutOG.x - (R_TextWidth_Ghosts(cg_Ghosts->ClientName[i].Name, 0x7FFFFFFF, SmallFont) / 2) * 0.4f), FeetPosOut.y, 0, 0, SmallFont, Color, 0.4f, 0.4f);
						}
					}
				}

				// Enemy Visible Text
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMYVISIBLE))
				{
					if (PlayerInfo[i].VisibleESP && !PlayerInfo[i].OnTeam)
					{
						DrawText_Ghosts("Visible Enemy!", ((cg_Ghosts->refdef.Width / 2) - (R_TextWidth_Ghosts("Visible Enemy!", 0x7FFFFFFF, HudSmallFont) / 2) * 0.7f), (cg_Ghosts->refdef.Height / 2) - 120, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
					}
				}

				// Proximity Warning Text
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWPROXIMITYWARNING))
				{
					if (!PlayerInfo[i].OnTeam)
					{
						float OutPos[3];
						GetTagPosistion(&Entities_Ghosts[i], i, "j_head", OutPos);
						if ((GetDistance(cg_Ghosts->refdef.ViewOrigin, OutPos) / 100 < 5))
						{
							DrawText_Ghosts("Proximity Warning!", ((cg_Ghosts->refdef.Width / 2) - (R_TextWidth_Ghosts("Proximity Warning!", 0x7FFFFFFF, HudSmallFont) / 2) * 0.7f), (cg_Ghosts->refdef.Height / 2) - 170, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
						}
					}
				}
			}

			// Continue if the client mumber is ours
			else continue;
		}

	}
}

void R_EndFrameHook()
{
	LocalClient_Ghosts = (pclientActive_t_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu17
	Entities_Ghosts = (pcentity_s_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu17
	cg_Ghosts = (pcg_s_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu17
	cgs_Ghosts = (pcgs_t_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu17

	// Setup the menu
	if (!menuReady)
	{
		SetupMenu();
	}

	// Monitor button presses for the menu
	MonitorButtons();

	// Draw our menu
	if (menuOpen)
	{
		DrawMenu_Ghosts();
	}
	else
	{
		menuWidth = 0;
		menuHeight = 0;
	}

	// Handles toggle options like no recoil, sway, uav, etc
	HandleToggleOptions(cgs_Ghosts);

	BOOL inGame = CheckInGame();
	// Draw our NiNJA Engine v1.0 text at the top right of screen pre-game
	if (!inGame && menuReady)
	{
		if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLENINJATEXT))
		{
			FLOAT X, Y;
			X = (FLOAT)*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY);
			Y = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY) + 0x4);
			DrawText_Ghosts("NiNJA Engine v1.0", ((X - 130) - (R_TextWidth_Ghosts("NiNJA Engine v1.0", 0x7FFFFFFF, SmallFont) / 2) * 0.5f), (Y / 2) - 330, 0, 0, SmallFont, Vector4(0.102f, 1, 0.102f, 0.50f), 0.5f, 0.5f);
		}
	}

	// Draw our menu controls
	DrawMenuControlsText();

	if (inGame && menuReady)
		DrawCustomRadar();

	// Call the original function
	R_EndFrameDetour->CallOriginal(0);
}

int Menu_PaintAllHook(int screenDisp, int r4, int r5)
{
	LocalClient_Ghosts = (pclientActive_t_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu17
	Entities_Ghosts = (pcentity_s_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu17
	cg_Ghosts = (pcg_s_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu17
	cgs_Ghosts = (pcgs_t_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu17

	// Call the original
	int Original = Menu_PaintAllDetour->CallOriginal(screenDisp, r4, r5);

	// Draw our ESP
	DrawESP();

	return Original;
}

PCHAR FindBestBoneByDamage(int EntIndex, BOOL HasRiotShield)
{
	Vector3 VisPos, MyPos;
	trace_t Trace;
	float totalDamage = 0;
	DWORD BoneIndex = 0;

	// Our hit location indexs
	BYTE HitLocationIndexs[6] = { 2, 1, 5, 4, 0x11, 0x10 };

	if (HasRiotShield)
	{
		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_Ghosts[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_Ghosts(0, cg_Ghosts, MyPos);
		CG_LocationalTraceGhosts(&Trace, &MyPos, &VisPos, EntIndex, 0x803003);

		// Check our trace fraction
		if (Trace.Fraction >= 0.95f)
			return "j_ankle_le";

		return NULL;
	}

	for (int i = 0; i < 6; i++)
	{
		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_Ghosts[EntIndex], EntIndex, VisCheckBones[i], (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_Ghosts(0, cg_Ghosts, MyPos);
		CG_LocationalTraceGhosts(&Trace, &MyPos, &VisPos, EntIndex, 0x803003);

		// Check our trace fraction
		if (Trace.Fraction >= 0.95f)
		{
			// Get weapon def
			PDWORD WeaponDef = BG_GetWeaponDef_Ghosts(Entities_Ghosts[cg_Ghosts->ClientNumber].WeaponID);

			// Get our temp damage from weaponDamage * HitLocationMultiplier
			float tempDamage = (float)*(int*)(WeaponDef + 0x230) * G_GetWeaponHitLocationMultiplier(HitLocationIndexs[i], Entities_Ghosts[cg_Ghosts->ClientNumber].WeaponID);
			if (tempDamage > totalDamage)
			{
				totalDamage = tempDamage;
				BoneIndex++;
			}
		}
	}

	if (totalDamage > 0)
		return VisCheckBones[BoneIndex];

	return NULL;
}

int FindBestVisibleBoneIndex(int EntIndex, BOOL HasRiotShield)
{
	Vector3 VisPos, MyPos;
	trace_t Trace;
	if (HasRiotShield)
	{
		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_Ghosts[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_Ghosts(0, cg_Ghosts, MyPos);
		CG_LocationalTraceGhosts(&Trace, &MyPos, &VisPos, EntIndex, 0x803003);

		// Check our trace fraction
		if (Trace.Fraction >= 0.95f)
			return 6;

		return -1;
	}

	for (int i = 0; i < 13; i++)
	{
		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_Ghosts[EntIndex], EntIndex, HitboxVisCheckBones[i], (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_Ghosts(0, cg_Ghosts, MyPos);
		CG_LocationalTraceGhosts(&Trace, &MyPos, &VisPos, EntIndex, 0x803003);

		// Check our trace fraction
		if (Trace.Fraction >= 0.95f)
			return i;

		continue;
	}

	return -1;
}

int FindBestAutoWallBoneIndex(int EntIndex, BOOL HasRiotShield)
{
	for (int i = 0; i < 7; i++)
	{
		BOOL Wallbangable = FALSE;

		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		Wallbangable = Autowall(EntIndex, AutoWallBones[i], HasRiotShield);
		if (Wallbangable)
			return i;

		continue;
	}

	return -1;
}

BOOL IsSecondaryShield(int ClientIndex)
{
	//DWORD ptr = *(DWORD*)0x82553764;
	//DWORD MW2_RefDef = (ptr - 0x98D0);
	//DWORD MW2_CclientInfo = ((DWORD)MW2_RefDef + 0x8C258);
	//DWORD dwStart = (MW2_CclientInfo + ((int)0x544 * ClientIndex));

	//DWORD Start = ((DWORD)dwStart + 0x58);
	//for (int i = 0; i < 5; i++)
	//{
	//	PCHAR WeaponModel = (PCHAR)(Start + (0x40 * i));
	//	if (!strcmp(WeaponModel, "weapon_riot_shield_mp"))
	//	{
	//		if (!(Entities_Ghosts[ClientIndex].weaponNum == CG_GetWeaponIndexForName("riotshield_mp")))
	//		{
	//			return true;
	//		}
	//
	//		return false;
	//	}
	//}
	return false;
}

void CalcPrediction(int clientIndex, Vector3 BonePos, int Scale)
{
	Vector3 EnemyVelocityTemp;
	Vector3 EnemyVelocity;

	VectorSubtract(*(Vector3*)((DWORD)&Entities_Ghosts[clientIndex] + 0x6C), *(Vector3*)((DWORD)&Entities_Ghosts[clientIndex] + 0xDC), EnemyVelocityTemp);

	EnemyVelocity.x = EnemyVelocityTemp[0];
	EnemyVelocity.y = EnemyVelocityTemp[1];
	EnemyVelocity.z = EnemyVelocityTemp[2];

	Vector3 CalcVec = EnemyVelocity.Normalize();

	CalcVec.x *= Scale;
	CalcVec.y *= Scale;
	CalcVec.z *= Scale;

	BonePos[0] += CalcVec.x;
	BonePos[1] += CalcVec.y;
}

DWORD FindBestTarget(BOOL UsingRagebot)
{
	DWORD dwClosestClient = -1;
	FLOAT Closest = 900000;
	eMenuNames AimbotMenu = UsingRagebot ? MENU_RAGEBOT : MENU_LEGITBOT;

	// Loop for all players
	for (int i = 0; i < cgs_Ghosts->MaxClients; i++) //
	{
		BOOL Visible = FALSE, Wallbangable = FALSE, UsingShield = FALSE;
		CHAR PlayerWeaponName[100];

		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_Ghosts->ClientNumber, UsingRagebot))
			continue;

		// Check the user for a shield
		GetWeaponHUDName_Ghosts(Entities_Ghosts[i].WeaponID, 0, PlayerWeaponName, 0x400);
		if (strstr(PlayerWeaponName, "riotshield"))
			UsingShield = TRUE;

		// If we want to ignore shields
		if (!GetMenuOptionValue(AimbotMenu, OPTION_TARGETSHIELDS) && UsingShield)
		{
			PlayerInfo[i].Wallbangable = FALSE; // Do this here since it doesn't get updated if we continue
			continue;
		}

		// Get our bone
		if (GetMenuOptionString(AimbotMenu, OPTION_AIMBONE) == "Best")
		{
			AimTag = "j_head";
			int BoneIndex = FindBestVisibleBoneIndex(i, UsingShield);
			if (BoneIndex == -1)
			{
				Visible = FALSE;
			}
			else
			{
				Visible = TRUE;
				AimTag = HitboxVisCheckBones[BoneIndex];
			}
		}
		else
		{
			AimTag = GetMenuOptionString(AimbotMenu, OPTION_AIMBONE);
		}

		
		// Get the players posistion for our visible check
		if (!Visible)
		{
			Vector3 VisPos, MyPos;
			GetTagPosistion(&Entities_Ghosts[i], i, AimTag, (float*)&VisPos);

			// Run our trace
			trace_t Trace;
			CG_GetPlayerViewOrigin_Ghosts(0, cg_Ghosts, MyPos);
			CG_LocationalTraceGhosts(&Trace, &MyPos, &VisPos, i, 0x803003);

			// Check our trace fraction
			Visible = ((Trace.Fraction >= 0.95f) ? TRUE : FALSE);
			if (!Visible && UsingRagebot)
			{
				// If they are prioritized, check multiple bones
				if (ClientOptionEnabled[1][i] && (GetMenuOptionString(MENU_RAGEBOT, OPTION_AIMBONE) == "Best"))
				{
					// Wallbang check 8 bones for prioritized users
					int AWallBoneIndex = FindBestAutoWallBoneIndex(i, UsingShield);
					if (AWallBoneIndex == -1)
					{
						Wallbangable = FALSE;
					}
					else
					{
						AimTag = AutoWallBones[AWallBoneIndex];
						Wallbangable = TRUE;
					}
		
				}
				else
				{
					// If they aren't visible, try to wallbang them
					Wallbangable = Autowall(i, AimTag, UsingShield);
					if (GetMenuOptionString(MENU_RAGEBOT, OPTION_AIMBONE) == "Best")
					{
						//if (!Wallbangable)
						//	Wallbangable = Autowall(i, "j_mainroot", UsingShield);
						//if (Wallbangable)
						//	AimTag = "j_mainroot";
					}
				}

				// Check a 2nd bone because the head is missed in various places
				/*if (!Wallbangable)
					Wallbangable = Autowall(i, "j_mainroot", UsingShield);
				if (Wallbangable)
					AimTag = "j_mainroot";

				// If they have anti-aim, lets check another bone
				if (ClientOptionEnabled[5][i])
				{
					if (!Wallbangable)
						Wallbangable = Autowall(i, "j_spineupper", UsingShield);
					if (Wallbangable)
						AimTag = "j_spineupper";
				}*/

				PlayerInfo[i].Wallbangable = Wallbangable;
			}
		}

		// Verify all were true
		if (UsingRagebot)
		{
			if (!Visible && !Wallbangable)
				continue;
		}
		else
		{
			// Make sure they are within our FOV for Legitbot
			BOOL WithinFOV = FOVCheck(i, AimTag);

			Visible = (Visible && WithinFOV);

			if (!Visible)
				continue;
		}

		// Grab our temp distance
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_Ghosts[i].Origin, (float*)&cg_Ghosts->refdef.ViewOrigin));

		// For now just make them seem closest
		if (ClientOptionEnabled[1][i])
			TempDistance = 5.0f;

		// Update our closest player info
		if (TempDistance < Closest)
		{
			dwClosestClient = i;
			Closest = TempDistance;
		}
	}

	return dwClosestClient;
}

DWORD FindBestAntiAimTarget()
{
	DWORD dwAAClosestClient = -1;
	FLOAT ClosestAA = 900000;

	// Loop for all players
	for (int i = 0; i < cgs_Ghosts->MaxClients; i++)
	{
		BOOL Visible = FALSE, Wallbangable = FALSE, UsingShield = FALSE;

		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_Ghosts->ClientNumber, NULL))
			continue;

		// Grab our temp distance
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_Ghosts[i].Origin, (float*)&cg_Ghosts->refdef.ViewOrigin));

		// For now just make them seem closest
		if (ClientOptionEnabled[1][i])
			TempDistance = 5.0f;

		// Update our closest player info
		if (TempDistance < ClosestAA)
		{
			dwAAClosestClient = i;
			ClosestAA = TempDistance;
		}
	}

	return dwAAClosestClient;
}

VOID Ragebot(pusercmd_s_Ghosts usercommand, float movementsave)
{
	Vector3 EnemyPosistion, MyOrigin, Angles, VectorAngles, FinalAngles, SpreadAngles;
	PCHAR AimbotTag = "j_head";
	CHAR TargetWeaponName[100];

	// Get our best target
	DWORD BestTarget = FindBestTarget(TRUE);
	GClosestClient = BestTarget;
	if (BestTarget == -1)
		return;

	// If we have dual wield, flip which gun we should shoot every frame
	if (IsWeaponDualWield())
		FiringDualWield = !FiringDualWield;

	// Use Right Ankle for shield users
	GetWeaponHUDName_Ghosts(Entities_Ghosts[BestTarget].WeaponID, 0, TargetWeaponName, 0x400);
	if (strstr(TargetWeaponName, "Shield") && (GetMenuOptionValue(MENU_RAGEBOT, OPTION_TARGETSHIELDS) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_TARGETSHIELDS)))
		AimTag = "j_ankle_ri";

	// Use Left Ankle for anti-aim shield users
	if (IsSecondaryShield(BestTarget) && (ClientOptionEnabled[5][BestTarget]))
		AimTag = "j_ankle_le";

	// Get the best targets posistion
	GetTagPosistion(&Entities_Ghosts[BestTarget], BestTarget, AimTag, (float*)&EnemyPosistion);

	// Calculate prediction if we want it
	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) != "Off")
		CalcPrediction(BestTarget, EnemyPosistion, FindPredicationScale());

	// Get our view origin
	CG_GetPlayerViewOrigin_Ghosts(0, cg_Ghosts, MyOrigin);

	// Calculate aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles);
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_Ghosts->SpawnAngles, FinalAngles);
	//AngleNormalize(FinalAngles);

	// Set our angles to our aimbot angles
	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_RAGEBOTTYPE) == "Silent")
	{
		usercommand->angles[0] = ANGLE2SHORT(FinalAngles.x);
		usercommand->angles[1] = ANGLE2SHORT(FinalAngles.y);

		// Fix our movement
		CorrectMovement(usercommand, FinalAngles);
	}
	else if (GetMenuOptionString(MENU_RAGEBOT, OPTION_RAGEBOTTYPE) == "Normal")
	{
		LocalClient_Ghosts->ViewAngles[0] = FinalAngles.x;
		LocalClient_Ghosts->ViewAngles[1] = FinalAngles.y;
	}

	// Do our no spread
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD))
	{
		// Get our seed from the current server time
		int Seed = usercommand->serverTime;

		// If dual wield, +10 to the seed (thanks brian)
		if (IsWeaponDualWield())
		{
			if (FiringDualWield)
			{
				Seed += 10;
			}
		}

		// Transform seed
		((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.TransformSeed))(&Seed);

		// Calculate zero spread
		GetSpread_Ghosts(&Seed, &SpreadAngles.x, &SpreadAngles.y);

		// Apply our no spread
		usercommand->angles[0] -= ANGLE2SHORT(-SpreadAngles.y);
		usercommand->angles[1] -= ANGLE2SHORT(-SpreadAngles.x);
	}

	// Autoshoot
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT))
	{
		if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD))
		{
			if (IsWeaponDualWield() && FiringDualWield)
			{
				usercommand->buttons |= 0x00080800;
			}
			else
			{
				usercommand->buttons |= 1;
			}
		}
		else
		{
			// If not using no spread, wait until aimed in before shooting
			float ZoomState = (*(int*)((DWORD)&cg_Ghosts + 0x424));
			if (ZoomState >= 0.3f)
			{
				if (IsWeaponDualWield() && FiringDualWield)
					usercommand->buttons |= 0x00080800;
				else
					usercommand->buttons |= 1;
			}
		}
	}
}

VOID Legitbot(pusercmd_s_Ghosts usercommand)
{
	Vector3 EnemyPosistion, MyOrigin, Angles, VectorAngles, FinalAngles, LegitAngles;
	BOOL DelayDone = FALSE;
	XINPUT_STATE State;

	// Get our best target
	DWORD BestTarget = FindBestTarget(FALSE);
	GClosestClient = BestTarget;
	if (BestTarget == -1)
		return;

	// Update our input state
	if (FAILED(XInputGetState(0, &State)))
		return;

	// Get the best targets posistion
	GetTagPosistion(&Entities_Ghosts[BestTarget], BestTarget, GetMenuOptionString(MENU_LEGITBOT, OPTION_AIMBONE), (float*)&EnemyPosistion);

	// Get our view origin
	CG_GetPlayerViewOrigin_Ghosts(0, cg_Ghosts, MyOrigin);

	// Calc our aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles); 
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_Ghosts->SpawnAngles, FinalAngles);
	//AngleNormalize(FinalAngles);

	// Get our smooth angles
	LegitAngles = SmoothAimbotAngles(FinalAngles, FindSmoothScale());

	// Apply our smooth angles if we are aimed in
	DWORD ReactionDelay = FindReactionDelayAmount();
	PCHAR AimKeyVal = GetMenuOptionString(MENU_LEGITBOT, OPTION_AIMKEY);
	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_REACTIONDELAY) != "Off")
	{
		// If we want a reaction delay
		if ((GReactionDelay > ReactionDelay))
		{
			// Set our angles
			if (AimKeyVal == "None" ||
				AimKeyVal == "LT" && (State.Gamepad.bLeftTrigger > 35) ||
				AimKeyVal == "RT" && (usercommand->buttons & 1) ||
				AimKeyVal == "LT & RT" && (usercommand->buttons & 1 && State.Gamepad.bLeftTrigger > 35) ||
				AimKeyVal == "DPAD UP" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ||
				AimKeyVal == "DPAD RIGHT" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ||
				AimKeyVal == "DPAD DOWN" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ||
				AimKeyVal == "DPAD LEFT" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ||
				AimKeyVal == "A" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_A) ||
				AimKeyVal == "B" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_B) ||
				AimKeyVal == "X" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_X) ||
				AimKeyVal == "Y" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ||
				AimKeyVal == "RB" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
				AimKeyVal == "LB" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ||
				AimKeyVal == "BACK" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ||
				AimKeyVal == "RS" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ||
				AimKeyVal == "LS" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB))
			{
				LocalClient_Ghosts->ViewAngles[0] = LegitAngles.x;
				LocalClient_Ghosts->ViewAngles[1] = LegitAngles.y;
			}
		}
		else
		{
			GReactionDelay++;
			return;
		}
	}
	else
	{
		// Set our angles
		if (AimKeyVal == "None" ||
			AimKeyVal == "LT" && (State.Gamepad.bLeftTrigger > 35) ||
			AimKeyVal == "RT" && (usercommand->buttons & 1) ||
			AimKeyVal == "LT & RT" && (usercommand->buttons & 1 && State.Gamepad.bLeftTrigger > 35) ||
			AimKeyVal == "DPAD UP" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ||
			AimKeyVal == "DPAD RIGHT" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ||
			AimKeyVal == "DPAD DOWN" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ||
			AimKeyVal == "DPAD LEFT" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ||
			AimKeyVal == "A" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_A) ||
			AimKeyVal == "B" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_B) ||
			AimKeyVal == "X" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_X) ||
			AimKeyVal == "Y" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ||
			AimKeyVal == "RB" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
			AimKeyVal == "LB" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ||
			AimKeyVal == "BACK" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ||
			AimKeyVal == "RS" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ||
			AimKeyVal == "LS" && (State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB))
		{
			LocalClient_Ghosts->ViewAngles[0] = LegitAngles.x;
			LocalClient_Ghosts->ViewAngles[1] = LegitAngles.y;
		}
	}

	// Autoshoot
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOSHOOT) || ((GetMenuOptionValue(MENU_LEGITBOT, OPTION_TRIGGERBOT) && TriggerBot(BestTarget))))
	{
		// Wait until aimed in before shooting
		if (*(float*)((DWORD)cg_Ghosts + 0x424) >= 0.3f)
		{
			usercommand->buttons |= 1;

			if (IsWeaponDualWield())
				usercommand->buttons |= 0x00080800;
		}
	}
}

VOID Spinbot(usercmd_s_Ghosts *UserCommand, BOOL RagebotEnabled)
{
	int RandomAdd = rand() % 71;
	int RandomUpDownSpeed = rand() % 71;
	int RandomLeft = rand() % 360;
	int RandomRight = rand() % -360;
	Vector3 MyPos;


	// Y Right
	SpinbotSpin = SpinbotSpin - FindYSpinSpeed();
	if (SpinbotSpin < -360)
		SpinbotSpin = 0;

	// Y Left
	SpinbotSpinFlip = SpinbotSpinFlip + FindYSpinSpeed();
	if (SpinbotSpinFlip > 360)
		SpinbotSpinFlip = 0;

	// Z Left
	SpinbotSpinZ = SpinbotSpinZ - FindZSpinSpeed();
	if (SpinbotSpinZ < -360)
		SpinbotSpinZ = 0;

	// Z Right
	SpinbotSpinZR = SpinbotSpinZR + FindZSpinSpeed();
	if (SpinbotSpinZR > 360)
		SpinbotSpinZR = 0;

	// If we aren't prone, not mantling, not dead, not climbing a ladder, not pressing LB or RB
	if ((*(int*)((DWORD)&Entities_Ghosts[cg_Ghosts->ClientNumber] + 0x1C4) & 0x1) == 0 || UserCommand->weapon == 0 || Entities_Ghosts[cg_Ghosts->ClientNumber].Stance & 0x08 || (cg_Ghosts->Health <= 0) || (UserCommand->buttons & 0x8000) || (UserCommand->buttons & 0x4000))
		return;

	DWORD BestAAClient = FindBestAntiAimTarget();
	Vector3 EnemyPos, MyOrigin, VectorAngles, FinalAngles, Angles;

	// Get our view origin / players posistion
	CG_GetPlayerViewOrigin_Ghosts(0, cg_Ghosts, MyOrigin);

	// Make sure they are valid
	if (!ValidPlayer(BestAAClient, cg_Ghosts->ClientNumber, RagebotEnabled))
		return;

	// Get player posistion
	if (!GetTagPosistion(&Entities_Ghosts[BestAAClient], BestAAClient, "j_mainroot", (float*)&EnemyPos))
		return;

	// Calculate aimbot angles
	VectorSubtract(EnemyPos, MyOrigin, Angles);
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_Ghosts->SpawnAngles, FinalAngles);

	int AAAngleX = UserCommand->angles[0];
	int AAAngleY = UserCommand->angles[1];

	// Anti-Aim X Look down
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Look Down")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(ffAngleNormalize(60.0f + ShortToAngle(UserCommand->angles[0]) - RefDefAngles.x)); //ANGLE2SHORT(72.0f);
		UserCommand->angles[0] = ANGLE2SHORT(69.99 - LocalClient_Ghosts->SpawnAngles.x);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Up & Down")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		int RandomShouldFlip = rand() % 12;
		if (RandomShouldFlip > 5)
		{
			UserCommand->angles[0] = ANGLE2SHORT(69.99 - LocalClient_Ghosts->SpawnAngles.x);
			SpinbotFlipX = TRUE;
		}
		else
		{
			UserCommand->angles[0] = ANGLE2SHORT(-45 - LocalClient_Ghosts->SpawnAngles.x);
			SpinbotFlipX = FALSE;
		}

	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Fake Up")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		if (flip)
		{
			UserCommand->angles[0] = ANGLE2SHORT(-69.99 - LocalClient_Ghosts->SpawnAngles.x);
		}
		else
		{
			UserCommand->angles[0] = ANGLE2SHORT(69.99 - LocalClient_Ghosts->SpawnAngles.x);
		}
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Detection")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 0);

		UserCommand->angles[0] = ANGLE2SHORT(69.99 - LocalClient_Ghosts->SpawnAngles.x);

		// If we took damage or they are wallbangable / visible
		if ((cg_Ghosts->Health < OurHealth) || PlayerInfo[BestAAClient].Wallbangable)
		{
			// Move our body move to try to avoid bullets
			//UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - (SpinbotFlip ? 80.0f : -90.0f));
			UserCommand->angles[0] = ANGLE2SHORT(rand() % 65 - LocalClient_Ghosts->SpawnAngles.x);
			SpinbotFlip = !SpinbotFlip;
		}

		// Update our health
		OurHealth = cg_Ghosts->Health;

	}

	// Anti-Aim Y Flip Flop
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Flip Flop")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		if (SpinbotFlip)
		{
			UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f + RandomLeft);
			SpinbotFlip = FALSE;
		}
		else
		{
			UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f - RandomLeft);
			SpinbotFlip = TRUE;
		}

	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Face Away")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (SpinbotFlip)
		{
			UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f + rand() % 30);
			SpinbotFlip = FALSE;
		}
		else
		{
			UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f - rand() % 30);
			SpinbotFlip = TRUE;
		}
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Jitter")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (BestAAClient != -1)
		{
			if (flip)
				AAAngleY -= ANGLE2SHORT(270.0f);

			else
				AAAngleY -= ANGLE2SHORT(90.0f);
		}
		else
		{
			if (flip)
				AAAngleY = ANGLE2SHORT(270.0f);

			else
				AAAngleY = ANGLE2SHORT(90.0f);
		}

		UserCommand->angles[1] = AAAngleY;
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Jitter Synced")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (LocalClient_Ghosts->CommandNumber % 3)
			UserCommand->angles[1] = UserCommand->angles[1] + ANGLE2SHORT(200.0);
		else
		{
			if (flip)
			{
				if (flip2)
					UserCommand->angles[1] += ANGLE2SHORT(90.0f);

				else
					UserCommand->angles[1] -= ANGLE2SHORT(90.0f);
			}
			else
			{
				UserCommand->angles[1] += ANGLE2SHORT(180.0f);
			}
		}
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Sideways")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - 100.0f);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Detection")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		// If we took damage or they are wallbangable / visible
		if ((cg_Ghosts->Health < OurHealth) || PlayerInfo[BestAAClient].Wallbangable)
		{
			// Move our body move to try to avoid bullets
			//UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - (SpinbotFlip ? 80.0f : -90.0f));
			UserCommand->angles[1] += ANGLE2SHORT(rand() % 360);
			SpinbotFlip = !SpinbotFlip;
		}
		else
		{
			if (SpinbotFlip)
			{
				UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f + RandomLeft);
				SpinbotFlip = FALSE;
			}
			else
			{
				UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f - RandomLeft);
				SpinbotFlip = TRUE;
			}
		}

		// Update our health
		OurHealth = cg_Ghosts->Health;
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Angel Backwards")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		//int originalAngle = UserCommand->ViewAngles[1];
		UserCommand->angles[1] = (FinalAngles.y - 180.0f) + ANGLE2SHORT(36000180.0);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Static")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (flip)
		{
			if (flip2)
				UserCommand->angles[1] += ANGLE2SHORT(90.0f);

			else
				UserCommand->angles[1] -= ANGLE2SHORT(90.0f);
		}
		else
		{
			UserCommand->angles[1] += ANGLE2SHORT(180.0f);
		}
	}

	// Anti-Aim X Look Up (Riot Shield)
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Riot Shield")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(72.0f);
		UserCommand->angles[0] = ANGLE2SHORT(-69.99 - LocalClient_Ghosts->SpawnAngles.x);
	}

	// Anti-Aim Y 180 Flip From Enemy (Riot Shield)
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Riot Shield")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[1] = ANGLE2SHORT(InverseNorm(AAX - 175.0f));
		UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f);
	}

	// X Spin Direction
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION) == "Look Down")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(ffAngleNormalize(60.0f + ShortToAngle(UserCommand->angles[0]) - RefDefAngles.x));
		UserCommand->angles[0] = ANGLE2SHORT(69.99 - LocalClient_Ghosts->SpawnAngles.x);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION) == "Look Up")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(ffAngleNormalize(-60.0f + ShortToAngle(UserCommand->angles[0]) - RefDefAngles.x));
		UserCommand->angles[0] = ANGLE2SHORT(-69.99 - LocalClient_Ghosts->SpawnAngles.x);
	}

	// Y Spin Direction
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION) == "Left")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);


		UserCommand->angles[1] = ANGLE2SHORT(SpinbotSpinFlip);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION) == "Right")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);


		UserCommand->angles[1] = ANGLE2SHORT(SpinbotSpin);
	}

	// Z Spin Direction
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION) == "Right")
	{
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		UserCommand->angles[2] = ANGLE2SHORT(SpinbotSpinZR);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION) == "Left")
	{
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		UserCommand->angles[2] = ANGLE2SHORT(SpinbotSpinZ);
	}


	// Random Z Spinning When Anti-Aim Is Selected
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) != "Off" && GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) != "Off")
		UserCommand->angles[2] = ANGLE2SHORT(SpinbotSpin);

	LocalClient_Ghosts->ViewAngles.z = -LocalClient_Ghosts->SpawnAngles.z;
}

void CL_CreateNewCommandsHook(int r3)
{
	LocalClient_Ghosts = (pclientActive_t_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu17
	Entities_Ghosts = (pcentity_s_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu17
	cg_Ghosts = (pcg_s_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu17
	cgs_Ghosts = (pcgs_t_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu17
	XINPUT_STATE State;

	// Call the original 1st
	CL_CreateNewCommandsDetour->CallOriginal(r3);

	// If we are ingame, start our aimbot
	if (CheckInGame() && (GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT) || GetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT)))
	{
		pusercmd_s_Ghosts OldUserCommand = LocalClient_Ghosts->GetUserCommand(LocalClient_Ghosts->GetUserCommandNumber() - 1);
		pusercmd_s_Ghosts CurrentUserCommand = LocalClient_Ghosts->GetUserCommand(LocalClient_Ghosts->GetUserCommandNumber());
		pusercmd_s_Ghosts NextUserCommand = LocalClient_Ghosts->GetUserCommand(LocalClient_Ghosts->GetUserCommandNumber() + 1);
		*NextUserCommand = *CurrentUserCommand;
		CurrentUserCommand->serverTime -= 1;
		LocalClient_Ghosts->CommandNumber += 1;

		// Save our angle for movement fix
		float MovementSave = SHORT2ANGLE(CurrentUserCommand->angles[1]);

		// Reset our player info structure when we enter a game
		if (!DroppedAllTargets)
		{
			for (int Target = 0; Target < cgs_Ghosts->MaxClients; Target++)
			{
				DropTarget(PlayerInfo[Target]);
			}

			DroppedAllTargets = TRUE;
		}

		// If Ragebot aimbot mode
		if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT))
			Ragebot(CurrentUserCommand, MovementSave);

		// If Legitbot aimbot mode
		if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT))
			Legitbot(CurrentUserCommand);

		// Rapid Fire
		if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_RAPIDFIRE) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_RAPIDFIRE))
		{
			if (FAILED(XInputGetState(0, &State)))
				goto Return;

			if (TriggerValue > 25)
			{
				if (RapidFireCtr > 2)
					RapidFireCtr = 0;

				if (RapidFireCtr == 0)
					CurrentUserCommand->buttons |= 1;
			}

			RapidFireCtr++;
		}
	}

Return:
	{
		return;
		//return CL_CreateNewCommandsDetour->CallOriginal(r3);
	}
}

int CL_SendCmdHook(int r3)
{
	LocalClient_Ghosts = (pclientActive_t_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu17
	Entities_Ghosts = (pcentity_s_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu17
	cg_Ghosts = (pcg_s_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu17
	cgs_Ghosts = (pcgs_t_Ghosts)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu17

	// Update our input state
	if (!menuReady)
		goto Return;

	if (CheckInGame())
	{
		// Do our server time 
		pusercmd_s_Ghosts OldUserCommand = LocalClient_Ghosts->GetUserCommand(LocalClient_Ghosts->GetUserCommandNumber() - 1);
		pusercmd_s_Ghosts CurrentUserCommand = LocalClient_Ghosts->GetUserCommand(LocalClient_Ghosts->GetUserCommandNumber());
		pusercmd_s_Ghosts NextUserCommand = LocalClient_Ghosts->GetUserCommand(LocalClient_Ghosts->GetUserCommandNumber() + 1);

		// Get Ragebot status
		BOOL Ragebot = GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT);

		// Do our spinbot
		if (GetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT))
			Spinbot(CurrentUserCommand, Ragebot);


		// Handle fake lag
		if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) != "Off")
		{
			FakeLagCount++;
			if (FakeLagCount >= PacketSkipAmount || (CurrentUserCommand->buttons != 0) || (OldUserCommand->buttons != 0) || (NextUserCommand->buttons != 0))
			{
				PacketSkipAmount = FindFakeLagAmount();
				FakeLagCount = 0;
			}
		}
		else
		{
			FakeLagCount = 0;
		}

	}
	else
	{
		ShouldSendPacket = TRUE;
		DroppedAllTargets = FALSE;
		FakeLagCount = 0;
	}

Return:
	{
		if (FakeLagCount == 0)
			return CL_SendCmdDetour->CallOriginal(r3);
		else
			return FALSE;
	}
}

int PlayLocalSoundAliasByNameHook(int localClientNum, char *aliasname, int a0)
{
	//DbgPrint("Name: %s", aliasname);

	// Disable main menu music
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUMUSIC))
	{
		if (!strcmp(aliasname, "music_mainmenu_mp"))
			return 0;
	}

	return PlayLocalSoundAliasByNameDetour->CallOriginal(localClientNum, aliasname, a0);
}

int getStatIndex(int msgt)
{
	return *(int*)(*(int*)(msgt + 8) + 1);
}

BYTE* getStatValue(int msgt)
{
	return (BYTE*)(*(int*)(msgt + 8) + 6);
}

void EngineDrawRadarHook(DWORD localClient, DWORD mapType, DWORD size, DWORD rectDef, DWORD shader, const float *color, DWORD unk1, DWORD unk2, DWORD unk3, float f1, float f2, float f3, float f4, float f5, float f6)
{
	bool inGame = CheckInGame();
	if (inGame && mapType == 0 && GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR))
	{
		LastCompassType = mapType;
		CRectDef *rd = (CRectDef*)(rectDef);
		memcpy(&lastCompass, (PVOID)rectDef, sizeof(CRectDef));
		DrawCompassTimer = GetTickCount() + 30;

		*(DWORD*)0x8220DD98 = 0x60000000; // DrawFriendlies
		*(DWORD*)0x8220DDB0 = 0x60000000; // DrawEnemies
		*(DWORD*)0x8220DE04 = 0x60000000; // DrawPlayer
	}
	else
	{
		*(DWORD*)0x8220DD98 = 0x48056CA1; // DrawFriendlies
		*(DWORD*)0x8220DDB0 = 0x48057499; // DrawEnemies
		*(DWORD*)0x8220DE04 = 0x4805303D; // DrawPlayer
	}
	EngineDrawRadarDetour->CallOriginal(localClient, mapType, size, rectDef, shader, color, unk1, unk2, unk3, f1, f2, f3, f4, f5, f6);
}

void LiveStats_WriteStatHook(int localClientNum, int msg_t)
{
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_ANTIDERANK) && CheckInGame())
	{
		// Some hash
		if (getStatIndex(msg_t) < 8)
			return;

		// Prestige
		if (getStatIndex(msg_t) == 0x4F24 || getStatIndex(msg_t) == 0x09)
			return;

		// Experience
		if (getStatIndex(msg_t) == 0x6D)
			return;

		// Prestige Tokens
		if (getStatIndex(msg_t) == 0x1D)
			return;

		// Unlock Tokens
		if (getStatIndex(msg_t) == 0x4C2C)
			return;
	}

	return LiveStats_WriteStatDetour->CallOriginal(localClientNum, msg_t);
}

/*HRESULT GenerateTexture(D3DDevice* Device, D3DTexture** Texture, DWORD Color)
{
	if (FAILED(Device->CreateTexture(4, 4, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, Texture, NULL)))
		return E_FAIL;

	WORD Color16 = ((WORD)((Color >> 28) & 0xF) << 12) | (WORD)(((Color >> 20) & 0xF) << 8) | (WORD)(((Color >> 12) & 0xF) << 4) | (WORD)(((Color >> 4) & 0xF) << 0);

	D3DLOCKED_RECT d3dlr;
	(*Texture)->LockRect(0, &d3dlr, 0, 0);

	WORD *Dst16 = (WORD*)d3dlr.pBits;

	for (INT xy = 0; xy < 8 * 8; xy++)
		*Dst16++ = Color16;

	(*Texture)->UnlockRect(0);

	return S_OK;
}

DWORD GetTeamNumberFromShader(PCHAR pShader)
{
	int playerTeam = 0;

	// Check for team 1 shader
	if (strstr(pShader, "tf_141_") || (strstr(pShader, "_ally_tf141_") || (strstr(pShader, "udt") || (strstr(pShader, "sniper_ghillie") || strstr(pShader, "us_army_") || (strstr(pShader, "seal_"))))))
		playerTeam = 1;

	// Check for team 2 shader
	else if (strstr(pShader, "opforce") || (strstr(pShader, "militia") || (strstr(pShader, "airborne_") || (strstr(pShader, "op_arctic") || (strstr(pShader, "arctic_") || strstr(pShader, "arab") || strstr(pShader, "head_suren") || (strstr(pShader, "op_ghille")))))))
		playerTeam = 2;

	return playerTeam;
}

static IDirect3DDevice9* gDevice = NULL;
void R_DrawXModelSkinnedCachedHook(int context, int modelSurf, int unk)
{
	char* ModelName = *(char**)*(int*)(context + 0xBC);
	gDevice = (D3DDevice*)(*(int*)(context + 0x90));
	int drawPlayer = 0;

	if (strstr(ModelName, "tf_141_") || (strstr(ModelName, "_ally_tf141_") || (strstr(ModelName, "udt") || (strstr(ModelName, "sniper_ghillie") || strstr(ModelName, "us_army_") || (strstr(ModelName, "seal_"))))))
	{
		drawPlayer = 1;
		D3DDevice_SetRenderState_Ghosts(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_Ghosts(gDevice, D3DRS_ZENABLE, D3DZB_FALSE);
		R_DrawXModelSkinnedCachedDetour->CallOriginal(context, modelSurf, unk);
		D3DDevice_SetRenderState_Ghosts(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_Ghosts(gDevice, D3DRS_ZENABLE, D3DZB_TRUE);
	}

	else if (strstr(ModelName, "opforce") || (strstr(ModelName, "militia") || (strstr(ModelName, "airborne_") || (strstr(ModelName, "op_arctic") || (strstr(ModelName, "arctic_") || strstr(ModelName, "arab") || strstr(ModelName, "head_suren") || (strstr(ModelName, "op_ghille")))))))
	{
		drawPlayer = 2;
		D3DDevice_SetRenderState_Ghosts(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_Ghosts(gDevice, D3DRS_ZENABLE, D3DZB_FALSE);
		R_DrawXModelSkinnedCachedDetour->CallOriginal(context, modelSurf, unk);
		D3DDevice_SetRenderState_Ghosts(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_Ghosts(gDevice, D3DRS_ZENABLE, D3DZB_TRUE);
	}

	R_DrawXModelSkinnedCachedDetour->CallOriginal(context, modelSurf, unk);
}

BOOL DidTextures = FALSE;
HRESULT R_DrawIndexedPrimitiveHook(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT StartIndex, UINT IndexCount, char *pShader, int linkRet)
{
	int playerTeam = 0;

	// Make sure the menu is ready
	if (!menuReady)
		goto Return;

	// If we want chams
	if (GetMenuOptionString(MENU_DRAWING, "Chams:") != "Off")
	{
		// Get players team number based on their shaders
		if (pShader != NULL)
			playerTeam = GetTeamNumberFromShader(pShader);

		// Make sure it's for the players
		if (linkRet == 0x82398344)
		{
			// Generate our textures
			if (!OrangeTexture || !LightBlueTexture)
			{
				if (!DidTextures)
				{
					GenerateTexture(pDevice, &OrangeTexture, D3DCOLOR_RGBA(230, 126, 34, 127));
					GenerateTexture(pDevice, &LightBlueTexture, D3DCOLOR_RGBA(52, 152, 219, 255));
					DidTextures = TRUE;
				}
			}

			// Team 1
			if (playerTeam == 1)
			{
				if (GetMenuOptionString(MENU_DRAWING, "Chams:") == "Wireframe")
					pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

				pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
				pDevice->SetTexture(0, LightBlueTexture);
				R_DrawIndexedPrimitiveDetour->CallOriginal(pDevice, PrimitiveType, BaseVertexIndex, StartIndex, IndexCount);
				pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			}

			// Team 2
			if (playerTeam == 2)
			{
				if (GetMenuOptionString(MENU_DRAWING, "Chams:") == "Wireframe")
					pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

				pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
				pDevice->SetTexture(0, OrangeTexture);
				R_DrawIndexedPrimitiveDetour->CallOriginal(pDevice, PrimitiveType, BaseVertexIndex, StartIndex, IndexCount);
				pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			}
		}
	}

Return:
	{
		if (playerTeam == 0)
			return R_DrawIndexedPrimitiveDetour->CallOriginal(pDevice, PrimitiveType, BaseVertexIndex, StartIndex, IndexCount);
	}
}

HRESULT DrawIndexedPrimitive(INT pDevice, INT Args)
{
	int linkRet;
	_asm mflr linkRet;

	char* pShader = **(char ***)((pDevice - 0x90) + 0xBC);
	D3DDevice *Device = *(D3DDevice **)(pDevice);

	return ((int(*)(...))0x820AB1C0)(Device, 4, 0, *(int*)(Args + 0x8), (*(int*)(Args + 0x4)) + ((*(int*)(Args + 0x4)) << 1), pShader, linkRet);
}*/

void SetupOffsets()
{
	va_Ghosts = (CONST PCHAR(__cdecl *)(CONST PCHAR, ...))getCryptedCoD(CheatPatches->CoDPatches.VA);//tu17
	Dvar_GetBool_Ghosts = (BOOL(__cdecl *)(CONST PCHAR))getCryptedCoD(CheatPatches->CoDPatches.Dvar_GetBool);//tu17
	CG_LocationalTraceGhosts = (void(_cdecl*)(void *, Vector3 *, Vector3 *, int, int))getCryptedCoD(CheatPatches->CoDPatches.CG_LocationalTrace);//tu17
	DB_FindXAssetHeader_Ghosts = (DWORD(__cdecl *)(XAssetType_Ghosts, CONST PCHAR, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DB_FindXAssetHeader);//tu17
	R_TextWidth_Ghosts = (DWORD(__cdecl *)(CONST PCHAR, DWORD, DWORD))getCryptedCoD(CheatPatches->CoDPatches.R_TextWidth);//tu17
	CL_DrawTextPhysical_Ghosts = (VOID(__cdecl*)(CONST PCHAR, DWORD, DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.CL_DrawTextPhysical);//tu17
	R_AddCmdDrawStretchPic_Ghosts = (VOID(__cdecl*)(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.R_AddCmdDrawStretchPic);//tu17
	CG_DrawRotatedPicPhysical_Ghosts = (VOID(__cdecl*)(DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.UI_FillRect);//tu17
	UI_FillRectPhysical_Ghosts = (VOID(__cdecl *)(FLOAT, FLOAT, FLOAT, FLOAT, Vector4&))getCryptedCoD(CheatPatches->CoDPatches.UI_FillRectPhysical);//tu17
	Cbuf_AddText_Ghosts = (void(__cdecl*)(DWORD, char*))getCryptedCoD(CheatPatches->CoDPatches.CBuf_AddText);//tu17
	SL_GetString_Ghosts = (short(__cdecl*)(PCHAR, DWORD))getCryptedCoD(CheatPatches->CoDPatches.SL_GetString);//tu17
	CG_GetWeaponIndexForName= (int(__cdecl*)(char*))getCryptedCoD(CheatPatches->CoDPatches.CG_GetWeaponIndexForName);//tu17
	PlayLocalSoundAliasByName = (int(__cdecl*)(int, char*, int))getCryptedCoD(CheatPatches->CoDPatches.PlayLocalSoundAliasByName);//tu17
	CG_GetEntityBModelBounds = (void(__cdecl*)(pcentity_s_Ghosts, float*, float*))getCryptedCoD(CheatPatches->CoDPatches.CG_GetEntityBModelBounds);//tu17
	R_NormalizedTextScale_Ghosts = (int(__cdecl*)(int, float))getCryptedCoD(CheatPatches->CoDPatches.R_NormalizedTextScale);//tu17
	CG_CalcNamePosition = (bool(__cdecl*)(int, Vector3*, float *, float *))getCryptedCoD(CheatPatches->CoDPatches.CG_CalcNamePosition);//tu17
	G_GetWeaponHitLocationMultiplier = (float(__cdecl*)(int, int))getCryptedCoD(CheatPatches->CoDPatches.G_GetWeaponHitLocationMultiplier);//TU8
	GetWeaponHUDName_Ghosts = (CONST PCHAR(__cdecl *)(DWORD, DWORD, PCHAR, DWORD))getCryptedCoD(CheatPatches->CoDPatches.GetWeaponHudName);//TU17

	// No Spread Functions
	BG_GetSpreadForWeapon_Ghosts = (VOID(__cdecl*)(pcg_s_Ghosts, DWORD, float*, float*))getCryptedCoD(CheatPatches->CoDPatches.BG_GetSpreadForWeapon);//TU17
	RandomBulletDir_Ghosts = (VOID(__cdecl*)(int*, float, float, float*, float*))getCryptedCoD(CheatPatches->CoDPatches.RandomBulletDir); //tu17
	BG_GetWeaponDef_Ghosts = (PDWORD(__cdecl *)(DWORD))getCryptedCoD(CheatPatches->CoDPatches.BG_GetWeaponDef);//tu23
	CG_GetPlayerViewOrigin_Ghosts = (VOID(__cdecl*)(int, pcg_s_Ghosts, Vector3&))getCryptedCoD(CheatPatches->CoDPatches.CG_GetPlayerViewOrigin);//TU17
	sub_8222EC70 = (float(__cdecl*)(int*))getCryptedCoD(CheatPatches->CoDPatches.SomeRandomFuckingShit); //tu17

	// Radar Functions
	CG_WorldPosToCompass_Ghosts = (BOOL(__cdecl*)(DWORD, pcg_s_Ghosts, Vector4&, Vector2&, Vector3&, Vector3&, Vector2&, Vector2&))getCryptedCoD(CheatPatches->CoDPatches.CG_WorldPosToCompass);//tu17
	CG_CompassCalcDimensions_Ghosts = (BOOL(__cdecl*)(DWORD, pcg_s_Ghosts, Vector2&, CRadarHud*, float*, float*, float*, float*))getCryptedCoD(CheatPatches->CoDPatches.CG_CompassCalcDimensions);//tu17
	CG_DrawRotatedPic_Ghosts = (VOID(__cdecl*)(DWORD, float, float, float, float, int, int, float, Vector4&, DWORD))0x822702F0;//tu17
	CG_CompassUpYawVector_Ghosts = (VOID(__cdecl*)(pcg_s_Ghosts, Vector2&))getCryptedCoD(CheatPatches->CoDPatches.CG_CompassUpYawVector);//tu17
	AngleNormalize360_Ghosts = (float(__cdecl*)(float))0x82552EC0;//tu17


	//AutoWall Functions
	FireBulletPenetrate_Ghosts = (VOID(__cdecl*)(DWORD, BulletFireParams*, DWORD, DWORD, pcentity_s_Ghosts, DWORD, Vector3&, bool))getCryptedCoD(CheatPatches->CoDPatches.FireBulletPenetrate);//TU17

}

void GhostsHax()
{
	SetupOffsets();

	Menu_PaintAllDetour = new Detour<int>;
	XInputGetStateDetour = new Detour<DWORD>;
	CL_SendCmdDetour = new Detour<int>;
	CL_CreateNewCommandsDetour = new Detour<void>;
	PlayLocalSoundAliasByNameDetour = new Detour<int>;
	R_EndFrameDetour = new Detour<void>;
	CG_EntityEventDetour = new Detour<void>;
	R_DrawXModelSkinnedCachedDetour = new Detour<void>;
	DrawIndexedPrimitiveDetour = new Detour<HRESULT>;
	R_DrawIndexedPrimitiveDetour = new Detour<HRESULT>;
	LiveStats_WriteStatDetour = new Detour<void>;
	EngineDrawRadarDetour = new Detour<void>;
	//NetDll_XNetGetTitleXnAddrDetour = new Detour<DWORD>;

	// Load our menu options from saved ini file
	LoadMenuOptions();

	Menu_PaintAllDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.Menu_PaintAll), (PVOID)Menu_PaintAllHook);
	XInputGetStateDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.XInputGetState), (PVOID)XInputGetStateHook);
	CL_SendCmdDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CL_SendCmd), (PVOID)CL_SendCmdHook);
	CL_CreateNewCommandsDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CL_CreateNewCommands), (PVOID)CL_CreateNewCommandsHook);

	PlayLocalSoundAliasByNameDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.PlayLocalSoundAliasByName), (PVOID)PlayLocalSoundAliasByNameHook);
	R_EndFrameDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.R_EndFrame), (PVOID)R_EndFrameHook);
	CG_EntityEventDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CG_EntityEvent), (PVOID)CG_EntityEventHook);
	LiveStats_WriteStatDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.LiveStats_WriteStat), (PVOID)LiveStats_WriteStatHook);

	EngineDrawRadarDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CG_CompassDrawPlayerMap), (PVOID)EngineDrawRadarHook);
	//NetDll_XNetGetTitleXnAddrDetour->SetupDetour(0x823B6994, (PVOID)NetDll_XNetGetTitleXnAddrHook);
	//RB_SetTessTechniqueDetour->SetupDetour(0x82373AF8, (PVOID)RB_SetTessTechniqueHook);
	//R_DrawXModelSkinnedCachedDetour->SetupDetour(0x823982C0, (PVOID)R_DrawXModelSkinnedCachedHook);
	//DrawIndexedPrimitiveDetour->SetupDetour(0x8242E2E0, (PVOID)DrawIndexedPrimitive); // tu23
	//R_DrawIndexedPrimitiveDetour->SetupDetour(0x820AB1C0, (PVOID)R_DrawIndexedPrimitiveHook); // tu23


	// Patch GetTagPos so it doesn't throw errors
	*(int *)0x82137A28 = 0x48000030;
}

void UninitializeGhostsCheater()
{
	Menu_PaintAllDetour->TakeDownDetour();
	XInputGetStateDetour->TakeDownDetour();
	CL_SendCmdDetour->TakeDownDetour();
	CL_CreateNewCommandsDetour->TakeDownDetour();
	PlayLocalSoundAliasByNameDetour->TakeDownDetour();
	R_EndFrameDetour->TakeDownDetour();
	CG_EntityEventDetour->TakeDownDetour();
	R_DrawXModelSkinnedCachedDetour->TakeDownDetour();
	DrawIndexedPrimitiveDetour->TakeDownDetour();
	R_DrawIndexedPrimitiveDetour->TakeDownDetour();
	LiveStats_WriteStatDetour->TakeDownDetour();
	EngineDrawRadarDetour->TakeDownDetour();


	//if (NetDll_XNetGetTitleXnAddrDetour->isDetoured())
	//	NetDll_XNetGetTitleXnAddrDetour->TakeDownDetour();


	Sleep(1000);

	delete Menu_PaintAllDetour;
	delete XInputGetStateDetour;
	delete CL_SendCmdDetour;
	delete CL_CreateNewCommandsDetour;
	delete PlayLocalSoundAliasByNameDetour;
	delete R_EndFrameDetour;
	delete CG_EntityEventDetour;
	delete R_DrawXModelSkinnedCachedDetour;
	delete DrawIndexedPrimitiveDetour;
	delete R_DrawIndexedPrimitiveDetour;
	delete LiveStats_WriteStatDetour;
	delete EngineDrawRadarDetour;
	//delete NetDll_XNetGetTitleXnAddrDetour;
}