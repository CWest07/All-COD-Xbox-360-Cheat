#include "stdafx.h"
#include "Utilities.h"
#include "Config.h"
#include <time.h>
#include "Globals.h"
#include "BO1.h"
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

//PCHAR MenuNames[10];
PCHAR SubMenuNames[18][MenuOptions];

pclientActive_t_BO1 LocalClient_BO1;
pcentity_s_BO1 Entities_BO1;
pcgs_t_BO1 cgs_BO1;
pcg_s_BO1 cg_BO1;
ClientInformation_BO1 PlayerInfo[18];

//__declspec(dllexport) BYTE freememory[0x1000];
Detour<void> *Menu_PaintAllDetour;
Detour<DWORD> *XInputGetStateDetour;
Detour<int> *CL_SendCmdDetour;
Detour<void> *CL_CreateNewCommandsDetour;
Detour<void> *PlayLocalSoundAliasByNameDetour;
Detour<void> *R_EndFrameDetour;
Detour<DWORD> *NetDll_XNetGetTitleXnAddrDetour;
Detour<void> *CG_EntityEventDetour;
Detour<void> *R_DrawXModelSkinnedCachedDetour;
Detour<HRESULT> *R_DrawIndexedPrimitiveDetour;
Detour<HRESULT> *DrawIndexedPrimitiveDetour;
Detour<void> *EngineDrawRadarDetour;
Detour<void> *RB_SetTessTechniqueDetour;
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
BOOL HearEveryoneValue = FALSE;
BOOL AutoCrouched = FALSE;
BOOL LobbyInit = FALSE;
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
int ExtraSmallFont;
int ExtraBigDevFont;
int BodyFontBold;
int WhiteMaterial;
int ScavMaterial;
int CompassPingMaterial;
int GradientFadeIn;
int yyCount = 0;
int BlacklistCtr = 0;
int RapidFireCtr = 0;
int TriggerValue = 0;
int SpinbotHealthSave = 0;
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
DWORD ArrowIcon = NULL;

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
	return (getCryptedCoD(CheatPatches->CoDPatches.NetInfoAddress) + (index * 0x200));
}

PCHAR GetNameFromNetInfo(int index)
{
	return (char*)(GetNetInfoAddress(index) + 0x08);
}

BYTE* GetIPFromNetInfo(int index)
{
	return (BYTE*)(GetNetInfoAddress(index) + 0x60); 
}

QWORD GetXUIDFromNetInfo(int index, BOOL read = FALSE)
{
	if (read)
		return *(QWORD*)GetNetInfoAddress(index);
	return GetNetInfoAddress(index);
}

QWORD GetMachineIDFromNetInfo(int index)
{
	return _byteswap_uint64(*(QWORD*)(GetNetInfoAddress(index) + 0x74)); 
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
	MenuOptionNames.push_back(E_ALLOC("Draw Through Walls"));
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
	MenuOptionNames.push_back(E_ALLOC("Level 50?"));
	MenuOptionNames.push_back(E_ALLOC("1,000,000 COD Points?"));
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
	AddMenuOption(MENU_MAIN, OPTION_FOV, 11, 0, "Normal", FOVS, FOVS_Size, iFieldOfViewIndex);
	AddMenuOption(MENU_MAIN, OPTION_THIRDPERSON, 12, bThirdPerson);
	AddMenuOption(MENU_MAIN, OPTION_FAKELAGAMOUNT, 13, 0, "Off", FakeLagTypes, FakeLagTypes_Size, iFakeLagAmount);
	AddMenuOption(MENU_MAIN, OPTION_ENDGAME, 14);
	AddMenuOption(MENU_MAIN, OPTION_LEAVEGAME, 15);


	AddMenuOption(MENU_RAGEBOT, OPTION_ENABLERAGEBOT, 0, bRagebot);
	AddMenuOption(MENU_RAGEBOT, OPTION_RAGEBOTTYPE, 1, 0, "Silent", RagebotTypes, RagebotTypes_Size, iRagebotType);
	//AddMenuOption(MENU_RAGEBOT, "Ragebot Logic:", 2, 0, "Distance", RagebotLogics, RagebotLogics_Size, RagebotLogics_Start);
	AddMenuOption(MENU_RAGEBOT, OPTION_AIMBONE, 2, 0, "Best", AimBones, AimBones_Size, iRBAimBone);
	AddMenuOption(MENU_RAGEBOT, OPTION_PREDICTION, 3, 0, "Off", PredictionScales, PredictionScales_Size, PredictionScales_Start);
	AddMenuOption(MENU_RAGEBOT, OPTION_NOSPREAD, 4, bNoSpread);
	AddMenuOption(MENU_RAGEBOT, OPTION_NOFLINCH, 5, bNoFlinch);
	AddMenuOption(MENU_RAGEBOT, OPTION_AUTOWALL, 6, bAutoWall);
	AddMenuOption(MENU_RAGEBOT, OPTION_AUTOSHOOT, 7, bAutoShoot);
	AddMenuOption(MENU_RAGEBOT, OPTION_AUTOAIM, 8, bAutoAim);
	AddMenuOption(MENU_RAGEBOT, OPTION_RAPIDFIRE, 9, bRapidFire);
	//AddMenuOption(MENU_RAGEBOT, OPTION_TARGETSHIELDS, 9);
	//AddMenuOption(MENU_RAGEBOT, "Anti-Blacklist", 11);
	AddMenuOption(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS, 10, bRagebotPreferred);



	AddMenuOption(MENU_LEGITBOT, OPTION_ENABLELEGITBOT, 0, bLegitbot);
	//AddMenuOption(MENU_LEGITBOT, "Legitbot Logic:", 1, 0, "Distance");
	AddMenuOption(MENU_LEGITBOT, OPTION_AIMBONE, 1, 0, "j_head", AimBonesLegit, AimBonesLegit_Size, iLBAimBone);
	AddMenuOption(MENU_LEGITBOT, OPTION_AIMKEY, 2, 0, "Off", AimKeys, AimKeys_Size, iLBAimButton);
	AddMenuOption(MENU_LEGITBOT, OPTION_AUTOSHOOT, 3, bLBAutoShoot);
	AddMenuOption(MENU_LEGITBOT, OPTION_AUTOAIM, 4, bLBAutoAim);
	AddMenuOption(MENU_LEGITBOT, OPTION_REACTIONDELAY, 5, 0, "Off", ReactionDelays, ReactionDelays_Size, iReactionDelay);
	AddMenuOption(MENU_LEGITBOT, OPTION_FOVSIZE, 6, 0, "80.0", FOVSizes, FOVSizes_Size, iFOVSize);
	AddMenuOption(MENU_LEGITBOT, OPTION_DRAWFOVBOX, 7, bDrawFOVBox);
	AddMenuOption(MENU_LEGITBOT, OPTION_SMOOTHSCALE, 8, 0, "10.0", SmoothScales, SmoothScales_Size, iSmoothScale);
	AddMenuOption(MENU_LEGITBOT, OPTION_TRIGGERBOT, 9, bTriggerbot);
	AddMenuOption(MENU_LEGITBOT, OPTION_RAPIDFIRE, 10, bLBRapidFire);
	//AddMenuOption(MENU_LEGITBOT, OPTION_TARGETSHIELDS, 10);
	AddMenuOption(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS, 11, bLegitbotPreferred);


	AddMenuOption(MENU_SPINBOT, OPTION_ENABLESPINBOT, 0, bSpinbot);
	AddMenuOption(MENU_SPINBOT, OPTION_XSPINDIRECTION, 1, 0, "Off", XSpinDirects, XSpinDirects_Size, iXSpinDirection);
	AddMenuOption(MENU_SPINBOT, OPTION_YSPINDIRECTION, 2, 0, "Off", YSpinDirects, YSpinDirects_Size, iYSpinDirection);
	AddMenuOption(MENU_SPINBOT, OPTION_YSPINSPEED, 3, 0, "10.0", YSpinSpeeds, YSpinSpeeds_Size, iYSpinSpeed);
	//AddMenuOption(MENU_SPINBOT, "Z Spin Direction:", 4, 0, "Off", ZSpinDirects, ZSpinDirects_Size, ZSpinDirects_Start);
	//AddMenuOption(MENU_SPINBOT, "Z Spin Speed:", 5, 0, "10.0", ZSpinSpeeds, ZSpinSpeeds_Size, ZSpinSpeeds_Start);
	//AddMenuOption(MENU_SPINBOT, "Funny Spinbots:", 4, 0, "Off", FunnySpinbots, FunnySpinbots_Size, iFunnySpinbots);
	AddMenuOption(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 4, 0, "Off", XAASpinbots, XAASpinbots_Size, iXAntiAimSpinbot);
	AddMenuOption(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 5, 0, "Off", YAASpinbots, YAASpinbots_Size, iYAntiAimSpinbot);
	AddMenuOption(MENU_SPINBOT, OPTION_PREFERREDSETTINGS, 6, bSpinbotPreferred);


	AddMenuOption(MENU_DRAWING, OPTION_DRAWENEMIES, 0, bDrawEnemies);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWFRIENDLIES, 1, bDrawFriendlies);
	AddMenuOption(MENU_DRAWING, OPTION_ESPBOXTYPE, 2, 0, "Off", ESPBoxes, ESPBoxes_Size, iESPBoxType);
	//AddMenuOption(MENU_DRAWING, "Player Color:", 3, 0, "Normal", PlayerColors, PlayerColors_Size, PlayerColors_Start);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWNAME, 3, bDrawName);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWWEAPON, 4, bDrawWeapon);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWBONES, 5, bDrawBones);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWEXPLOSIVES, 6, bDrawExplosives);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWSNAPLINES, 7, 0, "Off", SnaplinesTypes, SnaplinesTypes_Size, iSnaplines);
	//AddMenuOption(MENU_DRAWING, "Draw Killstreaks", 8, bDrawKillstreaks);
	AddMenuOption(MENU_DRAWING, OPTION_DIRECTIONALARROWS, 8, bDirectionalArrows);
	AddMenuOption(MENU_DRAWING, OPTION_CROSSHAIRTYPE, 9, 0, "Off", CrosshairTypes, CrosshairTypes_Size, iCrosshairType);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR, 10, bDrawCustomRadar);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS, 11, bDrawDroppedItems);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWENEMYVISIBLE, 12, bDrawEnemyVis);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWPROXIMITYWARNING, 13, bDrawProximityWrn);
	AddMenuOption(MENU_DRAWING, OPTION_DRAWTHROUGHWALLS, 14, bDrawThroughWalls);
	AddMenuOption(MENU_DRAWING, OPTION_CARTOONMODE, 15, bCartoonMode);


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
	AddMenuOption(MENU_SETTINGS, OPTION_LEVEL50, 5);
	AddMenuOption(MENU_SETTINGS, OPTION_CODPOINTS, 6);
	//AddMenuOption(MENU_SETTINGS, "Unlock All?", 10);
	//AddMenuOption(MENU_SETTINGS, "Set Legit Stats?", 11);
	AddMenuOption(MENU_SETTINGS, OPTION_INVERTTEAMCHECK, 7);
	AddMenuOption(MENU_SETTINGS, OPTION_LOGIPS, 8, bLogIPs);
	//AddMenuOption(MENU_SETTINGS, "Freeze All Consoles?", 9);

	HudSmallFont = DB_FindXAssetHeader_BO1(font_BO1, "fonts/hudsmallfont", 1);
	BigDevFont = DB_FindXAssetHeader_BO1(font_BO1, "fonts/bigDevFont", 1);
	SmallFont = DB_FindXAssetHeader_BO1(font_BO1, "fonts/smallfont", 1);
	ExtraBigDevFont = DB_FindXAssetHeader_BO1(font_BO1, "fonts/720/extraBigFont", 1);
	ExtraSmallFont = DB_FindXAssetHeader_BO1(font_BO1, "fonts/720/extraSmallFont", 1);
	BodyFontBold = DB_FindXAssetHeader_BO1(font_BO1, "fonts/bodyFontBold", 1);
	WhiteMaterial = DB_FindXAssetHeader_BO1(material_BO1, "white", 1);
	ScavMaterial = DB_FindXAssetHeader_BO1(material_BO1, "hud_scavenger_pickup", 1);
	ArrowIcon = DB_FindXAssetHeader_BO1(material_BO1, "compassping_player", 1);
	//CompassPingMaterial = DB_FindXAssetHeader_BO1(material_BO1, "compassping_player", 1);

	//memcpy(OriginalGT, (PVOID)(0x839691A8 + 4), 0x1E);
	//memcpy(OriginalXUID, (PVOID)(0x838BA853), 0xE);

	menuReady = TRUE;
}

bool UserInGame(BOOL onlyDisconected = FALSE)
{
	bool allLocalClientsDisconnected = ((bool(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CL_AllLocalClientsDisconnected))();
	if (onlyDisconected)
		return !allLocalClientsDisconnected;
	return *(bool*)(*(int*)getCryptedCoD(CheatPatches->CoDPatches.CL_InGame_Address) + 0x28) && !allLocalClientsDisconnected;
}

void PlayMenuSound(char* SoundName)
{
	// If we want menu sounds
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUSOUNDS))
		return;

	// Read the sound level (1.0, not sure if this changes per user)
	float SoundLevel = *(float*)0x8201020C;

	// If ingame, make the sound a little louder
	if (UserInGame())
		SoundLevel += 0.5f;

	// Call the games function to play the sound
	PlayLocalSoundAliasByName(SoundName, 0, SoundLevel, 0, 0, 0, 0);
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

		PlayMenuSound("uin_navigation_over");
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

		PlayMenuSound("uin_navigation_over");
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
		PlayMenuSound("uin_navigation_over");
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

		PlayMenuSound("uin_navigation_click");

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

		PlayMenuSound("uin_navigation_click");

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
			subScroll = 5;
		}
		else if (subScroll > 5)
		{
			subScroll = 1;
		}
	}

}

void DrawBox_BO1(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color)
{
	R_AddCmdDrawStretchPic_BO1(X, Y, W, H, 4, 4, 1, 1, Color, WhiteMaterial);
}

void DrawGradient_BO1(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color1, Vector4& Color2, bool horizontal)
{
	float fAdd = (horizontal ? 0 : W / 2 - H / 2);
	R_AddCmdDrawStretchPic_BO1(X, Y, W, H, 4, 4, 1, 1, Color2, WhiteMaterial);
	R_AddCmdDrawStretchPic_BO1(X + fAdd, Y - fAdd, (horizontal ? W : H), (horizontal ? H : W), 4, 4, 1, 1, Color1, GradientFadeIn);
}

void DrawText_BO1(CONST PCHAR Text, FLOAT X, FLOAT Y, DWORD index, FLOAT addy, DWORD Font, Vector4& Color, float fontXSize = 1.0, float fontYSize = 1.0)
{
	CL_DrawTextPhysical_BO1(Text, 0x7FFFFFFF, Font, X, Y + (index * addy), fontXSize, fontYSize, 0, Color, 0);
}

