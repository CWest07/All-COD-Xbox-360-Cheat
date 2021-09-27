#include "stdafx.h"
#include "Utilities.h"
#include "Config.h"
#include <time.h>
#include "COD4.h"
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

pclientActive_t_COD4 LocalClient_COD4;
pcentity_s_COD4 Entities_COD4;
pcgs_t_COD4 cgs_COD4;
pcg_s_COD4 cg_COD4;
ClientInformation_COD4 PlayerInfo[18];

//__declspec(dllexport) BYTE freememory[0x1000];
Detour<void> *Menu_PaintAllDetour;
Detour<DWORD> *XInputGetStateDetour;
Detour<int> *CL_ReadyToSendPacketDetour;
Detour<int> *PlayLocalSoundAliasByNameDetour;
Detour<void> *R_EndFrameDetour;
Detour<DWORD> *NetDll_XNetGetTitleXnAddrDetour;
Detour<void> *CG_EntityEventDetour;
Detour<void> *R_DrawXModelSkinnedCachedDetour;
Detour<HRESULT> *R_DrawIndexedPrimitiveDetour;
Detour<HRESULT> *DrawIndexedPrimitiveDetour;
Detour<void> *EngineDrawRadarDetour;
Detour<void> *UI_DrawTextDetour;
Detour<void> *RB_SetTessTechniqueDetour;
Detour<void> *LiveStorage_SetStatDetour;
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
int TeleportKillDelay = 0;
int ClosestClient;
int FakeLagCount = 0;
int PacketSkipAmount = 26;
int HudSmallFont;
int BigDevFont;
int SmallFont;
int BodyFontBold;
int ExtraSmallFont;
int ExtraBigDevFont;
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
	return (getCryptedCoD(CheatPatches->CoDPatches.NetInfoAddress) + (index * 0xB8));
}

PCHAR GetNameFromNetInfo(int index)
{
	return (char*)(GetNetInfoAddress(index) + 0x08);
}

BYTE* GetIPFromNetInfo(int index)
{
	return (BYTE*)(GetNetInfoAddress(index) + 0x58); 
}

QWORD GetXUIDFromNetInfo(int index)
{
	return (GetNetInfoAddress(index));
}

QWORD GetMachineIDFromNetInfo(int index)
{
	return _byteswap_uint64(*(QWORD*)(GetNetInfoAddress(index) + 0x6C)); 
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
	MenuOptionNames.push_back(E_ALLOC("Teleport Kill"));
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
	MenuOptionNames.push_back(E_ALLOC("Level 55?"));
	MenuOptionNames.push_back(E_ALLOC("Anti Derank"));
	MenuOptionNames.push_back(E_ALLOC("Reset Stats?"));
	MenuOptionNames.push_back(E_ALLOC("Invert Team Check"));
	MenuOptionNames.push_back(E_ALLOC("Log IP's"));
	MenuOptionNames.push_back(E_ALLOC("Freeze All Consoles?"));
}

void SetupMenu()
{
	SetupMenuText();

	AddMenuOption(MENU_MAIN, OPTION_RADAR, 0, bRadar);
	AddMenuOption(MENU_MAIN, OPTION_NORECOIL, 1, bNoRecoil);
	AddMenuOption(MENU_MAIN, OPTION_NOSWAY, 2, bNoSway);
	//AddMenuOption(MENU_MAIN, OPTION_CHANGETEAM, 3);
	//AddMenuOption(MENU_MAIN, OPTION_CHANGECLASS, 4);
	AddMenuOption(MENU_MAIN, OPTION_STEALGTS, 3, bStealGTs);
	AddMenuOption(MENU_MAIN, OPTION_RAINBOWGT, 4, bRainbowGT);
	AddMenuOption(MENU_MAIN, OPTION_SPOOFGT, 5);
	AddMenuOption(MENU_MAIN, OPTION_SHOWHOST, 6, bShowHost);
	AddMenuOption(MENU_MAIN, OPTION_SHOWPING, 7, bShowHost);
	AddMenuOption(MENU_MAIN, OPTION_FASTRELOAD, 8, bFastReload);
	AddMenuOption(MENU_MAIN, OPTION_EVERYONEHEARSME, 9, bEveryoneHearsMe);
	AddMenuOption(MENU_MAIN, OPTION_NEVERHOST, 10, bNeverHost);
	AddMenuOption(MENU_MAIN, OPTION_FOV, 11, 0, "Normal", FOVS, FOVS_Size, iFieldOfViewIndex);
	AddMenuOption(MENU_MAIN, OPTION_THIRDPERSON, 12);
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
	AddMenuOption(MENU_RAGEBOT, OPTION_TELEPORTKILL, 10, bTeleportKill);
	AddMenuOption(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS, 11, bRagebotPreferred);



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
	//AddMenuOption(MENU_LEGITBOT, "Target Shields", 10);
	AddMenuOption(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS, 11, bLegitbotPreferred);


	AddMenuOption(MENU_SPINBOT, OPTION_ENABLESPINBOT, 0, bSpinbot);
	AddMenuOption(MENU_SPINBOT, OPTION_XSPINDIRECTION, 1, 0, "Off", XSpinDirects, XSpinDirects_Size, iXSpinDirection);
	AddMenuOption(MENU_SPINBOT, OPTION_YSPINDIRECTION, 2, 0, "Off", YSpinDirects, YSpinDirects_Size, iYSpinDirection);
	AddMenuOption(MENU_SPINBOT, OPTION_YSPINSPEED, 3, 0, "10.0", YSpinSpeeds, YSpinSpeeds_Size, iYSpinSpeed);
	//AddMenuOption(MENU_SPINBOT, OPTION_ZSPINDIRECTION, 4, 0, "Off", ZSpinDirects, ZSpinDirects_Size, ZSpinDirects_Start);
	//AddMenuOption(MENU_SPINBOT, OPTION_ZSPINSPEED, 5, 0, "10.0", ZSpinSpeeds, ZSpinSpeeds_Size, ZSpinSpeeds_Start);
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
	AddMenuOption(MENU_SETTINGS, OPTION_DISABLEMENUMUSIC, 3, bDisableMainMenuMusic);
	AddMenuOption(MENU_SETTINGS, OPTION_PRESTIGEDISCO, 4, bPrestigeDisco);
	AddMenuOption(MENU_SETTINGS, OPTION_PRESTIGE, 5, 0, "N/A", Prestiges, Prestiges_Size, Prestiges_Start);
	AddMenuOption(MENU_SETTINGS, OPTION_LEVEL55, 6);
	AddMenuOption(MENU_SETTINGS, OPTION_ANTIDERANK, 7, bAntiDerank);
	AddMenuOption(MENU_SETTINGS, OPTION_RESETCURRENTSTATS, 8);
	//AddMenuOption(MENU_SETTINGS, "Unlock All?", 10);
	//AddMenuOption(MENU_SETTINGS, "Set Legit Stats?", 11);
	AddMenuOption(MENU_SETTINGS, OPTION_INVERTTEAMCHECK, 9);
	AddMenuOption(MENU_SETTINGS, OPTION_LOGIPS, 10, bLogIPs);
	AddMenuOption(MENU_SETTINGS, OPTION_FREEZEALLCONSOLES, 11);

	HudSmallFont = DB_FindXAssetHeader_COD4(font_COD4, "fonts/hudsmallfont", 1);
	BigDevFont = DB_FindXAssetHeader_COD4(font_COD4, "fonts/bigDevFont", 1);
	SmallFont = DB_FindXAssetHeader_COD4(font_COD4, "fonts/smallfont", 1);
	BodyFontBold = DB_FindXAssetHeader_COD4(font_COD4, "fonts/bodyFontBold", 1);
	ExtraBigDevFont = DB_FindXAssetHeader_COD4(font_COD4, "fonts/720/extraBigFont", 1);
	ExtraSmallFont = DB_FindXAssetHeader_COD4(font_COD4, "fonts/720/extraSmallFont", 1);
	WhiteMaterial = DB_FindXAssetHeader_COD4(material_COD4, "white", 1);
	ArrowIcon = DB_FindXAssetHeader_COD4(material_COD4, "compassping_player", 1);
	//ScavMaterial = DB_FindXAssetHeader_COD4(material_COD4, "hud_scavenger_pickup", 1);
	//CompassPingMaterial = DB_FindXAssetHeader_COD4(material_COD4, "compassping_player", 1);

	memcpy(OriginalGT, (PVOID)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1E);
	memcpy(OriginalXUID, (PVOID)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30), 0xE);

	menuReady = TRUE;
}

bool UserInGame()
{
	// Ingame & !AllClientsDisconnected()
	return Dvar_GetBool_COD4("cl_ingame") && !((bool(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CL_AllLocalClientsDisconnected))();
}

void PlayMenuSound(char* SoundName)
{
	// If we want menu sounds
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUSOUNDS))
		return;

	// Call the games function to play the sound
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

		PlayMenuSound("mouse_over");
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

		PlayMenuSound("mouse_over");
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
		PlayMenuSound("mouse_over");
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

		PlayMenuSound("mouse_click");

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

		PlayMenuSound("mouse_click");

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

void DrawBox_COD4(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color)
{
	R_AddCmdDrawStretchPic_COD4(X, Y, W, H, 4, 4, 1, 1, Color, WhiteMaterial);
}

void DrawGradient_COD4(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color1, Vector4& Color2, bool horizontal)
{
	float fAdd = (horizontal ? 0 : W / 2 - H / 2);
	R_AddCmdDrawStretchPic_COD4(X, Y, W, H, 4, 4, 1, 1, Color2, WhiteMaterial);
	R_AddCmdDrawStretchPic_COD4(X + fAdd, Y - fAdd, (horizontal ? W : H), (horizontal ? H : W), 4, 4, 1, 1, Color1, GradientFadeIn);
}

void DrawText_COD4(CONST PCHAR Text, FLOAT X, FLOAT Y, DWORD index, FLOAT addy, DWORD Font, Vector4& Color, float fontXSize = 1.0, float fontYSize = 1.0)
{
	CL_DrawTextPhysical_COD4(Text, 0x7FFFFFFF, Font, X, Y + (index * addy), fontXSize, fontYSize, 0, Color, 0);
}

bool WorldPosToScreenPos(float* In, Vector2* Out)
{
	Vector3 Position, Transform;

	VectorSubtract(In, cg_COD4->refdef.ViewOrg, Position);

	Transform.x = DotProduct(Position, cg_COD4->refdef.ViewAxis[1]);
	Transform.y = DotProduct(Position, cg_COD4->refdef.ViewAxis[2]);
	Transform.z = DotProduct(Position, cg_COD4->refdef.ViewAxis[0]);

	if (Transform.z < 0.1f)
		return false;

	Vector2 Center; Vector2 OutPut;
	Center.x = cg_COD4->refdef.Width * 0.5f; Center.y = cg_COD4->refdef.Height * 0.5f;
	OutPut.x = Center.x * (1 - (Transform.x / cg_COD4->refdef.Fov.x / Transform.z)); OutPut.y = Center.y * (1 - (Transform.y / cg_COD4->refdef.Fov.y / Transform.z));
	*Out = OutPut;
	return true;
}

void DrawLine_COD4(FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, FLOAT Width, Vector4& Color)
{
	FLOAT X, Y, Angle, L1, L2, H1;
	H1 = Y2 - Y1;
	L1 = X2 - X1;
	L2 = sqrt(L1 * L1 + H1 * H1);
	X = X1 + ((L1 - L2) / 2);
	Y = Y1 + (H1 / 2);
	Angle = (FLOAT)atan(H1 / L1) * (180 / 3.14159265358979323846);
	CG_DrawRotatedPicPhysical_COD4(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), X, Y, L2, Width, Angle, Color, WhiteMaterial);
}

void MakeLines(Vector3 Origin, FLOAT X1, FLOAT Y1, FLOAT Z1, FLOAT X2, FLOAT Y2, FLOAT Z2, Vector4& Color, int Size = 1)
{
	Vector2 Screen1, Screen2;
	Vector3 Origin1(Origin.x + X1, Origin.y + Y1, (Origin.z + Z1) - 4);
	Vector3 Origin2(Origin.x + X2, Origin.y + Y2, (Origin.z + Z2) - 4);
	if (WorldPosToScreenPos((float*)&Origin1, &Screen1) && WorldPosToScreenPos((float*)&Origin2, &Screen2)) {
		DrawLine_COD4(Screen1.x, Screen1.y, Screen2.x, Screen2.y, Size, Color);
	}
}