bool WorldPosToScreenPos(float* In, Vector2* Out)
{
	Vector3 Position, Transform;

	VectorSubtract(In, cg_BO1->refdef.ViewOrg, Position);

	Transform.x = DotProduct(Position, cg_BO1->refdef.ViewAxis[1]);
	Transform.y = DotProduct(Position, cg_BO1->refdef.ViewAxis[2]);
	Transform.z = DotProduct(Position, cg_BO1->refdef.ViewAxis[0]);

	if (Transform.z < 0.1f)
		return false;

	Vector2 Center; Vector2 OutPut;
	Center.x = cg_BO1->refdef.Width * 0.5f; Center.y = cg_BO1->refdef.Height * 0.5f;
	OutPut.x = Center.x * (1 - (Transform.x / cg_BO1->refdef.Fov.x / Transform.z)); OutPut.y = Center.y * (1 - (Transform.y / cg_BO1->refdef.Fov.y / Transform.z));
	*Out = OutPut;
	return true;
}

void DrawLine_BO1(FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, FLOAT Width, Vector4& Color)
{
	FLOAT X, Y, Angle, L1, L2, H1;
	H1 = Y2 - Y1;
	L1 = X2 - X1;
	L2 = sqrt(L1 * L1 + H1 * H1);
	X = X1 + ((L1 - L2) / 2);
	Y = Y1 + (H1 / 2);
	Angle = (FLOAT)atan(H1 / L1) * (180 / 3.14159265358979323846);
	CG_DrawRotatedPicPhysical_BO1(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), X, Y, L2, Width, Angle, Color, WhiteMaterial);
}

void MakeLines(Vector3 Origin, FLOAT X1, FLOAT Y1, FLOAT Z1, FLOAT X2, FLOAT Y2, FLOAT Z2, Vector4& Color, int Size = 1)
{
	Vector2 Screen1, Screen2;
	Vector3 Origin1(Origin.x + X1, Origin.y + Y1, (Origin.z + Z1) - 4);
	Vector3 Origin2(Origin.x + X2, Origin.y + Y2, (Origin.z + Z2) - 4);
	if (WorldPosToScreenPos((float*)&Origin1, &Screen1) && WorldPosToScreenPos((float*)&Origin2, &Screen2)) {
		DrawLine_BO1(Screen1.x, Screen1.y, Screen2.x, Screen2.y, Size, Color);
	}
}

void DrawCheckBox(FLOAT X, FLOAT Y, BOOL Checked = FALSE)
{
	Vector4 Color;
	if (Checked)
		Color = Vector4(0, 1, 0, 1);
	else
		Color = Vector4(1, 0, 0, 1);
	DrawLine_BO1(X - 7.5, Y - 7.5, X + 7.5, Y - 7.5, 2, Color);//Top
	DrawLine_BO1(X - 7.5, Y - 7.5, X - 7.5, Y + 7.5, 2, Color);//Left Side
	DrawLine_BO1(X + 7.5, Y - 7.5, X + 7.5, Y + 7.5, 2, Color);//Right Side
	DrawLine_BO1(X - 7.5, Y + 7.5, X + 7.5, Y + 7.5, 2, Color);//Bottom

	if (Checked)
	{
		DrawLine_BO1(X - 7.5, Y - 7.5, X + 7.5, Y + 7.5, 1.5, Color);//Check
		DrawLine_BO1(X - 7.5, Y + 7.5, X + 7.5, Y - 7.5, 1.5, Color);
	}
}