void DrawCheckBox(FLOAT X, FLOAT Y, BOOL Checked = FALSE)
{
	Y -= 4.0f;

	Vector4 Color;
	if (Checked)
		Color = Vector4(0, 1, 0, 1);
	else
		Color = Vector4(1, 0, 0, 1);
	DrawLine_COD4(X - 7.5, Y - 7.5, X + 7.5, Y - 7.5, 2, Color);//Top
	DrawLine_COD4(X - 7.5, Y - 7.5, X - 7.5, Y + 7.5, 2, Color);//Left Side
	DrawLine_COD4(X + 7.5, Y - 7.5, X + 7.5, Y + 7.5, 2, Color);//Right Side
	DrawLine_COD4(X - 7.5, Y + 7.5, X + 7.5, Y + 7.5, 2, Color);//Bottom

	if (Checked)
	{
		DrawLine_COD4(X - 7.5, Y - 7.5, X + 7.5, Y + 7.5, 1.5, Color);//Check
		DrawLine_COD4(X - 7.5, Y + 7.5, X + 7.5, Y - 7.5, 1.5, Color);
	}
}

void DrawMenu_COD4()
{
	FLOAT X, Y;
	X = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY));//cgs_COD4->Width; 0x82431240
	Y = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY) + 0x4);//cgs_COD4->Height; 0x82431244
	Vector4 ActiveTextColor = GetColorCode(colorVioletFlower);
	Vector4 InActiveTextColor = Vector4(1, 1, 1, 0.9);

	int MenuFont = SmallFont; // HudSmallFont;

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
	DrawBox_COD4(X * 0.331, Y * 0.25, menuWidth, menuHeight, Vector4(0, 0, 0, 0.420));
	DrawLine_COD4(X * 0.331, Y * 0.25, (X * 0.331) + menuWidth, Y * 0.25, 3.5, Vector4(1, 0, 0, 1));
	DrawLine_COD4(X * 0.331, Y * 0.25 + menuHeight, (X * 0.331) + menuWidth, (Y * 0.25) + menuHeight, 3.5, Vector4(1, 0, 0, 1));
	DrawLine_COD4((X * 0.331) + 115, (Y * 0.25), (X * 0.331) + 115, (Y * 0.25) + menuHeight, 3.5, Vector4(1, 0, 0, 1));

	FLOAT textScale = 0.7f;

	// Wait for the animation to be done
	if (menuWidth == 420 && menuHeight == 400)
	{
		//Draw Menu Names
		for (int i = 0; i < MenuOptions; i++)
		{
			DrawText_COD4(MenuNames[i], X * 0.3328, Y * 0.2861 - 4.0f, i, 22, MenuFont, (scroll == i) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
			DrawLine_COD4(X * 0.331, (Y * 0.28) + (i * 22), (X * 0.331) + 115, (Y * 0.28) + (22 * i), 3.5, Vector4(1, 0, 0, 1));
		}

		//Draw SubOptions
		for (int i = 0; i < SubMenuOptions[scroll]; i++)
		{
			if (subMenu == FALSE)
			{
				DrawText_COD4(SubMenuNames[i][scroll], X * 0.425, Y * 0.2861, i, 22, MenuFont, InActiveTextColor, textScale, textScale);
			}
			else if (subMenu == TRUE && clientMenu == FALSE)
			{
				DrawText_COD4(SubMenuNames[i][scroll], X * 0.425, Y * 0.2861, i, 22, MenuFont, (subScroll == i) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
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

				DrawText_COD4(SubMenuNames[selectedClient][Tabindex], X * 0.425, (Y * 0.2861) + (0 * 22), 0, 0, MenuFont, (subMenu == TRUE) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
				DrawText_COD4("Prioritize", X * 0.425, (Y * 0.2861), 1, 22, MenuFont, (subMenu == TRUE && subScroll == 1) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (1 * 22), ClientOptionEnabled[1][selectedClient]);

				DrawText_COD4("Whitelist", X * 0.425, (Y * 0.2861), 2, 22, MenuFont, (subMenu == TRUE && subScroll == 2) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (2 * 22), ClientOptionEnabled[2][selectedClient]);

				DrawText_COD4("Invert Team", X * 0.425, (Y * 0.2861), 3, 22, MenuFont, (subMenu == TRUE && subScroll == 3) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (3 * 22), ClientOptionEnabled[3][selectedClient]);


				DrawText_COD4("Set GT To Player's IP?", X * 0.425, (Y * 0.2861), 4, 22, MenuFont, (subMenu == TRUE && subScroll == 4) ? ActiveTextColor : InActiveTextColor, textScale, textScale);

				DrawText_COD4("Player Has Anti-Aim", X * 0.425, (Y * 0.2861), 5, 22, MenuFont, (subMenu == TRUE && subScroll == 5) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (5 * 22), ClientOptionEnabled[5][selectedClient]);

				BYTE* IP = GetIPFromNetInfo(selectedClient);
				DrawText_COD4(va_COD4("IP: %i.%i.%i.%i", IP[0], IP[1], IP[2], IP[3]), X * 0.425, (Y * 0.2861), 6, 22, MenuFont, (subMenu == TRUE && subScroll == 6) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
				QWORD MachineID = GetMachineIDFromNetInfo(selectedClient);
				DWORD MachineIDPT1 = *(DWORD*)(DWORD)&MachineID;
				DWORD MachineIDPT2 = *(DWORD*)((DWORD)&MachineID + 0x04);
				DrawText_COD4(va_COD4("Machine ID: %08X%08X", MachineIDPT1, MachineIDPT2), X * 0.425, Y * 0.2861, 7, 22, MenuFont, (subMenu == TRUE && subScroll == 7) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
				BYTE* XUID = (BYTE*)GetXUIDFromNetInfo(selectedClient);

				DrawText_COD4(va_COD4("XUID: %08X%08X", *(DWORD*)(DWORD)&XUID[0], *(DWORD*)(DWORD)&XUID[4]), X * 0.425, Y * 0.2861, 8, 22, MenuFont, (subMenu == TRUE && subScroll == 8) ? ActiveTextColor : InActiveTextColor, textScale, textScale);

				IN_ADDR ip = *(IN_ADDR*)GetIPFromNetInfo(selectedClient);
				if (PlayerInfo[selectedClient].ip.S_un.S_addr != ip.S_un.S_addr && (ip.S_un.S_addr != 0))
				{
					PlayerInfo[selectedClient].GrabGeoIPInfo = TRUE;
					PlayerInfo[selectedClient].ip = ip;
				}

				DrawText_COD4(va_COD4(E("City: %s"), PlayerInfo[selectedClient].city), X * 0.425, Y * 0.2861, 9, 22, SmallFont, (subMenu == TRUE && subScroll == 9) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
				DrawText_COD4(va_COD4(E("State: %s"), PlayerInfo[selectedClient].stateFull), X * 0.425, Y * 0.2861, 10, 22, SmallFont, (subMenu == TRUE && subScroll == 10) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
				DrawText_COD4(va_COD4(E("Country: %s"), PlayerInfo[selectedClient].country), X * 0.425, Y * 0.2861, 11, 22, SmallFont, (subMenu == TRUE && subScroll == 11) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
				DrawText_COD4(va_COD4(E("ISP: %s"), PlayerInfo[selectedClient].isp), X * 0.425, Y * 0.2861, 12, 22, SmallFont, (subMenu == TRUE && subScroll == 12) ? ActiveTextColor : InActiveTextColor, textScale, textScale);


			}

			if (clientMenu)
				continue;

			if (strstr(SubMenuNames[i][scroll], "?"))
				continue;

			if (!strstr(SubMenuNames[i][scroll], ":") && scroll != 5)
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (i * 22), OptionEnabled[i][scroll]);
			else if (strstr(SubMenuNames[i][scroll], ":"))
				DrawText_COD4(GetMenuOptionString((eMenuNames)scroll, SubMenuNames[i][scroll]), X * 0.58, (Y * 0.2841) + (i * 22), 0, 0, MenuFont, (subMenu == TRUE && subScroll == i) ? ActiveTextColor : InActiveTextColor, textScale, textScale);
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
	X = (FLOAT)cgs_COD4->Width * 0.78125;//*(FLOAT*)((DWORD)freememory);
	Y = (FLOAT)cgs_COD4->Height * 0.2;
	//X2 = *(FLOAT*)((DWORD)freememory + 8);
	//Y2 = *(FLOAT*)((DWORD)freememory + 12);
	RotateXY(cos(ToRadians(rotation)), sin(ToRadians(rotation)), X + (FLOAT)((FLOAT)R_TextWidth_COD4(Text, 0x7FFFFFFF, BigDevFont) / 2), Y, X, Y, &X, &Y);

	rotation += 0.25;
	if (rotation >= 360.0)
		rotation -= 360.0;

	CL_DrawTextPhysical_COD4(Text, 0x7FFFFFFF, BigDevFont, X, Y, 1, 1, rotation, Vector4(1, 0, 0, 0.75f), 0);
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
			BoneIndexs[i] = SL_GetString_COD4(BoneCache[i], 0);
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

int CG_DObjGetWorldTagPos_B(pcentity_s_COD4 centity_s, int Dobj, short Tag, float* Out)
{
	return ((int(*)(pcentity_s_COD4, int, short, float*))getCryptedCoD(CheatPatches->CoDPatches.CG_DObjGetWorldTagPos))(centity_s, Dobj, Tag, Out);
}

bool GetTagPosistion(pcentity_s_COD4 centity_s, int ClientIndex, PCHAR SelectedTag, float* OutPos)
{
	//char Temp[0x100];
	short TagIndex;
	int Dobj;

	//for (int i = 0; i <= strlen(SelectedTag); i++)
	//	Temp[i] = tolower(SelectedTag[i]);

	TagIndex = GetTagIndex(SelectedTag, ShouldUpdateTagCache()); //SL_GetString_COD4(Temp, 0);
	Dobj = Com_GetClientDObj_COD4(Entities_COD4[ClientIndex].ClientNumber, 0);

	if (Dobj == 0)
		return false;

	return CG_DObjGetWorldTagPos_B(centity_s, Dobj, TagIndex, OutPos) != 0;
}

short TagToShort(PCHAR Tag)
{
	char Temp[0x100];

	for (int i = 0; i <= strlen(Tag); i++)
		Temp[i] = tolower(Tag[i]);

	return SL_GetString_COD4(Temp, 0);
}

void DropTarget(ClientInformation_COD4 PlayerInfo)
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
	if (UserInGame() && (cg_COD4 != NULL))
	{
		if (strcmp(cg_COD4->clientInfo[RandomClient].Name, "") && (RandomClient != cg_COD4->ClientNumber) && (cg_COD4->clientInfo[RandomClient].Name != NULL))
		{
			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\\0\"", cg_COD4->clientInfo[RandomClient].Name, GetXUIDFromNetInfo(cg_COD4->ClientNumber));
			Cbuf_AddText_COD4(0, Output);
		}
	}
	else
	{
		if (strcmp(GetNameFromNetInfo(RandomClient), ""))
		{
			//sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\\0\"", GetNameFromNetInfo(RandomClient), getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
			//Cbuf_AddText_COD4(0, Output);
			strcpy_s((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1F, GetNameFromNetInfo(RandomClient));
		}
	}
}

void RainbowName()
{
	char Output[0x200], Color[10], PreGameOutput[0x50];

	//if (cg_COD4 == NULL)
	//	return;

	int RandomColor = rand() % 9;
	sprintf(Color, "^%i", RandomColor);

	if (UserInGame())
	{
		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\\0\"", OriginalGT, Color, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
		Cbuf_AddText_COD4(0, Output);
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
	if (!UserInGame())
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

		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\\0\"", OriginalGT, GetXUIDFromNetInfo(cg_COD4->ClientNumber));
		Cbuf_AddText_COD4(0, Output);
		strcpy_s((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), 0x1F, OriginalGT);
	}
	else if (wcGT[0] != 0 && wcGT[1] != 0 && !UserInGame())
	{
		wcscpy_s(wcOldGT, wcGT);
		wcstombs(OriginalGT, wcGT, 0x1F);
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
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_ZSPINSPEED) == "10.0")
		ZSpinSpeed = 10;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_ZSPINSPEED) == "20.0")
		ZSpinSpeed = 20;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_ZSPINSPEED) == "30.0")
		ZSpinSpeed = 30;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_ZSPINSPEED) == "40.0")
		ZSpinSpeed = 40;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_ZSPINSPEED) == "50.0")
		ZSpinSpeed = 50;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_ZSPINSPEED) == "60.0")
		ZSpinSpeed = 60;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_ZSPINSPEED) == "70.0")
		ZSpinSpeed = 70;

	if (GetMenuOptionString(MENU_SPINBOT, OPTION_ZSPINSPEED) == "Random")
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

BOOL IsPlayerEnemy(int ClientIndex, int myClientNum)
{
	// If whitelisted
	if (ClientOptionEnabled[2][ClientIndex])
		return FALSE;

	// Handle Normal / Inverted Team Check
	if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_INVERTTEAMCHECK))
	{
		if ((cg_COD4->clientInfo[myClientNum].Team == cg_COD4->clientInfo[ClientIndex].Team) && (cg_COD4->clientInfo[myClientNum].Team != 0))
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
		if ((cg_COD4->clientInfo[myClientNum].Team == cg_COD4->clientInfo[ClientIndex].Team) && (cg_COD4->clientInfo[myClientNum].Team != 0))
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

	// If Alive
	if (!Entities_COD4[ClientIndex].isAlive || Entities_COD4[ClientIndex].Type != 1 || !(*(BYTE*)((DWORD)&Entities_COD4[ClientIndex] + 0x2)))
		return false;

	// If enemy
	if (!IsPlayerEnemy(ClientIndex, myClientNum))
		return false;

	// If whitelisted
	if (ClientOptionEnabled[2][ClientIndex])
		return false;

	return true;
}

DWORD FindBestAntiAimTarget()
{
	DWORD dwAAClosestClient = -1;
	FLOAT ClosestAA = 900000;

	// Loop for all players
	for (int i = 0; i < cgs_COD4->MaxClients; i++)
	{
		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_COD4->ClientNumber, NULL))
			continue;

		// Grab our temp distance
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_COD4[i].Origin, (float*)&cg_COD4->refdef.ViewOrg));

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

void HandleToggleOptions(pcgs_t_COD4 cgs_COD4)
{
	// Make sure the menu is ready
	if (!menuReady)
		return;

	// Update Player Names
	UpdateClientsNames();

	// Handle UAV
	BOOL Radar = Dvar_GetBool_COD4("g_compassshowenemies");
	if (GetMenuOptionValue(MENU_MAIN, OPTION_RADAR))
	{
		if (UserInGame() && !Radar)
			Cbuf_AddText_COD4(0, "g_compassshowenemies 1");
	}
	else
	{
		if (UserInGame() && Radar)
			Cbuf_AddText_COD4(0, "g_compassshowenemies 0");
	}

	// Handle No Recoil
	if (GetMenuOptionValue(MENU_MAIN, OPTION_NORECOIL))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoRecoilOffset) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoRecoilOffset) = 0x4803DD41;
	}

	// Handle No Sway
	if (GetMenuOptionValue(MENU_MAIN, OPTION_NOSWAY))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset1) = 0x60000000;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset2) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset1) = 0x4BFFB3DD;
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoSwayOffset2) = 0x4BFFC3B5;
	}

	// Handle Show Ping
	if (GetMenuOptionValue(MENU_MAIN, OPTION_SHOWPING) && UserInGame())
	{
		char* Ping = va_COD4("Ping: %i", cg_COD4->snap->ping);

		DrawText_COD4(Ping, cg_COD4->refdef.Width - 1180, cg_COD4->refdef.Height - 685, 0, 0, HudSmallFont, Vector4(1, 1, 1, 1));
		//DrawText_COD4(Ping, X - 1180, Y - 690, 0, 0, HudSmallFont, Vector4(1, 1, 1, 1));
	}

	// Handle Hear All Players
	if (GetMenuOptionValue(MENU_MAIN, OPTION_EVERYONEHEARSME) && UserInGame())
	{
		if (!HearEveryoneValue)
		{
			HearEveryoneValue = TRUE;
			Cbuf_AddText_COD4(0, "cg_everyoneHearsEveryone 1");
		}
	}
	else if (!GetMenuOptionValue(MENU_MAIN, OPTION_EVERYONEHEARSME) && UserInGame())
	{
		if (HearEveryoneValue)
		{
			HearEveryoneValue = FALSE;
			Cbuf_AddText_COD4(0, "cg_everyoneHearsEveryone 0");
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
		char HostOut[0x100];
		char* HostName = Dvar_GetString_COD4("party_hostname");
		if (strcmp(HostName, ""))
		{
			FLOAT X, Y;
			if (!UserInGame())
			{
				X = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY));
				Y = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY) + 0x4);

			}
			else
			{
				X = cg_COD4->refdef.Width;
				Y = cg_COD4->refdef.Height;
			}

			sprintf(HostOut, "Host: %s", HostName);
			DrawText_COD4(HostOut, X - 1180, Y - 700, 0, 0, HudSmallFont, Vector4(1, 1, 1, 1));
		}
	}

	// Handle Custom Field Of View
	if (UserInGame())
	{
		*(float*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.FOVStruct)) + 0xC) = FindFieldOfViewSize();
	}

	// Handle End Game
	if (GetMenuOptionValue(MENU_MAIN, OPTION_ENDGAME) && UserInGame())
	{
		char* EndGameString;
		EndGameString = va_COD4("mr %i -1 endround", *(int*)getCryptedCoD(CheatPatches->CoDPatches.ServerIdOffset));

		Cbuf_AddText_COD4(0, EndGameString);
		SetMenuOptionValue(MENU_MAIN, OPTION_ENDGAME, 0);
	}

	// Handle Leave Game
	if (GetMenuOptionValue(MENU_MAIN, OPTION_LEAVEGAME))
	{
		Cbuf_AddText_COD4(0, "disconnect");
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

	// Disable over head names for friendlies
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMIES))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.OverheadNamesEnemyOffset) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.OverheadNamesEnemyOffset) = 0x4BFFF971;
	}

	// Disable over head names for enemies
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWFRIENDLIES))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.OverheadNamesFriendlyOffset) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.OverheadNamesFriendlyOffset) = 0x4BFFF6C9;
	}

	// Handle Wallhack
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWTHROUGHWALLS))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.WallhackOffset) = 0x38C0FF03;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.WallhackOffset) = 0x38C00004;
	}

	// Handle Steal Gamertags
	if (GetMenuOptionValue(MENU_MAIN, OPTION_STEALGTS) /*&& (Dvar_GetBool_COD4("cl_ingame"))*/)
	{
		//if (cgs_COD4 == NULL)
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
		//if (cgs_COD4 == NULL)
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
			DrawLine_COD4((cg_COD4->refdef.Width / 2) - 10, cg_COD4->refdef.Height / 2, (cg_COD4->refdef.Width / 2) + 10, cg_COD4->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine_COD4(cg_COD4->refdef.Width / 2, (cg_COD4->refdef.Height / 2) - 10, cg_COD4->refdef.Width / 2, (cg_COD4->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) == "x")
		{
			// X styled crosshair
			DrawLine_COD4((cg_COD4->refdef.Width / 2) - 8, (cg_COD4->refdef.Height / 2) - 8, (cg_COD4->refdef.Width / 2) + 8, (cg_COD4->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine_COD4((cg_COD4->refdef.Width / 2) - 8, (cg_COD4->refdef.Height / 2) + 8, (cg_COD4->refdef.Width / 2) + 8, (cg_COD4->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString(MENU_DRAWING, OPTION_CROSSHAIRTYPE) == "o")
		{
			DrawLine_COD4((cg_COD4->refdef.Width / 2) - 10, cg_COD4->refdef.Height / 2, (cg_COD4->refdef.Width / 2) + 10, cg_COD4->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine_COD4(cg_COD4->refdef.Width / 2, (cg_COD4->refdef.Height / 2) - 10, cg_COD4->refdef.Width / 2, (cg_COD4->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));

			DrawLine_COD4((cg_COD4->refdef.Width / 2) - 8, (cg_COD4->refdef.Height / 2) - 8, (cg_COD4->refdef.Width / 2) + 8, (cg_COD4->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine_COD4((cg_COD4->refdef.Width / 2) - 8, (cg_COD4->refdef.Height / 2) + 8, (cg_COD4->refdef.Width / 2) + 8, (cg_COD4->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
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

	// Set Level 55
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_LEVEL55))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.RankXPOffset) = 0x1EA32; // Rank XP
		SetMenuOptionValue(MENU_SETTINGS, OPTION_LEVEL55, 0);
	}

	// Reset Current Stats
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_RESETCURRENTSTATS))
	{
		Cbuf_AddText_COD4(0, "resetStats");
		SetMenuOptionValue(MENU_SETTINGS, OPTION_RESETCURRENTSTATS, 0);
	}

	// Spoof gamertag
	if (GetMenuOptionValue(MENU_MAIN, OPTION_SPOOFGT))
	{
		SetMenuOptionValue(MENU_MAIN, OPTION_SPOOFGT, 0);

		ExCreateThread(0, 0, 0, 0, (LPTHREAD_START_ROUTINE)SpoofGamertag, 0, 0);
	}

	// Handle Third Person
	if (UserInGame())
	{
		//if (Entities_COD4[*(int*)cg_COD4].isAlive)
		//{
			if (GetMenuOptionValue(MENU_MAIN, OPTION_THIRDPERSON))
				*(byte*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.ThirdPersonStruct)) + 0xC) = 1;
			else
				*(byte*)(*(int*)(getCryptedCoD(CheatPatches->CoDPatches.ThirdPersonStruct)) + 0xC) = 0;
		//}
	}

	// Draw Legitbot FOV Box
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_DRAWFOVBOX) && UserInGame() && !menuOpen && GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT))
	{
		int FOVSize = FindFOVSize();
		DrawBox_COD4((cg_COD4->refdef.Width / 2) - (FOVSize / 2), (cg_COD4->refdef.Height / 2) - (FOVSize / 2), FOVSize, FOVSize, Vector4(1, 1, 1, 0.50f));
	}

	// Handle No Flinch
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOFLINCH))
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoFlinchOffset) = 0x60000000;
	}
	else
	{
		*(int*)getCryptedCoD(CheatPatches->CoDPatches.NoFlinchOffset) = 0x4BFFF8A5;
	}

	// Handle Change Team
	if (GetMenuOptionValue(MENU_MAIN, OPTION_CHANGETEAM) && UserInGame())
	{
		char* NewTeamName;
		if (cg_COD4->clientInfo[cg_COD4->ClientNumber].Team == 2)
			NewTeamName = va_COD4("cmd mr %i 3 %s", *(int*)getCryptedCoD(CheatPatches->CoDPatches.ServerIdOffset), "axis");
		else
			NewTeamName = va_COD4("cmd mr %i 3 %s", *(int*)getCryptedCoD(CheatPatches->CoDPatches.ServerIdOffset), "allies");

		Cbuf_AddText_COD4(0, NewTeamName);
		SetMenuOptionValue(MENU_MAIN, OPTION_CHANGETEAM, 0);
	}

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
				char* IPName = va_COD4("%s's IP: %i.%i.%i.%i", ClientName, IP[0], IP[1], IP[2], IP[3]);
				if (Dvar_GetBool_COD4("cl_ingame"))
				{
					sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\\0\"", IPName, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
					Cbuf_AddText_COD4(0, Output);
				}
				else
				{
					strcpy((char*)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), IPName);
				}
			}
		}

		ClientOptionEnabled[4][selectedClient] = 0;
	}


	// Handle Spinbot Preferred Settings
	if (GetMenuOptionValue(MENU_SPINBOT, OPTION_PREFERREDSETTINGS))
	{
		// Disable these
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);
		SetMenuOptionString(MENU_SPINBOT, OPTION_ZSPINDIRECTION, 0);
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

	// Handle Cartoon Mode
	BOOL CartoonMode = Dvar_GetBool_COD4("r_fullbright");
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_CARTOONMODE))
	{
		if (UserInGame() && !CartoonMode)
			Cbuf_AddText_COD4(0, "r_fullbright 1");
	}
	else
	{
		if (UserInGame() && CartoonMode)
			Cbuf_AddText_COD4(0, "r_fullbright 0");
	}

	// Disable Auto Aim If No Spread Is Enabled
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD) && GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM))
	{
		SetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM, 0);
	}

	// Handle Teleport Kill
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_TELEPORTKILL) && UserInGame())
	{
		if (TeleportKillDelay > 50)
		{
			TeleportKillDelay = 0;

			// Get our target
			int TeleportTarget = FindBestAntiAimTarget();
			if (TeleportTarget != -1)
			{
				Vector3 TelePos;
				GetTagPosistion(&Entities_COD4[TeleportTarget], TeleportTarget, "j_head", (float*)&TelePos);

				// Teleport
				Cbuf_AddText_COD4(0, va_COD4("setviewpos %f %f %f", TelePos.x + 15, TelePos.y + 15, TelePos.z + 30));
			}
		}
		else
		{
			TeleportKillDelay++;
		}
	}

	// Freeze All Consoles
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_FREEZEALLCONSOLES))
	{
		if (Dvar_GetBool_COD4("cl_ingame"))
		{
			char Output[0x200];
			PCHAR Hax = "^\x0002NiNJA";

			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\\0\"", cg_COD4->clientInfo[cg_COD4->ClientNumber].Name, Hax, getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 0x30);
			Cbuf_AddText_COD4(0, Output);
		}
		else
		{
			PCHAR Hax = "^\x0002NiNJA";
			memcpy((PVOID)(getCryptedCoD(CheatPatches->CoDPatches.OurXuidOffset) + 4), Hax, sizeof(Hax));
		}

		SetMenuOptionValue(MENU_SETTINGS, OPTION_FREEZEALLCONSOLES, 0);
	}

	// Stop people from wiping our stats
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_ANTIDERANK))
	{
		// Disable the ability to wipe our stats
		//*(short*)getCryptedCoD(CheatPatches->CoDPatches.DisableStatWipe) = 0x4800;
		CONST PCHAR activeActionString = (CONST PCHAR)(*(DWORD*)(*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.ActiveActionOffset)) + 0x0C));
		if (strstr(activeActionString, "resetStats") || strstr(activeActionString, "defaultStatsInit"))
		{
			*(BYTE*)((DWORD)activeActionString) = 0;
			DbgPrint("Prevented stat reset!");
		}
	}
	else
	{
		//*(short*)getCryptedCoD(CheatPatches->CoDPatches.DisableStatWipe) = 0x419A;
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

	memcpy(viewaxis, cg_COD4->refdef.ViewAxis, sizeof(viewaxis));
	memcpy(vieworg, &cg_COD4->refdef.ViewOrg, sizeof(vieworg));
	width = (float)cg_COD4->refdef.Width;
	height = (float)cg_COD4->refdef.Height;
	tanHalfFovX = cg_COD4->refdef.Fov.x;
	tanHalfFovY = cg_COD4->refdef.Fov.y;

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

int GetWeaponDef(int WeaponIndex)
{
	int WeaponDef, WeaponDefAddr;
	WeaponDefAddr = getCryptedCoD(CheatPatches->CoDPatches.WeaponDefOffset);
	__asm
	{
		slwi      WeaponDef, WeaponIndex, 2
		lwzx      WeaponDef, WeaponDef, WeaponDefAddr
	};
	return WeaponDef;

	//return *(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.WeaponDefOffset) + ((WeaponIndex) << 2));
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
	//return *(int*)(GetWeaponDef(Weapon) + 0x518);

	int weaponDef = GetWeaponDef(Weapon);
	if (weaponDef)
	{
		DWORD weapDef2 = BG_GetWeaponDef_COD4(Weapon);
		if (weapDef2)
		{
			DWORD killIconType = *(DWORD*)(weapDef2 + 0x51C);
			getWeaponAspectRatio(killIconType, aspectW, aspectH);
			return *(int*)(weaponDef + 0x518);
		}
	}
	return 0;
}

CONST PCHAR getWeaponName_COD4(int weaponIndex)
{
	DWORD r11 = weaponIndex << 3;
	DWORD r7 = weaponIndex + r11;
	r11 = r7 << 2;
	DWORD r10 = *(DWORD*)0x828CE9CC;
	DWORD r31 = r10 + r11;
	p_weaponInfo_COD4 weaponInfo = (p_weaponInfo_COD4)r31;
	return weaponInfo->Name->weaponName;
}

void DrawSnapLine(Vector4 color, int Index, BOOL SameTeam)
{
	float xy2[2];
	Vector3 xy1, LinePos, LinePosOut;

	//Get center of screen coordinates
	xy2[0] = (float)cg_COD4->refdef.Width / 2.0f;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Bottom")
		xy2[1] = (float)cg_COD4->refdef.Height;
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Center")
		xy2[1] = (float)cg_COD4->refdef.Height / 2.0f;
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) == "Top")
		xy2[1] = (float)cg_COD4->refdef.Height / 10.0f - 65.0f;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_DRAWSNAPLINES) != "Top")
		LinePos = Entities_COD4[Index].Origin;
	else
	{
		GetTagPosistion(&Entities_COD4[Index], Index, "j_helmet", (float*)&LinePos);
	}

	//Get coordinates of person
	if (WorldToScreen(LinePos, xy1) == TRUE)
	{
		DrawLine_COD4((int)xy2[0], (int)xy2[1], (int)xy1[0], (int)xy1[1], 1.0f, color);
	}
}