void DrawMenu_BO1()
{
	FLOAT X, Y;
	X = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY));//cgs_BO1->Width; 0x829BE2B4
	Y = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY) + 0x4);//cgs_BO1->Height; 0x829BE2B8
	Vector4 ActiveTextColor = GetColorCode(colorVioletFlower);
	Vector4 InActiveTextColor = Vector4(1, 1, 1, 0.9);

	int MenuFont = SmallFont;

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
	DrawBox_BO1(X * 0.331, Y * 0.25, menuWidth, menuHeight, Vector4(0, 0, 0, 0.420));
	DrawLine_BO1(X * 0.331, Y * 0.25, (X * 0.331) + menuWidth, Y * 0.25, 3.5, Vector4(1, 0, 0, 1));
	DrawLine_BO1(X * 0.331, Y * 0.25 + menuHeight, (X * 0.331) + menuWidth, (Y * 0.25) + menuHeight, 3.5, Vector4(1, 0, 0, 1));
	DrawLine_BO1((X * 0.331) + 115, (Y * 0.25), (X * 0.331) + 115, (Y * 0.25) + menuHeight, 3.5, Vector4(1, 0, 0, 1));

	// Wait for the animation to be done
	if (menuWidth == 420 && menuHeight == 400)
	{
		//Draw Menu Names
		for (int i = 0; i < MenuOptions; i++)
		{
			DrawText_BO1(MenuNames[i], X * 0.3328, Y * 0.2861 - 4.0f, i, 22, MenuFont, (scroll == i) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
			DrawLine_BO1(X * 0.331, (Y * 0.28) + (i * 22), (X * 0.331) + 115, (Y * 0.28) + (22 * i), 3.5, Vector4(1, 0, 0, 1));
		}

		//Draw SubOptions
		for (int i = 0; i < SubMenuOptions[scroll]; i++)
		{
			if (subMenu == FALSE)
			{
				DrawText_BO1(SubMenuNames[i][scroll], X * 0.425, Y * 0.2861, i, 22, MenuFont, InActiveTextColor, 0.7f, 0.7f);
			}
			else if (subMenu == TRUE && clientMenu == FALSE)
			{
				DrawText_BO1(SubMenuNames[i][scroll], X * 0.425, Y * 0.2861, i, 22, MenuFont, (subScroll == i) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
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

				DrawText_BO1(SubMenuNames[selectedClient][Tabindex], X * 0.425, (Y * 0.2861) + (0 * 22), 0, 0, MenuFont, (subMenu == TRUE) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
				DrawText_BO1("Prioritize", X * 0.425, (Y * 0.2861), 1, 22, MenuFont, (subMenu == TRUE && subScroll == 1) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (1 * 22), ClientOptionEnabled[1][selectedClient]);

				DrawText_BO1("Whitelist", X * 0.425, (Y * 0.2861), 2, 22, MenuFont, (subMenu == TRUE && subScroll == 2) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (2 * 22), ClientOptionEnabled[2][selectedClient]);

				DrawText_BO1("Invert Team", X * 0.425, (Y * 0.2861), 3, 22, MenuFont, (subMenu == TRUE && subScroll == 3) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (3 * 22), ClientOptionEnabled[3][selectedClient]);


				DrawText_BO1("Set GT To Player's IP?", X * 0.425, (Y * 0.2861), 4, 22, MenuFont, (subMenu == TRUE && subScroll == 4) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);

				DrawText_BO1("Player Has Anti-Aim", X * 0.425, (Y * 0.2861), 5, 22, MenuFont, (subMenu == TRUE && subScroll == 5) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (5 * 22), ClientOptionEnabled[5][selectedClient]);

				BYTE* IP = GetIPFromNetInfo(selectedClient);
				DrawText_BO1(va_BO1("IP: %i.%i.%i.%i", IP[0], IP[1], IP[2], IP[3]), X * 0.425, (Y * 0.2861), 6, 22, MenuFont, (subMenu == TRUE && subScroll == 6) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
				QWORD MachineID = GetMachineIDFromNetInfo(selectedClient);
				DWORD MachineIDPT1 = *(DWORD*)(DWORD)&MachineID;
				DWORD MachineIDPT2 = *(DWORD*)((DWORD)&MachineID + 0x04);
				DrawText_BO1(va_BO1("Machine ID: %08X%08X", MachineIDPT1, MachineIDPT2), X * 0.425, Y * 0.2861, 7, 22, MenuFont, (subMenu == TRUE && subScroll == 7) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
				BYTE* XUID = (BYTE*)GetXUIDFromNetInfo(selectedClient);

				DrawText_BO1(va_BO1("XUID: %08X%08X", *(DWORD*)(DWORD)&XUID[0], *(DWORD*)(DWORD)&XUID[4]), X * 0.425, Y * 0.2861, 8, 22, MenuFont, (subMenu == TRUE && subScroll == 8) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);

				IN_ADDR ip = *(IN_ADDR*)GetIPFromNetInfo(selectedClient);
				if (PlayerInfo[selectedClient].ip.S_un.S_addr != ip.S_un.S_addr && (ip.S_un.S_addr != 0))
				{
					PlayerInfo[selectedClient].GrabGeoIPInfo = TRUE;
					PlayerInfo[selectedClient].ip = ip;
				}

				DrawText_BO1(va_BO1(E("City: %s"), PlayerInfo[selectedClient].city), X * 0.425, Y * 0.2861, 9, 22, SmallFont, (subMenu == TRUE && subScroll == 9) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
				DrawText_BO1(va_BO1(E("State: %s"), PlayerInfo[selectedClient].stateFull), X * 0.425, Y * 0.2861, 10, 22, SmallFont, (subMenu == TRUE && subScroll == 10) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
				DrawText_BO1(va_BO1(E("Country: %s"), PlayerInfo[selectedClient].country), X * 0.425, Y * 0.2861, 11, 22, SmallFont, (subMenu == TRUE && subScroll == 11) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
				DrawText_BO1(va_BO1(E("ISP: %s"), PlayerInfo[selectedClient].isp), X * 0.425, Y * 0.2861, 12, 22, SmallFont, (subMenu == TRUE && subScroll == 12) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);


			}

			if (clientMenu)
				continue;

			if (strstr(SubMenuNames[i][scroll], "?"))
				continue;

			if (!strstr(SubMenuNames[i][scroll], ":") && scroll != 5)
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (i * 22), OptionEnabled[i][scroll]);
			else if (strstr(SubMenuNames[i][scroll], ":"))
				DrawText_BO1(GetMenuOptionString((eMenuNames)scroll, SubMenuNames[i][scroll]), X * 0.58, (Y * 0.2841) + (i * 22), 0, 0, MenuFont, (subMenu == TRUE && subScroll == i) ? ActiveTextColor : InActiveTextColor, 0.7f, 0.7f);
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

PCHAR EngineText = NULL;
void DrawAd()
{
	FLOAT X, Y;
	if (EngineText == NULL)
		EngineText = E_ALLOC("NiNJA Engine v1.0");
	X = (FLOAT)cgs_BO1->Width * 0.78125;//*(FLOAT*)((DWORD)freememory);
	Y = (FLOAT)cgs_BO1->Height * 0.2;
	//X2 = *(FLOAT*)((DWORD)freememory + 8);
	//Y2 = *(FLOAT*)((DWORD)freememory + 12);
	RotateXY(cos(ToRadians(rotation)), sin(ToRadians(rotation)), X + (FLOAT)((FLOAT)R_TextWidth_BO1(EngineText, 0x7FFFFFFF, BigDevFont) / 2), Y, X, Y, &X, &Y);

	rotation += 0.25;
	if (rotation >= 360.0)
		rotation -= 360.0;

	CL_DrawTextPhysical_BO1(EngineText, 0x7FFFFFFF, BigDevFont, X, Y, 1, 1, rotation, Vector4(1, 0, 0, 0.75f), 0);
}

BOOL ShouldUpdateTagCache()
{
	if (UserInGame() && !LobbyInit)
	{
		LobbyInit = TRUE;
		return TRUE;
	}
	else if (!UserInGame() && LobbyInit)
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
			BoneIndexs[i] = SL_GetString_BO1(BoneCache[i], 0, 0);
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

int CG_DObjGetWorldTagPos_B(pcentity_s_BO1 centity_s, int Dobj, short Tag, float* Out)
{
	return ((int(*)(pcentity_s_BO1, int, short, float*))getCryptedCoD(CheatPatches->CoDPatches.CG_DObjGetWorldTagPos))(centity_s, Dobj, Tag, Out);
}

bool GetTagPosistion(pcentity_s_BO1 centity_s, int ClientIndex, PCHAR SelectedTag, float* OutPos)
{
	//char Temp[0x100];
	short TagIndex;
	int Dobj;

	//for (int i = 0; i <= strlen(SelectedTag); i++)
	//	Temp[i] = tolower(SelectedTag[i]);

	TagIndex = GetTagIndex(SelectedTag, ShouldUpdateTagCache()); //SL_GetString_BO1(Temp, 0, 0);
	Dobj = Com_GetClientDObj_BO1(Entities_BO1[ClientIndex].ClientNumber, 0);

	if (Dobj == 0 || *(int *)(Dobj + 0x54) == 0)
		return false;

	return CG_DObjGetWorldTagPos_B(centity_s, Dobj, TagIndex, OutPos) != 0;
}

short TagToShort(PCHAR Tag)
{
	char Temp[0x100];

	for (int i = 0; i <= strlen(Tag); i++)
		Temp[i] = tolower(Tag[i]);

	return SL_GetString_BO1(Temp, 0, 0);
}

void DropTarget(ClientInformation_BO1 PlayerInfo)
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

	int RandomClient = rand() % 17;
	if (UserInGame() && (cg_BO1 != NULL))
	{
		if (strcmp(cg_BO1->clientInfo[RandomClient].Name, "") && (RandomClient != cg_BO1->ClientNumber) && (cg_BO1->clientInfo[RandomClient].Name != NULL))
		{
			char xuid[20];
			_ui64toa(GetXUIDFromNetInfo(cg_BO1->ClientNumber, TRUE), xuid, 0x10);
			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", cg_BO1->clientInfo[RandomClient].Name, xuid);
			Cbuf_AddText_BO1(0, Output);
			//CL_AddReliableCommand(0, Output);
		}
	}
	else
	{
		if (strcmp(GetNameFromNetInfo(RandomClient), ""))
		{
			//sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", GetNameFromNetInfo(RandomClient), GetXUIDFromNetInfo(cg_BO1->ClientNumber));
			//Cbuf_AddText_BO1(0, Output);
			//strcpy_s((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1F, GetNameFromNetInfo(RandomClient));
		}
	}
}

void RainbowName()
{
	char Output[0x200], Color[10];

	//if (cg_BO1 == NULL)
	//	return;

	int RandomColor = rand() % 9;
	sprintf(Color, "^%i", RandomColor);

	if (UserInGame())
	{
		//char* Gamertag = (OriginalGT != NULL) ? OriginalGT : GetNameFromNetInfo(cg_BO1->ClientNumber);

		char xuid[20];
		_ui64toa(GetXUIDFromNetInfo(cg_BO1->ClientNumber, TRUE), xuid, 0x10);
		char *name = GetNameFromNetInfo(cg_BO1->ClientNumber);
		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", name, Color, xuid);
		Cbuf_AddText_BO1(0, Output);
		//CL_AddReliableCommand(0, Output);
	}
	else
	{
		//sprintf(PreGameOutput, "^%i%s", RandomColor, OriginalGT);
		//strcpy_s((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1F, PreGameOutput);
	}
}

void PrestigeDisco()
{
	int RandomPrestige = rand() % 16;
	if (!UserInGame(TRUE))
	{
		*(BYTE*)getCryptedCoD(CheatPatches->CoDPatches.PrestigeOffset) = RandomPrestige;
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

	if (wcGT[0] != 0 && wcGT[1] != 0 && UserInGame())
	{
		wcscpy_s(wcOldGT, wcGT);
		wcstombs(OriginalGT, wcGT, 0x1F);

		char xuid[20];
		_ui64toa(GetXUIDFromNetInfo(cg_BO1->ClientNumber, TRUE), xuid, 0x10);
		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", OriginalGT, xuid);
		Cbuf_AddText_BO1(0, Output);
		//CL_AddReliableCommand(0, Output);
		//strcpy_s((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1F, OriginalGT);
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
	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "10.0")
		ZSpinSpeed = 10;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "20.0")
		ZSpinSpeed = 20;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "30.0")
		ZSpinSpeed = 30;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "40.0")
		ZSpinSpeed = 40;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "50.0")
		ZSpinSpeed = 50;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "60.0")
		ZSpinSpeed = 60;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "70.0")
		ZSpinSpeed = 70;

	if (GetMenuOptionString(MENU_SPINBOT, "Z Spin Speed:") == "Random")
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

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "12th")
		Prestige = 12;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "13th")
		Prestige = 13;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "14th")
		Prestige = 14;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "15th")
		Prestige = 15;

	if (GetMenuOptionString(MENU_SETTINGS, OPTION_PRESTIGE) == "16th")
		Prestige = 16;

	return Prestige;
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

	for (int i = 0; i < 18; i++)
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

void HandleToggleOptions(pcgs_t_BO1 cgs_BO1)
{
	// Make sure the menu is ready
	if (!menuReady) return;

	// Update Player Names
	UpdateClientsNames();

	// Handle UAV
	if (GetMenuOptionValue(MENU_MAIN, OPTION_RADAR))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.RadarOffset) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.RadarOffset) = 0x419A0050;
	}

	// Handle No Recoil
	if (GetMenuOptionValue(MENU_MAIN, OPTION_NORECOIL))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoRecoilOffset) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoRecoilOffset) = 0x4BF67EC5;
	}

	// Handle No Sway
	if (GetMenuOptionValue(MENU_MAIN, OPTION_NOSWAY))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset1) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset1) = 0x4BFFEB09;
	}

	// Handle Show Ping
	if (GetMenuOptionValue(MENU_MAIN, OPTION_SHOWPING) && UserInGame())
	{
		char* Ping = va_BO1("Ping: %i", cg_BO1->snap->Ping);
		DrawText_BO1(Ping, 100, 40, 0, 0, HudSmallFont, Vector4(1, 1, 1, 1));
	}

	// Handle Hear All Players
	if (GetMenuOptionValue(MENU_MAIN, OPTION_EVERYONEHEARSME) && UserInGame())
	{
		if (!HearEveryoneValue)
		{
			HearEveryoneValue = TRUE;
			Cbuf_AddText_BO1(0, "cg_everyoneHearsEveryone 1");
		}
	}
	else if(!GetMenuOptionValue(MENU_MAIN, OPTION_EVERYONEHEARSME) && UserInGame())
	{
		if (HearEveryoneValue)
		{
			HearEveryoneValue = FALSE;
			Cbuf_AddText_BO1(0, "cg_everyoneHearsEveryone 0");
		}
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
	if (GetMenuOptionValue(MENU_MAIN, OPTION_SHOWHOST))
	{
		char* HostName = (char*)*(int*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.HostNameStruct)) + 0x18);
		if (strcmp(HostName, ""))
		{
			char HostOut[0x100];
			sprintf(HostOut, "Host: %s", HostName);
			DrawText_BO1(HostOut, 100, 25, 0, 0, HudSmallFont, Vector4(1, 1, 1, 1));
		}
	}

	// Handle Custom Field Of View
	if (UserInGame())
	{
		*(float*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.FOVStruct)) + 0x18) = FindFieldOfViewSize();
	}

	// Handle End Game
	if (GetMenuOptionValue(MENU_MAIN, OPTION_ENDGAME) && UserInGame())
	{
		char* EndGameString;
		EndGameString = va_BO1(E("mr %i %i endround"), *(int*)getCryptedCoD(CheatPatches->CoDPatches.ServerIdOffset), -1);

		Cbuf_AddText_BO1(0, EndGameString);
		SetMenuOptionValue(MENU_MAIN, OPTION_ENDGAME, 0);
	}

	// Handle Leave Game
	if (GetMenuOptionValue(MENU_MAIN, OPTION_LEAVEGAME))
	{
		Cbuf_AddText_BO1(0, "disconnect");
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
		SetMenuOptionString(MENU_LEGITBOT, OPTION_AIMKEY, 1);

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
		// Disable over head names
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.OverheadNamesEnemyOffset) = 0x60000000;
	}
	else
	{
		// Enable over head names
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.OverheadNamesEnemyOffset) = 0x4BFFEF21;
	}

	// Handle Wallhack
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWTHROUGHWALLS))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.WallhackOffset) = 0x38C0FF03;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.WallhackOffset) = 0x7FA6EB78;
	}

	// Handle Steal Gamertags
	if (GetMenuOptionValue(MENU_MAIN, OPTION_STEALGTS) /*&& (Dvar_GetBool_BO1("cl_ingame"))*/)
	{
		//if (cgs_BO1 == NULL)
		//	return;

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
		//if (cgs_BO1 == NULL)
		//	return;

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
	if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) != "Off" && UserInGame())
	{
		if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) == "+")
		{
			// + styled crosshair
			DrawLine_BO1((cg_BO1->refdef.Width / 2) - 10, cg_BO1->refdef.Height / 2, (cg_BO1->refdef.Width / 2) + 10, cg_BO1->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine_BO1(cg_BO1->refdef.Width / 2, (cg_BO1->refdef.Height / 2) - 10, cg_BO1->refdef.Width / 2, (cg_BO1->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) == "x")
		{
			// X styled crosshair
			DrawLine_BO1((cg_BO1->refdef.Width / 2) - 8, (cg_BO1->refdef.Height / 2) - 8, (cg_BO1->refdef.Width / 2) + 8, (cg_BO1->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine_BO1((cg_BO1->refdef.Width / 2) - 8, (cg_BO1->refdef.Height / 2) + 8, (cg_BO1->refdef.Width / 2) + 8, (cg_BO1->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) == "o")
		{
			DrawLine_BO1((cg_BO1->refdef.Width / 2) - 10, cg_BO1->refdef.Height / 2, (cg_BO1->refdef.Width / 2) + 10, cg_BO1->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine_BO1(cg_BO1->refdef.Width / 2, (cg_BO1->refdef.Height / 2) - 10, cg_BO1->refdef.Width / 2, (cg_BO1->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));

			DrawLine_BO1((cg_BO1->refdef.Width / 2) - 8, (cg_BO1->refdef.Height / 2) - 8, (cg_BO1->refdef.Width / 2) + 8, (cg_BO1->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine_BO1((cg_BO1->refdef.Width / 2) - 8, (cg_BO1->refdef.Height / 2) + 8, (cg_BO1->refdef.Width / 2) + 8, (cg_BO1->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
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

	// Set Level 50
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_LEVEL50))
	{
		*(int*)0x8408E805 = 0xA4431300; // Rank XP
		*(BYTE*)0x8408E801 = 0x37; // Rank
		SetMenuOptionValue(MENU_SETTINGS, OPTION_LEVEL50, 0);
	}

	// 1 million squad points
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_CODPOINTS))
	{
		*(int*)0x8408E3F1 = _byteswap_ulong(_byteswap_ulong(*(int*)0x8408E3F1) + 1000000);

		// Disable the menu option
		SetMenuOptionValue(MENU_SETTINGS, OPTION_CODPOINTS, 0);
	}

	// Spoof gamertag
	if (GetMenuOptionValue(MENU_MAIN, OPTION_SPOOFGT))
	{
		SetMenuOptionValue(MENU_MAIN, OPTION_SPOOFGT, 0);

		ExCreateThread(0, 0, 0, 0, (LPTHREAD_START_ROUTINE)SpoofGamertag, 0, 0);
	}

	// Handle Third Person
	//BOOL ThirdPerson = Dvar_GetBool_BO1("cg_thirdperson");
	if (UserInGame())
	{
		//if ((*(int*)((DWORD)&Entities_BO1[*(int*)cg_BO1] + 0x318) & 0x40000000) != 0)
		//{
			if (GetMenuOptionValue(MENU_MAIN, OPTION_THIRDPERSON))
			{
				*(BYTE*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.ThirdPersonStruct)) + 0x18) = 1;
			}
			else
			{
				*(BYTE*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.ThirdPersonStruct)) + 0x18) = 0;
			}
		//}
	}

	// Draw Legitbot FOV Box
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_DRAWFOVBOX) && UserInGame() && !menuOpen && GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT))
	{
		int FOVSize = FindFOVSize();
		DrawBox_BO1((cg_BO1->refdef.Width / 2) - (FOVSize / 2), (cg_BO1->refdef.Height / 2) - (FOVSize / 2), FOVSize, FOVSize, Vector4(1, 1, 1, 0.50f));
	}

	// Handle No Flinch
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOFLINCH))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoFlinchOffset) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoFlinchOffset) = 0x4BFFF77D;
	}

	// Handle Anti-Blacklist
	/*if (GetMenuOptionValue(MENU_RAGEBOT, "Anti-Blacklist") && Dvar_GetBool_BO1("cl_ingame"))
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

	// Handle Change Team
	//if (GetMenuOptionValue(MENU_MAIN, "Change Team?") && UserInGame())
	//{
	//	char* NewTeamName;
	//	/*int NewTeam = (cg_BO1->clientInfo[cg_BO1->ClientNumber].Team == 1) ? 2 : 1;
	//	NewTeamName = va_BO1("cmd mr %i 3 %s", *(int*)0x826237E0, TeamNames[NewTeam]);*/

	//	if (cg_BO1->clientInfo[cg_BO1->ClientNumber].Team == 2)
	//		NewTeamName = va_BO1("cmd mr %i 3 %s", *(int*)getCryptedCoD(CheatPatches->CoDPatches.ServerIdOffset), "axis");
	//	else
	//		NewTeamName = va_BO1("cmd mr %i 3 %s", *(int*)getCryptedCoD(CheatPatches->CoDPatches.ServerIdOffset), "allies");


	//	Cbuf_AddText_BO1(0, NewTeamName);
	//	SetMenuOptionValue(MENU_MAIN, "Change Team?", 0);
	//}

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
	if (ClientOptionEnabled[4][selectedClient] && UserInGame())
	{
		char Output[0x200];
		BYTE* IP = GetIPFromNetInfo(selectedClient);
		char* ClientName = GetNameFromNetInfo(selectedClient);

		BYTE* PlayerXUID = (BYTE*)GetXUIDFromNetInfo(selectedClient);
		if (strcmp(ClientName, ""))
		{
			if (PlayerXUID[0] == 0x0 && PlayerXUID[1] == 0x09 && ClientName[0] != 0)
			{
				char* IPName = va_BO1("%s's IP: %i.%i.%i.%i", ClientName, IP[0], IP[1], IP[2], IP[3]);
				sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", IPName, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
				//Cbuf_AddText_BO1(0, Output);
				//strcpy_s((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1F, IPName);
				//strcpy((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), IPName);
				CL_AddReliableCommand(0, Output);
			}
		}

		ClientOptionEnabled[4][selectedClient] = 0;
	}

	// Freeze All Consoles
	/*if (GetMenuOptionValue(MENU_SETTINGS, "Freeze All Consoles?"))
	{
		if (Dvar_GetBool_BO1("cl_ingame"))
		{
			char Output[0x200];
			PCHAR Hax = "^\x0002NiNJA";

			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", cg_BO1->clientInfo[cg_BO1->ClientNumber].Name, Hax, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
			Cbuf_AddText_BO1(0, Output);
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
		//SetMenuOptionString(MENU_SPINBOT, "Z Spin Direction:", 0);
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

	// Handle Player Body Color
	/*if (GetMenuOptionString(MENU_DRAWING, "Player Color:") != "Normal")
	{
		memcpy((VOID*)0x83c56038, PlayerColorTable[OptionArrayCurrent[3][4]], sizeof(DWORD) * 3);
	}
	else if (GetMenuOptionString(MENU_DRAWING, "Player Color:") == "Normal")
	{
		memcpy((VOID*)0x83c56038, PlayerColorTable[0], sizeof(DWORD) * 3);
	}*/

	// Handle Cartoon Mode
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_CARTOONMODE))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.CartoonMode1) = 1;
		*(short*)getCryptedCoD(CheatPatches->CoDPatches.CartoonMode2) = 0x0404;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.CartoonMode1) = 3;
		*(short*)getCryptedCoD(CheatPatches->CoDPatches.CartoonMode2) = 0x0805;
	}

	// Disable Auto Aim If No Spread Is Enabled
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD) && GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM))
	{
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM, 0);
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

	memcpy(viewaxis, cg_BO1->refdef.ViewAxis, sizeof(viewaxis));
	memcpy(vieworg, &cg_BO1->refdef.ViewOrg, sizeof(vieworg));
	width = (float)cg_BO1->refdef.Width;
	height = (float)cg_BO1->refdef.Height;
	tanHalfFovX = cg_BO1->refdef.Fov.x;
	tanHalfFovY = cg_BO1->refdef.Fov.y;

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

float GetBoxWidth(int value)
{
	if (value & 0x8)
		return 35;
	else return 17;
}

float GetBoxHeight(int value)
{
	if (value & 0x8)
		return 25;
	else if (value & 0x4)
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
	int weaponDef = BG_GetWeaponDef_BO1(Weapon);
	if (weaponDef) {
		DWORD killIconType = *(DWORD*)(weaponDef + 0x57c);
		getWeaponAspectRatio(killIconType, aspectW, aspectH);
		return *(int*)(weaponDef + 0x578);
	}
}

CONST PCHAR getWeaponName_BO1(int weaponIndex)
{
	DWORD r11 = weaponIndex << 3;
	DWORD r7 = weaponIndex + r11;
	r11 = r7 << 2;
	DWORD r10 = *(DWORD*)0x828CE9CC;
	DWORD r31 = r10 + r11;
	p_weaponInfo_BO1 weaponInfo = (p_weaponInfo_BO1)r31;
	return weaponInfo->Name->weaponName;
}

void DrawSnapLine(Vector4 color, int Index, BOOL SameTeam)
{
	float xy2[2];
	Vector3 xy1, LinePos, LinePosOut;

	//Get center of screen coordinates
	xy2[0] = (float)cg_BO1->refdef.Width / 2.0f;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Bottom")
		xy2[1] = (float)cg_BO1->refdef.Height;
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Center")
		xy2[1] = (float)cg_BO1->refdef.Height / 2.0f;
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Top")
		xy2[1] = (float)cg_BO1->refdef.Height / 10.0f - 65.0f;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) != "Top")
		LinePos = Entities_BO1[Index].Origin;
	else
	{
		GetTagPosistion(&Entities_BO1[Index], Index, "j_helmet", (float*)&LinePos);
	}

	//Get coordinates of person
	if (WorldToScreen(LinePos, xy1) == TRUE)
	{
		DrawLine_BO1((int)xy2[0], (int)xy2[1], (int)xy1[0], (int)xy1[1], 1.0f, color);
	}
}