void DrawArrows(float* color, int Index)
{
	Vector3 Output;
	VectorSubtract(Entities_COD4[Index].Origin, Entities_COD4[cg_COD4->ClientNumber].Origin, Output);

	((void(*)(...))getCryptedCoD(CheatPatches->CoDPatches.CG_DrawGrenadePointer))(0, cg_COD4->refdef.Width / 2.0f, cg_COD4->refdef.Height / 2.0f, &Output, color, 0.0f, 1.0f);
}

void DrawBones(Vector4 color, int Index)
{
	Vector3 TagPos[2], TagScreen[2];
	for (int b = 0; b < ARRAYSIZE(bonesESPCOD4) - 1; b++)
	{
		if (!GetTagPosistion(&Entities_COD4[Index], Index, bonesESPCOD4[b], (float*)&TagPos[0]) || !GetTagPosistion(&Entities_COD4[Index], Index, bonesESPCOD4[b + 1], (float*)&TagPos[1]))
			return;

		if (WorldToScreen(TagPos[0], TagScreen[0]) && WorldToScreen(TagPos[1], TagScreen[1]))
			DrawLine_COD4(TagScreen[0].x, TagScreen[0].y, TagScreen[1].x, TagScreen[1].y,  2.0f, color);
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
		GetTagPosistion(&Entities_COD4[index], index, ThreeDBoxesBones[t], (float*)&BonePos[t]);
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
		DrawLine_COD4(minsX, minsY, corner2X, corner2Y, 1.0f, color);
		DrawLine_COD4(minsX, minsY, corner4X, corner4Y, 1.0f, color);
		DrawLine_COD4(minsX, minsY, corner6X, corner6Y, 1.0f, color);
		//maxes to 3, 5, and 7
		DrawLine_COD4(maxesX, maxesY, corner3X, corner3Y, 1.0f, color);
		DrawLine_COD4(maxesX, maxesY, corner5X, corner5Y, 1.0f, color);
		DrawLine_COD4(maxesX, maxesY, corner7X, corner7Y, 1.0f, color);
		//from 2 to 3 and 7
		DrawLine_COD4(corner2X, corner2Y, corner3X, corner3Y, 1.0f, color);
		DrawLine_COD4(corner2X, corner2Y, corner7X, corner7Y, 1.0f, color);
		//from 4 to 3 and 5
		DrawLine_COD4(corner4X, corner4Y, corner3X, corner3Y, 1.0f, color);
		DrawLine_COD4(corner4X, corner4Y, corner5X, corner5Y, 1.0f, color);
		//from 6 to 5 and 7
		DrawLine_COD4(corner6X, corner6Y, corner5X, corner5Y, 1.0f, color);
		DrawLine_COD4(corner6X, corner6Y, corner7X, corner7Y, 1.0f, color);
	}
}

void DrawESPBox(Vector4 color, int Index)
{
	float fHeight, fWidth;
	Vector3 HeadPos, FeetLocation, HeadLocation, MainRootPos;

	if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "Corners")
	{
		if (!GetTagPosistion(&Entities_COD4[Index], Index, "j_head", (float*)&HeadPos))
			return;

		if (WorldToScreen(Entities_COD4[Index].Origin, FeetLocation) && WorldToScreen(HeadPos, HeadLocation))
		{
			fHeight = FeetLocation.y - HeadLocation.y;
			if (*(int*)((DWORD)&Entities_COD4[Index] + 0x1D4) & 0x08)
				fWidth = fHeight / 0.9;
			else
				fWidth = fHeight / 2;

			DrawBox_COD4((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawBox_COD4((FeetLocation.x - (fWidth / 2)), FeetLocation.y, (fWidth / 4), 1, color);
			DrawBox_COD4((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawBox_COD4((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawBox_COD4((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawBox_COD4((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), FeetLocation.y, (fWidth / 4), 1, color);
			DrawBox_COD4((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);
			DrawBox_COD4((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);

		}
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "3D")
	{
		CalcBoundingBox(Index, color);
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "2D" || GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "2D Filled")
	{
	
		Vector3 MainRoot, MainRootOut, FeetOut, Feet;
		GetTagPosistion(&Entities_COD4[Index], Index, "j_mainroot", (float*)&MainRoot);
		MainRoot.z += 10;

		int BoxHeight = 55; // 8 or 0x0A = prone|| 4 or 6 = crouched||0 or 2 = standing
		if (Entities_COD4[Index].Stance & 0x08)
			BoxHeight = 17;
		else if (Entities_COD4[Index].Stance & 0x04)
			BoxHeight = 35;

		Feet.x = MainRoot.x; Feet.y = MainRoot.y; Feet.z = (MainRoot.z - -BoxHeight - 10);

		if (WorldToScreen(MainRoot, MainRootOut) == TRUE && WorldToScreen(Feet, FeetOut) == TRUE)
		{
			FLOAT Height = (MainRootOut.y - FeetOut.y);
			FLOAT Width = (Height / 3.0f);

			if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) != "2D Filled")
			{
				DrawLine_COD4((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), 1.0f, color); // Left side line
				DrawLine_COD4((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), 1.0f, color); // Bottom line
				DrawLine_COD4((int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Right side line
				DrawLine_COD4((int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Top line
			}
			else 
				R_AddCmdDrawStretchPic_COD4((MainRootOut.x - Width), (MainRootOut.y - (Height*0.5f)), Width*2.0f, Height*1.5f, 0.0f, 0.0f, 0.0f, 0.0f, color, WhiteMaterial);
		}
	}
	else if (GetMenuOptionString(MENU_DRAWING, OPTION_ESPBOXTYPE) == "Pyramid")
	{
		Vector3 Center = Entities_COD4[Index].Origin;

		float W = GetBoxWidth(*(int*)((DWORD)&Entities_COD4[Index] + 0x1D4)), H = GetBoxHeight(*(int*)((DWORD)&Entities_COD4[Index] + 0x1D4));

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
			DrawText_COD4(Output, (X - 360), Y - 20, 0, 0, SmallFont, Vector4(1, 1, 1, 0.75f), 0.9f, 0.9f);
		}
		else
		{
			sprintf(Output, "Press \x016 & \x12 To %s ^7NiNJA Engine", menuOpen ? "^1Close" : "^2Open");
			DrawText_COD4(Output, (cg_COD4->refdef.Width - 1200), cg_COD4->refdef.Height - 25, 0, 0, SmallFont, Vector4(1, 1, 1, 0.75f), 0.9f, 0.9f);
		}
	}
}

double Radians(float Degrees)
{
	return  0.01745329238474369 * Degrees;
}

void GetSpread_COD4(int seed, float* x, float* y)
{
	float spread = ((FLOAT)((*(FLOAT*)((DWORD)cg_COD4 + 0xF09DC)) * 0.00392156862));

	float min, max, x2, y2;
	int playerState = (((DWORD)cg_COD4 + 0x44744));
	int weaponDef = *(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.WeaponDefOffset) + ((Entities_COD4[cg_COD4->ClientNumber].weaponNum) << 2));

	BG_GetSpreadForWeapon_COD4(playerState, weaponDef, &min, &max);
	spread = min + (max - min) * spread;
	RandomBulletDir_COD4(seed, &x2, &y2);
	*x = x2 * spread;
	*y = y2 * spread;
}

Vector3 SmoothAimbotAngles(Vector3 AimbotAngles, float SmoothValue)
{
	float FinishedAngles[3]; float smoothdiff[3];
	AimbotAngles[0] -= LocalClient_COD4->ViewAngles.x;
	AimbotAngles[1] -= LocalClient_COD4->ViewAngles.y;
	if (AimbotAngles[0] > 180)  AimbotAngles[0] -= 360;
	if (AimbotAngles[1] > 180)  AimbotAngles[1] -= 360;
	if (AimbotAngles[0] < -180) AimbotAngles[0] += 360;
	if (AimbotAngles[1] < -180) AimbotAngles[1] += 360;
	smoothdiff[0] = AimbotAngles[0] / SmoothValue;
	smoothdiff[1] = AimbotAngles[1] / SmoothValue;
	FinishedAngles[0] = LocalClient_COD4->ViewAngles.x + smoothdiff[0];
	FinishedAngles[1] = LocalClient_COD4->ViewAngles.y + smoothdiff[1];
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
	float distanceSq = cg_COD4->refdef.ViewOrg.Distance(WorldPos);
	float distance = sqrt(distanceSq);
	float distFrac = (float)(distance - 64) / 448;
	float distScale = (float)(distFrac * scale) + (float)(1 - distFrac);
	*OutDist = distFrac;
	float x2 = (FLOAT)(floor(-((((FLOAT)R_TextWidth_COD4(Text, 0x7FFFFFFF, Font) * R_NormalizedTextScale_COD4(Font, scale * distScale)) * 0.5) - *x) + 0.5));

	__asm
	{
		frsp x2, x2
	}
	*x = x2;
	*y = (FLOAT)(floor(*y + 0.5));
}

bool IsWeaponDualWield()
{
	return false;
	//return (bool)*(BYTE*)(BG_GetWeaponDef_COD4(Entities_COD4[cg_COD4->ClientNumber].weaponNum) + 0x56A);
}

bool Autowall(int Index, PCHAR TagToUse, BOOL UsingShield)
{
	Vector3 TagPos, Angles, dir, Hax[3], MyPos, Start;
	int PlayerPTR = (((DWORD)cg_COD4 + 0x44744));

	if (!GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL))
		return false;

	//if (GetMenuOptionValue(MENU_RAGEBOT, "Target Shields") && UsingShield)
	//	TagToUse = "j_ankle_le";

	if (!GetTagPosistion(&Entities_COD4[Index], Index, TagToUse, (float*)&TagPos))
		return false;

	CG_GetPlayerViewOrigin_COD4(0, PlayerPTR, MyPos);
	
	BulletFireParams bulletFireParams;
	bulletFireParams.ignoreEntIndex = cg_COD4->ClientNumber;
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
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.DynEntCl_DynEntImpactEvent) = 0x4E800020;
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_BulletHitEvent) = 0x4E800020;

	// Allow 360 degree autowall
	//*(int*)0x822EC778 = 0x38600001;

	FireBulletPenetrate_COD4(0, &bulletFireParams, GetWeaponDef(Entities_COD4[cg_COD4->ClientNumber].weaponNum), &Entities_COD4[cg_COD4->ClientNumber], MyPos, false);

	// Re-enable the bullet traces
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.DynEntCl_DynEntImpactEvent) = 0x7D8802A6;
	*(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.CG_BulletHitEvent) = 0x7D8802A6;

	return (bulletFireParams.ignoreEntIndex == Index);
}

void CorrectMovement(usercmd_s_COD4 *usercommand, Vector3 vec)
{
	Vector3 Fix = vec - (LocalClient_COD4->ViewAngles);

	int forward = usercommand->movement[0];
	int side = usercommand->movement[1];

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

	usercommand->movement[0] = ClampChar(fixedforward);
	usercommand->movement[1] = ClampChar(fixedside);
}

bool TriggerBot(int Index)
{
	Vector3 HeadOrigin, Hitboxes;
	Vector3 FeetOrigin = Entities_COD4[Index].Origin;
	GetTagPosistion(&Entities_COD4[Index], Index, "j_head", (float*)&HeadOrigin);
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
			GetTagPosistion(&Entities_COD4[Index], Index, TriggerBotBones[b], (float*)&Hitboxes);
			if (WorldPosToScreenPos((float*)&Hitboxes, &ScreenPos))
			{
				if (ScreenPos.x - ((fWidth / 5) / 2) < (cg_COD4->refdef.Width / 2) &&
					ScreenPos.x + ((fWidth / 5) / 2) > (cg_COD4->refdef.Width / 2) &&
					ScreenPos.y - ((fWidth / 5) / 2) < (cg_COD4->refdef.Height / 2) &&
					ScreenPos.y + ((fWidth / 5) / 2) > (cg_COD4->refdef.Height / 2))
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

bool FOVCheck(int index, PCHAR Tag)
{
	Vector2 ScreenPos; float Output[3];
	int FOVSize = FindFOVSize();
	GetTagPosistion(&Entities_COD4[index], index, Tag, Output);
	if (WorldPosToScreenPos(Output, &ScreenPos))
	{
		if (ScreenPos.x > ((cg_COD4->refdef.Width / 2) - FOVSize) &&
			ScreenPos.x < ((cg_COD4->refdef.Width / 2) + FOVSize) &&
			ScreenPos.y >((cg_COD4->refdef.Height / 2) - FOVSize) &&
			ScreenPos.y < ((cg_COD4->refdef.Height / 2) + FOVSize))
		{
			return true;
		}
	}

	return false;
}

void CG_EntityEventHook(int localClient, int entityState_t, int Event, int eventParam)
{
	if (GetMenuOptionValue(MENU_MAIN, OPTION_FASTRELOAD))
	{
		if (Event == 20)
		{
			int ClientReloadIndex = *(short*)(entityState_t + 0xC0);
			if (ClientReloadIndex == (short)cg_COD4->ClientNumber)
				WasReloading = TRUE;
		}
		else if (Event == 23)
		{
			int ClientSwitchIndex = *(short*)(entityState_t + 0xC0);
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
	{
		rectDef->horizAlign = 0x1;
		rectDef->vertAlign = 0x1;
	}
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
		horizAlign = 0x1;
		vertAlign = 0x1;
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
		int screenInfo = getCryptedCoD(CheatPatches->CoDPatches.CL_GetScreenDimensions);
		screenMaxX = *(int*)(screenInfo + 0xea0);
		screenMaxY = *(int*)(screenInfo + 0xea4);
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
	int playerStatePtr = cg_t + 0x44744;
	Vector3 MyPos;
	CG_GetPlayerViewOrigin_COD4(0, playerStatePtr, MyPos);
	WorldPosToCompass(cg_t, LastCompassType, RadarSize, Yaw, MyPos, World, outClipped);

	Vector2 widthHeight(16, 16);
	Compass.x = -((widthHeight.x * 0.5) - x2) + outClipped.x;
	Compass.y = -((widthHeight.y * 0.5) - y2) + outClipped.y;

	if (LastCompassType == 0)
	{
		Compass.x -= 2.0f;
		Compass.y -= 2.0f;
	}
	else
	{
		Compass.x += 258.0f;
		Compass.y -= 138.0f;
	}
}
void WorldToCompass(vec3 *World, Vector2& Compass)
{
	int cg_t = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	if (cg_t)
		WorldToCompass_Internal(cg_t, World, Compass);
}

void WorldToCompass(Vector3& World, Vector2 &Compass)
{
	WorldToCompass((vec3*)&World.x, Compass);
}

DWORD getRefDef()
{
	int cg_t = *(int*)getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer);
	if (cg_t)
		return (DWORD)cg_t + 0x478cc;
	return 0;
}
int getViewAxis_ptr()
{
	DWORD refDef = getRefDef();
	if (refDef)
		return refDef + 0x20;
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
	if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR) && DrawCompassTimer > GetTickCount())
	{
		// Loop for all clients
		for (int i = 0; i < 18; i++)
		{
			// Make sure the client is not us
			if (Entities_COD4[i].ClientNumber != cg_COD4->ClientNumber && cg_COD4->ClientNumber != i)
			{
				// Alive and is player
				if (!Entities_COD4[i].isAlive || Entities_COD4[i].Type != 1 || !(*(BYTE*)((DWORD)&Entities_COD4[i] + 0x2)))
				{
					// Invalid player
					continue;
				}

				//if (LastCompassType == 1)
				//	goto skip;

				int centity = *(DWORD*)getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer);

				if (centity)
				{
					centity = centity + (i * 0x1d0);

					vec3 entityView = *(vec3*)(centity + 0xfc);
					vec3 myView = getRefDefViewAngles();

					float yaw = entityView.y;

					float rotation = (360.0f - yaw);
					if (LastCompassType == 0)
						rotation += myView.y;
					//else if (currentTitle == MW3_TITLE_ID)
					//	rotation += 90.0f;

					float w = 20.0, h = 20.0;
					Vector2 Compass;
					WorldToCompass(Entities_COD4[i].Origin, Compass);
					float aspectW, aspectH;
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
				int playerStatePtr = (((DWORD)cg_COD4 + 0x44744));
				Vector3 MyPos;
				CG_GetPlayerViewOrigin_COD4(0, playerStatePtr, MyPos);
				WorldToCompass(MyPos, Compass);
				float w = 20.0f, h = 20.0f;
				DrawRadarPic(Compass.x, Compass.y, w, h, 0.0f, (float*)&GetColorCode(colorWhite), ArrowIcon);
			}
		}
	}
}

void DrawESP()
{
	LocalClient_COD4 = (pclientActive_t_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu4
	Entities_COD4 = (pcentity_s_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu4
	cg_COD4 = (pcg_s_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu11
	cgs_COD4 = (pcgs_t_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu11

	// If in-game, start our ESP
	if (UserInGame() && menuReady && (cgs_COD4 != NULL))
	{
		// Loop for all ents (this 1st so our player ESP draws over it if they over lap)
		for (int i = 18; i < 0x7FF; i++)
		{
			// Draw Explosives
			if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWEXPLOSIVES))
			{
				// Make sure it's a explosive
				if (Entities_COD4[i].isAlive && Entities_COD4[i].Type == 4 && (Entities_COD4[i].weaponNum < 0x800))
				{
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					TopLocation = Entities_COD4[i].Origin;
					if (WorldToScreen(TopLocation, TopLocationOut))
					{
						// Get the explosive id
						int ExplosiveID = Entities_COD4[i].weaponNum;

						// Fix flashbangs not drawing
						if (ExplosiveID == CG_GetWeaponIndexForName_COD4("flash_grenade_mp", 0))
						{
							ExplosiveID = CG_GetWeaponIndexForName_COD4("concussion_grenade_mp", 0);
						}

						// Make sure it's valid
						if (ExplosiveID < 0x800 && ExplosiveID != 0)
						{
							float aspectW, aspectH;
							int ExplosiveIcon = GetWeaponIcon(ExplosiveID, &aspectW, &aspectH);
							if (ExplosiveIcon != 0)
							{
								CG_DrawRotatedPicPhysical_COD4(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), TopLocationOut.x - (aspectW/2), TopLocationOut.y - (aspectH/2), aspectW, aspectH, 0.0, GetColorCode(colorYellow), ExplosiveIcon);
							}
						}
					}
				}
			}

			// Draw Dropped Items
			if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS))
			{
				BOOL IsWeapon = TRUE, ValidItem = FALSE;

				//DbgPrint("%i %i\n", fixDroppedWeaponNum(127), fixDroppedWeaponNum(129));

				// Make sure it's a dropped item
				if (Entities_COD4[i].isAlive && Entities_COD4[i].Type == 3 && (GetDistance(cg_COD4->refdef.ViewOrg, Entities_COD4[i].Origin) < 1500.0f))
				{
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					TopLocation = Entities_COD4[i].Origin;
					if (WorldToScreen(TopLocation, TopLocationOut))
					{
						// Get the dropped weapon id
						int DroppedWeapon = *(int*)((DWORD)&Entities_COD4[i] + 0x148) % 0x80;

						// Make sure it's valid / not a weapon
						if (DroppedWeapon < 0x800 && DroppedWeapon != 0)
						{
							ValidItem = TRUE;
						}

						// Draw the item if it's valid
						if (ValidItem)
						{
							float aspectW, aspectH;
							int DroppedIcon = GetWeaponIcon(DroppedWeapon, &aspectW, &aspectH);
							if (DroppedIcon != 0)
							{
								CG_DrawRotatedPicPhysical_COD4(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), TopLocationOut.x - (aspectW/2), TopLocationOut.y - (aspectH/2), aspectW, aspectH, 0.0, Vector4(1, 1, 1, 1), DroppedIcon);
							}
						}
					}
				}
			}
		}

		// Loop for all clients
		for (int i = 0; i < cgs_COD4->MaxClients; i++)
		{
			// Make sure the client is not us
			if (Entities_COD4[i].ClientNumber != cg_COD4->ClientNumber && cg_COD4->ClientNumber != i)
			{
				// Alive and is player
				if (!Entities_COD4[i].isAlive || Entities_COD4[i].Type != 1 || !(*(BYTE*)((DWORD)&Entities_COD4[i] + 0x2)))
				//if (*(short*)((DWORD)&Entities_COD4[i] + 0x29A) != 1 || (*(int*)((DWORD)&Entities_COD4[i] + 0x318) & 0x40000000) == 0 || (*(int*)((DWORD)&Entities_COD4[i] + 0x1E0) & 0x40000))
				{
					// Invalid player
					continue;
				}

				// Set some of the players info in our struct
				PlayerInfo[i].Alive = TRUE;
				PlayerInfo[i].ClientNumber = Entities_COD4[i].ClientNumber;
				PlayerInfo[i].Name = cg_COD4->clientInfo[i].Name;

				// Team check
				if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_INVERTTEAMCHECK))
				{
					if ((cg_COD4->clientInfo[cg_COD4->ClientNumber].Team == cg_COD4->clientInfo[i].Team) && (cg_COD4->clientInfo[cg_COD4->ClientNumber].Team != 0))
					{
						// Save players team
						PlayerInfo[i].Team = cg_COD4->clientInfo[i].Team;
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

						PlayerInfo[i].Team = cg_COD4->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = FALSE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
					}
				}
				else
				{
					// Reverse team check
					if ((cg_COD4->clientInfo[cg_COD4->ClientNumber].Team == cg_COD4->clientInfo[i].Team) && (cg_COD4->clientInfo[cg_COD4->ClientNumber].Team != 0))
					{
						// If we want to draw enemies
						if (!GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMIES))
							continue;

						PlayerInfo[i].Team = cg_COD4->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = FALSE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
					}
					else
					{
						// Save players team
						PlayerInfo[i].Team = cg_COD4->clientInfo[i].Team;
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
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_COD4(0, &Entities_COD4[i], TagToShort("j_head"));

					// Get the players posistion for our visible check
					GetTagPosistion(&Entities_COD4[i], i, "j_head", (float*)&PlayerInfo[i].TagPosistion);

					// Run our trace
					BYTE CL_TraceResult[100];
					CG_LocationalTraceCOD4((void*)CL_TraceResult, &cg_COD4->refdef.ViewOrg, &PlayerInfo[i].TagPosistion, i, 0x803003);

					// If the trace fraction is >= 0.97 let's say they are visible
					float traceFraction = *(float*)(CL_TraceResult);
					PlayerInfo[i].VisibleESP = ((traceFraction >= 0.97f) ? TRUE : FALSE);
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_COD4(0, &Entities_COD4[i], TagToShort("j_head"));

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
					GetTagPosistion(&Entities_COD4[i], i, "j_helmet", (float*)&Pos[1]);
					FeetPos = Entities_COD4[i].Origin;

					if (WorldToScreen(Pos[1], xy1[0]) && WorldToScreen(FeetPos, FeetPosOut))
					{
						if (Entities_COD4[i].weaponNum < 0x800 && Entities_COD4[i].weaponNum != 0)
						{
							float aspectW, aspectH;
							int WeaponIcon = GetWeaponIcon(Entities_COD4[i].weaponNum, &aspectW, &aspectH);
							if (WeaponIcon != 0)
							{
								CG_DrawRotatedPicPhysical_COD4(getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical), xy1[0].x - (aspectW/2), xy1[0].y - (aspectH/2) - 10, 50, 20, 0.0, Color, WeaponIcon);
							}
						}
					}
				}

				// ESP Name
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWNAME))
				{
					Vector3 HeadPos, HeadPosOut, FeetPos, FeetPosOut;
					GetTagPosistion(&Entities_COD4[i], i, "j_head", (float*)&HeadPos);
					FeetPos = Entities_COD4[i].Origin;

					if (WorldToScreen(HeadPos, HeadPosOut) && WorldToScreen(FeetPos, FeetPosOut))
					{
						if (MmIsAddressValid((PCHAR*)cg_COD4->clientInfo[i].Name))
						{
							float DisanceOut;
							Vector3 FeetPosOutOG, HeadPosOutOG;
							VectorCopy(FeetPosOut, FeetPosOutOG);
							VectorCopy(HeadPosOut, HeadPosOutOG);

							FixTextForDistance(cg_COD4->clientInfo[i].Name, &FeetPosOut.x, &FeetPosOut.y, SmallFont, 1, FeetPos, &DisanceOut);
							//FeetPosOut.y -= ((HeadPosOutOG.y - FeetPosOutOG.y) * 0.750);
							FeetPosOut.y += (((FeetPosOutOG.y - HeadPosOutOG.y) * (DisanceOut * 0.0001f) + 15));

							DrawText_COD4(cg_COD4->clientInfo[i].Name, (FeetPosOutOG.x - (R_TextWidth_COD4(cg_COD4->clientInfo[i].Name, 0x7FFFFFFF, SmallFont) / 2) * 0.5f), FeetPosOut.y, 0, 0, SmallFont, Color, 0.5f, 0.5f);
						}
					}
				}

				// Enemy Visible Text
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMYVISIBLE))
				{
					if (PlayerInfo[i].VisibleESP && !PlayerInfo[i].OnTeam)
					{
						DrawText_COD4("Visible Enemy!", ((cg_COD4->refdef.Width / 2) - (R_TextWidth_COD4("Visible Enemy!", 0x7FFFFFFF, HudSmallFont) / 2) * 0.7f), (cg_COD4->refdef.Height / 2) - 120, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
					}
				}

				// Proximity Warning Text
				if (GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWPROXIMITYWARNING))
				{
					if (!PlayerInfo[i].OnTeam)
					{
						float OutPos[3];
						GetTagPosistion(&Entities_COD4[i], i, "j_head", OutPos);
						if ((GetDistance(cg_COD4->refdef.ViewOrg, OutPos) / 100 < 5))
						{
							DrawText_COD4("Proximity Warning!", ((cg_COD4->refdef.Width / 2) - (R_TextWidth_COD4("Proximity Warning!", 0x7FFFFFFF, HudSmallFont) / 2) * 0.7f), (cg_COD4->refdef.Height / 2) - 170, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
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
			DrawRadar_COD4();
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
	LocalClient_COD4 = (pclientActive_t_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu11
	Entities_COD4 = (pcentity_s_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu11
	cg_COD4 = (pcg_s_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu11
	cgs_COD4 = (pcgs_t_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu11

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
		DrawMenu_COD4();
	}
	else
	{
		menuHeight = 0;
		menuWidth = 0;
	}

	// Handles toggle options like no recoil, sway, uav, etc
	HandleToggleOptions(cgs_COD4);

	bool inGame = UserInGame();

	// Draw our NiNJA Engine v1.0 text at the top right of screen pre-game
	if (!inGame && menuReady)
	{
		if (!GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLENINJATEXT))
		{
			FLOAT X, Y;
			X = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY));
			Y = (FLOAT)*(DWORD*)(getCryptedCoD(CheatPatches->CoDPatches.CG_S_XY) + 0x4);
			DrawText_COD4("NiNJA Engine v1.0", ((X - 130) - (R_TextWidth_COD4("NiNJA Engine v1.0", 0x7FFFFFFF, SmallFont) / 2) * 0.7f), (Y / 2) - 330, 0, 0, SmallFont, Vector4(0.102f, 1, 0.102f, 0.50f), 0.5f, 0.5f);
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
	LocalClient_COD4 = (pclientActive_t_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu4
	Entities_COD4 = (pcentity_s_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu4
	cg_COD4 = (pcg_s_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu4
	cgs_COD4 = (pcgs_t_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu4

	// Draw our ESP
	DrawESP();

	// Call the original
	return Menu_PaintAllDetour->CallOriginal(r3, r4);
}

PCHAR FindBestBoneByDamage(int EntIndex, BOOL HasRiotShield)
{
	Vector3 VisPos, MyPos;
	float totalDamage = 0;
	DWORD BoneIndex = 0;
	BYTE CL_TraceResult[100];
	float traceFraction;

	// Our hit location indexs
	BYTE HitLocationIndexs[6] = { 2, 1, 5, 4, 0x11, 0x10 };
	int PlayerPTR = (((DWORD)cg_COD4 + 0x44744));

	if (HasRiotShield)
	{
		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_COD4[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_COD4(0, PlayerPTR, MyPos);
		CG_LocationalTraceCOD4((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003);

		// Check our trace fraction
		traceFraction = *(float*)(CL_TraceResult);
		if (traceFraction >= 0.95f)
			return "j_ankle_le";

		return NULL;
	}

	for (int i = 0; i < 6; i++)
	{
		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_COD4[EntIndex], EntIndex, VisCheckBones[i], (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_COD4(0, PlayerPTR, MyPos);
		CG_LocationalTraceCOD4((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003);

		// Check our trace fraction
		traceFraction = *(float*)(CL_TraceResult);
		if (traceFraction >= 0.95f)
		{
			// Get weapon def
			DWORD WeaponDef = BG_GetWeaponDef_COD4(Entities_COD4[cg_COD4->ClientNumber].weaponNum);

			// Get our temp damage from weaponDamage * HitLocationMultiplier
			/*float tempDamage = (float)*(int*)(WeaponDef + 0x230) * G_GetWeaponHitLocationMultiplier(HitLocationIndexs[i], Entities_COD4[cg_COD4->ClientNumber].WeaponIndex);
			if (tempDamage > totalDamage)
			{
				totalDamage = tempDamage;
				BoneIndex++;
			}*/
		}
	}

	if (totalDamage > 0)
		return VisCheckBones[BoneIndex];

	return NULL;
}

int FindBestVisibleBoneIndex(int EntIndex, BOOL HasRiotShield)
{
	Vector3 VisPos, MyPos;
	BYTE CL_TraceResult[100];
	int PlayerPTR = (((DWORD)cg_COD4 + 0x44744));
	float TraceFraction;

	if (HasRiotShield)
	{
		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_COD4[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_COD4(0, PlayerPTR, MyPos);
		CG_LocationalTraceCOD4((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003);

		// Check our trace fraction
		TraceFraction = *(float*)(CL_TraceResult);
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
		GetTagPosistion(&Entities_COD4[EntIndex], EntIndex, HitboxVisCheckBones[i], (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_COD4(0, PlayerPTR, MyPos);
		CG_LocationalTraceCOD4((void*)CL_TraceResult, &MyPos, &VisPos, EntIndex, 0x803003);

		// Check our trace fraction
		TraceFraction = *(float*)(CL_TraceResult);
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

void CalcPrediction(int clientIndex, Vector3 BonePos, int Scale)
{
	Vector3 EnemyVelocityTemp;
	Vector3 EnemyVelocity;

	VectorSubtract(Entities_COD4[clientIndex].Origin, Entities_COD4[clientIndex].OldOrigin, EnemyVelocityTemp);

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

	// Grab cg to get playerstate pointer
	int PlayerPTR = (((DWORD)cg_COD4 + 0x44744));

	// Loop for all players
	for (int i = 0; i < cgs_COD4->MaxClients; i++)
	{
		BOOL Visible = FALSE, Wallbangable = FALSE, UsingShield = FALSE;

		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_COD4->ClientNumber, UsingRagebot))
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
			GetTagPosistion(&Entities_COD4[i], i, AimTag, (float*)&VisPos);

			// Run our trace
			BYTE CL_TraceResult[100];
			CG_GetPlayerViewOrigin_COD4(0, PlayerPTR, MyPos);
			CG_LocationalTraceCOD4((void*)CL_TraceResult, &MyPos, &VisPos, i, 0x803003);

			float traceFraction = *(float*)(CL_TraceResult);

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
				}

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
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_COD4[i].Origin, (float*)&cg_COD4->refdef.ViewOrg));

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

VOID Ragebot(pusercmd_s_COD4 usercommand, pusercmd_s_COD4 oldusercommand, pusercmd_s_COD4 nextusercommand)
{
	Vector3 EnemyPosistion, MyOrigin, Angles, VectorAngles, FinalAngles, SpreadAngles;
	PCHAR AimbotTag = "j_head";
	int PlayerPTR = (((DWORD)cg_COD4 + 0x44744));
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
	GetTagPosistion(&Entities_COD4[BestTarget], BestTarget, AimTag, (float*)&EnemyPosistion);

	// Calculate prediction if we want it
	if (GetMenuOptionString(MENU_RAGEBOT, OPTION_PREDICTION) != "Off")
		CalcPrediction(BestTarget, EnemyPosistion, FindPredicationScale());

	// Get our view origin
	CG_GetPlayerViewOrigin_COD4(0, PlayerPTR, MyOrigin);

	// Calculate aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles);
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_COD4->SpawnAngles, FinalAngles);

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
		LocalClient_COD4->ViewAngles[0] = FinalAngles.x;
		LocalClient_COD4->ViewAngles[1] = FinalAngles.y;
	}

	// Do our no spread
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD))
	{
		// Get our seed from the current server time
		int Seed = nextusercommand->serverTime;//*(int*)PlayerPTR;

		// Calculate our no spread
		GetSpread_COD4(Seed, &SpreadAngles.x, &SpreadAngles.y);

		// Apply our no spread
		usercommand->ViewAngles[0] -= ANGLE2SHORT(-SpreadAngles.y);
		usercommand->ViewAngles[1] -= ANGLE2SHORT(-SpreadAngles.x);
	}

	// Autoshoot
	if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT))
	{
		if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD))
		{
			nextusercommand->Buttons |= 1;
			if (usercommand->Buttons & 1)
			{
				usercommand->Buttons &= 0xFFFFFFFEu;
			}

			//if (IsWeaponDualWield())
			//	usercommand->Buttons |= 0x00100280;
		}
		else
		{
			// If not using no spread, wait until aimed in before shooting
			//if (cg_COD4->ZoomState >= 0.5f)
			//{
			nextusercommand->Buttons |= 1;
			if (usercommand->Buttons & 1)
			{
				usercommand->Buttons &= 1;
			}
			//}
		}
	}

	// Hit shots without autoshoot
	if (!GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT) && (State.Gamepad.bRightTrigger > 35))
	{
		nextusercommand->Buttons |= 1;
		if (usercommand->Buttons & 1)
		{
			usercommand->Buttons &= 0xFFFFFFFEu;
		}
	}
}

VOID Legitbot(pusercmd_s_COD4 usercommand, pusercmd_s_COD4 nextusercommand)
{
	Vector3 EnemyPosistion, MyOrigin, Angles, VectorAngles, FinalAngles, LegitAngles;
	BOOL DelayDone = FALSE;
	XINPUT_STATE State;
	int PlayerPTR = (((DWORD)cg_COD4 + 0x44744));

	// Get our best target
	DWORD BestTarget = FindBestTarget(FALSE);
	GClosestClient = BestTarget;
	if (BestTarget == -1)
		return;

	// Update our input state
	if (FAILED(XInputGetState(0, &State)))
		return;

	// Get the best targets posistion
	GetTagPosistion(&Entities_COD4[BestTarget], BestTarget, GetMenuOptionString(MENU_LEGITBOT, OPTION_AIMBONE), (float*)&EnemyPosistion);

	// Get our view origin
	CG_GetPlayerViewOrigin_COD4(0, PlayerPTR, MyOrigin);

	// Calc our aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles); 
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_COD4->SpawnAngles, FinalAngles);

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
		if ((GReactionDelay > ReactionDelay) /*&& cg_COD4->ZoomState >= 0.5f*/)
		{
			// Set our angles
			if (AimKeyVal == "None" ||
			AimKeyVal == "LT" && (State.Gamepad.bLeftTrigger > 35) ||
			AimKeyVal == "RT" && (usercommand->Buttons & 1) ||
			AimKeyVal == "LT & RT" && (usercommand->Buttons & 1 && State.Gamepad.bLeftTrigger > 35) ||
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
				LocalClient_COD4->ViewAngles[0] = LegitAngles.x;
				LocalClient_COD4->ViewAngles[1] = LegitAngles.y;
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
		//if (cg_COD4->ZoomState >= 0.5f)
		//{
		// Set our angles
		if (AimKeyVal == "None" ||
			AimKeyVal == "LT" && (State.Gamepad.bLeftTrigger > 35) ||
			AimKeyVal == "RT" && (usercommand->Buttons & 1) ||
			AimKeyVal == "LT & RT" && (usercommand->Buttons & 1 && State.Gamepad.bLeftTrigger > 35) ||
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
			LocalClient_COD4->ViewAngles[0] = LegitAngles.x;
			LocalClient_COD4->ViewAngles[1] = LegitAngles.y;
		}
		//}
	}

	// Autoshoot
	if (GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOSHOOT) || ((GetMenuOptionValue(MENU_LEGITBOT, OPTION_TRIGGERBOT) && TriggerBot(BestTarget))))
	{
		// Wait until aimed in before shooting
		//if (cg_COD4->ZoomState >= 0.5f)
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

VOID Spinbot(usercmd_s_COD4 *UserCommand)
{
	int RandomAdd = rand() % 71;
	int RandomUpDownSpeed = rand() % 71;
	int RandomLeft = rand() % 360;
	int RandomRight = rand() % -360;
	int PlayerPTR = (((DWORD)cg_COD4 + 0x44744));

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
	if ((Entities_COD4[cg_COD4->ClientNumber].Stance & 0x8) || (cg_COD4->Health <= 0) || UserCommand->Buttons & 0x00010000 || UserCommand->Buttons & 0x00020000)
		return;

	Vector3 MyPosAA;
	DWORD BestAAClient = FindBestAntiAimTarget();
	Vector3 MyPos; //= *(Vector3*)GetViewOriginPointer();
	Vector3 EnemyPos, Angles, VectorAngles, FinalAngles;

	if (BestAAClient == -1)
		return;

	CG_GetPlayerViewOrigin_COD4(0, PlayerPTR, MyPos);
	GetTagPosistion(&Entities_COD4[BestAAClient], BestAAClient, "j_mainroot", (float*)&EnemyPos);

	// Calculate aimbot angles
	VectorSubtract(EnemyPos, MyPos, Angles);
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_COD4->SpawnAngles, FinalAngles);

	int AAAngleX = UserCommand->ViewAngles[0];
	int AAAngleY = UserCommand->ViewAngles[1];

	// Anti-Aim X Look down
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Look Down")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(ffAngleNormalize(60.0f + ShortToAngle(UserCommand->angles[0]) - RefDefAngles.x)); //ANGLE2SHORT(72.0f);
		UserCommand->ViewAngles[0] = ANGLE2SHORT(62.99 - LocalClient_COD4->SpawnAngles.x);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Up & Down")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		int RandomShouldFlip = rand() % 12;
		if (RandomShouldFlip > 5)
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_COD4->SpawnAngles.x);
			SpinbotFlipX = TRUE;
		}
		else
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(-45 - LocalClient_COD4->SpawnAngles.x);
			SpinbotFlipX = FALSE;
		}
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Fake Up")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		if (flip)
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(-69.99 - LocalClient_COD4->SpawnAngles.x);
		}
		else
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_COD4->SpawnAngles.x);
		}
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT) == "Detection")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION, 0);

		UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_COD4->SpawnAngles.x);

		// If we took damage or they are wallbangable / visible
		if ((cg_COD4->Health < OurHealth) || PlayerInfo[BestAAClient].Wallbangable)
		{
			// Move our body move to try to avoid bullets
			//UserCommand->angles[1] = ANGLE2SHORT(FinalAngles.y - (SpinbotFlip ? 80.0f : -90.0f));
			UserCommand->ViewAngles[0] = ANGLE2SHORT(rand() % 65 - LocalClient_COD4->SpawnAngles.x);
			SpinbotFlip = !SpinbotFlip;
		}

		// Update our health
		OurHealth = cg_COD4->Health;

	}

	// Anti-Aim Y Flip Flop
	if (GetMenuOptionString(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT) == "Flip Flop")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_YSPINDIRECTION, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

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
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

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
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

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

		if (LocalClient_COD4->CommandNumber % 3)
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
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		// If we took damage or they are wallbangable / visible
		if ((cg_COD4->Health < OurHealth) || PlayerInfo[BestAAClient].Wallbangable)
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
		OurHealth = cg_COD4->Health;
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
		UserCommand->ViewAngles[0] = ANGLE2SHORT(62.99 - LocalClient_COD4->SpawnAngles.x);
	}
	else if (GetMenuOptionString(MENU_SPINBOT, OPTION_XSPINDIRECTION) == "Look Up")
	{
		// Disable other spinbots
		SetMenuOptionString(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT, 0);
		//SetMenuOptionString(MENU_SPINBOT, "Funny Spinbots:", 0);

		//UserCommand->angles[0] = ANGLE2SHORT(ffAngleNormalize(-60.0f + ShortToAngle(UserCommand->angles[0]) - RefDefAngles.x));
		UserCommand->ViewAngles[0] = ANGLE2SHORT(-62.99 - LocalClient_COD4->SpawnAngles.x);
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

int CL_ReadyToSendPacketHook(int r3)
{
	LocalClient_COD4 = (pclientActive_t_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.LocalClientPointer));//tu11
	Entities_COD4 = (pcentity_s_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.EntitiesPointer));//tu11
	cg_COD4 = (pcg_s_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CG_Pointer));//tu11
	cgs_COD4 = (pcgs_t_COD4)GetPointer(getCryptedCoD(CheatPatches->CoDPatches.CGS_Pointer));//tu11
	XINPUT_STATE State;

	// Update our input state
	if (!menuReady)
		goto Return;

	if (UserInGame() && (GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT) || GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT) || GetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT)))
	{
		// Do our server time 
		pusercmd_s_COD4 OldUserCommand = LocalClient_COD4->GetUserCommand(LocalClient_COD4->GetUserCommandNumber() - 1);
		pusercmd_s_COD4 CurrentUserCommand = LocalClient_COD4->GetUserCommand(LocalClient_COD4->GetUserCommandNumber());
		pusercmd_s_COD4 NextUserCommand = LocalClient_COD4->GetUserCommand(LocalClient_COD4->GetUserCommandNumber() + 1);
		*NextUserCommand = *CurrentUserCommand;
		CurrentUserCommand->serverTime -= 1;
		LocalClient_COD4->CommandNumber += 1;

		// Do our spinbot
		if (GetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT))
			Spinbot(NextUserCommand);

		// Reset our player info structure when we enter a game
		if (!DroppedAllTargets)
		{
			for (int Target = 0; Target < cgs_COD4->MaxClients; Target++)
			{
				DropTarget(PlayerInfo[Target]);
			}

			DroppedAllTargets = TRUE;
		}

		// If Ragebot aimbot mode
		if (GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT))
			Ragebot(CurrentUserCommand, OldUserCommand, NextUserCommand);

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
					NextUserCommand->Buttons |= 1;
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
			return CL_ReadyToSendPacketDetour->CallOriginal(r3);
		else return FALSE;
	}
}