void DrawArrows(float* color, int Index)
{
	Vector3 Output;
	VectorSubtract(Entities_BO1[Index].Origin, Entities_BO1[cg_BO1->ClientNumber].Origin, Output);

	((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CG_DrawGrenadePointer))(0, cg_BO1->refdef.Width / 2.0f, cg_BO1->refdef.Height / 2.0f, &Output, color, 0.0f, 1.0f);
}

void DrawBones(Vector4 color, int Index)
{
	Vector3 TagPos[2], TagScreen[2];
	for (int b = 0; b < ARRAYSIZE(bonesESPBO1) - 1; b++)
	{
		if (!GetTagPosistion(&Entities_BO1[Index], Index, bonesESPBO1[b], (float*)&TagPos[0]) || !GetTagPosistion(&Entities_BO1[Index], Index, bonesESPBO1[b + 1], (float*)&TagPos[1]))
			return;

		if (WorldToScreen(TagPos[0], TagScreen[0]) && WorldToScreen(TagPos[1], TagScreen[1]))
			DrawLine_BO1(TagScreen[0].x, TagScreen[0].y, TagScreen[1].x, TagScreen[1].y,  2.0f, color);
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
		GetTagPosistion(&Entities_BO1[index], index, ThreeDBoxesBones[t], (float*)&BonePos[t]);
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
		DrawLine_BO1(minsX, minsY, corner2X, corner2Y, 1.0f, color);
		DrawLine_BO1(minsX, minsY, corner4X, corner4Y, 1.0f, color);
		DrawLine_BO1(minsX, minsY, corner6X, corner6Y, 1.0f, color);
		//maxes to 3, 5, and 7
		DrawLine_BO1(maxesX, maxesY, corner3X, corner3Y, 1.0f, color);
		DrawLine_BO1(maxesX, maxesY, corner5X, corner5Y, 1.0f, color);
		DrawLine_BO1(maxesX, maxesY, corner7X, corner7Y, 1.0f, color);
		//from 2 to 3 and 7
		DrawLine_BO1(corner2X, corner2Y, corner3X, corner3Y, 1.0f, color);
		DrawLine_BO1(corner2X, corner2Y, corner7X, corner7Y, 1.0f, color);
		//from 4 to 3 and 5
		DrawLine_BO1(corner4X, corner4Y, corner3X, corner3Y, 1.0f, color);
		DrawLine_BO1(corner4X, corner4Y, corner5X, corner5Y, 1.0f, color);
		//from 6 to 5 and 7
		DrawLine_BO1(corner6X, corner6Y, corner5X, corner5Y, 1.0f, color);
		DrawLine_BO1(corner6X, corner6Y, corner7X, corner7Y, 1.0f, color);
	}
}

void DrawESPBox(Vector4 color, int Index)
{
	float fHeight, fWidth;
	Vector3 HeadPos, FeetLocation, HeadLocation, MainRootPos;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "Corners")
	{
		if (!GetTagPosistion(&Entities_BO1[Index], Index, "j_head", (float*)&HeadPos))
			return;

		if (WorldToScreen(Entities_BO1[Index].Origin, FeetLocation) && WorldToScreen(HeadPos, HeadLocation))
		{
			fHeight = FeetLocation.y - HeadLocation.y;
			if (*(int*)((DWORD)&Entities_BO1[Index] + 0x1D4) & 0x08)
				fWidth = fHeight / 0.9;
			else
				fWidth = fHeight / 2;

			DrawBox_BO1((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawBox_BO1((FeetLocation.x - (fWidth / 2)), FeetLocation.y, (fWidth / 4), 1, color);
			DrawBox_BO1((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawBox_BO1((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawBox_BO1((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawBox_BO1((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), FeetLocation.y, (fWidth / 4), 1, color);
			DrawBox_BO1((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);
			DrawBox_BO1((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);

		}
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "3D")
	{
		CalcBoundingBox(Index, color);
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "2D" || GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "2D Filled")
	{
	
		Vector3 MainRoot, MainRootOut, FeetOut, Feet;
		GetTagPosistion(&Entities_BO1[Index], Index, "j_mainroot", (float*)&MainRoot);
		MainRoot.z += 10;

		int BoxHeight = 55; // 8 or 0x0A = prone|| 4 or 6 = crouched||0 or 2 = standing
		if (*(int*)((DWORD)&Entities_BO1[Index] + 0x1D4) & 0x08)
			BoxHeight = 17;
		else if (*(int*)((DWORD)&Entities_BO1[Index] + 0x1D4) & 0x04)
			BoxHeight = 35;

		Feet.x = MainRoot.x; Feet.y = MainRoot.y; Feet.z = (MainRoot.z - -BoxHeight - 10);

		if (WorldToScreen(MainRoot, MainRootOut) == TRUE && WorldToScreen(Feet, FeetOut) == TRUE)
		{
			FLOAT Height = (MainRootOut.y - FeetOut.y);
			FLOAT Width = (Height / 3.0f);

			if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) != "2D Filled")
			{
				DrawLine_BO1((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), 1.0f, color); // Left side line
				DrawLine_BO1((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), 1.0f, color); // Bottom line
				DrawLine_BO1((int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Right side line
				DrawLine_BO1((int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Top line
			}
			else 
				R_AddCmdDrawStretchPic_BO1((MainRootOut.x - Width), (MainRootOut.y - (Height*0.5f)), Width*2.0f, Height*1.5f, 0.0f, 0.0f, 0.0f, 0.0f, color, WhiteMaterial);
		}
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "Pyramid")
	{
		Vector3 Center = Entities_BO1[Index].Origin;

		float W = GetBoxWidth(*(int*)((DWORD)&Entities_BO1[Index] + 0x1D4)), H = GetBoxHeight(*(int*)((DWORD)&Entities_BO1[Index] + 0x1D4));

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
		if (!UserInGame())
		{
			FLOAT X, Y;
			X = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY));
			Y = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY) + 0x4);
			sprintf(Output, "Press \x016 & \x12 To %s ^7NiNJA Engine", menuOpen ? "^1Close" : "^2Open");
			DrawText_BO1(Output, (X - 360), Y - 20, 0, 0, SmallFont, Vector4(1, 1, 1, 0.75f), 0.7f, 0.7f);
		}
		else
		{
			sprintf(Output, "Press \x016 & \x12 To %s ^7NiNJA Engine", menuOpen ? "^1Close" : "^2Open");
			DrawText_BO1(Output, (cg_BO1->refdef.Width - 1200), cg_BO1->refdef.Height - 25, 0, 0, SmallFont, Vector4(1, 1, 1, 0.75f), 0.7f, 0.7f);
		}
	}
}

double Radians(float Degrees)
{
	return  0.01745329238474369 * Degrees;
}

void GetSpread_BO1(int* seed, float* x, float* y)
{
	float flSpread, Spread, FinalSpread, minSpread, maxSpread, fx, fy;
	int Weapon = Entities_BO1[cg_BO1->ClientNumber].WeaponID;
	int PlayerPTR = ((DWORD)cg_BO1 + 0x3EF9C);
	int randSeed = *seed;

	fx = Radians(RandomBulletDir_BO1(&randSeed) * 360);
	fy = RandomBulletDir_BO1(&randSeed);

	flSpread = *(float*)((DWORD)cg_BO1 + 0x69264);
	Spread = (flSpread * 0.00392156862);

	R_GetSpread_BO1(PlayerPTR, (DWORD)BG_GetWeaponDef_BO1_2(Weapon), &minSpread, &maxSpread);
	FinalSpread = minSpread + ((maxSpread - minSpread) * Spread);

	*x = cos(fx) * fy * FinalSpread;
	*y = sin(fx) * fy * FinalSpread;
}

Vector3 SmoothAimbotAngles(Vector3 AimbotAngles, float SmoothValue)
{
	float FinishedAngles[3]; float smoothdiff[3];
	AimbotAngles[0] -= LocalClient_BO1->ViewAngles.x;
	AimbotAngles[1] -= LocalClient_BO1->ViewAngles.y;
	if (AimbotAngles[0] > 180)  AimbotAngles[0] -= 360;
	if (AimbotAngles[1] > 180)  AimbotAngles[1] -= 360;
	if (AimbotAngles[0] < -180) AimbotAngles[0] += 360;
	if (AimbotAngles[1] < -180) AimbotAngles[1] += 360;
	smoothdiff[0] = AimbotAngles[0] / SmoothValue;
	smoothdiff[1] = AimbotAngles[1] / SmoothValue;
	FinishedAngles[0] = LocalClient_BO1->ViewAngles.x + smoothdiff[0];
	FinishedAngles[1] = LocalClient_BO1->ViewAngles.y + smoothdiff[1];
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
	float distanceSq = cg_BO1->refdef.ViewOrg.Distance(WorldPos);
	float distance = sqrt(distanceSq);
	float distFrac = (float)(distance - 64) / 448;
	float distScale = (float)(distFrac * scale) + (float)(1 - distFrac);
	*OutDist = distFrac;
	float x2 = (FLOAT)(floor(-((((FLOAT)R_TextWidth_BO1(Text, 0x7FFFFFFF, Font) * R_NormalizedTextScale_BO1(Font, scale * distScale)) * 0.5) - *x) + 0.5));

	__asm
	{
		frsp x2, x2
	}
	*x = x2;
	*y = (FLOAT)(floor(*y + 0.5));
}

bool IsWeaponDualWield()
{
	//return false;
	return (bool)*(BYTE*)(BG_GetWeaponDef_BO1(Entities_BO1[cg_BO1->ClientNumber].WeaponID) + 0x56A);
}

bool Autowall(int Index, PCHAR TagToUse, BOOL UsingShield)
{
	Vector3 TagPos, Angles, dir, Hax[3], MyPos, Start;
	int PlayerPTR = ((DWORD)cg_BO1 + 0x3EF9C);

	if (!GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL))
		return false;

	//if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_TARGETSHIELDS) && UsingShield)
	//	TagToUse = "j_ankle_le";

	if (!GetTagPosistion(&Entities_BO1[Index], Index, TagToUse, (float*)&TagPos))
		return false;

	CG_GetPlayerViewOrigin_BO1(0, PlayerPTR, MyPos);
	
	BulletFireParams bulletFireParams;
	BulletTraceResults BTR;
	bulletFireParams.ignoreEntIndex = cg_BO1->ClientNumber;
	bulletFireParams.weaponEntIndex = 1022;
	bulletFireParams.damageMultiplier = 1.0f;
	bulletFireParams.methodOfDeath = 2;
	VectorCopy(MyPos, bulletFireParams.origStart);
	VectorCopy(MyPos, bulletFireParams.start);
	VectorCopy(TagPos, bulletFireParams.end);
	VectorSubtract(TagPos, MyPos, dir);
	Angles = VectorToAngles(dir);
	
	AngleVectors(Angles, &Hax[0], NULL, NULL);
	VectorCopy(Hax[0], bulletFireParams.dir);

	// Disable the bullet traces
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.DynEntCl_DynEntImpactEvent) = 0x4E800020; // DynEntCl_DynEntImpactEvent
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_BulletHitEvent) = 0x4E800020; // CG_BulletHitEvent

	// Fix FMJ
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.FMJFix1) = 0x60000000; // FMJ Check
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.FMJFix2) = 0x60000000; // FMJ Check

	FireBulletPenetrate_BO1(0, &bulletFireParams, Entities_BO1[cg_BO1->ClientNumber].WeaponID, &Entities_BO1[cg_BO1->ClientNumber], MyPos, 0, 1, &BTR);

	// Re-enable the bullet traces
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.DynEntCl_DynEntImpactEvent) = 0x7D8802A6; // DynEntCl_DynEntImpactEvent
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_BulletHitEvent) = 0x7D8802A6; // CG_BulletHitEvent

	// Fix FMJ
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.FMJFix1) = 0x409A0008; // FMJ Check
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.FMJFix2) = 0x4BFE2CD9; // FMJ Check

	return (bulletFireParams.ignoreEntIndex == Index);
}

void CorrectMovement(usercmd_s_BO1 *usercommand, Vector3 vec)
{
	Vector3 Fix = vec - (LocalClient_BO1->ViewAngles);

	int forward = usercommand->forwardmove;
	int side = usercommand->sidemove;

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
	usercommand->sidemove = ClampChar(fixedside);
}

bool TriggerBot(int Index)
{
	Vector3 HeadOrigin, Hitboxes;
	Vector3 FeetOrigin = Entities_BO1[Index].Origin;
	GetTagPosistion(&Entities_BO1[Index], Index, "j_head", (float*)&HeadOrigin);
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
			GetTagPosistion(&Entities_BO1[Index], Index, TriggerBotBones[b], (float*)&Hitboxes);
			if (WorldPosToScreenPos((float*)&Hitboxes, &ScreenPos))
			{
				if (ScreenPos.x - ((fWidth / 5) / 2) < (cg_BO1->refdef.Width / 2) &&
					ScreenPos.x + ((fWidth / 5) / 2) > (cg_BO1->refdef.Width / 2) &&
					ScreenPos.y - ((fWidth / 5) / 2) < (cg_BO1->refdef.Height / 2) &&
					ScreenPos.y + ((fWidth / 5) / 2) > (cg_BO1->refdef.Height / 2))
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
	float SmoothScale = 0.5f;
	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "0.5")
		SmoothScale = 0.5f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "1.0")
		SmoothScale = 1.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "2.0")
		SmoothScale = 2.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "3.0")
		SmoothScale = 3.0f;

	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) == "4.0")
		SmoothScale = 4.0f;

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
	GetTagPosistion(&Entities_BO1[index], index, Tag, Output);
	if (WorldPosToScreenPos(Output, &ScreenPos))
	{
		if (ScreenPos.x > ((cg_BO1->refdef.Width / 2) - FOVSize) &&
			ScreenPos.x < ((cg_BO1->refdef.Width / 2) + FOVSize) &&
			ScreenPos.y >((cg_BO1->refdef.Height / 2) - FOVSize) &&
			ScreenPos.y < ((cg_BO1->refdef.Height / 2) + FOVSize))
		{
			return true;
		}
	}

	return false;
}

float ScaleRadarX(float X, float ScreenWidth)
{
	return (X * (640.0f / ScreenWidth));
}

float ScaleRadarY(float Y, float ScreenHeight)
{
	return (Y * (480.0f / ScreenHeight));
}

void WorldToCompass_BO1(unkRadarStruct* radarHud, Vector2& size, Vector3& World, Vector2& Compass, Vector2& widthHeight)
{
	Vector4 RadarSize;
	CG_CompassCalcDimensions_BO1(0, cg_BO1, size, radarHud, &RadarSize.x, &RadarSize.y, &RadarSize.z, &RadarSize.a);

	float x2 = (RadarSize.z * 0.5) + RadarSize.x;
	float y2 = (RadarSize.a * 0.5) + RadarSize.y;

	Vector2 out, outClipped, Yaw;

	CG_CompassUpYawVector_BO1(cg_BO1, Yaw);

	CG_WorldPosToCompass_BO1(0, cg_BO1, RadarSize.z, RadarSize.a, Yaw, Entities_BO1[cg_BO1->ClientNumber].Origin, World, out, outClipped);

	Compass.x = -((widthHeight.x * 0.5) - x2) + outClipped.x;
	Compass.y = -((widthHeight.y * 0.5) - y2) + outClipped.y;
}

void DrawRadar_BO1()
{
	unkRadarStruct radarStruct;
	Vector2 size;
	//DWORD compassPingFriendly = *(DWORD*)0x82BBB2EC;
	size.x = ScaleRadarX(12, cg_BO1->refdef.Width);
	size.y = ScaleRadarY(12, cg_BO1->refdef.Height);

	radarStruct.fontName = 0x8201A394;
	radarStruct.materialName = 0x820089F0;
	radarStruct.Resolution.x = (float)cgs_BO1->Width;
	radarStruct.Resolution.y = (float)cgs_BO1->Height;
	radarStruct.Resolution2.x = (float)cgs_BO1->Width;
	radarStruct.Resolution2.y = (float)cgs_BO1->Height;


	*(float*)(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement) + 0x24) = 90.0f;  // Alignment
	*(float*)(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement) + 0x28) = 40.0f;  // Height
	*(float*)(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement) + 0x2C) = 270.0f; // Width
	*(float*)(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement) + 0x30) = 140.0f; // Map Size


	DrawEngineRadar_BO1(0, 0, getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), &radarStruct, 1.0f, WhiteMaterial);

	// Draw Dropped Items
	for (int i = 100; i < 800; i++)
	{
		/*if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS))
		{
			if ((*(int*)((DWORD)&Entities_BO1[i] + 0x1C4) & 0x1) == 1 && Entities_BO1[i].Type == 3)
			{
				Vector2 Screen;
				Vector3 Origin = Entities_BO1[i].Origin;

				// Make sure it's not a scavenger pack
				if ((Entities_BO1[i].WeaponNum != CG_GetWeaponIndexForName_BO1("scavenger_bag_mp")))
					continue;


				Vector2 Compass, widthHeight;
				widthHeight.x = 25;
				widthHeight.y = 15;
				WorldToCompass_BO1(&radarStruct, size, Entities_BO1[i].Origin, Compass, widthHeight);

				int horz = radarHud.unk >> 24;
				int vert = (radarHud.unk >> 16) & 0xFF;

				CG_DrawRotatedPic_BO1(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), Compass.x, Compass.y, widthHeight.x, widthHeight.y, horz, vert, 0, Vector4(1, 1, 1, 0.75f), ScavMaterial);
				//CG_DrawRotatedPic_BO1(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), Compass.x, Compass.y, widthHeight.x, widthHeight.y, horz, Vector4(1, 1, 1, 0.75f), ScavMaterial);
			}
		}*/
	}

	// Draw Enemies
	for (int client = 0; client < 18; client++)
	{
		/*if (Entities_BO1[client].Type != 1 || (*(int*)((DWORD)&Entities_BO1[client] + 0xCC) & 0x20) || (*(int*)((DWORD)&Entities_BO1[client] + 0x1C4) & 0x1) == 0)
			continue;

		if ((cg_BO1->clientInfo[cg_BO1->ClientNumber].Team == cg_BO1->clientInfo[i].Team) && (cg_BO1->clientInfo[cg_BO1->ClientNumber].Team != 0))
			continue;

		Vector2 Compass, widthHeight;
		widthHeight.x = 16;
		widthHeight.y = 16;
		WorldToCompass_BO1(&radarHud, size, Entities_BO1[client].Origin, Compass, widthHeight);

		int horz = radarHud.unk >> 24;
		int vert = (radarHud.unk >> 16) & 0xFF;

		float angle = *(float*)((DWORD)cg_BO1 + 0x69F58 + (client * 0x808));
		CG_DrawRotatedPic_BO1(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), Compass.x, Compass.y, widthHeight.x, widthHeight.y, horz, vert, angle, Vector4(1, 0, 0, 0.75f), compassPingFriendly);
		//CG_DrawRotatedPic_BO1(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), Compass.x, Compass.y, widthHeight.x, widthHeight.y, horz, Vector4(1, 0, 0, 0.75f), compassPingFriendly); */
	}

	//DrawEnemies_BO1(0, 0, getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), &radarStruct, 1.0f, WhiteMaterial);
	DrawFriendlies_BO1(0, 0, getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), &radarStruct, 1.0f, WhiteMaterial);
	DrawVehicles_BO1(0, 0, getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), &radarStruct, 1.0f, WhiteMaterial);
	DrawPlayer_BO1(0, 0, getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), &radarStruct, 1.0f, WhiteMaterial);
}

void CG_EntityEventHook(int localClient, int entityState_t, int Event, int eventParam)
{
	if (GetMenuOptionValue(MENU_MAIN, OPTION_FASTRELOAD))
	{
		if (Event == 23)
		{
			short ClientReloadIndex = *(short*)(entityState_t + 0x1DC);
			if (ClientReloadIndex == (short)cg_BO1->ClientNumber)
				WasReloading = TRUE;
		}
		else if (Event == 26)
		{
			short ClientSwitchIndex = *(short*)(entityState_t + 0x8);
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
	if (GetMenuOptionValue(MENU_MAIN, OPTION_FASTRELOAD) && UserInGame())
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
		rectDef->horizAlign = 0x08080000;
	else
	{
		rectDef->horizAlign = 0x3;
		rectDef->vertAlign = 0x1;
	}

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
	if (LastCompassType == 0)
	{
		horizAlign = 0x8;
		vertAlign = 0x8;
	}
	else
	{
		horizAlign = 0x3;
		vertAlign = 0x1;
	}
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
	int playerStatePtr = cg_t + 0x3EF9C;
	Vector3 MyPos;
	CG_GetPlayerViewOrigin_BO1(0, playerStatePtr, MyPos);
	WorldPosToCompass(cg_t, LastCompassType, RadarSize, Yaw, MyPos, World, outClipped);

	Vector2 widthHeight(16, 16);
	Compass.x = -((widthHeight.x * 0.5) - x2) + outClipped.x;
	Compass.y = -((widthHeight.y * 0.5) - y2) + outClipped.y;

	if (LastCompassType == 0)
	{
		Compass.x -= 3.0f;
		Compass.y -= 3.0f;
	}
	else
	{
		Compass.x += 250.0f;
		Compass.y -= 100.0f;
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
		return (DWORD)cg_t + 0x41a04;
	return 0;
}
int getViewAxis_ptr()
{
	DWORD refDef = getRefDef();
	if (refDef)
		return refDef + 0x3c;
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
			if ((*(int*)((DWORD)&Entities_BO1[i] + 0x318) & 0x40000000) != 0 && *(short*)((DWORD)&Entities_BO1[i] + 0x29A) == 3)
			{
				int DroppedWeapon = *(int*)((DWORD)&Entities_BO1[i] + 0x274);

				// Make sure it's a scavenger pack
				if (strstr(getWeaponName_BO1(DroppedWeapon), "SCAVENGER"))
					IsWeapon = FALSE;

				// Set color based on weapon/pack
				Vector4 color = !IsWeapon ? Vector4(1, 1, 1, 1) : Vector4(0.51f, 1, 1, 1);
				if (IsWeapon)
				{
					if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR) && DrawCompassTimer > GetTickCount())
					{
						Vector2 compass;
						WorldToCompass(Entities_BO1[i].Origin, compass);
						((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.UI_DrawHandlePic))(getCryptedCoD(CheatPatches->CoDPatches.ScreenPlacement), compass.x, compass.y, 20.0f, 10.0f, 0x8, 0x8, 0.0f, &color, ScavMaterial);
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
			if (Entities_BO1[i].ClientNumber != cg_BO1->ClientNumber && cg_BO1->ClientNumber != i)
			{
				// Alive and is player
				if (*(short*)((DWORD)&Entities_BO1[i] + 0x29A) != 1 || (*(int*)((DWORD)&Entities_BO1[i] + 0x318) & 0x40000000) == 0 || (*(int*)((DWORD)&Entities_BO1[i] + 0x1E0) & 0x40000))
				{
					// Invalid player
					continue;
				}

				//if (LastCompassType == 1)
				//	goto skip;

				int centity = *(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer);

				if (centity)
				{
					centity = centity + (i * 0x31c);

					vec3 entityView = *(vec3*)(centity + 0x1a0);
					vec3 myView = getRefDefViewAngles();

					float yaw = entityView.y;

					float rotation = (360.0f - yaw);
					if (LastCompassType == 0)
						rotation += myView.y;
					//else if (currentTitle == MW3_TITLE_ID)
					//	rotation += 90.0f;

					float w = 20.0, h = 20.0;
					Vector2 Compass;
					WorldToCompass(Entities_BO1[i].Origin, Compass); 
					DrawRadarPic(Compass.x, Compass.y, w, h, rotation, PlayerInfo[i].OnTeam ? (float*)&Vector4(0, 1, 0, 0.75f) : (float*)&Vector4(1, 0, 0, 0.75f), ArrowIcon);
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
				int playerStatePtr = ((DWORD)cg_BO1 + 0x3EF9C);
				Vector3 MyPos;
				CG_GetPlayerViewOrigin_BO1(0, playerStatePtr, MyPos);
				WorldToCompass(MyPos, Compass);
				float w = 20.0f, h = 20.0f;
				DrawRadarPic(Compass.x, Compass.y, w, h, 0.0f, (float*)&GetColorCode(colorWhite), ArrowIcon);
			}
		}
	}
}

void DrawESP()
{
	LocalClient_BO1 = (pclientActive_t_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu11
	Entities_BO1 = (pcentity_s_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu11
	cg_BO1 = (pcg_s_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu11
	cgs_BO1 = (pcgs_t_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu11

	// If in-game, start our ESP
	if (UserInGame() && menuReady && (cgs_BO1 != NULL))
	{
		// Loop for all ents (this 1st so our player ESP draws over it if they over lap)
		for (int i = 18; i < 0x7FF; i++)
		{
			// Draw Explosives
			if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWEXPLOSIVES))
			{
				// Make sure it's a explosive
				if ((*(int*)((DWORD)&Entities_BO1[i] + 0x318) & 0x40000000) != 0 && *(short*)((DWORD)&Entities_BO1[i] + 0x29A) == 4 && (Entities_BO1[i].WeaponID < 0x800))
				{
					//DbgPrint("Weapon ID: 0x%08X", Entities_BO1[i].WeaponID);
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					TopLocation = Entities_BO1[i].Origin;
					if (WorldToScreen(TopLocation, TopLocationOut))
					{
						// Get the explosive id
						int ExplosiveID = Entities_BO1[i].WeaponID;

						// Since we can't draw flashbangs for whatever reason, lets just draw the stun icon for them
						if (ExplosiveID == 0x673)
						{
							ExplosiveID = 0x675;
						}

						// Make sure it's valid
						if (ExplosiveID < 0x800 && ExplosiveID != 0)
						{
							Vector4 color = Vector4(1, 1, 0, 1);

							float aspectW, aspectH;
							int ExplosiveIcon = GetWeaponIcon(ExplosiveID, &aspectW, &aspectH);
							if (ExplosiveIcon != 0)
							{
								CG_DrawRotatedPicPhysical_BO1(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), TopLocationOut.x - (aspectW/2), TopLocationOut.y - (aspectH/2), aspectW, aspectH, 0.0, GetColorCode(colorYellow), ExplosiveIcon);
							}
						}
					}
				}
			}

			// Draw Dropped Items
			if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS))
			{
				BOOL IsWeapon = TRUE, ValidItem = FALSE;

				// Make sure it's a dropped item
				if ((*(int*)((DWORD)&Entities_BO1[i] + 0x318) & 0x40000000) != 0 && *(short*)((DWORD)&Entities_BO1[i] + 0x29A) == 3 && (GetDistance(cg_BO1->refdef.ViewOrg, Entities_BO1[i].Origin) < 1500.0f))
				{
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					TopLocation = Entities_BO1[i].Origin;
					if (WorldToScreen(TopLocation, TopLocationOut))
					{
						// Get the dropped weapon id
						int DroppedWeapon = *(int*)((DWORD)&Entities_BO1[i] + 0x274);

						// Make sure it's valid / not a weapon
						if (DroppedWeapon < 0x800 && DroppedWeapon != 0)
						{
							if (strstr(getWeaponName_BO1(DroppedWeapon), "SCAVENGER"))
								IsWeapon = FALSE;

							ValidItem = TRUE;
						}

						// Set color based on weapon/pack
						Vector4 color = IsWeapon ? Vector4(1, 1, 1, 1) : Vector4(0.51f, 1, 1, 1);

						// Draw the item if it's valid
						if (ValidItem)
						{
							if (IsWeapon)
							{
								float aspectW, aspectH;
								int DroppedIcon = GetWeaponIcon(DroppedWeapon, &aspectW, &aspectH);
								if (DroppedIcon != 0)
								{
									CG_DrawRotatedPicPhysical_BO1(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), TopLocationOut.x - (aspectW/2), TopLocationOut.y - (aspectH/2), aspectW, aspectH, 0.0, color, DroppedIcon);
								}

							}
							else
							{
								CG_DrawRotatedPicPhysical_BO1(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), TopLocationOut.x - 30, TopLocationOut.y - 10, 50, 25, 0.0, color, ScavMaterial);
							}
						}
					}
				}
			}
		}

		// Loop for all clients
		for (int i = 0; i < cgs_BO1->MaxClients; i++)
		{
			// Make sure the client is not us
			if (Entities_BO1[i].ClientNumber != cg_BO1->ClientNumber && cg_BO1->ClientNumber != i)
			{
				// Alive and is player
				if (*(short*)((DWORD)&Entities_BO1[i] + 0x29A) != 1 || (*(int*)((DWORD)&Entities_BO1[i] + 0x318) & 0x40000000) == 0 || (*(int*)((DWORD)&Entities_BO1[i] + 0x1E0) & 0x40000))
				{
					// Invalid player
					continue;
				}

				// Set some of the players info in our struct
				PlayerInfo[i].Alive = TRUE;
				PlayerInfo[i].ClientNumber = Entities_BO1[i].ClientNumber;
				PlayerInfo[i].Name = cg_BO1->clientInfo[i].Name;

				// Team check
				if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_INVERTTEAMCHECK))
				{
					if ((cg_BO1->clientInfo[cg_BO1->ClientNumber].Team == cg_BO1->clientInfo[i].Team) && (cg_BO1->clientInfo[cg_BO1->ClientNumber].Team != 0))
					{
						// Save players team
						PlayerInfo[i].Team = cg_BO1->clientInfo[i].Team;
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

						PlayerInfo[i].Team = cg_BO1->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = FALSE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
					}
				}
				else
				{
					// Reverse team check
					if ((cg_BO1->clientInfo[cg_BO1->ClientNumber].Team == cg_BO1->clientInfo[i].Team) && (cg_BO1->clientInfo[cg_BO1->ClientNumber].Team != 0))
					{
						// If we want to draw enemies
						if (!GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMIES))
							continue;

						PlayerInfo[i].Team = cg_BO1->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = FALSE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
					}
					else
					{
						// Save players team
						PlayerInfo[i].Team = cg_BO1->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = TRUE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;

						// If we want to draw friendlies
						if (!GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWFRIENDLIES))
							continue;
					}
				}

				// Visible / Wallbang Check
				if (!PlayerInfo[i].OnTeam)
				{
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_BO1(0, &Entities_BO1[i], TagToShort("j_head"));

					// Get the players posistion for our visible check
					GetTagPosistion(&Entities_BO1[i], i, "j_head", (float*)&PlayerInfo[i].TagPosistion);

					// Run our trace
					BYTE CL_TraceResult[100];
					CG_LocationalTraceBO1((void*)CL_TraceResult, &cg_BO1->refdef.ViewOrg, &PlayerInfo[i].TagPosistion, i, 0x803003, 0, 0);

					// If the trace fraction is >= 0.97 let's say they are visible
					float traceFraction = *(float*)(CL_TraceResult + 0x10);
					PlayerInfo[i].VisibleESP = ((traceFraction >= 0.97f) ? TRUE : FALSE);
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_BO1(0, &Entities_BO1[i], TagToShort("j_head"));

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
				Vector4 Color = PlayerInfo[i].OnTeam ? Vector4(0.51f, 1, 1, 0.75f) : GetColorCode(colorOrange); // Vector4(0.51f, 1, 1, 0.50f)

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
					GetTagPosistion(&Entities_BO1[i], i, "j_helmet", (float*)&Pos[1]);
					FeetPos = Entities_BO1[i].Origin;

					if (WorldToScreen(Pos[1], xy1[0]) && WorldToScreen(FeetPos, FeetPosOut))
					{
						if (Entities_BO1[i].WeaponID < 0x800 && Entities_BO1[i].WeaponID != 0)
						{
							float aspectW, aspectH;
							int WeaponIcon = GetWeaponIcon(Entities_BO1[i].WeaponID, &aspectW, &aspectH);

							if (WeaponIcon != 0)
							{
								CG_DrawRotatedPicPhysical_BO1(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), xy1[0].x - (aspectW/2), xy1[0].y - (aspectH/2) - 10, 50, 20, 0.0, Color, WeaponIcon);
							}
						}
					}
				}

				// ESP Name
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWNAME))
				{
					Vector3 HeadPos, HeadPosOut, FeetPos, FeetPosOut;
					GetTagPosistion(&Entities_BO1[i], i, "j_head", (float*)&HeadPos);
					FeetPos = Entities_BO1[i].Origin;

					if (WorldToScreen(HeadPos, HeadPosOut) && WorldToScreen(FeetPos, FeetPosOut))
					{
						if (MmIsAddressValid((PCHAR*)cg_BO1->clientInfo[i].Name))
						{
							float DisanceOut;
							Vector3 FeetPosOutOG, HeadPosOutOG;
							VectorCopy(FeetPosOut, FeetPosOutOG);
							VectorCopy(HeadPosOut, HeadPosOutOG);

							FixTextForDistance(cg_BO1->clientInfo[i].Name, &FeetPosOut.x, &FeetPosOut.y, SmallFont, 1, FeetPos, &DisanceOut);
							//FeetPosOut.y -= ((HeadPosOutOG.y - FeetPosOutOG.y) * 0.750);
							FeetPosOut.y += (((FeetPosOutOG.y - HeadPosOutOG.y) * (DisanceOut * 0.0001f) + 15));

							DrawText_BO1(cg_BO1->clientInfo[i].Name, (FeetPosOutOG.x - (R_TextWidth_BO1(cg_BO1->clientInfo[i].Name, 0x7FFFFFFF, SmallFont) / 2) * 0.5f), FeetPosOut.y, 0, 0, SmallFont, Color, 0.5f, 0.5f);
						}
					}
				}

				// Enemy Visible Text
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMYVISIBLE))
				{
					if (PlayerInfo[i].VisibleESP && !PlayerInfo[i].OnTeam)
					{
						DrawText_BO1("Visible Enemy!", ((cg_BO1->refdef.Width / 2) - (R_TextWidth_BO1("Visible Enemy!", 0x7FFFFFFF, HudSmallFont) / 2) * 0.7f), (cg_BO1->refdef.Height / 2) - 120, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
					}
				}

				// Proximity Warning Text
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWPROXIMITYWARNING))
				{
					if (!PlayerInfo[i].OnTeam)
					{
						float OutPos[3];
						GetTagPosistion(&Entities_BO1[i], i, "j_head", OutPos);
						if ((GetDistance(cg_BO1->refdef.ViewOrg, OutPos) / 100 < 5))
						{
							DrawText_BO1("Proximity Warning!", ((cg_BO1->refdef.Width / 2) - (R_TextWidth_BO1("Proximity Warning!", 0x7FFFFFFF, HudSmallFont) / 2) * 0.7f), (cg_BO1->refdef.Height / 2) - 170, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
						}
					}
				}
			}

			// Continue if the client mumber is ours
			else continue;
		}

		/*// Draw our custom radar
		if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR))
		{
			*(DWORD*)0x823E0D00 = 0x60000000; // DrawEngineRadar
			*(DWORD*)0x823E0F08 = 0x60000000; // DrawFriendlies
			*(DWORD*)0x823E0F88 = 0x60000000; // DrawVehicles
			*(DWORD*)0x823E0F24 = 0x60000000; // DrawEnemies
			*(DWORD*)0x823E0FC0 = 0x60000000; // DrawPlayer
			DrawRadar_BO1();
		}
		else
		{
			*(DWORD*)0x823E0D00 = 0x4BFF9D89; // DrawEngineRadar
			*(DWORD*)0x823E0F08 = 0x4BFFC0D1; // DrawFriendlies
			*(DWORD*)0x823E0F88 = 0x4BFFD7E1; // DrawVehicles
			*(DWORD*)0x823E0F24 = 0x4BFFC95D; // DrawEnemies
			*(DWORD*)0x823E0FC0 = 0x4BFFE609; // DrawPlayer
		}*/
	}
}