int PlayLocalSoundAliasByNameHook(int localClient, char* soundName, int aa0)
{
	// We can use this to disable or re-direct any local sounds
	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUMUSIC))
	{
		if (!strcmp(soundName, "music_mainmenu_mp"))
			return 0;
	}

	return PlayLocalSoundAliasByNameDetour->CallOriginal(localClient, soundName, aa0);
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

void UI_DrawTextHook(int scrPlace, const char *text, int maxChars, int font, float x, float y, int horzAlign, int vertAlign, float scale, const float *color, int style)
{
	return UI_DrawTextDetour->CallOriginal(scrPlace, text, maxChars, font, x, y, horzAlign, vertAlign, scale, color, style);
}

void RB_SetTessTechniqueHook(int r3, int r4)
{
	// Hook this to prevent crashes from people setting invalid text
	if (r3 < 0x82000000)
		return;

	return RB_SetTessTechniqueDetour->CallOriginal(r3, r4);
}

void LiveStorage_SetStatHook(const int controllerIndex, int index, const int value)
{
	if (!Dvar_GetBool_COD4("cl_ingame"))
		return LiveStorage_SetStatDetour->CallOriginal(controllerIndex, index, value);

	if (GetMenuOptionValue(MENU_SETTINGS, OPTION_ANTIDERANK))
	{
		*(int*)0x822D13DC = 0x60000000;
		*(int*)0x821A36B8 = 0x60000000;
		if (index == 2301 || index == 2326 || index == 2350 || index == 2300 || index == 299)
		{
			DbgPrint("Someone attempted to reset your stats! %d", index);
			return;
		}
	}
	else
	{
		*(int*)0x822D13DC = 0x4BED186D;
		*(int*)0x821A36B8 = 0x4BFFF591;
	}

	return LiveStorage_SetStatDetour->CallOriginal(controllerIndex, index, value);
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
BOOL DidTextures = FALSE;

HRESULT DrawIndexedPrimitive(INT pDevice, INT Args)
{
	int playerTeam = 0;

	char* pShader = **(char ***)(pDevice + 0x118);
	D3DDevice *Device = *(D3DDevice **)(pDevice + 0xF0);

	if (1 == 1)
	{
		// Get players team number based on their shaders
		if (pShader != NULL)
		{
			// Check for team 1 shader
			if (strstr(pShader, "tf_141_") || (strstr(pShader, "_ally_tf141_") || (strstr(pShader, "udt") || (strstr(pShader, "sniper_ghillie") || strstr(pShader, "us_army_") || (strstr(pShader, "seal_"))))))
				playerTeam = 1;

			// Check for team 2 shader
			else if (strstr(pShader, "opforce") || (strstr(pShader, "militia") || (strstr(pShader, "airborne_") || (strstr(pShader, "op_arctic") || (strstr(pShader, "arctic_") || strstr(pShader, "arab") || strstr(pShader, "head_suren") || (strstr(pShader, "op_ghille")))))))
				playerTeam = 2;
		}

		// Generate our textures
		if (!OrangeTexture || !LightBlueTexture)
		{
			if (!DidTextures)
			{
				GenerateTexture(Device, &OrangeTexture, D3DCOLOR_RGBA(230, 126, 34, 127));
				GenerateTexture(Device, &LightBlueTexture, D3DCOLOR_RGBA(52, 152, 219, 255));
				DidTextures = TRUE;
			}
		}

		// Team 1
		if (playerTeam == 1)
		{
			//if (GetMenuOptionString(MENU_DRAWING, "Chams:") == "Wireframe")
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

			//pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			Device->SetTexture(0, LightBlueTexture);
			R_DrawIndexedPrimitiveDetour->CallOriginal(pDevice, Args);
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			//pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		}

		// Team 2
		if (playerTeam == 2)
		{
			//if (GetMenuOptionString(MENU_DRAWING, "Chams:") == "Wireframe")
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

			//pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			Device->SetTexture(0, OrangeTexture);
			R_DrawIndexedPrimitiveDetour->CallOriginal(pDevice, Args);
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			//pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		}
	}

	if (playerTeam == 0)
		return R_DrawIndexedPrimitiveDetour->CallOriginal(pDevice, Args);

	//return ((int(*)(...))0x820AB150)(Device, 4, 0, *(int*)(Args + 0x8), (*(int*)(Args + 0x4)) + ((*(int*)(Args + 0x4)) << 1));
}*/

void SetupCoD4Offsets()
{
	va_COD4 = (CONST PCHAR(__cdecl *)(CONST PCHAR, ...))getCryptedCoD(CheatPatches->CoDPatches.VA);//tu4
	Dvar_GetBool_COD4 = (BOOL(__cdecl *)(CONST PCHAR))getCryptedCoD(CheatPatches->CoDPatches.Dvar_GetBool);//tu4
	Dvar_GetString_COD4 = (PCHAR(__cdecl *)(CONST PCHAR))getCryptedCoD(CheatPatches->CoDPatches.Dvar_GetString);//tu4
	CG_LocationalTraceCOD4 = (void(_cdecl*)(void *, Vector3 *, Vector3*, int, int))getCryptedCoD(CheatPatches->CoDPatches.CG_LocationalTrace); //tu4
	DB_FindXAssetHeader_COD4 = (DWORD(__cdecl *)(XAssetType_COD4, CONST PCHAR, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DB_FindXAssetHeader);//tu4
	R_TextWidth_COD4 = (DWORD(__cdecl *)(CONST PCHAR, DWORD, DWORD))getCryptedCoD(CheatPatches->CoDPatches.R_TextWidth);//tu4
	CL_DrawTextPhysical_COD4 = (VOID(__cdecl*)(CONST PCHAR, DWORD, DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.CL_DrawTextPhysical);//tu4
	R_AddCmdDrawStretchPic_COD4 = (VOID(__cdecl*)(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.R_AddCmdDrawStretchPic);//tu4
	CG_DrawRotatedPicPhysical_COD4 = (VOID(__cdecl*)(DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPicPhysical2);//tu4
	BG_GetWeaponDef_COD4 = (DWORD(__cdecl *)(DWORD))getCryptedCoD(CheatPatches->CoDPatches.BG_GetWeaponDef);//tu11
	Cbuf_AddText_COD4 = (void(__cdecl*)(DWORD, char*))getCryptedCoD(CheatPatches->CoDPatches.Cbuf_AddText);//tu4
	SL_GetString_COD4 = (short(__cdecl*)(PCHAR, DWORD))getCryptedCoD(CheatPatches->CoDPatches.SL_GetString);//tu4
	Com_GetClientDObj_COD4 = (DWORD(__cdecl *)(DWORD, DWORD))getCryptedCoD(CheatPatches->CoDPatches.Com_GetClientDObj);//tu4
	CG_GetWeaponIndexForName_COD4 = (DWORD(__cdecl *)(CONST PCHAR, int))getCryptedCoD(CheatPatches->CoDPatches.CG_GetWeaponIndexForName);//tu4
	PlayLocalSoundAliasByName = (int(__cdecl*)(int, char*, int))getCryptedCoD(CheatPatches->CoDPatches.PlayLocalSoundAliasByName);//tu4
	CG_GetEntityBModelBounds = (void(__cdecl*)(pcentity_s_COD4, float*, float*))getCryptedCoD(CheatPatches->CoDPatches.CG_GetEntityBModelBounds);//tu23
	R_NormalizedTextScale_COD4 = (int(__cdecl*)(int, float))getCryptedCoD(CheatPatches->CoDPatches.R_NormalizedTextScale);//tu4
	CG_CalcNamePosition = (bool(__cdecl*)(int, Vector3*, float *, float *))getCryptedCoD(CheatPatches->CoDPatches.CG_CalcNamePosition);//tu4
	CL_SetStance = (VOID(__cdecl*)(int localClientNum, int stance))getCryptedCoD(CheatPatches->CoDPatches.CL_SetStance);
	G_GetWeaponHitLocationMultiplier_COD4 = (FLOAT(__cdecl*)(DWORD, DWORD))getCryptedCoD(CheatPatches->CoDPatches.G_GetWeaponHitLocationMultiplier);//TU11
	AimAssist_GetTagPos_COD4 = (VOID(__cdecl *)(DWORD, pcentity_s_COD4, WORD, Vector3&))getCryptedCoD(CheatPatches->CoDPatches.AimAssist_GetTagPos);//tu4

	// No Spread Functions
	CG_GetPlayerViewOrigin_COD4 = (VOID(__cdecl*)(DWORD, DWORD, Vector3&))getCryptedCoD(CheatPatches->CoDPatches.CG_GetPlayerViewOrigin);//TU4
	BG_GetSpreadForWeapon_COD4 = (VOID(__cdecl*)(DWORD, DWORD, float*, float*))getCryptedCoD(CheatPatches->CoDPatches.BG_GetSpreadForWeapon);//TU4
	RandomBulletDir_COD4 = (VOID(__cdecl *)(DWORD, float*, float*))getCryptedCoD(CheatPatches->CoDPatches.RandomBulletDir);//TU4

	// Radar Functions
	DrawEngineRadar_COD4 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DrawEngineRadar);//tu18
	DrawEnemies_COD4 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DrawEnemies);//tu18
	DrawFriendlies_COD4 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DrawFriendlies);//tu18
	DrawVehicles_COD4 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DrawVehicles);//tu18
	DrawPlayer_COD4 = (VOID(__cdecl*)(DWORD, DWORD, DWORD, unkRadarStruct*, float, DWORD))getCryptedCoD(CheatPatches->CoDPatches.DrawPlayer);//tu18
	CG_WorldPosToCompass_COD4 = (BOOL(__cdecl*)(DWORD, pcg_s_COD4, float, float, Vector2&, Vector3&, Vector3&, Vector2&, Vector2&))getCryptedCoD(CheatPatches->CoDPatches.CG_WorldPosToCompass);//TU18
	CG_CompassCalcDimensions_COD4 = (BOOL(__cdecl*)(DWORD, pcg_s_COD4, Vector2&, unkRadarStruct*, float*, float*, float*, float*))getCryptedCoD(CheatPatches->CoDPatches.CG_CompassCalcDimensions);//tu18
	CG_DrawRotatedPic_COD4 = (VOID(__cdecl*)(DWORD, float, float, float, float, int, int, float, Vector4&, DWORD))getCryptedCoD(CheatPatches->CoDPatches.CG_DrawRotatedPic); //tu18
	CG_CompassUpYawVector_COD4 = (VOID(__cdecl*)(pcg_s_COD4, Vector2&))getCryptedCoD(CheatPatches->CoDPatches.CG_CompassUpYawVector);//tu18
	AngleNormalize360_COD4 = (float(__cdecl*)(float))getCryptedCoD(CheatPatches->CoDPatches.AngleNormalize360);//tu18

	// AutoWall Functions
	FireBulletPenetrate_COD4 = (VOID(__cdecl*)(DWORD, BulletFireParams*, DWORD, pcentity_s_COD4, Vector3&, bool))getCryptedCoD(CheatPatches->CoDPatches.FireBulletPenetrate);//TU4
}

void InitializeCOD4Hooks()
{
	SetupCoD4Offsets();

	Menu_PaintAllDetour = new Detour<void>;
	XInputGetStateDetour = new Detour<DWORD>;
	CL_ReadyToSendPacketDetour = new Detour<int>;
	PlayLocalSoundAliasByNameDetour = new Detour<int>;
	R_EndFrameDetour = new Detour<void>;
	//NetDll_XNetGetTitleXnAddrDetour = new Detour<DWORD>;
	CG_EntityEventDetour = new Detour<void>;
	R_DrawXModelSkinnedCachedDetour = new Detour<void>;
	DrawIndexedPrimitiveDetour = new Detour<HRESULT>;
	R_DrawIndexedPrimitiveDetour = new Detour<HRESULT>;
	EngineDrawRadarDetour = new Detour<void>;
	UI_DrawTextDetour = new Detour<void>;
	RB_SetTessTechniqueDetour = new Detour<void>;
	LiveStorage_SetStatDetour = new Detour<void>;

	// Load our menu options from saved ini file
	LoadMenuOptions();
	Sleep(100);
	//SaveMenuOptions();

	Menu_PaintAllDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.Menu_PaintAll), (PVOID)Menu_PaintAllHook);
	XInputGetStateDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.XInputGetState), (PVOID)XInputGetStateHook);
	CL_ReadyToSendPacketDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CL_ReadyToSendPacket), (PVOID)CL_ReadyToSendPacketHook);
	PlayLocalSoundAliasByNameDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.PlayLocalSoundAliasByName), (PVOID)PlayLocalSoundAliasByNameHook);
	R_EndFrameDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.R_EndFrame), (PVOID)R_EndFrameHook);
	CG_EntityEventDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CG_EntityEvent), (PVOID)CG_EntityEventHook);
	RB_SetTessTechniqueDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.RB_SetTessTechnique), (PVOID)RB_SetTessTechniqueHook);
	LiveStorage_SetStatDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.LiveStorage_SetStat), (PVOID)LiveStorage_SetStatHook);
	EngineDrawRadarDetour->SetupDetour(getCryptedCoD(CheatPatches->CoDPatches.CG_CompassDrawPlayerMap), (PVOID)EngineDrawRadarHook);
	//R_DrawIndexedPrimitiveDetour->SetupDetour(0x82135140, (PVOID)DrawIndexedPrimitive);

	// Patch AimTarget_GetTagPos
	*(short*)0x82346528 = 0x4800;
	*(short*)0x82348DB8 = 0x4800;

	//SV_SetClientStat_COD4(0, 2301, 0);
}