void R_EndFrameHook()
{
	LocalClient_BO1 = (pclientActive_t_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu11
	Entities_BO1 = (pcentity_s_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu11
	cg_BO1 = (pcg_s_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu11
	cgs_BO1 = (pcgs_t_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu11

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
		DrawMenu_BO1();
	}
	else
	{
		menuWidth = 0;
		menuHeight = 0;
	}

	// Handles toggle options like no recoil, sway, uav, etc
	HandleToggleOptions(cgs_BO1);

	bool inGame = UserInGame();

	// Draw our NiNJA Engine v1.0 text at the top right of screen pre-game
	if (!inGame && menuReady)
	{

		if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLENINJATEXT))
		{
			FLOAT X, Y;
			X = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY));
			Y = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY) + 0x4);
			if (EngineText == NULL)
				EngineText = E_ALLOC("NiNJA Engine v1.0");
			DrawText_BO1(EngineText, ((X - 130) - (R_TextWidth_BO1(EngineText, 0x7FFFFFFF, SmallFont) / 2) * 0.7f), (Y / 2) - 330, 0, 0, SmallFont, Vector4(0.102f, 1, 0.102f, 0.50f), 0.5f, 0.5f);
		}
	}
	else if (inGame && menuReady)
	{
		// NiNJA Engine v1.0
		if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLENINJATEXT))
			DrawAd();
	}

	// Draw our menu controls
	DrawMenuControlsText();

	if (inGame && menuReady)
		DrawCustomRadar();

	// Call the original function
	R_EndFrameDetour->CallOriginal(0);
}

void Menu_PaintAllHook(int r3, int r4)
{
	LocalClient_BO1 = (pclientActive_t_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu11
	Entities_BO1 = (pcentity_s_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu11
	cg_BO1 = (pcg_s_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu11
	cgs_BO1 = (pcgs_t_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu11

	// Draw our ESP
	DrawESP();

	// Call the original
	return Menu_PaintAllDetour->CallOriginal(r3, r4);
}

int FindBestBoneByDamage(int EntIndex, BOOL HasRiotShield)
{
	Vector3 VisPos, MyPos;
	float totalDamage = 0;
	DWORD BoneIndex = 0;
	BYTE CL_TraceResult[100];
	float traceFraction;

	// Our hit location indexs
	BYTE HitLocationIndexs[7] = { 2, 6, 4, 8, 7, 0x12, 0x11 };
	BYTE HitLocTest[13] = { HITLOC_HEAD,  HITLOC_NECK, HITLOC_TORSO_UPR, HITLOC_TORSO_MID, HITLOC_TORSO_LWR, HITLOC_R_ARM_UPR, HITLOC_L_ARM_UPR, HITLOC_R_HAND, HITLOC_L_HAND, HITLOC_R_LEG_UPR, HITLOC_L_LEG_UPR, HITLOC_R_FOOT, HITLOC_L_FOOT };
	int PlayerPTR = ((DWORD)cg_BO1 + 0x3EF9C);

	if (HasRiotShield)
	{
		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_BO1[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_BO1(0, PlayerPTR, MyPos);
		CG_LocationalTraceBO1((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003, 0, 0);

		// Check our trace fraction
		traceFraction = *(float*)(CL_TraceResult + 0x10);
		if (traceFraction >= 0.95f)
			return 5;

		return -1;
	}

	for (int i = 0; i < 13; i++)
	{
		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_BO1[EntIndex], EntIndex, HitboxVisCheckBones[i], (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_BO1(0, PlayerPTR, MyPos);
		CG_LocationalTraceBO1((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003, 0, 0);

		// Check our trace fraction
		traceFraction = *(float*)(CL_TraceResult + 0x10);
		if (traceFraction >= 0.95f)
		{
			// Get weapon def
			DWORD WeaponDef = BG_GetWeaponDef_BO1(Entities_BO1[cg_BO1->ClientNumber].WeaponID);

			// Get our temp damage from weaponDamage * HitLocationMultiplier
			float tempDamage = (float)*(int*)(WeaponDef + 0x35C) * G_GetWeaponHitLocationMultiplier_BO1(HitLocTest[i], Entities_BO1[cg_BO1->ClientNumber].WeaponID);
			if (tempDamage > totalDamage)
			{
				totalDamage = tempDamage;
				BoneIndex = i;
			}
		}
	}

	if (totalDamage > 0)
		return BoneIndex;

	return -1;
}

int FindBestVisibleBoneIndex(int EntIndex, BOOL HasRiotShield)
{
	Vector3 VisPos, MyPos;
	BYTE CL_TraceResult[100];
	int PlayerPTR = ((DWORD)cg_BO1 + 0x3EF9C);
	float TraceFraction;

	if (HasRiotShield)
	{
		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_BO1[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_BO1(0, PlayerPTR, MyPos);
		CG_LocationalTraceBO1((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003, 0, 0);

		// Check our trace fraction
		TraceFraction = *(float*)(CL_TraceResult + 0x10);
		if (TraceFraction >= 0.95f)
			return 6;

		return -1;
	}

	for (int i = 0; i < 13; i++)
	{
		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_BO1[EntIndex], EntIndex, HitboxVisCheckBones[i], (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_BO1(0, PlayerPTR, MyPos);
		CG_LocationalTraceBO1((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003, 0, 0);

		// Check our trace fraction
		TraceFraction = *(float*)(CL_TraceResult + 0x10);
		if (TraceFraction >= 0.95f)
			return i;

		continue;
	}

	return -1;
}

int FindBestAutoWallBoneIndex(int EntIndex, BOOL HasRiotShield)
{
	for (int i = 0; i < 7; i++)
	{
		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		BOOL Wallbangable = FALSE;
		Wallbangable = Autowall(EntIndex, AutoWallBones[i], HasRiotShield);
		if (Wallbangable)
			return i;

		continue;
	}

	return -1;
}

BOOL IsPlayerEnemy(int ClientIndex, int myClientNum)
{
	// If whitelisted
	if (ClientOptionEnabled[2][ClientIndex])
		return FALSE;

	// Handle Normal / Inverted Team Check
	if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_INVERTTEAMCHECK))
	{
		if ((cg_BO1->clientInfo[myClientNum].Team == cg_BO1->clientInfo[ClientIndex].Team) && (cg_BO1->clientInfo[myClientNum].Team != 0))
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
	}
	else
	{
		// Reverse team check
		if ((cg_BO1->clientInfo[myClientNum].Team == cg_BO1->clientInfo[ClientIndex].Team) && (cg_BO1->clientInfo[myClientNum].Team != 0))
		{
			// Invert individual player team check
			if (ClientOptionEnabled[3][ClientIndex])
				return FALSE;
			else
				return TRUE;
		}
		else
		{
			// Invert individual player team check
			if (ClientOptionEnabled[3][ClientIndex])
				return TRUE;
			else
				return FALSE;
		}
	}


	// We shouldn't get here
	return FALSE;
}

BOOL ValidPlayer(int ClientIndex, int myClientNum, BOOL UsingRagebot)
{
	// If it's us
	if (ClientIndex == myClientNum)
		return false;

	// If alive
	if (*(short*)((DWORD)&Entities_BO1[ClientIndex] + 0x29A) != 1 || (*(int*)((DWORD)&Entities_BO1[ClientIndex] + 0x318) & 0x40000000) == 0 || (*(int*)((DWORD)&Entities_BO1[ClientIndex] + 0x1E0) & 0x40000))
		return false;

	// If enemy
	if (!IsPlayerEnemy(ClientIndex, myClientNum))
		return false;

	// If they have god mode
	if (Entities_BO1[ClientIndex].GodmodeCheck == 0)
		return false;

	// If whitelisted
	if (ClientOptionEnabled[2][ClientIndex])
		return false;

	return true;
}

void CalcPrediction(int clientIndex, Vector3 BonePos, int Scale)
{
	Vector3 EnemyVelocityTemp;
	Vector3 EnemyVelocity;

	VectorSubtract(*(Vector3*)((DWORD)&Entities_BO1[clientIndex] + 0x1F4), *(Vector3*)((DWORD)&Entities_BO1[clientIndex] + 0x17C), EnemyVelocityTemp);

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

int bestTarget = -1;
DWORD FindBestTarget(BOOL UsingRagebot)
{
	DWORD dwClosestClient = -1;
	FLOAT Closest = 900000;
	eMenuNames AimbotMenu = UsingRagebot ? MENU_RAGEBOT : MENU_LEGITBOT;

	// Grab cg to get playerstate pointer
	int PlayerPTR = ((DWORD)cg_BO1 + 0x3EF9C);

	// Loop for all players
	for (int i = 0; i < cgs_BO1->MaxClients; i++)
	{
		BOOL Visible = FALSE, Wallbangable = FALSE, UsingShield = FALSE;

		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_BO1->ClientNumber, UsingRagebot))
			continue;

		// Check the user for a shield
		//if (Entities_BO1[i].WeaponID == CG_GetWeaponIndexForName_BO1("riotshield_mp"))
		//	UsingShield = TRUE;

		// If we want to ignore shields
		if (!GetMenuOptionValue(AimbotMenu, OPTION_TARGETSHIELDS) && UsingShield)
			continue;

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
			GetTagPosistion(&Entities_BO1[i], i, AimTag, (float*)&VisPos);

			// Run our trace
			BYTE CL_TraceResult[100];
			CG_GetPlayerViewOrigin_BO1(0, PlayerPTR, MyPos);
			CG_LocationalTraceBO1((void*)CL_TraceResult, &MyPos, &VisPos, i, 0x803003, 0, 0);

			float traceFraction = *(float*)(CL_TraceResult + 0x10);

			// Check our trace fraction
			Visible = ((traceFraction >= 0.95f) ? TRUE : FALSE);
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
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_BO1[i].Origin, (float*)&cg_BO1->refdef.ViewOrg));

		// For now just make them seem closest
		if (ClientOptionEnabled[1][i])
			TempDistance = 5.0f;

		// Update our closest player info
		if (TempDistance < Closest)
		{
			dwClosestClient = i;
			Closest = TempDistance;
			bestTarget = dwClosestClient;
		}
	}

	return dwClosestClient;
}

DWORD FindBestAntiAimTarget()
{
	DWORD dwAAClosestClient = -1;
	FLOAT ClosestAA = 900000;

	// Loop for all players
	for (int i = 0; i < cgs_BO1->MaxClients; i++)
	{
		BOOL Visible = FALSE, Wallbangable = FALSE, UsingShield = FALSE;

		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_BO1->ClientNumber, NULL))
			continue;

		// Grab our temp distance
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_BO1[i].Origin, (float*)&cg_BO1->refdef.ViewOrg));

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

void TestThread(unsigned int r3)
{
	printf("thread started!\n");
	((VOID(*)(...))0x823EED68)(3);
	//__try
	//{
		while (true)
		{
			//printf("running thread\n");
			if (UserInGame())
			{
				// Draw our ESP
				DrawESP();
			}
		}
	//}
//	__except (1)
	//{
	//	printf("Thread crashed\n");
	//}
}

VOID Ragebot(pusercmd_s_BO1 usercommand, pusercmd_s_BO1 oldusercommand, pusercmd_s_BO1 nextusercommand, float movementsave)
{
	Vector3 EnemyPosistion, MyOrigin, Angles, VectorAngles, FinalAngles, SpreadAngles;
	PCHAR AimbotTag = "j_head";
	int PlayerPTR = ((DWORD)cg_BO1 + 0x3EF9C);
	XINPUT_STATE State;

	// Get our best target
	DWORD BestTarget = FindBestTarget(TRUE);
	GClosestClient = BestTarget;
	if (BestTarget == -1)
		return;

	// Update our input state
	if (FAILED(XInputGetState(0, &State)))
		return;

	// Get the best targets posistion
	GetTagPosistion(&Entities_BO1[BestTarget], BestTarget, AimTag, (float*)&EnemyPosistion);

	// Calculate prediction if we want it
	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) != "Off")
		CalcPrediction(BestTarget, EnemyPosistion, FindPredicationScale());

	// Get our view origin
	CG_GetPlayerViewOrigin_BO1(0, PlayerPTR, MyOrigin);

	// Calculate aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles);
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_BO1->SpawnAngles, FinalAngles);

	// Apply our aimbot angles
	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_RAGEBOTTYPE) == "Silent")
	{
		usercommand->ViewAngles[0] = ANGLE2SHORT(FinalAngles.x);
		usercommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y);

		// Fix our movement
		CorrectMovement(usercommand, FinalAngles);

	}
	else if(GetMenuOptionString(MENU_RAGEBOT, OPTION_RAGEBOTTYPE) == "Normal")
	{
		// Set our angles 
		LocalClient_BO1->ViewAngles[0] = FinalAngles.x;
		LocalClient_BO1->ViewAngles[1] = FinalAngles.y;
	}

	// Do our no spread
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD))
	{
		// Get our seed from the current server time
		int Seed = nextusercommand->serverTime;

		// Calculate our no spread
		GetSpread_BO1(&Seed, &SpreadAngles.x, &SpreadAngles.y);

		// Apply our no spread
		usercommand->ViewAngles[0] -= ANGLE2SHORT(-SpreadAngles.y);
		usercommand->ViewAngles[1] -= ANGLE2SHORT(-SpreadAngles.x);
	}

	// Autoshoot
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT))
	{
		if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD))
		{
			nextusercommand->Buttons |= 0x80000000;
			if (usercommand->Buttons & 0x80000000)
			{
				usercommand->Buttons &= 0x7FFFFFFF;
			}

			if (IsWeaponDualWield())
			{
				nextusercommand->Buttons |= 0x00100280;
			}
		}

		else
		{
			// If not using no spread, wait until aimed in before shooting
			//if (cg_BO1->ZoomState >= 0.5f)
			//{
				nextusercommand->Buttons |= 0x80000000;
				if (usercommand->Buttons & 0x80000000)
				{
					usercommand->Buttons &= 0x7FFFFFFF;
				}

				if (IsWeaponDualWield())
				{
					nextusercommand->Buttons |= 0x00100280;
				}
			//}
		}
	}

	// Hit shots without autoshoot
	if (!GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT) && (State.Gamepad.bRightTrigger > 35))
	{
		nextusercommand->Buttons |= 0x80000000;
		if (usercommand->Buttons & 0x80000000)
		{
			usercommand->Buttons &= 0x7FFFFFFF;
		}

		if (IsWeaponDualWield())
		{
			nextusercommand->Buttons |= 0x00100280;
		}
	}
}

VOID Legitbot(pusercmd_s_BO1 usercommand, pusercmd_s_BO1 nextusercommand)
{
	Vector3 EnemyPosistion, MyOrigin, Angles, VectorAngles, FinalAngles, LegitAngles;
	BOOL DelayDone = FALSE;
	XINPUT_STATE State;
	int PlayerPTR = ((DWORD)cg_BO1 + 0x3EF9C);

	// Get our best target
	DWORD BestTarget = FindBestTarget(FALSE);
	GClosestClient = BestTarget;
	if (BestTarget == -1)
		return;

	// Update our input state
	if (FAILED(XInputGetState(0, &State)))
		return;

	// Get the best targets posistion
	GetTagPosistion(&Entities_BO1[BestTarget], BestTarget, GetMenuOptionString(MENU_LEGITBOT, OPTION_AIMBONE), (float*)&EnemyPosistion);

	// Get our view origin
	CG_GetPlayerViewOrigin_BO1(0, PlayerPTR, MyOrigin);

	// Calc our aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles); 
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_BO1->SpawnAngles, FinalAngles);

	// Get our smooth angles
	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_SMOOTHSCALE) != "Off")
		LegitAngles = SmoothAimbotAngles(FinalAngles, FindSmoothScale());
	else
		LegitAngles = FinalAngles;

	// Apply our smooth angles if we are aimed in
	DWORD ReactionDelay = FindReactionDelayAmount();
	PCHAR AimKeyVal = GetMenuOptionString(MENU_LEGITBOT, OPTION_AIMKEY);
	if (GetMenuOptionString(MENU_LEGITBOT, OPTION_REACTIONDELAY) != "Off")
	{
		// If we want a reaction delay
		if ((GReactionDelay > ReactionDelay) /*&& cg_BO1->ZoomState >= 0.5f*/)
		{
			// Set our angles
			if (AimKeyVal == "None" ||
				AimKeyVal == "LT" && (State.Gamepad.bLeftTrigger > 35) ||
				AimKeyVal == "RT" && (usercommand->Buttons & 0x80000000) ||
				AimKeyVal == "LT & RT" && (usercommand->Buttons & 0x80000000 && State.Gamepad.bLeftTrigger > 35) ||
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
				LocalClient_BO1->ViewAngles[0] = LegitAngles.x;
				LocalClient_BO1->ViewAngles[1] = LegitAngles.y;
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
			AimKeyVal == "RT" && (usercommand->Buttons & 0x80000000) ||
			AimKeyVal == "LT & RT" && (usercommand->Buttons & 0x80000000 && State.Gamepad.bLeftTrigger > 35) ||
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
			LocalClient_BO1->ViewAngles[0] = LegitAngles.x;
			LocalClient_BO1->ViewAngles[1] = LegitAngles.y;
		}
	}

	// Autoshoot
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOSHOOT) || ((GetMenuOptionValue(MENU_LEGITBOT, OPTION_TRIGGERBOT) && TriggerBot(BestTarget))))
	{
		// Wait until aimed in before shooting
		//if (cg_BO1->ZoomState >= 0.5f)
		//{
			nextusercommand->Buttons |= 0x80000000;
			if (usercommand->Buttons & 0x80000000)
			{
				usercommand->Buttons &= 0x7FFFFFFFu;
			}

			if (IsWeaponDualWield())
				usercommand->Buttons |= 0x00100280;
		//}
	}
}

VOID Spinbot(usercmd_s_BO1 *UserCommand)
{
	int RandomAdd = rand() % 71;
	int RandomUpDownSpeed = rand() % 71;
	int RandomLeft = rand() % 360;
	int RandomRight = rand() % -360;
	int PlayerPTR = ((DWORD)cg_BO1 + 0x3EF9C);

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
	//if ((*(BYTE*)((DWORD)cg_BO1 + 0x18F) & 0x8) || /*(cg_BO1->Health <= 0) ||*/ UserCommand->weaponID == 0 || UserCommand->Buttons & 0x00010000 || UserCommand->Buttons & 0x00020000)
	//	return;

	Vector3 MyPosAA;
	DWORD BestAAClient = FindBestAntiAimTarget();
	Vector3 MyPos; //= *(Vector3*)GetViewOriginPointer();
	Vector3 EnemyPos, Angles, VectorAngles, FinalAngles;

	CG_GetPlayerViewOrigin_BO1(0, PlayerPTR, MyPos);
	GetTagPosistion(&Entities_BO1[BestAAClient], BestAAClient, "j_mainroot", (float*)&EnemyPos);

	// Calculate aimbot angles
	VectorSubtract(EnemyPos, MyPos, Angles);
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_BO1->SpawnAngles, FinalAngles);

	int AAAngleX = UserCommand->ViewAngles[0];
	int AAAngleY = UserCommand->ViewAngles[1];

	// Anti-Aim X Look down
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Look Down")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(ffAngleNormalize(60.0f + ShortToAngle(UserCommand->angles[0]) - RefDefAngles.x)); //ANGLE2SHORT(72.0f);
		UserCommand->ViewAngles[0] = ANGLE2SHORT(62.99 - LocalClient_BO1->SpawnAngles.x);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Up & Down")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		int RandomShouldFlip = rand() % 12;
		if (RandomShouldFlip > 5)
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_BO1->SpawnAngles.x);
			SpinbotFlipX = TRUE;
		}
		else
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(-45 - LocalClient_BO1->SpawnAngles.x);
			SpinbotFlipX = FALSE;
		}
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Fake Up")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);

		if (flip)
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(-69.99 - LocalClient_BO1->SpawnAngles.x);
		}
		else
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_BO1->SpawnAngles.x);
		}
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Detection")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);

		UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_BO1->SpawnAngles.x);

		// If we took damage or they are wallbangable / visible
		if ((*(int*)((DWORD)cg_BO1 + 0x254) < OurHealth) || PlayerInfo[BestAAClient].Wallbangable)
		{
			// Move our body move to try to avoid bullets
			//UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - (SpinbotFlip ? 80.0f : -90.0f));
			UserCommand->ViewAngles[0] = ANGLE2SHORT(rand() % 65 - LocalClient_BO1->SpawnAngles.x);
			SpinbotFlip = !SpinbotFlip;
		}

		// Update our health
		OurHealth = *(int*)((DWORD)cg_BO1 + 0x254);

	}

	// Anti-Aim Y Flip Flop
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Flip Flop")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (SpinbotFlip)
		{
			UserCommand->ViewAngles[1] = ANGLE2SHORT(InverseNorm(FinalAngles.y - 180.0f + RandomLeft));
			SpinbotFlip = FALSE;
		}
		else
		{
			UserCommand->ViewAngles[1] = ANGLE2SHORT(InverseNorm(FinalAngles.y - 180.0f - RandomRight));
			SpinbotFlip = TRUE;
		}

	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Face Away")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (SpinbotFlip)
		{
			UserCommand->ViewAngles[1] = ANGLE2SHORT(EnemyPos.y - 180.0f + rand() % 30);
			SpinbotFlip = FALSE;
		}
		else
		{
			UserCommand->ViewAngles[1] = ANGLE2SHORT(EnemyPos.y - 180.0f - rand() % 30);
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

		UserCommand->ViewAngles[1] = AAAngleY;
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Jitter Synced")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (LocalClient_BO1->CommandNumber % 3)
			UserCommand->ViewAngles[1] = UserCommand->ViewAngles[1] + ANGLE2SHORT(200.0);
		else
		{
			if (flip)
			{
				if (flip2)
					UserCommand->ViewAngles[1] += ANGLE2SHORT(90.0f);

				else
					UserCommand->ViewAngles[1] -= ANGLE2SHORT(90.0f);
			}
			else
			{
				UserCommand->ViewAngles[1] += ANGLE2SHORT(180.0f);
			}
		}
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Sideways")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		UserCommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y - 100.0f);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Detection")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		// If we took damage or they are wallbangable / visible
		if ((*(int*)((DWORD)cg_BO1 + 0x254) < OurHealth) || PlayerInfo[BestAAClient].Wallbangable)
		{
			// Move our body move to try to avoid bullets
			//UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - (SpinbotFlip ? 80.0f : -90.0f));
			UserCommand->ViewAngles[1] += ANGLE2SHORT(rand() % 360);
			SpinbotFlip = !SpinbotFlip;
		}
		else
		{
			if (SpinbotFlip)
			{
				UserCommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f + RandomLeft);
				SpinbotFlip = FALSE;
			}
			else
			{
				UserCommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f - RandomLeft);
				SpinbotFlip = TRUE;
			}
		}

		// Update our health
		OurHealth = *(int*)((DWORD)cg_BO1 + 0x254);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Angel Backwards")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		//int originalAngle = UserCommand->ViewAngles[1];
		UserCommand->ViewAngles[1] = (FinalAngles.y - 180.0f) + ANGLE2SHORT(36000180.0);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Static")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);

		if (flip)
		{
			if (flip2)
				UserCommand->ViewAngles[1] += ANGLE2SHORT(90.0f);

			else
				UserCommand->ViewAngles[1] -= ANGLE2SHORT(90.0f);
		}
		else
		{
			UserCommand->ViewAngles[1] += ANGLE2SHORT(180.0f);
		}
	}


	// X Spin Direction
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION) == "Look Down")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(ffAngleNormalize(60.0f + ShortToAngle(UserCommand->angles[0]) - RefDefAngles.x));
		UserCommand->ViewAngles[0] = ANGLE2SHORT(62.99 - LocalClient_BO1->SpawnAngles.x);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION) == "Look Up")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(ffAngleNormalize(-60.0f + ShortToAngle(UserCommand->angles[0]) - RefDefAngles.x));
		UserCommand->ViewAngles[0] = ANGLE2SHORT(-62.99 - LocalClient_BO1->SpawnAngles.x);
	}

	// Y Spin Direction
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION) == "Left")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);


		UserCommand->ViewAngles[1] = ANGLE2SHORT(SpinbotSpinFlip);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION) == "Right")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);


		UserCommand->ViewAngles[1] = ANGLE2SHORT(SpinbotSpin);
	}
}