void UninitializeCOD4Cheater()
{
	Menu_PaintAllDetour->TakeDownDetour();
	XInputGetStateDetour->TakeDownDetour();
	CL_ReadyToSendPacketDetour->TakeDownDetour();
	PlayLocalSoundAliasByNameDetour->TakeDownDetour();
	R_EndFrameDetour->TakeDownDetour();
	//NetDll_XNetGetTitleXnAddrDetour->TakeDownDetour();
	CG_EntityEventDetour->TakeDownDetour();
	R_DrawXModelSkinnedCachedDetour->TakeDownDetour();
	DrawIndexedPrimitiveDetour->TakeDownDetour();
	R_DrawIndexedPrimitiveDetour->TakeDownDetour();
	EngineDrawRadarDetour->TakeDownDetour();
	UI_DrawTextDetour->TakeDownDetour();
	RB_SetTessTechniqueDetour->TakeDownDetour();
	LiveStorage_SetStatDetour->TakeDownDetour();

	Sleep(1000);

	delete Menu_PaintAllDetour;
	delete XInputGetStateDetour;
	delete CL_ReadyToSendPacketDetour;
	delete PlayLocalSoundAliasByNameDetour;
	delete R_EndFrameDetour;
	//delete NetDll_XNetGetTitleXnAddrDetour;
	delete CG_EntityEventDetour;
	delete R_DrawXModelSkinnedCachedDetour;
	delete DrawIndexedPrimitiveDetour;
	delete R_DrawIndexedPrimitiveDetour;
	delete EngineDrawRadarDetour;
	delete UI_DrawTextDetour;
	delete RB_SetTessTechniqueDetour;
	delete LiveStorage_SetStatDetour;
}