int CL_SendCmdHook(int r3)
{
	LocalClient_BO1 = (pclientActive_t_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu11
	Entities_BO1 = (pcentity_s_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu11
	cg_BO1 = (pcg_s_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu11
	cgs_BO1 = (pcgs_t_BO1)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu11
	XINPUT_STATE State;

	// Update our input state
	if (!menuReady)
		goto Return;

	if (UserInGame() && (GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT) || GetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT)))
	{
		// Do our server time 
		pusercmd_s_BO1 OldUserCommand = LocalClient_BO1->GetUserCommand(LocalClient_BO1->GetUserCommandNumber() - 1);
		pusercmd_s_BO1 CurrentUserCommand = LocalClient_BO1->GetUserCommand(LocalClient_BO1->GetUserCommandNumber());
		pusercmd_s_BO1 NextUserCommand = LocalClient_BO1->GetUserCommand(LocalClient_BO1->GetUserCommandNumber() + 1);
		*NextUserCommand = *CurrentUserCommand;
		CurrentUserCommand->serverTime -= 1;
		LocalClient_BO1->CommandNumber += 1;

		// Save our angle for movement fix
		float MovementSave = SHORT2ANGLE(CurrentUserCommand->ViewAngles[1]);

		// Do our spinbot
		if (GetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT))
			Spinbot(NextUserCommand);

		// Reset our player info structure when we enter a game
		if (!DroppedAllTargets)
		{
			for (int Target = 0; Target < cgs_BO1->MaxClients; Target++)
			{
				DropTarget(PlayerInfo[Target]);
			}

			DroppedAllTargets = TRUE;
		}

		// If Ragebot aimbot mode
		if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT))
			Ragebot(CurrentUserCommand, OldUserCommand, NextUserCommand, MovementSave);

		// If Legitbot aimbot mode
		if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT))
			Legitbot(CurrentUserCommand, NextUserCommand);

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
					NextUserCommand->Buttons |= 0x80000000;
			}

			RapidFireCtr++;
		}

		// Handle fake lag
		if (GetMenuOptionString(MENU_MAIN, OPTION_FAKELAGAMOUNT) != "Off")
		{
			FakeLagCount++;
			if (FakeLagCount >= PacketSkipAmount || (CurrentUserCommand->Buttons != 0) || (OldUserCommand->Buttons != 0) || (NextUserCommand->Buttons != 0))
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

void PlayLocalSoundAliasByNameHook(char *aliasname, int a0, float soundLevel, int r6, int r7, int r8, int r9)
{
	// We can use this to disable or re-direct any local sounds

	if (strcmp(aliasname, "mus_mp_frontend"))
		 DbgPrint("Sound Name: %s", aliasname);

	PlayLocalSoundAliasByNameDetour->CallOriginal(aliasname, a0, soundLevel, r6, r7, r8, r9);
}

void EngineDrawRadarHook(DWORD localClient, DWORD mapType, DWORD size, DWORD rectDef, DWORD shader, const float *color, DWORD unk1, DWORD unk2, DWORD unk3, float f1, float f2, float f3, float f4, float f5, float f6)
{
	if (UserInGame() && mapType == 0 && GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR))
	{
		LastCompassType = mapType;
		CRectDef *rd = (CRectDef*)(rectDef);
		memcpy(&lastCompass, (PVOID)rectDef, sizeof(CRectDef));
		DrawCompassTimer = GetTickCount() + 30;
	}
	EngineDrawRadarDetour->CallOriginal(localClient, mapType, size, rectDef, shader, color, unk1, unk2, unk3, f1, f2, f3, f4, f5, f6);
}

void RB_SetTessTechniqueHook(int r3, int r4)
{
	// Hook this to prevent crashes from people setting invalid text
	if (r3 < 0x82000000)
		return;

	return RB_SetTessTechniqueDetour->CallOriginal(r3, r4);
} 

void SetupBO1Offsets()
{
	va_BO1 = (CONST PCHAR(__cdecl *)(CONST PCHAR, ...))getCryptedCoD(CheatPatches->CoDPatches.VA);//tu11
	Dvar_GetBool_BO1 = (BOOL(__cdecl *)(CONST PCHAR))getCryptedCoD(CheatPatches->CoDPatches.Dvar_GetBool);//tu11
	CG_LocationalTraceBO1 = (void(_cdecl*)(void *, Vector3 *, Vector3*, int, int, bool, DWORD))getCryptedCoD(CheatPatches->CoDPatches.CG_LocationalTrace); //tu11
	DB_FindXAssetHeader_BO1 = (DWORD(__cdecl *)(XAssetType_BO1, CONST PCHAR, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DB_FindXAssetHeader);//tu11
	R_TextWidth_BO1 = (DWORD(__cdecl *)(CONST PCHAR, DWORD, DWORD))getCryptedCoD(CheatPatches->CoDPatches.R_TextWidth);//tu11
	CL_DrawTextPhysical_BO1 = (VOID(__cdecl*)(CONST PCHAR, DWORD, DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.CL_DrawTextPhysical);//tu11 might be wrong
	R_AddCmdDrawStretchPic_BO1 = (VOID(__cdecl*)(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.R_AddCmdDrawStretchPic);//tu11
	CG_DrawRotatedPicPhysical_BO1 = (VOID(__cdecl*)(DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical2);//tu11
	BG_GetWeaponDef_BO1_2 = (PDWORD(__cdecl *)(DWORD))getCryptedCoD(CheatPatches->CoDPatches.BG_GetWeaponDef2);//tu11
	BG_GetWeaponDef_BO1 = (DWORD(__cdecl *)(DWORD))getCryptedCoD(CheatPatches->CoDPatches.BG_GetWeaponDef);//tu11
	Cbuf_AddText_BO1 = (void(__cdecl*)(DWORD, char*))getCryptedCoD(CheatPatches->CoDPatches.Cbuf_AddText);//tu11
	CL_AddReliableCommand = (void(__cdecl*)(DWORD, char*))getCryptedCoD(CheatPatches->CoDPatches.CL_AddReliableCommand);//tu11
	SL_GetString_BO1 = (short(__cdecl*)(PCHAR, DWORD, DWORD))getCryptedCoD(CheatPatches->CoDPatches.SL_GetString);//tu11
	Com_GetClientDObj_BO1 = (DWORD(__cdecl *)(DWORD, DWORD))getCryptedCoD(CheatPatches->CoDPatches.Com_GetClientDObj_BO1);//tu11
	CG_GetWeaponIndexForName_BO1 = (DWORD(__cdecl *)(CONST PCHAR))getCryptedCoD(CheatPatches->CoDPatches.CG_GetWeaponIndexForName);//tu11
	PlayLocalSoundAliasByName = (void(__cdecl*)(char *, int, float, int, int, int, int))getCryptedCoD(CheatPatches->CoDPatches.PlayLocalSoundAliasByName);//tu11
	R_NormalizedTextScale_BO1 = (int(__cdecl*)(int, float))getCryptedCoD(CheatPatches->CoDPatches.R_NormalizedTextScale);//tu11
	CG_CalcNamePosition = (bool(__cdecl*)(int, Vector3*, float *, float *))getCryptedCoD(CheatPatches->CoDPatches.CG_CalcNamePos);//tu11
	CL_SetStance = (VOID(__cdecl*)(int localClientNum, int stance))getCryptedCoD(CheatPatches->CoDPatches.CL_SetStace);
	G_GetWeaponHitLocationMultiplier_BO1 = (FLOAT(__cdecl*)(DWORD, DWORD))getCryptedCoD(CheatPatches->CoDPatches.G_GetWeaponHitLocationMultiplier);//TU11

	// No Spread Functions
	CG_GetPlayerViewOrigin_BO1 = (VOID(__cdecl *)(DWORD, DWORD, Vector3&))getCryptedCoD(CheatPatches->CoDPatches.CG_GetPlayerViewOrigin);//tu11
	R_GetSpread_BO1 = (VOID(__cdecl *)(DWORD, DWORD, float*, float*))getCryptedCoD(CheatPatches->CoDPatches.R_GetSpread);//tu11
	TransformSeed_BO1 = (DWORD(__cdecl*)(int*))getCryptedCoD(CheatPatches->CoDPatches.TransformSeed);
	RandomBulletDir_BO1 = (FLOAT(__cdecl *)(int*))getCryptedCoD(CheatPatches->CoDPatches.RandomBulletDir);//TU11

	// Radar Functions
	DrawEngineRadar_BO1 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DrawEngineRadarFunc);//tu18
	DrawEnemies_BO1 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DrawEnemiesFunc);//tu18
	DrawFriendlies_BO1 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DrawFriendliesFunc);//tu18
	DrawVehicles_BO1 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DrawVehiclesFunc);//tu18
	DrawPlayer_BO1 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DrawPlayerFunc);//tu18
	CG_WorldPosToCompass_BO1 = (BOOL(__cdecl*)(DWORD, pcg_s_BO1, float, float, Vector2&, Vector3&, Vector3&, Vector2&, Vector2&))getCryptedCoD(CheatPatches->CoDPatches.CG_WorldPosToCompass);//TU18
	CG_CompassCalcDimensions_BO1 = (BOOL(__cdecl*)(DWORD, pcg_s_BO1, Vector2&, unkRadarStruct*, float*, float*, float*, float*))getCryptedCoD(CheatPatches->CoDPatches.CG_CompassCalcDimensions);//tu18
	CG_DrawRotatedPic_BO1 = (VOID(__cdecl*)(DWORD, float, float, float, float, int, int, float, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPic); //tu18
	CG_CompassUpYawVector_BO1 = (VOID(__cdecl*)(pcg_s_BO1, Vector2&))getCryptedCoD(CheatPatches->CoDPatches.CG_CompassUpYawVector);//tu18
	AngleNormalize360_BO1 = (float(__cdecl*)(float))getCryptedCoD(CheatPatches->CoDPatches.AngleNormalize360);//tu18

	// AutoWall Functions
	FireBulletPenetrate_BO1 = (VOID(__cdecl*)(DWORD, BulletFireParams*, DWORD, pcentity_s_BO1, Vector3&, DWORD, bool, BulletTraceResults*))getCryptedCoD(CheatPatches->CoDPatches.FireBulletPenetrate);//TU11
}


void InitializeBO1Hooks()
{
	SetupBO1Offsets();

	Menu_PaintAllDetour = new Detour<void>;
	XInputGetStateDetour = new Detour<DWORD>;
	CL_SendCmdDetour = new Detour<int>;
	CL_CreateNewCommandsDetour = new Detour<void>;
	PlayLocalSoundAliasByNameDetour = new Detour<void>;
	R_EndFrameDetour = new Detour<void>;
	//NetDll_XNetGetTitleXnAddrDetour = new Detour<DWORD>;
	CG_EntityEventDetour = new Detour<void>;
	R_DrawXModelSkinnedCachedDetour = new Detour<void>;
	DrawIndexedPrimitiveDetour = new Detour<HRESULT>;
	R_DrawIndexedPrimitiveDetour = new Detour<HRESULT>;
	EngineDrawRadarDetour = new Detour<void>;
	RB_SetTessTechniqueDetour = new Detour<void>;

	// Load our menu options from saved ini file
	LoadMenuOptions();

	Menu_PaintAllDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.Menu_PaintAll), (PVOID)Menu_PaintAllHook);
	XInputGetStateDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.XInputGetState), (PVOID)XInputGetStateHook);
	CL_SendCmdDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CL_SendCmd), (PVOID)CL_SendCmdHook); // 0x8223B3C8 0x8223B8C8

	//PlayLocalSoundAliasByNameDetour->SetupDetour(0x824B19D0, (PVOID)PlayLocalSoundAliasByNameHook);
	R_EndFrameDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.R_EndFrame), (PVOID)R_EndFrameHook);
	//NetDll_XNetGetTitleXnAddrDetour->SetupDetour(0x823B6994, (PVOID)NetDll_XNetGetTitleXnAddrHook);
	CG_EntityEventDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CG_EntityEvent), (PVOID)CG_EntityEventHook);
	RB_SetTessTechniqueDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.RB_SetTessTechnique), (PVOID)RB_SetTessTechniqueHook);
	//UI_DrawTextDetour->SetupDetour(0x8238DEE8, (PVOID)UI_DrawTextHook);
	EngineDrawRadarDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CG_CompassDrawPlayerMap), (PVOID)EngineDrawRadarHook);
	//RB_SetTessTechniqueDetour->SetupDetour(0x82373AF8, (PVOID)RB_SetTessTechniqueHook);
	//R_DrawXModelSkinnedCachedDetour->SetupDetour(0x823982C0, (PVOID)R_DrawXModelSkinnedCachedHook);
	//DrawIndexedPrimitiveDetour->SetupDetour(0x8242E2E0, (PVOID)DrawIndexedPrimitive); // tu23
	//R_DrawIndexedPrimitiveDetour->SetupDetour(0x820AB1C0, (PVOID)R_DrawIndexedPrimitiveHook); // tu23

	//HANDLE hThread; DWORD hThreadID;
	//ExCreateThread(&hThread, 0, &hThreadID, (PVOID)XapiThreadStartup, (LPTHREAD_START_ROUTINE)TestThread, NULL, 0x2);
	//XSetThreadProcessor(hThread, 4); ResumeThread(hThread); CloseHandle(hThread);

	//void(*Sys_CreateThread)(void(*function)(unsigned int), unsigned int threadContext) = (void(*)(void(*)(unsigned int), unsigned int))0x82358B10;
	//Sys_CreateThread(TestThread, 3);
	//((VOID(*)(...))0x82358B10)(TestThread, 1);

	// FOF patch
	*(QWORD*)0x821DA3C4 = 0x3EA0820162B5D99C;
}

void UninitializeBO1Cheater()
{
	Menu_PaintAllDetour->TakeDownDetour();
	XInputGetStateDetour->TakeDownDetour();
	CL_SendCmdDetour->TakeDownDetour();
	CL_CreateNewCommandsDetour->TakeDownDetour();
	PlayLocalSoundAliasByNameDetour->TakeDownDetour();
	R_EndFrameDetour->TakeDownDetour();
	//NetDll_XNetGetTitleXnAddrDetour->TakeDownDetour();
	CG_EntityEventDetour->TakeDownDetour();
	R_DrawXModelSkinnedCachedDetour->TakeDownDetour();
	DrawIndexedPrimitiveDetour->TakeDownDetour();
	R_DrawIndexedPrimitiveDetour->TakeDownDetour();
	EngineDrawRadarDetour->TakeDownDetour();
	RB_SetTessTechniqueDetour->TakeDownDetour();



	Sleep(1000);

	delete Menu_PaintAllDetour;
	delete XInputGetStateDetour;
	delete CL_SendCmdDetour;
	delete CL_CreateNewCommandsDetour;
	delete PlayLocalSoundAliasByNameDetour;
	delete R_EndFrameDetour;
	//delete NetDll_XNetGetTitleXnAddrDetour;
	delete CG_EntityEventDetour;
	delete R_DrawXModelSkinnedCachedDetour;
	delete DrawIndexedPrimitiveDetour;
	delete R_DrawIndexedPrimitiveDetour;
	delete EngineDrawRadarDetour;
	delete RB_SetTessTechniqueDetour;
}