#include "stdafx.h"
#include "Utilities.h"
#include "Config.h"
#include <time.h>
#include "Globals.h"
#include "AW.h"
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

PCHAR MenuNames[10];
PCHAR SubMenuNames[18][MenuOptions];

pclientActive_t_AW LocalClient_AW;
pcentity_s_AW Entities_AW;
pcgs_t_AW cgs_AW;
pcg_s_AW cg_AW;
prefdef_s_AW refdef_AW;

ClientInformation_AW PlayerInfo[18];

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
Detour<VOID> *DrawMenuTextDetour;
Detour<DWORD> *RenderObjectColorDetour;
Detour<void> *LiveStats_WriteStatDetour;
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
int BodyFontBold;
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

float startX = 0.0f;
float endX = 0.0f;
float myStartX = 66.0f;
float myEndX = 268.0f;
float currentX = 0.0f;
BOOL goingForward = TRUE;

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

CONST PCHAR GetMenuOptionString(CONST PCHAR SubMenu, CONST PCHAR Name)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(SubMenu, MenuNames[i]))
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

int GetMenuOptionValue(CONST PCHAR SubMenu, CONST PCHAR Name)
{
	if (!menuReady) return 0;
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(SubMenu, MenuNames[i]))
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
		if (!strcmp(Name, SubMenuNames[i][Tabindex]))
		{
			return OptionEnabled[i][Tabindex];
		}
	}
	return 0;
}

void SetMenuOptionValue(CONST PCHAR SubMenu, CONST PCHAR Name, DWORD ValueToSet)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(SubMenu, MenuNames[i]))
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
		if (!strcmp(Name, SubMenuNames[i][Tabindex]))
		{
			OptionEnabled[i][Tabindex] = ValueToSet;
		}
	}
	return;
}

void SetMenuOptionString(CONST PCHAR SubMenu, CONST PCHAR Name, DWORD ArrayNumber)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(SubMenu, MenuNames[i]))
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
		if (!strcmp(Name, SubMenuNames[i][Tabindex]))
		{
			if (!strcmp(Name, SubMenuNames[i][Tabindex]) && (OptionValues[i][Tabindex] != NULL))
			{
				OptionArrayCurrent[i][Tabindex] = ArrayNumber;
			}
		}
	}
	return;
}

void AddMenuOption(CONST PCHAR SubMenu, CONST PCHAR Name, int index, BOOL defaultValue = FALSE, CONST PCHAR defaultString = "false", char** Array = NULL, int ArraySize = NULL, int ArrayCur = 0)
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp(SubMenu, MenuNames[i]))
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
	SubMenuNames[index][Tabindex] = Name;
	SubMenuOptions[Tabindex] += 1;

}

int GetNetInfoAddress(int index)
{
	return (0x82B7E640 + (index * 0x350));
}

PCHAR GetNameFromNetInfo(int index)
{
	return (char*)(GetNetInfoAddress(index));
}

BYTE* GetIPFromNetInfo(int index)
{
	return (BYTE*)(GetNetInfoAddress(index) + 0x29C); 
}

QWORD GetXUIDFromNetInfo(int index)
{
	return (GetNetInfoAddress(index) + 0x2E8);
}

QWORD GetMachineIDFromNetInfo(int index)
{
	return _byteswap_uint64(*(QWORD*)(GetNetInfoAddress(index) + 0x2B0)); 
}

void UpdateClientsNames()
{
	int Tabindex = -1;
	for (int i = 0; i < MenuOptions; i++)
	{
		if (!strcmp("Clients", MenuNames[i]))
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
				SubMenuNames[iClients][Tabindex] = "Searching...";
		}
		else
			SubMenuNames[iClients][Tabindex] = "Searching...";
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

void SetupMenu()
{
	MenuNames[0] = "Main";
	MenuNames[1] = "Ragebot";
	MenuNames[2] = "Legitbot";
	MenuNames[3] = "Spinbot";
	MenuNames[4] = "Drawing";
	MenuNames[5] = "Clients";
	MenuNames[6] = "Settings";

	AddMenuOption("Main", "Radar", 0);
	AddMenuOption("Main", "No Recoil", 1);
	AddMenuOption("Main", "No Sway", 2);
	//AddMenuOption("Main", "Change Team?", 3);
	//AddMenuOption("Main", "Change Class", 4);
	AddMenuOption("Main", "Steal GT's", 3);
	AddMenuOption("Main", "Rainbow GT", 4);
	AddMenuOption("Main", "Spoof GT?", 5);
	AddMenuOption("Main", "Show Host", 6);
	AddMenuOption("Main", "Show Ping", 7);
	AddMenuOption("Main", "Fast Reload", 8);
	AddMenuOption("Main", "Everyone Hears Me", 9);
	AddMenuOption("Main", "Never Host", 10);
	AddMenuOption("Main", "Field Of View:", 11, 0, "Normal", FOVS, FOVS_Size, FOVSizes_Start);
	AddMenuOption("Main", "Third Person", 12);
	AddMenuOption("Main", "Fake Lag Amount:", 13, 0, "Off", FakeLagTypes, FakeLagTypes_Size, FakeLagTypes_Start);
	//AddMenuOption("Main", "Jugg Class?", 14);
	AddMenuOption("Main", "End Game?", 14);
	AddMenuOption("Main", "Leave Game?", 15);


	AddMenuOption("Ragebot", "Enable Ragebot", 0);
	AddMenuOption("Ragebot", "Ragebot Type:", 1, 0, "Silent", RagebotTypes, RagebotTypes_Size, RagebotTypes_Start);
	//AddMenuOption("Ragebot", "Ragebot Logic:", 2, 0, "Distance", RagebotLogics, RagebotLogics_Size, RagebotLogics_Start);
	AddMenuOption("Ragebot", "Aim Bone:", 2, 0, "Best", AimBones, AimBones_Size, AimBones_Start);
	AddMenuOption("Ragebot", "Prediction Scale:", 3, 0, "Off", PredictionScales, PredictionScales_Size, PredictionScales_Start);
	AddMenuOption("Ragebot", "No Spread", 4);
	AddMenuOption("Ragebot", "No Flinch", 5);
	AddMenuOption("Ragebot", "Auto Wall", 6);
	AddMenuOption("Ragebot", "Auto Shoot", 7);
	AddMenuOption("Ragebot", "Auto Aim", 8);
	AddMenuOption("Ragebot", "Rapid Fire", 9);
	AddMenuOption("Ragebot", "Target Shields", 10);
	//AddMenuOption("Ragebot", "Anti-Blacklist", 11);
	AddMenuOption("Ragebot", "Preferred Settings", 11);


	AddMenuOption("Legitbot", "Enable Legitbot", 0);
	//AddMenuOption("Legitbot", "Legitbot Logic:", 1, 0, "Distance");
	AddMenuOption("Legitbot", "Aim Bone:", 1, 0, "j_head", AimBonesLegit, AimBonesLegit_Size, AimBonesLegit_Start);
	AddMenuOption("Legitbot", "Aim Button:", 2, 0, "Off", AimKeys, AimKeys_Size, AimKeys_Start);
	AddMenuOption("Legitbot", "Auto Shoot", 3);
	AddMenuOption("Legitbot", "Auto Aim", 4);
	AddMenuOption("Legitbot", "Reaction Delay:", 5, 0, "Off", ReactionDelays, ReactionDelays_Size, ReactionDelays_Start);
	AddMenuOption("Legitbot", "FOV Size:", 6, 0, "80.0", FOVSizes, FOVSizes_Size, FOVSizes_Start);
	AddMenuOption("Legitbot", "Draw FOV Box", 7);
	AddMenuOption("Legitbot", "Smooth Scale:", 8, 0, "10.0", SmoothScales, SmoothScales_Size, SmoothScales_Start);
	AddMenuOption("Legitbot", "Triggerbot", 9);
	AddMenuOption("Legitbot", "Rapid Fire", 10);
	AddMenuOption("Legitbot", "Target Shields", 11);
	AddMenuOption("Legitbot", "Preferred Settings", 12);


	AddMenuOption("Spinbot", "Enable Spinbot", 0);
	AddMenuOption("Spinbot", "X Spin Direction:", 1, 0, "Off", XSpinDirects, XSpinDirects_Size, XSpinDirects_Start);
	AddMenuOption("Spinbot", "Y Spin Direction:", 2, 0, "Off", YSpinDirects, YSpinDirects_Size, YSpinDirects_Start);
	AddMenuOption("Spinbot", "Y Spin Speed:", 3, 0, "10.0", YSpinSpeeds, YSpinSpeeds_Size, YSpinSpeeds_Start);
	AddMenuOption("Spinbot", "Z Spin Direction:", 4, 0, "Off", ZSpinDirects, ZSpinDirects_Size, ZSpinDirects_Start);
	AddMenuOption("Spinbot", "Z Spin Speed:", 5, 0, "10.0", ZSpinSpeeds, ZSpinSpeeds_Size, ZSpinSpeeds_Start);
	//AddMenuOption("Spinbot", "Funny Spinbots:", 6, 0, "Off", FunnySpinbots, FunnySpinbots_Size, FunnySpinbots_Start);
	AddMenuOption("Spinbot", "X Anti-Aim Spinbot:", 6, 0, "Off", XAASpinbots, XAASpinbots_Size, XAASpinbots_Start);
	AddMenuOption("Spinbot", "Y Anti-Aim Spinbot:", 7, 0, "Off", YAASpinbots, YAASpinbots_Size, YAASpinbots_Start);
	AddMenuOption("Spinbot", "Preferred Settings", 8);


	AddMenuOption("Drawing", "Draw Enemies", 0);
	AddMenuOption("Drawing", "Draw Friendlies", 1);
	AddMenuOption("Drawing", "ESP Box Type:", 2, 0, "Off", ESPBoxes, ESPBoxes_Size, ESPBoxes_Start);
	//AddMenuOption("Drawing", "Chams:", 3, 0, "Off", ChamTypes, ChamTypes_Size, ChamTypes_Start);
	AddMenuOption("Drawing", "Draw Name", 3);
	AddMenuOption("Drawing", "Draw Weapon", 4);
	AddMenuOption("Drawing", "Draw Bones", 5);
	AddMenuOption("Drawing", "Draw Green Bodies", 6);
	AddMenuOption("Drawing", "Draw Explosives", 7);
	AddMenuOption("Drawing", "Draw Snaplines:", 8, 0, "Off", SnaplinesTypes, SnaplinesTypes_Size, SnaplinesTypes_Start);
	//AddMenuOption("Drawing", "Draw Killstreaks", 9);
	AddMenuOption("Drawing", "Directional Arrows", 9);
	AddMenuOption("Drawing", "Crosshair Type:", 10, 0, "Off", CrosshairTypes, CrosshairTypes_Size, CrosshairTypess_Start);
	AddMenuOption("Drawing", "Draw Custom Radar", 11);
	AddMenuOption("Drawing", "Draw Dropped Items", 12);
	AddMenuOption("Drawing", "Draw Enemy Visible Text", 13);
	AddMenuOption("Drawing", "Draw Proximity Warning", 14);
	AddMenuOption("Drawing", "Draw Through Walls", 15);
	AddMenuOption("Drawing", "Cartoon Mode", 16);


	AddMenuOption("Clients", "Searching...", 0);
	AddMenuOption("Clients", "Searching...", 1);
	AddMenuOption("Clients", "Searching...", 2);
	AddMenuOption("Clients", "Searching...", 3);
	AddMenuOption("Clients", "Searching...", 4);
	AddMenuOption("Clients", "Searching...", 5);
	AddMenuOption("Clients", "Searching...", 6);
	AddMenuOption("Clients", "Searching...", 7);
	AddMenuOption("Clients", "Searching...", 8);
	AddMenuOption("Clients", "Searching...", 9);
	AddMenuOption("Clients", "Searching...", 10);
	AddMenuOption("Clients", "Searching...", 11);
	AddMenuOption("Clients", "Searching...", 12);
	AddMenuOption("Clients", "Searching...", 13);
	AddMenuOption("Clients", "Searching...", 14);
	AddMenuOption("Clients", "Searching...", 15);
	AddMenuOption("Clients", "Searching...", 16);
	AddMenuOption("Clients", "Searching...", 17);


	//AddMenuOption("Settings", "Move Menu?", 0);
	//AddMenuOption("Settings", "Menu Open:", 1, 0, "\x0016 & \x0012", MenuControls, MenuControls_Size, MenuControls_Start);
	AddMenuOption("Settings", "Disable Menu Sounds", 0);
	AddMenuOption("Settings", "Disable Menu Instructions", 1);
	AddMenuOption("Settings", "Disable NiNJA Engine Text", 2);
	//AddMenuOption("Settings", "Disable Item Descriptions", 5);
	//AddMenuOption("Settings", "Disable Main Menu Music", 3);
	AddMenuOption("Settings", "Prestige Disco", 3);
	AddMenuOption("Settings", "Prestige:", 4, 0, "N/A", Prestiges, Prestiges_Size, Prestiges_Start);
	AddMenuOption("Settings", "Level 50?", 5);
	//AddMenuOption("Settings", "1,000 Squad Points?", 6);
	AddMenuOption("Settings", "Unlock All?", 6);
	AddMenuOption("Settings", "Anti Derank", 7);
	//AddMenuOption("Settings", "Set Legit Stats?", 11);
	AddMenuOption("Settings", "Invert Team Check", 8);
	AddMenuOption("Settings", "Log IP's", 9);
	//AddMenuOption("Settings", "Freeze All Consoles?", 9);

	HudSmallFont = DB_FindXAssetHeader_AW(font, "fonts/hudsmallfont", 1);
	BigDevFont = DB_FindXAssetHeader_AW(font, "fonts/bigDevFont", 1);
	SmallFont = DB_FindXAssetHeader_AW(font, "fonts/smallfont", 1);
	BodyFontBold = DB_FindXAssetHeader_AW(font, "fonts/bodyFontBold", 1);
	WhiteMaterial = DB_FindXAssetHeader_AW(material, "white", 1);
	ScavMaterial = DB_FindXAssetHeader_AW(material, "scavenger_pickup", 1);
	CompassPingMaterial = DB_FindXAssetHeader_AW(material, "compassping_player", 1);

	memcpy(OriginalGT, (PVOID)(0x843DECB0 + 4), 0x1E);
	memcpy(OriginalXUID, (PVOID)(0x843DECB0 + 0x30), 0xE);

	menuReady = TRUE;
}

void PlayMenuSound(char* SoundName)
{
	if (GetMenuOptionValue("Settings", "Disable Menu Sounds"))
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

		PlayMenuSound("ui_mouse_over");
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

		PlayMenuSound("ui_mouse_over");
	}
	if (!(currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) && DPads[1] == TRUE)
	{
		DPads[1] = FALSE;
	}

	if ((currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) && (currentState.Gamepad.bLeftTrigger) && menuWait == FALSE)
	{
		menuWait = TRUE;
		menuOpen ^= 1;
		PlayMenuSound("ui_mouse_over");
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

		PlayMenuSound("ui_enter_screen");

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

		PlayMenuSound("ui_enter_screen");

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

void DrawBox_AW(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color)
{
	R_AddCmdDrawStretchPic_AW(X, Y, W, H, 4, 4, 1, 1, Color, WhiteMaterial);
}

void DrawGradient_AW(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color1, Vector4& Color2, bool horizontal)
{
	float fAdd = (horizontal ? 0 : W / 2 - H / 2);
	R_AddCmdDrawStretchPic_AW(X, Y, W, H, 4, 4, 1, 1, Color2, WhiteMaterial);
	R_AddCmdDrawStretchPic_AW(X + fAdd, Y - fAdd, (horizontal ? W : H), (horizontal ? H : W), 4, 4, 1, 1, Color1, GradientFadeIn);
}

void DrawText_AW(CONST PCHAR Text, FLOAT X, FLOAT Y, DWORD index, FLOAT addy, DWORD Font, Vector4& Color, float fontXSize = 0.9, float fontYSize = 0.9)
{
	CL_DrawTextPhysical_AW(Text, 0x7FFFFFFF, Font, X, Y + (index * addy), fontXSize, fontYSize, 0, Color, 0);
}

bool WorldPosToScreenPos(float* In, Vector2* Out)
{
	Vector3 Position, Transform;

	VectorSubtract(In, cg_AW->refdef.ViewOrigin, Position);

	Transform.x = DotProduct(Position, cg_AW->refdef.ViewAxis[1]);
	Transform.y = DotProduct(Position, cg_AW->refdef.ViewAxis[2]);
	Transform.z = DotProduct(Position, cg_AW->refdef.ViewAxis[0]);

	if (Transform.z < 0.1f)
		return false;

	Vector2 Center; Vector2 OutPut;
	Center.x = cg_AW->refdef.Width * 0.5f; Center.y = cg_AW->refdef.Height * 0.5f;
	OutPut.x = Center.x * (1 - (Transform.x / cg_AW->refdef.Fov.x / Transform.z)); OutPut.y = Center.y * (1 - (Transform.y / cg_AW->refdef.Fov.y / Transform.z));
	*Out = OutPut;
	return true;
}

void DrawLine_AW(FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, FLOAT Width, Vector4& Color)
{
	FLOAT X, Y, Angle, L1, L2, H1;
	H1 = Y2 - Y1;
	L1 = X2 - X1;
	L2 = sqrt(L1 * L1 + H1 * H1);
	X = X1 + ((L1 - L2) / 2);
	Y = Y1 + (H1 / 2);
	Angle = (FLOAT)atan(H1 / L1) * (180 / 3.14159265358979323846);
	CG_DrawRotatedPicPhysical_AW(0x82B94570, X, Y, L2, Width, Angle, Color, WhiteMaterial);
}

void MakeLines(Vector3 Origin, FLOAT X1, FLOAT Y1, FLOAT Z1, FLOAT X2, FLOAT Y2, FLOAT Z2, Vector4& Color, int Size = 1)
{
	Vector2 Screen1, Screen2;
	Vector3 Origin1(Origin.x + X1, Origin.y + Y1, (Origin.z + Z1) - 4);
	Vector3 Origin2(Origin.x + X2, Origin.y + Y2, (Origin.z + Z2) - 4);
	if (WorldPosToScreenPos((float*)&Origin1, &Screen1) && WorldPosToScreenPos((float*)&Origin2, &Screen2)) {
		DrawLine_AW(Screen1.x, Screen1.y, Screen2.x, Screen2.y, Size, Color);
	}
}

void DrawCheckBox(FLOAT X, FLOAT Y, BOOL Checked = FALSE)
{
	Vector4 Color;
	if (Checked)
		Color = Vector4(0, 1, 0, 1);
	else
		Color = Vector4(1, 0, 0, 1);
	DrawLine_AW(X - 7.5, Y - 7.5, X + 7.5, Y - 7.5, 2, Color);//Top
	DrawLine_AW(X - 7.5, Y - 7.5, X - 7.5, Y + 7.5, 2, Color);//Left Side
	DrawLine_AW(X + 7.5, Y - 7.5, X + 7.5, Y + 7.5, 2, Color);//Right Side
	DrawLine_AW(X - 7.5, Y + 7.5, X + 7.5, Y + 7.5, 2, Color);//Bottom

	if (Checked)
	{
		DrawLine_AW(X - 7.5, Y - 7.5, X + 7.5, Y + 7.5, 1.5, Color);//Check
		DrawLine_AW(X - 7.5, Y + 7.5, X + 7.5, Y - 7.5, 1.5, Color);
	}
}

void DrawMenu_AW()
{
	FLOAT X, Y;
	X = *(DWORD*)0x82B0E8E8;//cgs_AW->Width;
	Y = *(DWORD*)0x82B0E8EC;//cgs_AW->Height;

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
	DrawBox_AW(X * 0.331, Y * 0.25, menuWidth, menuHeight, Vector4(0, 0, 0, 0.420));
	DrawLine_AW(X * 0.331, Y * 0.25, (X * 0.331) + menuWidth, Y * 0.25, 3.5, Vector4(1, 0, 0, 1));
	DrawLine_AW(X * 0.331, Y * 0.25 + menuHeight, (X * 0.331) + menuWidth, (Y * 0.25) + menuHeight, 3.5, Vector4(1, 0, 0, 1));
	DrawLine_AW((X * 0.331) + 115, (Y * 0.25), (X * 0.331) + 115, (Y * 0.25) + menuHeight, 3.5, Vector4(1, 0, 0, 1));

	// Wait for the animation to be done
	if (menuWidth == 420 && menuHeight == 400)
	{
		//Draw Menu Names
		for (int i = 0; i < MenuOptions; i++)
		{
			DrawText_AW(MenuNames[i], X * 0.3328, Y * 0.2841, i, 22, HudSmallFont, (scroll == i) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25)); // Y * 0.2861
			DrawLine_AW(X * 0.331, (Y * 0.28) + (i * 22), (X * 0.331) + 115, (Y * 0.28) + (22 * i), 3.5, Vector4(1, 0, 0, 1));
		}

		//Draw SubOptions
		for (int i = 0; i < SubMenuOptions[scroll]; i++)
		{
			if (subMenu == FALSE)
			{
				DrawText_AW(SubMenuNames[i][scroll], X * 0.425, Y * 0.2861, i, 22, HudSmallFont, Vector4(1, 1, 1, 0.25));
			}
			else if (subMenu == TRUE && clientMenu == FALSE)
			{
				DrawText_AW(SubMenuNames[i][scroll], X * 0.425, Y * 0.2861, i, 22, HudSmallFont, (subScroll == i) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
			}
			else if (subMenu == TRUE && clientMenu == TRUE && i == 0)
			{
				int Tabindex = -1;
				for (int i = 0; i < MenuOptions; i++)
				{
					if (!strcmp("Clients", MenuNames[i]))
					{
						Tabindex = i;
						break;
					}
				}

				DrawText_AW(SubMenuNames[selectedClient][Tabindex], X * 0.425, (Y * 0.2861) + (0 * 22), 0, 0, HudSmallFont, (subMenu == TRUE) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				DrawText_AW("Prioritize", X * 0.425, (Y * 0.2861), 1, 22, HudSmallFont, (subMenu == TRUE && subScroll == 1) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (1 * 22), ClientOptionEnabled[1][selectedClient]);

				DrawText_AW("Whitelist", X * 0.425, (Y * 0.2861), 2, 22, HudSmallFont, (subMenu == TRUE && subScroll == 2) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (2 * 22), ClientOptionEnabled[2][selectedClient]);

				DrawText_AW("Invert Team", X * 0.425, (Y * 0.2861), 3, 22, HudSmallFont, (subMenu == TRUE && subScroll == 3) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (3 * 22), ClientOptionEnabled[3][selectedClient]);


				DrawText_AW("Set GT To Player's IP?", X * 0.425, (Y * 0.2861), 4, 22, HudSmallFont, (subMenu == TRUE && subScroll == 4) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));

				DrawText_AW("Player Has Anti-Aim", X * 0.425, (Y * 0.2861), 5, 22, HudSmallFont, (subMenu == TRUE && subScroll == 5) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (5 * 22), ClientOptionEnabled[5][selectedClient]);

				BYTE* IP = GetIPFromNetInfo(selectedClient);
				DrawText_AW(va_AW("IP: %i.%i.%i.%i", IP[0], IP[1], IP[2], IP[3]), X * 0.425, (Y * 0.2861), 6, 22, HudSmallFont, (subMenu == TRUE && subScroll == 6) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				QWORD MachineID = GetMachineIDFromNetInfo(selectedClient);
				DWORD MachineIDPT1 = *(DWORD*)(DWORD)&MachineID;
				DWORD MachineIDPT2 = *(DWORD*)((DWORD)&MachineID + 0x04);
				DrawText_AW(va_AW("Machine ID: %08X%08X", MachineIDPT1, MachineIDPT2), X * 0.425, Y * 0.2861, 7, 22, HudSmallFont, (subMenu == TRUE && subScroll == 7) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				BYTE* XUID = (BYTE*)GetXUIDFromNetInfo(selectedClient);

				DrawText_AW(va_AW("XUID: %08X%08X", *(DWORD*)(DWORD)&XUID[0], *(DWORD*)(DWORD)&XUID[4]), X * 0.425, Y * 0.2861, 8, 22, HudSmallFont, (subMenu == TRUE && subScroll == 8) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));


			}

			if (clientMenu)
				continue;

			if (strstr(SubMenuNames[i][scroll], "?"))
				continue;

			if (!strstr(SubMenuNames[i][scroll], ":") && scroll != 5)
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (i * 22), OptionEnabled[i][scroll]);
			else if (strstr(SubMenuNames[i][scroll], ":"))
				DrawText_AW(GetMenuOptionString(MenuNames[scroll], SubMenuNames[i][scroll]), X * 0.58, (Y * 0.2841) + (i * 22), 0, 0, HudSmallFont, (subMenu == TRUE && subScroll == i) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
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
	X = (FLOAT)cg_AW->refdef.Width * 0.78125;//*(FLOAT*)((DWORD)freememory);
	Y = (FLOAT)cg_AW->refdef.Height * 0.13888888888;//*(FLOAT*)((DWORD)freememory + 4);
	//X2 = *(FLOAT*)((DWORD)freememory + 8);
	//Y2 = *(FLOAT*)((DWORD)freememory + 12);
	RotateXY(cos(ToRadians(rotation)), sin(ToRadians(rotation)), X + (FLOAT)((FLOAT)R_TextWidth_AW(Text, 0x7FFFFFFF, BigDevFont) / 2), Y, X, Y, &X, &Y);

	rotation += 0.25;
	if (rotation >= 360.0)
		rotation -= 360.0;



	CL_DrawTextPhysical_AW(Text, 0x7FFFFFFF, BigDevFont, X, Y, 1, 1, rotation, Vector4(1, 0, 0, 0.50), 0);
}

short SL_GetString(char* String)
{
	return ((short(*)(...))0x8242FD70)(String, 0, strlen(String) + 1, 8);
}

BOOL ShouldUpdateTagCache()
{
	if (Dvar_GetBool_AW("cl_ingame") && !LobbyInit)
	{
		LobbyInit = TRUE;
		return TRUE;
	}
	else if (!Dvar_GetBool_AW("cl_ingame") && LobbyInit)
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
			BoneIndexs[i] = SL_GetString(BoneCache[i]);
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

int Com_GetClientDObj(short handle, int localClientNum)
{
	int DObj_s = *(short *)((0x83B09C2C - 0x340C) +
		((handle + (localClientNum * 0x602)) << 1));

	if (!DObj_s)
		return 0;

	return *(int *)0x83B09C2C + (DObj_s * 0x160);
}

bool GetTagPosistion(pcentity_s_AW centity_s, int ClientIndex, PCHAR SelectedTag, float* OutPos)
{
	//char Temp[0x100];
	short TagIndex;
	int Dobj;

	//if (ClientIndex < 18)
	//	return false;

	//for (int i = 0; i <= strlen(SelectedTag); i++)
	//	Temp[i] = tolower(SelectedTag[i]);

	TagIndex = GetTagIndex(SelectedTag, ShouldUpdateTagCache()); //SL_GetString(Temp);
	Dobj = Com_GetClientDObj(ClientIndex, 0);

	if (Dobj == 0)
		return false;

	return ((bool(*)(...))0x82623E10)(centity_s, Dobj, TagIndex, OutPos);
}

short TagToShort(PCHAR Tag)
{
	char Temp[0x100];

	for (int i = 0; i <= strlen(Tag); i++)
		Temp[i] = tolower(Tag[i]);

	return SL_GetString(Temp);
}

void DropTarget(ClientInformation_AW PlayerInfo)
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
	if (Dvar_GetBool_AW("cl_ingame") && (cg_AW != NULL))
	{
		if (strcmp(cg_AW->playerNames2[RandomClient].PlayerName, "") && (RandomClient != cg_AW->ClientNumber) && (cg_AW->playerNames2[RandomClient].PlayerName != NULL))
		{
			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", cg_AW->playerNames2[RandomClient].PlayerName, 0x843DECB0 + 0x30);
			Cbuf_AddText_AW(0, Output);
		}
	}
	else
	{
		if (strcmp(GetNameFromNetInfo(RandomClient), ""))
		{
			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", GetNameFromNetInfo(RandomClient), 0x843DECB0 + 0x30);
			Cbuf_AddText_AW(0, Output);
			strcpy_s((char*)(0x843DECB0 + 4), 0x1F, GetNameFromNetInfo(RandomClient));
		}
	}
}

void RainbowName()
{
	char Output[0x200], Color[10], PreGameOutput[0x50];
	int RandomColor = rand() % 9;
	sprintf(Color, "^%i", RandomColor);

	if (Dvar_GetBool_AW("cl_ingame"))
	{
		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", OriginalGT, Color, 0x843DECB0 + 0x30);
		Cbuf_AddText_AW(0, Output);
	}
	else
	{
		sprintf(PreGameOutput, "^%i%s", RandomColor, OriginalGT);
		strcpy_s((char*)(0x843DECB0 + 4), 0x1F, PreGameOutput);
	}
}

void PrestigeDisco()
{
	int RandomPrestige = rand() % 11;
	if (!Dvar_GetBool_AW("cl_ingame"))
	{
		*(BYTE*)0x83A6C151 = RandomPrestige;
	}
	else
	{
		//sprintf(Output, "userinfo \"\\name\\%s\\xuid\\%s\\natType\\1\rank\\%i\\prestige\\%i\"", OriginalGT, 0x83F0A358 + 0x30, RandomPrestige, RandomPrestige);
		//Cbuf_AddText_AW(0, Output);
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

		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", OriginalGT, 0x843DECB0 + 0x30);
		Cbuf_AddText_AW(0, Output);
		strcpy_s((char*)(0x843DECB0 + 4), 0x1F, OriginalGT);
	}

	ExitThread(ERROR_SUCCESS);
}

int FindFOVSize()
{
	int FOVSize = 80;
	if (GetMenuOptionString("Legitbot", "FOV Size:") == "80.0")
		FOVSize = 80;

	if (GetMenuOptionString("Legitbot", "FOV Size:") == "90.0")
		FOVSize = 90;

	if (GetMenuOptionString("Legitbot", "FOV Size:") == "100.0")
		FOVSize = 100;

	if (GetMenuOptionString("Legitbot", "FOV Size:") == "120.0")
		FOVSize = 120;

	if (GetMenuOptionString("Legitbot", "FOV Size:") == "140.0")
		FOVSize = 140;

	if (GetMenuOptionString("Legitbot", "FOV Size:") == "180.0")
		FOVSize = 180;

	if (GetMenuOptionString("Legitbot", "FOV Size:") == "220.0")
		FOVSize = 220;

	return FOVSize;
}

int FindFieldOfViewSize()
{
	int FieldOfViewSize = 65;
	if (GetMenuOptionString("Main", "Field Of View:") == "Normal")
		FieldOfViewSize = 65;

	if (GetMenuOptionString("Main", "Field Of View:") == "70.0")
		FieldOfViewSize = 70;

	if (GetMenuOptionString("Main", "Field Of View:") == "75.0")
		FieldOfViewSize = 75;

	if (GetMenuOptionString("Main", "Field Of View:") == "80.0")
		FieldOfViewSize = 80;

	if (GetMenuOptionString("Main", "Field Of View:") == "90.0")
		FieldOfViewSize = 90;

	if (GetMenuOptionString("Main", "Field Of View:") == "100.0")
		FieldOfViewSize = 100;

	if (GetMenuOptionString("Main", "Field Of View:") == "110.0")
		FieldOfViewSize = 110;

	return FieldOfViewSize;
}

int FindYSpinSpeed()
{
	int YSpinSpeed = 10;
	if (GetMenuOptionString("Spinbot", "Y Spin Speed:") == "10.0")
		YSpinSpeed = 10;

	if (GetMenuOptionString("Spinbot", "Y Spin Speed:") == "20.0")
		YSpinSpeed = 20;

	if (GetMenuOptionString("Spinbot", "Y Spin Speed:") == "30.0")
		YSpinSpeed = 30;

	if (GetMenuOptionString("Spinbot", "Y Spin Speed:") == "40.0")
		YSpinSpeed = 40;

	if (GetMenuOptionString("Spinbot", "Y Spin Speed:") == "50.0")
		YSpinSpeed = 50;

	if (GetMenuOptionString("Spinbot", "Y Spin Speed:") == "60.0")
		YSpinSpeed = 60;

	if (GetMenuOptionString("Spinbot", "Y Spin Speed:") == "70.0")
		YSpinSpeed = 70;

	if (GetMenuOptionString("Spinbot", "Y Spin Speed:") == "Random")
		YSpinSpeed = rand() % 71;

	return YSpinSpeed;
}

int FindZSpinSpeed()
{
	int ZSpinSpeed = 10;
	if (GetMenuOptionString("Spinbot", "Z Spin Speed:") == "10.0")
		ZSpinSpeed = 10;

	if (GetMenuOptionString("Spinbot", "Z Spin Speed:") == "20.0")
		ZSpinSpeed = 20;

	if (GetMenuOptionString("Spinbot", "Z Spin Speed:") == "30.0")
		ZSpinSpeed = 30;

	if (GetMenuOptionString("Spinbot", "Z Spin Speed:") == "40.0")
		ZSpinSpeed = 40;

	if (GetMenuOptionString("Spinbot", "Z Spin Speed:") == "50.0")
		ZSpinSpeed = 50;

	if (GetMenuOptionString("Spinbot", "Z Spin Speed:") == "60.0")
		ZSpinSpeed = 60;

	if (GetMenuOptionString("Spinbot", "Z Spin Speed:") == "70.0")
		ZSpinSpeed = 70;

	if (GetMenuOptionString("Spinbot", "Z Spin Speed:") == "Random")
		ZSpinSpeed = rand() % 71;

	return ZSpinSpeed;
}

int FindFakeLagAmount()
{
	int FakeLagAmount = 5;
	if (GetMenuOptionString("Main", "Fake Lag Amount:") == "5.0")
		FakeLagAmount = 5;

	if (GetMenuOptionString("Main", "Fake Lag Amount:") == "10.0")
		FakeLagAmount = 10;

	if (GetMenuOptionString("Main", "Fake Lag Amount:") == "15.0")
		FakeLagAmount = 15;

	if (GetMenuOptionString("Main", "Fake Lag Amount:") == "20.0")
		FakeLagAmount = 20;

	if (GetMenuOptionString("Main", "Fake Lag Amount:") == "25.0")
		FakeLagAmount = 25;

	if (GetMenuOptionString("Main", "Fake Lag Amount:") == "30.0")
		FakeLagAmount = 30;

	if (GetMenuOptionString("Main", "Fake Lag Amount:") == "Random")
		FakeLagAmount = rand() % 25;

	return FakeLagAmount;
}

int FindReactionDelayAmount()
{
	int ReactionDelayAmount = 20;
	if (GetMenuOptionString("Main", "Reaction Delay:") == "2.0")
		ReactionDelayAmount = 20;

	if (GetMenuOptionString("Main", "Reaction Delay:") == "3.0")
		ReactionDelayAmount = 30;

	if (GetMenuOptionString("Main", "Reaction Delay:") == "4.0")
		ReactionDelayAmount = 40;

	if (GetMenuOptionString("Main", "Reaction Delay:") == "5.0")
		ReactionDelayAmount = 50;

	if (GetMenuOptionString("Main", "Reaction Delay:") == "6.0")
		ReactionDelayAmount = 60;

	if (GetMenuOptionString("Main", "Reaction Delay:") == "7.0")
		ReactionDelayAmount = 70;

	if (GetMenuOptionString("Main", "Reaction Delay:") == "Random")
		ReactionDelayAmount = rand() % 80;

	return ReactionDelayAmount;
}

BYTE FindPrestigeValue()
{
	BYTE Prestige = *(BYTE*)0x83A6C151;

	if (GetMenuOptionString("Settings", "Prestige:") == "N/A")
		Prestige = 0;

	if (GetMenuOptionString("Settings", "Prestige:") == "0th")
		Prestige = 0;

	if (GetMenuOptionString("Settings", "Prestige:") == "1st")
		Prestige = 1;

	if (GetMenuOptionString("Settings", "Prestige:") == "2nd")
		Prestige = 2;

	if (GetMenuOptionString("Settings", "Prestige:") == "3rd")
		Prestige = 3;

	if (GetMenuOptionString("Settings", "Prestige:") == "4th")
		Prestige = 4;

	if (GetMenuOptionString("Settings", "Prestige:") == "5th")
		Prestige = 5;

	if (GetMenuOptionString("Settings", "Prestige:") == "6th")
		Prestige = 6;

	if (GetMenuOptionString("Settings", "Prestige:") == "7th")
		Prestige = 7;

	if (GetMenuOptionString("Settings", "Prestige:") == "8th")
		Prestige = 8;

	if (GetMenuOptionString("Settings", "Prestige:") == "9th")
		Prestige = 9;

	if (GetMenuOptionString("Settings", "Prestige:") == "10th")
		Prestige = 10;

	if (GetMenuOptionString("Settings", "Prestige:") == "11th")
		Prestige = 11;

	if (GetMenuOptionString("Settings", "Prestige:") == "12th")
		Prestige = 12;

	if (GetMenuOptionString("Settings", "Prestige:") == "13th")
		Prestige = 13;

	if (GetMenuOptionString("Settings", "Prestige:") == "14th")
		Prestige = 14;

	if (GetMenuOptionString("Settings", "Prestige:") == "15th")
		Prestige = 15;

	if (GetMenuOptionString("Settings", "Prestige:") == "16th")
		Prestige = 16;

	if (GetMenuOptionString("Settings", "Prestige:") == "17th")
		Prestige = 17;

	if (GetMenuOptionString("Settings", "Prestige:") == "18th")
		Prestige = 18;

	if (GetMenuOptionString("Settings", "Prestige:") == "19th")
		Prestige = 19;

	if (GetMenuOptionString("Settings", "Prestige:") == "20th")
		Prestige = 20;

	if (GetMenuOptionString("Settings", "Prestige:") == "21st")
		Prestige = 21;

	if (GetMenuOptionString("Settings", "Prestige:") == "22nd")
		Prestige = 22;

	if (GetMenuOptionString("Settings", "Prestige:") == "23rd")
		Prestige = 23;

	if (GetMenuOptionString("Settings", "Prestige:") == "24th")
		Prestige = 24;

	if (GetMenuOptionString("Settings", "Prestige:") == "25th")
		Prestige = 25;

	if (GetMenuOptionString("Settings", "Prestige:") == "26th")
		Prestige = 26;

	if (GetMenuOptionString("Settings", "Prestige:") == "27th")
		Prestige = 27;

	if (GetMenuOptionString("Settings", "Prestige:") == "28th")
		Prestige = 28;

	if (GetMenuOptionString("Settings", "Prestige:") == "29th")
		Prestige = 29;

	if (GetMenuOptionString("Settings", "Prestige:") == "30th")
		Prestige = 30;

	if (GetMenuOptionString("Settings", "Prestige:") == "31st")
		Prestige = 31;

	return Prestige;
}

int FindPredicationScale()
{
	int PredicationScale = 0;
	if (GetMenuOptionString("Ragebot", "Prediction Scale:") == "2.0")
		PredicationScale = 2;

	if (GetMenuOptionString("Ragebot", "Prediction Scale:") == "5.0")
		PredicationScale = 5;

	if (GetMenuOptionString("Ragebot", "Prediction Scale:") == "10.0")
		PredicationScale = 10;

	if (GetMenuOptionString("Ragebot", "Prediction Scale:") == "15.0")
		PredicationScale = 15;

	if (GetMenuOptionString("Ragebot", "Prediction Scale:") == "20.0")
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

void HandleToggleOptions(pcgs_t_AW cgs_AW)
{
	// Make sure the menu is ready
	if (!menuReady) return;

	// Update Player Names
	UpdateClientsNames();

	// Handle UAV
	if (GetMenuOptionValue("Main", "Radar"))
	{
		*(int*)0x826352A8 = 0x39400001;
	}
	else
	{
		*(int*)0x826352A8 = 0x5519EFFE;
	}

	// Handle No Recoil
	if (GetMenuOptionValue("Main", "No Recoil"))
	{
		*(int*)0x82648CEC = 0x60000000;
		*(int*)0x82648CF0 = 0x60000000;
	}
	else
	{
		*(int*)0x82648CEC = 0x41820020;
		*(int*)0x82648CF0 = 0x895B1869;
	}

	// Handle No Sway
	if (GetMenuOptionValue("Main", "No Sway"))
	{
		*(int*)0x82640418 = 0x60000000;
		*(int*)0x826404D0 = 0x60000000;
	}
	else
	{
		*(int*)0x82640418 = 0x4BFFFC59;
		*(int*)0x826404D0 = 0x4BFFFD39;
	}

	// Handle Show Ping
	if (GetMenuOptionValue("Main", "Show Ping"))
	{
		//*(int*)0x8213A470 = 0x60000000;
	
	}
	else
	{
		//*(int*)0x8213A470 = 0x419A0014;
	}

	// Handle Hear All Players
	BOOL HearEveryoneValue = Dvar_GetBool_AW("cg_everyoneHearsEveryone");
	if (GetMenuOptionValue("Main", "Everyone Hears Me") && (Dvar_GetBool_AW("cl_ingame")))
	{
		if (!HearEveryoneValue)
			Cbuf_AddText_AW(0, "cg_everyoneHearsEveryone 1");
	}
	else if(!GetMenuOptionValue("Main", "Everyone Hears Me") && (Dvar_GetBool_AW("cl_ingame")))
	{
		if (HearEveryoneValue)
			Cbuf_AddText_AW(0, "cg_everyoneHearsEveryone 0");
	}

	// Handle Never Host
	if (GetMenuOptionValue("Main", "Never Host"))
	{
		*(int*)0x843DEBAC = 1000;
	}
	else
	{
		*(int*)0x843DEBAC = 10000000;
	}

	// Handle Show Host
	if (GetMenuOptionValue("Main", "Show Host") && (Dvar_GetBool_AW("cl_ingame")))
	{
		if (cgs_AW == NULL)
			return;

		char* HostName = va_AW("Host: %s", cgs_AW->HostName);
		DrawText_AW(HostName, 100, 25, 0, 0, HudSmallFont, Vector4(1, 1, 1, 1));
	}

	// Handle Custom Field Of View
	*(float*)(*(int*)(0x829AAEB4) + 0xC) = FindFieldOfViewSize();

	// Handle End Game
	if (GetMenuOptionValue("Main", "End Game?") && (Dvar_GetBool_AW("cl_ingame")))
	{
		char* EndGameString;
		EndGameString = va_AW("cmd lui 22 1 %i", *(int*)0x82B4F1D4);

		Cbuf_AddText_AW(0, EndGameString);
		SetMenuOptionValue("Main", "End Game?", 0);
	}

	// Handle Leave Game
	if (GetMenuOptionValue("Main", "Leave Game?"))
	{
		Cbuf_AddText_AW(0, "disconnect");
		SetMenuOptionValue("Main", "Leave Game?", 0);
	}

	// Handle Ragebot Preferred Settings
	if (GetMenuOptionValue("Ragebot", "Preferred Settings"))
	{
		// Enable these
		SetMenuOptionValue("Ragebot", "Enable Ragebot", 1);
		SetMenuOptionValue("Ragebot", "No Spread", 1);
		SetMenuOptionValue("Ragebot", "Auto Wall", 1);
		SetMenuOptionValue("Ragebot", "Auto Shoot", 1);
		SetMenuOptionValue("Ragebot", "No Flinch", 1);
		SetMenuOptionValue("Main", "No Recoil", 1);
		SetMenuOptionValue("Main", "No Sway", 1);
		//SetMenuOptionString("Ragebot", "Ragebot Logic:", 1);

		// Disable These
		SetMenuOptionValue("Legitbot", "Enable Legitbot", 0);
		SetMenuOptionValue("Legitbot", "Preferred Settings", 0);
		SetMenuOptionValue("Ragebot", "Auto Aim", 0);
		SetMenuOptionValue("Ragebot", "Rapid Fire", 0);
		SetMenuOptionValue("Legitbot", "Draw Fov Box", 0);


		// Copy over menu settings before forcing these, and apply those back if this get disabled
	}

	// Handle Legitbot Preferred Settings
	if (GetMenuOptionValue("Legitbot", "Preferred Settings"))
	{
		// Enable these
		SetMenuOptionValue("Legitbot", "Enable Legitbot", 1);
		SetMenuOptionString("Legitbot", "FOV Size:", 1);
		SetMenuOptionString("Legitbot", "Smooth Scale:", 3);
		SetMenuOptionString("Legitbot", "Aim Bone:", 4);

		// Disable These
		SetMenuOptionValue("Ragebot", "Enable Ragebot", 0);
		SetMenuOptionValue("Ragebot", "Preferred Settings", 0);
		SetMenuOptionValue("Ragebot", "No Flinch", 0);


		// Set FOV Size, Smooth Scale, Aim Delay, and Legitbot Logic
		// Copy over menu settings before forcing these, and apply those back if this get disabled
	}

	// Handle Custom Spinbot Settings
	if (GetMenuOptionValue("Spinbot", "Preferred Settings"))
	{
		// Enable These
		SetMenuOptionValue("Spinbot", "X Anti-Aim Spinbot:", 1);
		SetMenuOptionValue("Spinbot", "Y Anti-Aim Spinbot:", 1);

		// Disable These
		SetMenuOptionValue("Spinbot", "Funny Spinbots:", 0);
	}

	// Handle Ragebot Enabled
	if (GetMenuOptionValue("Ragebot", "Enable Ragebot"))
	{
		// Disable These
		SetMenuOptionValue("Legitbot", "Preferred Settings", 0);
		SetMenuOptionValue("Legitbot", "Enable Legitbot", 0);

		// Enable these
		SetMenuOptionValue("Main", "No Recoil", 1);
		SetMenuOptionValue("Main", "No Sway", 1);
	}

	// Handle Legitbot Enabled
	if (GetMenuOptionValue("Legitbot", "Enable Legitbot"))
	{
		// Disable These
		SetMenuOptionValue("Ragebot", "Preferred Settings", 0);
		SetMenuOptionValue("Ragebot", "Enable Ragebot", 0);
		SetMenuOptionValue("Ragebot", "No Spread", 0);
		SetMenuOptionValue("Ragebot", "Auto Wall", 0);
		SetMenuOptionValue("Ragebot", "Auto Shoot", 0);
	}

	// Handle Name ESP
	if (GetMenuOptionValue("Drawing", "Draw Name"))
	{
		// Disable over head names for friendlies
		if (GetMenuOptionValue("Drawing", "Draw Enemies"))
		{
			//*(int*)0x8226BA18 = 0x60000000; // Enemy Names
		}
		else
		{
			//*(int*)0x8226BA18 = 0x4BFFE9B1; // Enemy Names
		}

		// Disable over head names for enemies
		if (GetMenuOptionValue("Drawing", "Draw Friendlies"))
		{
			//*(int*)0x8226BC0C = 0x60000000; // Friendly Names
		}
		else
		{
			//*(int*)0x8226BC0C = 0x4BFFE7BD; // Friendly Names
		}
	}

	// Handle Cartoon Mode
	if (GetMenuOptionValue("Drawing", "Cartoon Mode"))
	{
		//*(int*)0x8427192C = 0x00000002;
	}
	else
	{
		//*(int*)0x8427192C = 0x00000007;
	}

	// Handle Steal Gamertags
	if (GetMenuOptionValue("Main", "Steal GT's") /*&& (Dvar_GetBool_AW("cl_ingame"))*/)
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
	if (GetMenuOptionValue("Main", "Rainbow GT"))
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
	if (GetMenuOptionString("Drawing", "Crosshair Type:") != "Off" && (Dvar_GetBool_AW("cl_ingame")))
	{
		if (GetMenuOptionString("Drawing", "Crosshair Type:") == "+")
		{
			// + styled crosshair
			DrawLine_AW((cg_AW->refdef.Width / 2) - 10, cg_AW->refdef.Height / 2, (cg_AW->refdef.Width / 2) + 10, cg_AW->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine_AW(cg_AW->refdef.Width / 2, (cg_AW->refdef.Height / 2) - 10, cg_AW->refdef.Width / 2, (cg_AW->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString("Drawing", "Crosshair Type:") == "x")
		{
			// X styled crosshair
			DrawLine_AW((cg_AW->refdef.Width / 2) - 8, (cg_AW->refdef.Height / 2) - 8, (cg_AW->refdef.Width / 2) + 8, (cg_AW->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine_AW((cg_AW->refdef.Width / 2) - 8, (cg_AW->refdef.Height / 2) + 8, (cg_AW->refdef.Width / 2) + 8, (cg_AW->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString("Drawing", "Crosshair Type:") == "o")
		{
			DrawLine_AW((cg_AW->refdef.Width / 2) - 10, cg_AW->refdef.Height / 2, (cg_AW->refdef.Width / 2) + 10, cg_AW->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine_AW(cg_AW->refdef.Width / 2, (cg_AW->refdef.Height / 2) - 10, cg_AW->refdef.Width / 2, (cg_AW->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));

			DrawLine_AW((cg_AW->refdef.Width / 2) - 8, (cg_AW->refdef.Height / 2) - 8, (cg_AW->refdef.Width / 2) + 8, (cg_AW->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine_AW((cg_AW->refdef.Width / 2) - 8, (cg_AW->refdef.Height / 2) + 8, (cg_AW->refdef.Width / 2) + 8, (cg_AW->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
		}
	}

	// Handle Prestige Disco
	if (GetMenuOptionValue("Settings", "Prestige Disco"))
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

	// Set level 50
	if (GetMenuOptionValue("Settings", "Level 50?"))
	{
		*(int*)0x83A6C1ED = 0x744A0F00;

		SetMenuOptionValue("Settings", "Level 50?", 0);
	}

	// Spoof gamertag
	if (GetMenuOptionValue("Main", "Spoof GT?"))
	{
		SetMenuOptionValue("Main", "Spoof GT?", 0);

		ExCreateThread(0, 0, 0, 0, (LPTHREAD_START_ROUTINE)SpoofGamertag, 0, 0);
	}

	// Handle Third Person
	if (GetMenuOptionValue("Main", "Third Person") && Dvar_GetBool_AW("cl_ingame"))
	{
		*(BYTE*)(*(int*)(0x82A773A8) + 0xC) = 1;
	}
	else
	{
		*(BYTE*)(*(int*)(0x82A773A8) + 0xC) = 0;
	}

	// Draw Legitbot FOV Box
	if (GetMenuOptionValue("Legitbot", "Draw FOV Box") && Dvar_GetBool_AW("cl_ingame") && !menuOpen && GetMenuOptionValue("Legitbot", "Enable Legitbot"))
	{
		int FOVSize = FindFOVSize();
		DrawBox_AW((cg_AW->refdef.Width / 2) - (FOVSize / 2), (cg_AW->refdef.Height / 2) - (FOVSize / 2), FOVSize, FOVSize, Vector4(1, 1, 1, 0.50f));
	}

	// Handle No Flinch
	if (GetMenuOptionValue("Ragebot", "No Flinch"))
	{
		*(int*)0x826019F4 = 0x60000000;
	}
	else
	{
		*(int*)0x826019F4 = 0x4BFFF365;
	}

	// Handle Anti-Blacklist
	/*if (GetMenuOptionValue("Ragebot", "Anti-Blacklist") && Dvar_GetBool_AW("cl_ingame"))
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
	if (GetMenuOptionValue("Settings", "Log IP's"))
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
				char* IPName = va_AW("%s's IP: %i.%i.%i.%i", ClientName, IP[0], IP[1], IP[2], IP[3]);
				sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", IPName, 0x843DECB0 + 0x30);
				Cbuf_AddText_AW(0, Output);
				//strcpy_s((char*)(0x83F0A358 + 4), 0x1F, IPName);
				strcpy((char*)(0x843DECB0 + 4), IPName);
			}
		}

		ClientOptionEnabled[4][selectedClient] = 0;
	}

	// Freeze All Consoles
	/*if (GetMenuOptionValue("Settings", "Freeze All Consoles?"))
	{
		if (Dvar_GetBool_AW("cl_ingame"))
		{
			char Output[0x200];
			PCHAR Hax = "^\x0002NiNJA";

			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", cg_AW->clientInfo[cg_AW->ClientNumber].Name, Hax, 0x83F0A358 + 0x30);
			Cbuf_AddText_AW(0, Output);
		}
		else
		{
			PCHAR Hax = "^\x0002NiNJA";
			memcpy((PVOID)(0x83F0A358 + 4), Hax, sizeof(Hax));
		}

		SetMenuOptionValue("Settings", "Freeze All Consoles?", 0);
	}*/

	// Handle Spinbot Preferred Settings
	if (GetMenuOptionValue("Spinbot", "Preferred Settings"))
	{
		// Disable these
		SetMenuOptionString("Spinbot", "X Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Y Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Z Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);


		// Enable these
		SetMenuOptionValue("Spinbot", "Enable Spinbot", 1);
		SetMenuOptionString("Spinbot", "X Anti-Aim Spinbot:", 1);
		SetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:", 1);
	}

	// Handle Set Prestige
	if (GetMenuOptionString("Settings", "Prestige:") != "N/A")
	{
		BYTE Prestige = FindPrestigeValue();
		*(BYTE*)0x83A6C151 = Prestige;
	}

	// Handle Jugg Class
	if (GetMenuOptionValue("Main", "Jugg Class?"))
	{
		char* JuggClass = va_AW("cmd lui 5 1;wait 20;cmd mr %i 10 custom%i", *(int*)0x82B4F1D4, *(int*)0x82B4F1D4, 1);

		Cbuf_AddText_AW(0, JuggClass);

		SetMenuOptionValue("Main", "Jugg Class?", 0);
	}

	// Correct Some Settings
	if (GetMenuOptionValue("Ragebot", "No Spread") && GetMenuOptionValue("Ragebot", "Auto Aim"))
	{
		SetMenuOptionValue("Ragebot", "Auto Aim", 0);
	}

	// Unlock All
	if (GetMenuOptionValue("Settings", "Unlock All?"))
	{
		// Set Unlock All Data
		memset((LPVOID)0x83A71880, 0xFE, 0x1770);

		// Disable the menu option
		SetMenuOptionValue("Settings", "Unlock All?", 0);
	}

	// Green Bodies
	if (GetMenuOptionValue("Drawing", "Draw Green Bodies") && Dvar_GetBool_AW("cl_ingame"))
	{
		*(int*)0x8228C348 = 0x60000000;
	}
	else
	{
		*(int*)0x8228C348 = 0x41820010;
	}

	// Handle wallhack
	if (GetMenuOptionValue("Drawing", "Draw Through Walls") && Dvar_GetBool_AW("cl_ingame"))
	{
		if (GetMenuOptionValue("Drawing", "Draw Green Bodies"))
		{
			*(int*)0x8260659C = 0x38C02503;
		}
		else
		{
			*(int*)0x8260659C = 0x38C00003;
		}
	}
	else
	{
		*(int*)0x8260659C = 0x7FC6F378;
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

	memcpy(viewaxis, cg_AW->refdef.ViewAxis, sizeof(viewaxis));
	memcpy(vieworg, &cg_AW->refdef.ViewOrigin, sizeof(vieworg));
	width = (float)cg_AW->refdef.Width;
	height = (float)cg_AW->refdef.Height;
	tanHalfFovX = cg_AW->refdef.Fov.x;
	tanHalfFovY = cg_AW->refdef.Fov.y;

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

float GetBoxWidth(pcentity_s_AW Entity) {
	if (Entity->Flags & 0x08)
		return 35;
	else return 17;
}

float GetBoxHeight(pcentity_s_AW Entity) {
	if (Entity->Flags & 0x08)
		return 25;
	else if (Entity->Flags & 0x04)
		return 55;
	else return 70;
}

int GetWeaponDef(int Weapon)
{
	return ((int(*)(...))0x820F01D0)(Weapon, 0);
}

int GetWeaponIcon(int Weapon)
{
	int weaponDef = GetWeaponDef(Weapon);
	if (!weaponDef)
		return 0;
	else
		return *(int*)(weaponDef + 0x778);
}

void DrawSnapLine(Vector4 color, int Index, BOOL SameTeam)
{
	float xy2[2];
	Vector3 xy1, LinePos, LinePosOut;

	//Get center of screen coordinates
	xy2[0] = (float)cg_AW->refdef.Width / 2.0f;

	if (GetMenuOptionString("Drawing", "Draw Snaplines:") == "Bottom")
		xy2[1] = (float)cg_AW->refdef.Height;
	else if (GetMenuOptionString("Drawing", "Draw Snaplines:") == "Center")
		xy2[1] = (float)cg_AW->refdef.Height / 2.0f;
	else if (GetMenuOptionString("Drawing", "Draw Snaplines:") == "Top")
		xy2[1] = (float)cg_AW->refdef.Height / 10.0f - 65.0f;

	if (GetMenuOptionString("Drawing", "Draw Snaplines:") != "Top")
		LinePos = Entities_AW[Index].Origin;
	else
	{
		GetTagPosistion(&Entities_AW[Index], Index, "j_helmet", (float*)&LinePos);
	}

	//Get coordinates of person
	if (WorldToScreen(LinePos, xy1) == TRUE)
	{
		DrawLine_AW((int)xy2[0], (int)xy2[1], (int)xy1[0], (int)xy1[1], 1.0f, color);
	}
}

void DrawArrows(float* color, int Index)
{
	Vector3 Output;
	VectorSubtract(Entities_AW[Index].Origin, Entities_AW[cg_AW->ClientNumber].Origin, Output);

	// Change the material to the uav icon
	*(SHORT*)0x826302D6 = 0x0988;

	// Change the size of the icon
	(*(DWORD*)(*(DWORD*)(0x829AAEA0) + 0x0C)) = 0x41F00000; // Height
	(*(DWORD*)(*(DWORD*)(0x829AAE9C) + 0x0C)) = 0x420C0000; // Width

	((void(*)(...))0x826300B0)(0, cg_AW->refdef.Width / 2.0f, cg_AW->refdef.Height / 2.0f, &Output, color, 0.0f, 1.0f);

	// Change the material to the original
	*(SHORT*)0x826302D6 = 0xD010;

	// Reset icon size to default
	(*(DWORD*)(*(DWORD*)(0x829AAEA0) + 0x0C)) = 0x42100000; // Height
	(*(DWORD*)(*(DWORD*)(0x829AAE9C) + 0x0C)) = 0x42960000; // Width
}

void DrawBones(Vector4 color, int Index)
{
	Vector3 TagPos[2], TagScreen[2];
	for (int b = 0; b < ARRAYSIZE(bonesESPMW2) - 1; b++)
	{
		if (!GetTagPosistion(&Entities_AW[Index], Index, bonesESPMW2[b], (float*)&TagPos[0]) || !GetTagPosistion(&Entities_AW[Index], Index, bonesESPMW2[b + 1], (float*)&TagPos[1]))
			return;

		if (WorldToScreen(TagPos[0], TagScreen[0]) && WorldToScreen(TagPos[1], TagScreen[1]))
			DrawLine_AW(TagScreen[0].x, TagScreen[0].y, TagScreen[1].x, TagScreen[1].y,  2.0f, color);
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
		GetTagPosistion(&Entities_AW[index], index, ThreeDBoxesBones[t], (float*)&BonePos[t]);
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
		DrawLine_AW(minsX, minsY, corner2X, corner2Y, 1.0f, color);
		DrawLine_AW(minsX, minsY, corner4X, corner4Y, 1.0f, color);
		DrawLine_AW(minsX, minsY, corner6X, corner6Y, 1.0f, color);
		//maxes to 3, 5, and 7
		DrawLine_AW(maxesX, maxesY, corner3X, corner3Y, 1.0f, color);
		DrawLine_AW(maxesX, maxesY, corner5X, corner5Y, 1.0f, color);
		DrawLine_AW(maxesX, maxesY, corner7X, corner7Y, 1.0f, color);
		//from 2 to 3 and 7
		DrawLine_AW(corner2X, corner2Y, corner3X, corner3Y, 1.0f, color);
		DrawLine_AW(corner2X, corner2Y, corner7X, corner7Y, 1.0f, color);
		//from 4 to 3 and 5
		DrawLine_AW(corner4X, corner4Y, corner3X, corner3Y, 1.0f, color);
		DrawLine_AW(corner4X, corner4Y, corner5X, corner5Y, 1.0f, color);
		//from 6 to 5 and 7
		DrawLine_AW(corner6X, corner6Y, corner5X, corner5Y, 1.0f, color);
		DrawLine_AW(corner6X, corner6Y, corner7X, corner7Y, 1.0f, color);
	}
}

void DrawESPBox(Vector4 color, int Index)
{
	float fHeight, fWidth;
	Vector3 HeadPos, FeetLocation, HeadLocation, MainRootPos;

	if (GetMenuOptionString("Drawing", "ESP Box Type:") == "Corners")
	{
		if (!GetTagPosistion(&Entities_AW[Index], Index, "j_head", (float*)&HeadPos))
			return;

		if (WorldToScreen(Entities_AW[Index].Origin, FeetLocation) && WorldToScreen(HeadPos, HeadLocation))
		{
			fHeight = FeetLocation.y - HeadLocation.y;
			if (Entities_AW[Index].Flags & 0x8)
				fWidth = fHeight / 0.9;
			else
				fWidth = fHeight / 2;

			DrawBox_AW((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawBox_AW((FeetLocation.x - (fWidth / 2)), FeetLocation.y, (fWidth / 4), 1, color);
			DrawBox_AW((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawBox_AW((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawBox_AW((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawBox_AW((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), FeetLocation.y, (fWidth / 4), 1, color);
			DrawBox_AW((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);
			DrawBox_AW((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);

		}
	}
	else if (GetMenuOptionString("Drawing", "ESP Box Type:") == "3D")
	{
		CalcBoundingBox(Index, color);
	}
	else if (GetMenuOptionString("Drawing", "ESP Box Type:") == "2D" || GetMenuOptionString("Drawing", "ESP Box Type:") == "2D Filled")
	{
	
		Vector3 MainRoot, MainRootOut, FeetOut, Feet;
		GetTagPosistion(&Entities_AW[Index], Index, "j_mainroot", (float*)&MainRoot);
		MainRoot.z += 10;

		int BoxHeight = 55; // 8 or 0x0A = prone|| 4 or 6 = crouched||0 or 2 = standing
		if (Entities_AW[Index].Flags & 0x8)
			BoxHeight = 17;
		else if (Entities_AW[Index].Flags & 0x4)
			BoxHeight = 35;

		Feet.x = MainRoot.x; Feet.y = MainRoot.y; Feet.z = (MainRoot.z - -BoxHeight - 10);

		if (WorldToScreen(MainRoot, MainRootOut) == TRUE && WorldToScreen(Feet, FeetOut) == TRUE)
		{
			FLOAT Height = (MainRootOut.y - FeetOut.y);
			FLOAT Width = (Height / 3.0f);

			if (GetMenuOptionString("Drawing", "ESP Box Type:") != "2D Filled")
			{
				DrawLine_AW((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), 1.0f, color); // Left side line
				DrawLine_AW((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), 1.0f, color); // Bottom line
				DrawLine_AW((int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Right side line
				DrawLine_AW((int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Top line
			}
			else 
				R_AddCmdDrawStretchPic_AW((MainRootOut.x - Width), (MainRootOut.y - (Height*0.5f)), Width*2.0f, Height*1.5f, 0.0f, 0.0f, 0.0f, 0.0f, color, WhiteMaterial);
		}
	}
	else if (GetMenuOptionString("Drawing", "ESP Box Type:") == "Pyramid")
	{
		Vector3 Center = Entities_AW[Index].Origin;

		float W = GetBoxWidth(&Entities_AW[Index]), H = GetBoxHeight(&Entities_AW[Index]);

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
	//char Output[0x200];
	if (!GetMenuOptionValue("Settings", "Disable Menu Instructions"))
	{
		if (!Dvar_GetBool_AW("cl_ingame"))
		{
			FLOAT X, Y;
			X = *(DWORD*)0x82B0E8E8;//cgs_AW->Width;
			Y = *(DWORD*)0x82B0E8EC;//cgs_AW->Height;

			DrawText_AW(va_AW("Press \x0016 & \x0012 To %s ^7NiNJA Engine", menuOpen ? "^1Close" : "^2Open"), (X - 365), Y - 15, 0, 0, BodyFontBold, Vector4(1, 1, 1, 0.75f), 0.7f, 0.7f);
		
		}
		else
		{
			DrawText_AW(va_AW("Press \x0016 & \x0012 To %s ^7NiNJA Engine", menuOpen ? "^1Close" : "^2Open"), (cg_AW->refdef.Width - 365), cg_AW->refdef.Height - 15, 0, 0, BodyFontBold, Vector4(1, 1, 1, 0.75f), 0.7f, 0.7f);
		}
	}
}

double Radians(float Degrees)
{
	return  0.01745329238474369 * Degrees;
}

void GetSpread_AW(int* seed, float* x, float* y)
{
	float x2, y2, min, max, spread;

	BG_GetSpreadForWeapon_AW(cg_AW, &min, &max);
	spread = (max - min) * (*(FLOAT*)((DWORD)cg_AW + 0xBDB80) * 0.00392156862) + min;

	RandomBulletDir_AW(seed, 0, 360, &x2, &y2);
	float unk2 = BG_Random_AW(seed);
	*x = unk2 * x2 * spread;
	*y = unk2 * y2 * spread;
}

Vector3 SmoothAimbotAngles(Vector3 AimbotAngles, float SmoothValue)
{
	float FinishedAngles[3]; float smoothdiff[3];
	AimbotAngles[0] -= LocalClient_AW->ViewAngles.x;
	AimbotAngles[1] -= LocalClient_AW->ViewAngles.y;
	if (AimbotAngles[0] > 180)  AimbotAngles[0] -= 360;
	if (AimbotAngles[1] > 180)  AimbotAngles[1] -= 360;
	if (AimbotAngles[0] < -180) AimbotAngles[0] += 360;
	if (AimbotAngles[1] < -180) AimbotAngles[1] += 360;
	smoothdiff[0] = AimbotAngles[0] / SmoothValue;
	smoothdiff[1] = AimbotAngles[1] / SmoothValue;
	FinishedAngles[0] = LocalClient_AW->ViewAngles.x + smoothdiff[0];
	FinishedAngles[1] = LocalClient_AW->ViewAngles.y + smoothdiff[1];
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

float R_NormalizedTextScale_AW(int font, float scale)
{
	return((scale * 48) / ((*(int*)(font + 0x4))));
}

void FixTextForDistance(CONST PCHAR Text, float* x, float* y, DWORD Font, float scale, Vector3 WorldPos, float* OutDist)
{
	float distanceSq = cg_AW->refdef.ViewOrigin.Distance(WorldPos);
	float distance = sqrt(distanceSq);
	float distFrac = (float)(distance - 64) / 448;
	float distScale = (float)(distFrac * scale) + (float)(1 - distFrac);
	*OutDist = distFrac;
	float x2 = (FLOAT)(floor(-((((FLOAT)R_TextWidth_AW(Text, 0x7FFFFFFF, Font) * R_NormalizedTextScale_AW(Font, scale * distScale)) * 0.7) - *x) + 0.7));

	__asm
	{
		frsp x2, x2
	}
	*x = x2;
	*y = (FLOAT)(floor(*y + 0.7));
}

bool IsWeaponDualWield()
{
	return ((bool(*)(...))0x82641898)(Entities_AW[cg_AW->ClientNumber].weaponNum);
}

bool Autowall(int Index, PCHAR TagToUse, BOOL UsingShield)
{
	Vector3 TagPos, Angles, dir, Hax[3], MyPos;

	if (!GetMenuOptionValue("Ragebot", "Auto Wall"))
		return false;

	if (GetMenuOptionValue("Ragebot", "Target Shields") && UsingShield)
		TagToUse = "j_ankle_le";

	if (!GetTagPosistion(&Entities_AW[Index], Index, TagToUse, (float*)&TagPos))
		return false;

	CG_GetPlayerViewOrigin_AW(0, cg_AW, MyPos);


	BulletFireParams bulletFireParams;
	bulletFireParams.skipNum = cg_AW->ClientNumber;
	bulletFireParams.worldEntNum = 1534;
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
	*(QWORD*)0x825D22F8 = 0x386000004E800020;
	*(QWORD*)0x82535D78 = 0x386000004E800020;

	// Allow 360 degree autowall checks
	int restoreVal = *(int*)0x825D27DC;
	*(int*)0x825D27DC = 0x38600001;

	FireBulletPenetrate_AW(0, &bulletFireParams, Entities_AW[cg_AW->ClientNumber].weaponNum, 0, 0, &Entities_AW[cg_AW->ClientNumber], 0xFFFFFFFF, MyPos, false);

	// Unpatch 360 degree autowall check
	*(int*)0x825D27DC = restoreVal;

	// Restore visual effects
	*(QWORD*)0x825D22F8 = 0x7D8802A64BC21C09;
	*(QWORD*)0x82535D78 = 0x7D8802A64BCBE175;

	return (bulletFireParams.skipNum == Index);
}

void CorrectMovement(usercmd_s_AW *usercommand, Vector3 vec)
{
	Vector3 Fix = vec - (LocalClient_AW->ViewAngles);

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
	Vector3 FeetOrigin = Entities_AW[Index].Origin;
	GetTagPosistion(&Entities_AW[Index], Index, "j_head", (float*)&HeadOrigin);
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
			GetTagPosistion(&Entities_AW[Index], Index, TriggerBotBones[b], (float*)&Hitboxes);
			if (WorldPosToScreenPos((float*)&Hitboxes, &ScreenPos))
			{
				if (ScreenPos.x - ((fWidth / 5) / 2) < (cg_AW->refdef.Width / 2) &&
					ScreenPos.x + ((fWidth / 5) / 2) > (cg_AW->refdef.Width / 2) &&
					ScreenPos.y - ((fWidth / 5) / 2) < (cg_AW->refdef.Height / 2) &&
					ScreenPos.y + ((fWidth / 5) / 2) > (cg_AW->refdef.Height / 2))
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
	if (GetMenuOptionString("Legitbot", "Smooth Scale:") == "5.0")
		SmoothScale = 5.0f;

	if (GetMenuOptionString("Legitbot", "Smooth Scale:") == "10.0")
		SmoothScale = 10.0f;

	if (GetMenuOptionString("Legitbot", "Smooth Scale:") == "15.0")
		SmoothScale = 15.0f;

	if (GetMenuOptionString("Legitbot", "Smooth Scale:") == "20.0")
		SmoothScale = 20.0f;

	if (GetMenuOptionString("Legitbot", "Smooth Scale:") == "25.0")
		SmoothScale = 25.0f;

	if (GetMenuOptionString("Legitbot", "Smooth Scale:") == "30.0")
		SmoothScale = 30.0f;

	return SmoothScale;
}

bool FOVCheck(int index, PCHAR Tag)
{
	Vector2 ScreenPos; float Output[3];
	int FOVSize = FindFOVSize();
	GetTagPosistion(&Entities_AW[index], index, Tag, Output);
	if (WorldPosToScreenPos(Output, &ScreenPos))
	{
		if (ScreenPos.x > ((cg_AW->refdef.Width / 2) - FOVSize) &&
			ScreenPos.x < ((cg_AW->refdef.Width / 2) + FOVSize) &&
			ScreenPos.y >((cg_AW->refdef.Height / 2) - FOVSize) &&
			ScreenPos.y < ((cg_AW->refdef.Height / 2) + FOVSize))
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
	if (!GetMenuOptionValue("Settings", "Invert Team Check"))
	{
		if ((cg_AW->clientInfo[myClientNum].Team == cg_AW->clientInfo[ClientIndex].Team) && (cg_AW->clientInfo[myClientNum].Team != 0))
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
		if ((cg_AW->clientInfo[myClientNum].Team == cg_AW->clientInfo[ClientIndex].Team) && (cg_AW->clientInfo[myClientNum].Team != 0))
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
	if ((Entities_AW[ClientIndex].Alive & 1) == FALSE)
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
	CG_CompassCalcDimensions_AW(0, cg_AW, size, radarHud, &RadarSize.x, &RadarSize.y, &RadarSize.z, &RadarSize.a);

	float x2 = (RadarSize.z * 0.5) + RadarSize.x;
	float y2 = (RadarSize.a * 0.5) + RadarSize.y;

	Vector2 out, outClipped, Yaw;

	CG_CompassUpYawVector_AW(cg_AW, Yaw);

	CG_WorldPosToCompass_AW(0, cg_AW, RadarSize, Yaw, Entities_AW[cg_AW->ClientNumber].Origin, World, out, outClipped);

	Compass.x = -((widthHeight.x * 0.5) - x2) + outClipped.x;
	Compass.y = -((widthHeight.y * 0.5) - y2) + outClipped.y;
}

void DrawRadar_AW()
{
	CRadarHud radarHud;
	Vector2 size;
	DWORD compassPingFriendly = *(DWORD*)0x82ACD048;
	size.x = 0;//ScaleRadarX(12, cg_AW->refdef.Width);
	size.y = 0;//ScaleRadarY(12, cg_AW->refdef.Height);
	radarHud.x = ScaleRadarX(12, cg_AW->refdef.Width);
	radarHud.y = ScaleRadarY(12, cg_AW->refdef.Height);
	radarHud.w = 178.667;
	radarHud.h = 93.3333;
	radarHud.unk = 0x0C0D0000;

	DrawEngineRadar_AW(0, 0, size, &radarHud, WhiteMaterial, Vector4(1, 1, 1, 1));

	// Draw Dropped Items
	for (int i = 100; i < 800; i++)
	{
		if (GetMenuOptionValue("Drawing", "Draw Dropped Items"))
		{
			if ((*(int*)((DWORD)&Entities_AW[i] + 0x130) & 0x20) && Entities_AW[i].EntityType == 3)
			{
				Vector2 Screen;
				Vector3 Origin = Entities_AW[i].Origin;

				// Make sure it's not a scavenger pack
				if ((Entities_AW[i].weaponNum != CG_GetWeaponIndexForName("scavenger_bag_mp")))
					continue;


				Vector2 Compass, widthHeight;
				widthHeight.x = 25;
				widthHeight.y = 15;
				WorldToCompass_Mw2(&radarHud, size, Entities_AW[i].Origin, Compass, widthHeight);

				int horz = radarHud.unk >> 24;
				int vert = (radarHud.unk >> 16) & 0xFF;

				//float unk = sub_8228C068(*(FLOAT*)((DWORD)cg_Mw2 + 0x6EB8C) - *(FLOAT*)0x8259C334);

				CG_DrawRotatedPic_AW(0x82B94570, Compass.x, Compass.y, widthHeight.x, widthHeight.y, horz, vert, 0, Vector4(1, 1, 1, 0.75f), DB_FindXAssetHeader_AW(material, "scavenger_pickup", 1));
			}
		}
	}

	// Draw Enemies
	for (int client = 0; client < 18; client++)
	{
		if (client == cg_AW->ClientNumber || Entities_AW[client].EntityType != 1 || (*(int*)((DWORD)&Entities_AW[client] + 0x130) & 0x20) || (*(int*)((DWORD)&Entities_AW[client] + 0x1D8) & 0x1) == 0)
			continue;

		Vector2 Compass, widthHeight;
		widthHeight.x = 16;
		widthHeight.y = 16;
		WorldToCompass_Mw2(&radarHud, size, Entities_AW[client].Origin, Compass, widthHeight);

		int horz = radarHud.unk >> 24;
		int vert = (radarHud.unk >> 16) & 0xFF;

		float angle = AngleNormalize360_AW(*(FLOAT*)((DWORD)cg_AW + 0x720BC) - Entities_AW[client].Angles.y);
		CG_DrawRotatedPic_AW(0x82B94570, Compass.x, Compass.y, widthHeight.x, widthHeight.y, horz, vert, angle, !IsPlayerEnemy(client, cg_AW->ClientNumber) ? Vector4(0, 1, 0, 0.75f) : Vector4(1, 0, 0, 0.75f), compassPingFriendly);
	}

	//DrawEnemies_Mw2(0, 0, size, &radarHud, Vector4(1,0,0,1));
	//DrawFriendlies_AW(0, 0, size, &radarHud, Vector4(0, 1, 0, 0.75f));
	DrawVehicles_AW(0, 0, size, &radarHud, Vector4(0, 0, 1, 0.75f));
	DrawPlayer_AW(0, 0, size, &radarHud, Vector4(1, 1, 0, 1));
}

void CG_EntityEventHook(int localClient, int entityState_t, int Event, int eventParam)
{
	if (GetMenuOptionValue("Main", "Fast Reload"))
	{
		if (Event == 20)
		{
			int ClientReloadIndex = *(int*)(entityState_t + 0xD6);
			if (ClientReloadIndex == cg_AW->ClientNumber)
				WasReloading = TRUE;
		}
		else if (Event == 23)
		{
			int ClientSwitchIndex = *(int*)(entityState_t + 0xD6);
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
	if (GetMenuOptionValue("Main", "Fast Reload") && Dvar_GetBool_AW("cl_ingame"))
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
	if ((GetMenuOptionValue("Ragebot", "Auto Aim") || GetMenuOptionValue("Legitbot", "Auto Aim") || GetMenuOptionValue("Legitbot", "Triggerbot")) && (GClosestClient != -1))
		pState->Gamepad.bLeftTrigger = 100;

	// Remove game input when menu is open
	if (menuOpen)
		pState->Gamepad.wButtons &= ~(XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT | XINPUT_GAMEPAD_DPAD_RIGHT | XINPUT_GAMEPAD_A | XINPUT_GAMEPAD_B);

	// Rapid Fire
	if (GetMenuOptionValue("Ragebot", "Rapid Fire") || GetMenuOptionValue("Legitbot", "Rapid Fire"))
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

void DrawESP()
{
	LocalClient_AW = (pclientActive_t_AW)GetPointer(0x82B0D308);//tu17
	Entities_AW = (pcentity_s_AW)GetPointer(0x82ACFE30);//tu17
	cg_AW = (pcg_s_AW)GetPointer(0x82AD0298);//tu17
	cgs_AW = (pcgs_t_AW)GetPointer(0x82ACFE34);//tu17

	// If in-game, start our ESP
	if (Dvar_GetBool_AW("cl_ingame") && menuReady && (cgs_AW != NULL))
	{
		// NiNJA Engine v1.0
		if (!GetMenuOptionValue("Settings", "Disable NiNJA Engine Text"))
			DrawAd();

		// Loop for all ents (this 1st so our player ESP draws over it if they over lap)
		for (int i = 18; i < 0x7FF; i++)
		{
			// Draw Explosives
			if (GetMenuOptionValue("Drawing", "Draw Explosives"))
			{
				// Make sure it's a explosive
				if ((Entities_AW[i].Alive & 1) != FALSE && Entities_AW[i].EntityType == 4)
				{
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					TopLocation = Entities_AW[i].Origin;
					if (WorldToScreen(TopLocation, TopLocationOut))
					{
						// Get the explosive id
						int ExplosiveID = Entities_AW[i].weaponNum;

						// Make sure it's valid
						if (ExplosiveID != 0)
						{
							int ExplosiveIcon = GetWeaponIcon(ExplosiveID);
							if (ExplosiveIcon != 0 && MmIsAddressValid((DWORD*)ExplosiveIcon))
							{
								CG_DrawRotatedPicPhysical_AW(0x82B94570, TopLocationOut.x - 30, TopLocationOut.y - 10, 20, 20, 0.0, GetColorCode(colorYellow), ExplosiveIcon);
							}
						}
					}
				}
			}

			// Draw Dropped Items
			if (GetMenuOptionValue("Drawing", "Draw Dropped Items"))
			{
				BOOL IsWeapon = TRUE;

				// Make sure it's a dropped item
				if ((Entities_AW[i].Alive & 1) != FALSE && Entities_AW[i].EntityType == 3 && (GetDistance(cg_AW->refdef.ViewOrigin, Entities_AW[i].Origin) < 1500.0f))
				{
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					TopLocation = Entities_AW[i].Origin; 
					if (WorldToScreen(TopLocation, TopLocationOut))
					{
						// Get the dropped item id
						int DroppedWeapon = Entities_AW[i].weaponNum;

						// Make sure it's not a scavenger pack
						if (DroppedWeapon != CG_GetWeaponIndexForName("scavenger_bag_mp"))
							IsWeapon = FALSE;

						// Set color based on weapon/pack
						Vector4 color = !IsWeapon ? Vector4(1, 1, 1, 0.80f) : Vector4(0.51f, 1, 1, 0.80f);

						// Draw scav packs & weapons
						if (!IsWeapon)
						{
							int DroppedIcon = GetWeaponIcon(DroppedWeapon);
							if (DroppedIcon != 0 && MmIsAddressValid((DWORD*)DroppedIcon))
							{
								CG_DrawRotatedPicPhysical_AW(0x82B94570, TopLocationOut.x - 30, TopLocationOut.y - 10, 25, 30, 0.0, color, DroppedIcon);
							}
						}
						else
						{
							CG_DrawRotatedPicPhysical_AW(0x82B94570, TopLocationOut.x - 30, TopLocationOut.y - 10, 25, 30, 0.0, color, DB_FindXAssetHeader_AW(material, "scavenger_pickup", 1));
						}
					}
				}
			}
		}

		// Loop for all clients
		for (int i = 0; i < cgs_AW->MaxClients; i++)
		{
			// Make sure the client is not us
			if (Entities_AW[i].ClientNumber != cg_AW->ClientNumber && cg_AW->ClientNumber != i)
			{
				// Alive and is player
				//if (*(char*)((DWORD)&Entities_AW[i] + 0xE4) != 1 || (*(int*)((DWORD)&Entities_AW[i] + 0x130) & 0x20) || (*(int*)((DWORD)&Entities_AW[i] + 0x1D8) & 0x1) == 0)
				if ((Entities_AW[i].Alive & 1) == FALSE)
				{
					// Invalid player
					continue;
				}

				// Set some of the players info in our struct
				PlayerInfo[i].Alive = TRUE;
				PlayerInfo[i].ClientNumber = Entities_AW[i].ClientNumber;
				PlayerInfo[i].Name = cg_AW->playerNames2[i].PlayerName;

				// Team check
				if (!GetMenuOptionValue("Settings", "Invert Team Check"))
				{
					if ((cg_AW->clientInfo[cg_AW->ClientNumber].Team == cg_AW->clientInfo[i].Team) && (cg_AW->clientInfo[cg_AW->ClientNumber].Team != 0))
					{
						// Save players team
						PlayerInfo[i].Team = cg_AW->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = TRUE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;

						// If we want to draw friendlies
						if (!GetMenuOptionValue("Drawing", "Draw Friendlies"))
							continue;
					}
					else
					{
						// If we want to draw enemies
						if (!GetMenuOptionValue("Drawing", "Draw Enemies"))
							continue;

						PlayerInfo[i].Team = cg_AW->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = FALSE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
					}
				}
				else
				{
					// Reverse team check
					if ((cg_AW->clientInfo[cg_AW->ClientNumber].Team == cg_AW->clientInfo[i].Team) && (cg_AW->clientInfo[cg_AW->ClientNumber].Team != 0))
					{
						// If we want to draw enemies
						if (!GetMenuOptionValue("Drawing", "Draw Enemies"))
							continue;

						PlayerInfo[i].Team = cg_AW->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = FALSE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
					}
					else
					{
						// Save players team
						PlayerInfo[i].Team = cg_AW->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = TRUE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;

						// If we want to draw friendlies
						if (!GetMenuOptionValue("Drawing", "Draw Friendlies"))
							continue;
					}
				}

				// Visible / Wallbang Check
				if (!PlayerInfo[i].OnTeam)
				{
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_AW(0, &Entities_AW[i], TagToShort("j_head"));

					// Get the players posistion for our visible check
					GetTagPosistion(&Entities_AW[i], i, "j_head", (float*)&PlayerInfo[i].TagPosistion);

					// Run our trace
					trace_t Trace;
					CG_LocationalTrace_AW(&Trace, &cg_AW->refdef.ViewOrigin, &PlayerInfo[i].TagPosistion, i, 0x803003);

					// If the trace fraction is >= 0.97 let's say they are visible
					PlayerInfo[i].VisibleESP = ((Trace.Fraction >= 0.97f) ? TRUE : FALSE);
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_AW(0, &Entities_AW[i], TagToShort("j_head"));

					if (!GetMenuOptionValue("Ragebot", "Auto Wall"))
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
				if (GetMenuOptionString("Drawing", "Draw Snaplines:") != "Off")
				{
					DrawSnapLine(Color, i, PlayerInfo[i].OnTeam);
				}

				// ESP Arrows
				if (GetMenuOptionValue("Drawing", "Directional Arrows"))
				{
					DrawArrows((float*)&Color, i);
				}

				// ESP Bones
				if (GetMenuOptionValue("Drawing", "Draw Bones"))
				{
					DrawBones(Color, i);
				}

				// ESP Boxes
				if (GetMenuOptionString("Drawing", "ESP Box Type:") != "Off")
				{
					DrawESPBox(Color, i);
				}

				// ESP Weapon 
				if (GetMenuOptionValue("Drawing", "Draw Weapon"))
				{
					Vector3 Pos[3], xy1[2], FeetPos, FeetPosOut;
					GetTagPosistion(&Entities_AW[i], i, "j_head", (float*)&Pos[1]);
					FeetPos = Entities_AW[i].Origin;

					//if (WorldToScreen(Pos[1], xy1[0]) && WorldToScreen(FeetPos, FeetPosOut))
					if (CG_CalcNamePosition(0, &Pos[1], &xy1[0].x, &xy1[0].y))
					{
						int WeaponIcon = GetWeaponIcon(Entities_AW[i].weaponNum);
						if (WeaponIcon != 0 && MmIsAddressValid((DWORD*)WeaponIcon))
						{
							CG_DrawRotatedPicPhysical_AW(0x82B94570, xy1[0].x - 60 / 2.0f, xy1[0].y - 25, 50, 20, 0.0, Color, WeaponIcon);
						}

						//PCHAR WeaponName = GetWeaponHudName(Entities_AW[i].CurrentWeapon);
						//if (MmIsAddressValid((PCHAR*)WeaponName))
						//	DrawText_AW(WeaponName, (xy1[0].x - (R_TextWidth_AW(WeaponName, 0x7FFFFFFF, SmallFont) / 2) * 0.5f), (xy1[0].y), 0, 0, SmallFont, Vector4(1, 1, 1, 0.50f), 0.5f, 0.5f);
					}
				}

				// ESP Name
				if (GetMenuOptionValue("Drawing", "Draw Name"))
				{
					Vector3 HeadPos, HeadPosOut, FeetPos, FeetPosOut;
					GetTagPosistion(&Entities_AW[i], i, "j_head", (float*)&HeadPos);
					FeetPos = Entities_AW[i].Origin;

					//if (WorldToScreen(HeadPos, HeadPosOut) && WorldToScreen(FeetPos, FeetPosOut))
					if (CG_CalcNamePosition(0, &HeadPos, &HeadPosOut.x, &HeadPosOut.y) && CG_CalcNamePosition(0, &FeetPos, &FeetPosOut.x, &FeetPosOut.y))
					{
						if (MmIsAddressValid((PCHAR*)cg_AW->playerNames2[i].PlayerName))
						{
							float DisanceOut;
							Vector3 FeetPosOutOG, HeadPosOutOG;
							VectorCopy(FeetPosOut, FeetPosOutOG);
							VectorCopy(HeadPosOut, HeadPosOutOG);

							FixTextForDistance(cg_AW->playerNames2[i].PlayerName, &FeetPosOut.x, &FeetPosOut.y, SmallFont, 1, FeetPos, &DisanceOut);
							//FeetPosOut.y -= ((HeadPosOutOG.y - FeetPosOutOG.y) * 0.750);
							FeetPosOut.y += (((FeetPosOutOG.y - HeadPosOutOG.y) * (DisanceOut * 0.0001f) + 15));

							DrawText_AW(cg_AW->playerNames2[i].PlayerName, (FeetPosOutOG.x - (R_TextWidth_AW(cg_AW->playerNames2[i].PlayerName, 0x7FFFFFFF, SmallFont) / 2) * 0.7f), FeetPosOut.y, 0, 0, SmallFont, Color, 0.7f, 0.7f);
						}
					}
				}

				// Enemy Visible Text
				if (GetMenuOptionValue("Drawing", "Draw Enemy Visible Text"))
				{
					if (PlayerInfo[i].VisibleESP && !PlayerInfo[i].OnTeam)
					{
						DrawText_AW("Visible Enemy!", ((cg_AW->refdef.Width / 2) - (R_TextWidth_AW("Visible Enemy!", 0x7FFFFFFF, HudSmallFont) / 2) * 0.7f), (cg_AW->refdef.Height / 2) - 120, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
					}
				}

				// Proximity Warning Text
				if (GetMenuOptionValue("Drawing", "Draw Proximity Warning"))
				{
					if (!PlayerInfo[i].OnTeam)
					{
						float OutPos[3];
						GetTagPosistion(&Entities_AW[i], i, "j_head", OutPos);
						if ((GetDistance(cg_AW->refdef.ViewOrigin, OutPos) / 100 < 5))
						{
							DrawText_AW("Proximity Warning!", ((cg_AW->refdef.Width / 2) - (R_TextWidth_AW("Proximity Warning!", 0x7FFFFFFF, HudSmallFont) / 2) * 0.7f), (cg_AW->refdef.Height / 2) - 170, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
						}
					}
				}
			}

			// Continue if the client mumber is ours
			else continue;
		}

		// Draw our custom radar
		if (GetMenuOptionValue("Drawing", "Draw Custom Radar"))
		{
			//*(DWORD*)0x8220D964 = 0x60000000; // DrawEngineRadar
			//*(DWORD*)0x8220DD98 = 0x60000000; // DrawFriendlies
			//*(DWORD*)0x8220DDC8 = 0x60000000; // DrawVehicles
			//*(DWORD*)0x8220DDB0 = 0x60000000; // DrawEnemies
			//*(DWORD*)0x8220DE04 = 0x60000000; // DrawPlayer
			//DrawRadar_AW();
		}
		else
		{
			//*(DWORD*)0x8220D964 = 0x480531ED; // DrawEngineRadar
			//*(DWORD*)0x8220DD98 = 0x48056CA1; // DrawFriendlies
			//*(DWORD*)0x8220DDC8 = 0x48058FA9; // DrawVehicles
			//*(DWORD*)0x8220DDB0 = 0x48057499; // DrawEnemies
			//*(DWORD*)0x8220DE04 = 0x4805303D; // DrawPlayer
		}
	}
}

void R_EndFrameHook()
{
	LocalClient_AW = (pclientActive_t_AW)GetPointer(0x82B0D308);//tu17
	Entities_AW = (pcentity_s_AW)GetPointer(0x82ACFE30);//tu17
	cg_AW = (pcg_s_AW)GetPointer(0x82AD0298);//tu17
	cgs_AW = (pcgs_t_AW)GetPointer(0x82ACFE34);//tu17

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
		DrawMenu_AW();
	}
	else
	{
		menuWidth = 0;
		menuHeight = 0;
	}

	// Handles toggle options like no recoil, sway, uav, etc
	HandleToggleOptions(cgs_AW);

	// Draw our NiNJA Engine v1.0 text at the top right of screen pre-game
	if (!Dvar_GetBool_AW("cl_ingame") && menuReady)
	{
		if (!GetMenuOptionValue("Settings", "Disable NiNJA Engine Text"))
		{
			FLOAT X, Y;
			X = *(DWORD*)0x82B0E8E8;//cgs_AW->Width;
			Y = *(DWORD*)0x82B0E8EC;//cgs_AW->Height;
			DrawText_AW("NiNJA Engine v1.0", ((X - 130) - (R_TextWidth_AW("NiNJA Engine v1.0", 0x7FFFFFFF, SmallFont) / 2) * 0.7f), (Y / 2) - 330, 0, 0, BodyFontBold, Vector4(0.102f, 1, 0.102f, 0.50f), 0.6f, 0.6f);
		}
	}

	// Draw our menu controls
	DrawMenuControlsText();

	// Call the original function
	R_EndFrameDetour->CallOriginal();
}

int Menu_PaintAllHook(int screenDisp, int r4, int r5)
{
	LocalClient_AW = (pclientActive_t_AW)GetPointer(0x82B0D308);//tu17
	Entities_AW = (pcentity_s_AW)GetPointer(0x82ACFE30);//tu17
	cg_AW = (pcg_s_AW)GetPointer(0x82AD0298);//tu17
	cgs_AW = (pcgs_t_AW)GetPointer(0x82ACFE34);//tu17

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
		GetTagPosistion(&Entities_AW[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_AW(0, cg_AW, MyPos);
		CG_LocationalTrace_AW(&Trace, &MyPos, &VisPos, EntIndex, 0x803003);

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
		GetTagPosistion(&Entities_AW[EntIndex], EntIndex, VisCheckBones[i], (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_AW(0, cg_AW, MyPos);
		CG_LocationalTrace_AW(&Trace, &MyPos, &VisPos, EntIndex, 0x803003);

		// Check our trace fraction
		if (Trace.Fraction >= 0.95f)
		{
			// Get weapon def
			//PDWORD WeaponDef = BG_GetWeaponDef_AW(Entities_AW[cg_AW->ClientNumber].weaponNum);

			// Get our temp damage from weaponDamage * HitLocationMultiplier
			//float tempDamage = (float)*(int*)(WeaponDef + 0x230) * G_GetWeaponHitLocationMultiplier(HitLocationIndexs[i], Entities_AW[cg_AW->ClientNumber].weaponNum);
			//if (tempDamage > totalDamage)
			//{
			//	totalDamage = tempDamage;
			//	BoneIndex++;
			//}
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
		GetTagPosistion(&Entities_AW[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_AW(0, cg_AW, MyPos);
		CG_LocationalTrace_AW(&Trace, &MyPos, &VisPos, EntIndex, 0x803003);

		// Check our trace fraction
		if (Trace.Fraction >= 0.95f)
			return 6;

		return -1;
	}

	for (int i = 0; i < 12; i++)
	{

		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_AW[EntIndex], EntIndex, HitboxVisCheckBones[i], (float*)&VisPos);

		// Run our trace
		CG_GetPlayerViewOrigin_AW(0, cg_AW, MyPos);
		CG_LocationalTrace_AW(&Trace, &MyPos, &VisPos, EntIndex, 0x803003);

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
	//		if (!(Entities_AW[ClientIndex].weaponNum == CG_GetWeaponIndexForName("riotshield_mp")))
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

	VectorSubtract(*(Vector3*)((DWORD)&Entities_AW[clientIndex] + 0x7C), *(Vector3*)((DWORD)&Entities_AW[clientIndex] + 0x140), EnemyVelocityTemp);

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
	PCHAR AimbotMenu = UsingRagebot ? "Ragebot" : "Legitbot";

	// Loop for all players
	for (int i = 0; i < cgs_AW->MaxClients; i++) //
	{
		BOOL Visible = FALSE, Wallbangable = FALSE, UsingShield = FALSE;
		CHAR PlayerWeaponName[100];

		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_AW->ClientNumber, UsingRagebot))
			continue;

		// Check the user for a shield
		GetWeaponHUDName_AW(Entities_AW[i].weaponNum, 0, PlayerWeaponName);
		if (strstr(PlayerWeaponName, "riotshield"))
			UsingShield = TRUE;

		// If we want to ignore shields
		if (!GetMenuOptionValue(AimbotMenu, "Target Shields") && UsingShield)
		{
			PlayerInfo[i].Wallbangable = FALSE; // Do this here since it doesn't get updated if we continue
			continue;
		}

		// Get our bone
		if (GetMenuOptionString(AimbotMenu, "Aim Bone:") == "Best")
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
			AimTag = GetMenuOptionString(AimbotMenu, "Aim Bone:");
		}

		
		// Get the players posistion for our visible check
		if (!Visible)
		{
			Vector3 VisPos, MyPos;
			GetTagPosistion(&Entities_AW[i], i, AimTag, (float*)&VisPos);

			// Run our trace
			trace_t Trace;
			CG_GetPlayerViewOrigin_AW(0, cg_AW, MyPos);
			CG_LocationalTrace_AW(&Trace, &MyPos, &VisPos, i, 0x803003);

			// Check our trace fraction
			Visible = ((Trace.Fraction >= 0.95f) ? TRUE : FALSE);
			if (!Visible && UsingRagebot)
			{
				// If they are prioritized, check multiple bones
				if (ClientOptionEnabled[1][i] && (GetMenuOptionString("Ragebot", "Aim Bone:") == "Best"))
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
					if (GetMenuOptionString("Ragebot", "Aim Bone:") == "Best")
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
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_AW[i].Origin, (float*)&cg_AW->refdef.ViewOrigin));

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
	for (int i = 0; i < cgs_AW->MaxClients; i++)
	{
		BOOL Visible = FALSE, Wallbangable = FALSE, UsingShield = FALSE;

		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_AW->ClientNumber, NULL))
			continue;

		// Grab our temp distance
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_AW[i].Origin, (float*)&cg_AW->refdef.ViewOrigin));

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

int TransformSeed(int seed)
{
	return 0x343FD * (0x343FD * (0x343FD * (0x343FD * seed + 0x269EC3) + 0x269EC3) + 0x269EC3) + 0x269EC3;
}

VOID Ragebot(pusercmd_s_AW usercommand, pusercmd_s_AW nextusercommand, pusercmd_s_AW oldusercommand, float movementsave)
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
	GetWeaponHUDName_AW(Entities_AW[BestTarget].weaponNum, 0, TargetWeaponName);
	if (strstr(TargetWeaponName, "shield") && (GetMenuOptionValue("Ragebot", "Target Shields") || GetMenuOptionValue("Legitbot", "Target Shields")))
		AimTag = "j_ankle_ri";

	// Use Left Ankle for anti-aim shield users
	if (IsSecondaryShield(BestTarget) && (ClientOptionEnabled[5][BestTarget]))
		AimTag = "j_ankle_le";

	// Get the best targets posistion
	GetTagPosistion(&Entities_AW[BestTarget], BestTarget, AimTag, (float*)&EnemyPosistion);

	// Calculate prediction if we want it
	if (GetMenuOptionString("Ragebot", "Prediction Scale:") != "Off")
		CalcPrediction(BestTarget, EnemyPosistion, FindPredicationScale());

	// Get our view origin
	CG_GetPlayerViewOrigin_AW(0, cg_AW, MyOrigin);

	// Calculate aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles);
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_AW->SpawnAngles, FinalAngles);
	//AngleNormalize(FinalAngles);

	// Set our angles to our aimbot angles
	if (GetMenuOptionString("Ragebot", "Ragebot Type:") == "Silent")
	{
		usercommand->ViewAngles[0] = ANGLE2SHORT(FinalAngles.x);
		usercommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y);

		// Fix our movement
		CorrectMovement(usercommand, FinalAngles);
	}
	else if (GetMenuOptionString("Ragebot", "Ragebot Type:") == "Normal")
	{
		LocalClient_AW->ViewAngles[0] = FinalAngles.x;
		LocalClient_AW->ViewAngles[1] = FinalAngles.y;
	}

	// Do our no spread
	if (GetMenuOptionValue("Ragebot", "No Spread"))
	{
		// Get our seed from the current server time
		int Seed = usercommand->ServerTime;

		// If dual wield, +10 to the seed (thanks brian)
		if (IsWeaponDualWield())
		{
			if (FiringDualWield)
			{
				Seed += 10;
			}
		}

		// Transform seed
		Seed = TransformSeed(Seed);

		// Calculate zero spread
		GetSpread_AW(&Seed, &SpreadAngles.x, &SpreadAngles.y);

		// Apply our no spread
		usercommand->ViewAngles[0] -= ANGLE2SHORT(-SpreadAngles.y);
		usercommand->ViewAngles[1] -= ANGLE2SHORT(-SpreadAngles.x);
	}

	// Autoshoot
	if (GetMenuOptionValue("Ragebot", "Auto Shoot"))
	{
		if (GetMenuOptionValue("Ragebot", "No Spread"))
		{
			if (IsWeaponDualWield() && FiringDualWield)
			{
				usercommand->Buttons |= 0x00080800;
			}
			else
			{
				usercommand->Buttons |= 1;
			}
		}
		else
		{
			// If not using no spread, wait until aimed in before shooting
			float ZoomState = (*(int*)((DWORD)&cg_AW + 0x5B0));
			if (ZoomState >= 0.9f)
			{
				if (IsWeaponDualWield() && FiringDualWield)
					usercommand->Buttons |= 0x00080800;
				else
					usercommand->Buttons |= 1;
			}
		}
	}
}

VOID Legitbot(pusercmd_s_AW usercommand)
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
	GetTagPosistion(&Entities_AW[BestTarget], BestTarget, GetMenuOptionString("Legitbot", "Aim Bone:"), (float*)&EnemyPosistion);

	// Get our view origin
	CG_GetPlayerViewOrigin_AW(0, cg_AW, MyOrigin);

	// Calc our aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles); 
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_AW->SpawnAngles, FinalAngles);
	//AngleNormalize(FinalAngles);

	// Get our smooth angles
	LegitAngles = SmoothAimbotAngles(FinalAngles, FindSmoothScale());

	// Apply our smooth angles if we are aimed in
	DWORD ReactionDelay = FindReactionDelayAmount();
	PCHAR AimKeyVal = GetMenuOptionString("Legitbot", "Aim Button:");
	if (GetMenuOptionString("Legitbot", "Reaction Delay:") != "Off")
	{
		// If we want a reaction delay
		if ((GReactionDelay > ReactionDelay) && *(float*)((DWORD)cg_AW + 0x424) >= 0.3f)
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
				LocalClient_AW->ViewAngles[0] = LegitAngles.x;
				LocalClient_AW->ViewAngles[1] = LegitAngles.y;
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
		if (*(float*)((DWORD)cg_AW + 0x424) >= 0.3f)
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
				LocalClient_AW->ViewAngles[0] = LegitAngles.x;
				LocalClient_AW->ViewAngles[1] = LegitAngles.y;
			}
		}
	}

	// Autoshoot
	if (GetMenuOptionValue("Legitbot", "Auto Shoot") || ((GetMenuOptionValue("Legitbot", "Triggerbot") && TriggerBot(BestTarget))))
	{
		// Wait until aimed in before shooting
		if (*(float*)((DWORD)cg_AW + 0x424) >= 0.3f)
		{
			usercommand->Buttons |= 1;

			if (IsWeaponDualWield())
				usercommand->Buttons |= 0x00080800;
		}
	}
}

VOID Spinbot(usercmd_s_AW *UserCommand, BOOL RagebotEnabled)
{
	int RandomAdd = rand() % 71;
	int RandomUpDownSpeed = rand() % 71;
	int RandomLeft = rand() % 360;
	int RandomRight = rand() % -360;
	Vector3 MyPos;
	BOOL ValidAATarget = FALSE;


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
	//if ((*(int*)((DWORD)&Entities_AW[cg_AW->ClientNumber] + 0x1C4) & 0x1) == 0 || UserCommand->weapon == 0 || Entities_AW[cg_AW->ClientNumber].Stance & 0x08 || (cg_AW->Health <= 0) || (UserCommand->buttons & 0x8000) || (UserCommand->buttons & 0x4000))
	//	return;

	DWORD BestAAClient = FindBestAntiAimTarget();
	Vector3 EnemyPos, MyOrigin, VectorAngles, FinalAngles, Angles;

	if (BestAAClient != -1)
	{
		CG_GetPlayerViewOrigin_AW(0, cg_AW, MyOrigin);
		AimAssist_GetTagPos(0, &Entities_AW[BestAAClient], TagToShort("j_mainroot"), EnemyPos);

		// Calculate aimbot angles
		VectorSubtract(EnemyPos, MyOrigin, Angles);
		VectorToAngle((float*)&Angles, (float*)&VectorAngles);
		VectorSubtract(VectorAngles, LocalClient_AW->SpawnAngles, FinalAngles);

		ValidAATarget = TRUE;
	}

	int AAAngleX = UserCommand->ViewAngles[0];
	int AAAngleY = UserCommand->ViewAngles[1];

	// Anti-Aim X Look down
	if (GetMenuOptionString("Spinbot", "X Anti-Aim Spinbot:") == "Look Down")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "X Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		//UserCommand->ViewAngles[0] = ANGLE2SHORT(ffAngleNormalize(60.0f + ShortToAngle(UserCommand->ViewAngles[0]) - RefDefAngles.x)); //ANGLE2SHORT(72.0f);
		UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_AW->SpawnAngles.x);
	}
	else if (GetMenuOptionString("Spinbot", "X Anti-Aim Spinbot:") == "Up & Down")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "X Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		int RandomShouldFlip = rand() % 12;
		if (RandomShouldFlip > 5)
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_AW->SpawnAngles.x);
			SpinbotFlipX = TRUE;
		}
		else
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(-45 - LocalClient_AW->SpawnAngles.x);
			SpinbotFlipX = FALSE;
		}

	}

	// Anti-Aim Y Flip Flop
	if (GetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:") == "Flip Flop")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "Y Spin Direction:", 0);

		if (ValidAATarget)
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
		else
		{
			UserCommand->ViewAngles[1] = ANGLE2SHORT(rand() % 360);
		}
	}
	else if (GetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:") == "Face Away")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "Y Spin Direction:", 0);

		if (ValidAATarget)
		{
			if (SpinbotFlip)
			{
				UserCommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f + rand() % 30);
				SpinbotFlip = FALSE;
			}
			else
			{
				UserCommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f - rand() % 30);
				SpinbotFlip = TRUE;
			}
		}
		else
		{
			UserCommand->ViewAngles[1] = ANGLE2SHORT(rand() % 360);
		}
	}
	else if (GetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:") == "Jitter")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "Y Spin Direction:", 0);

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
	else if (GetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:") == "Sideways")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "Y Spin Direction:", 0);

		UserCommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y - 100.0f);
	}
	else if (GetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:") == "Detection")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "Y Spin Direction:", 0);

		// If we took damage or they are wallbangable / visible
		if ((cg_AW->Health < OurHealth) || PlayerInfo[BestAAClient].Wallbangable)
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
		OurHealth = cg_AW->Health;
	}

	// Anti-Aim X Look Up (Riot Shield)
	if (GetMenuOptionString("Spinbot", "X Anti-Aim Spinbot:") == "Riot Shield")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "X Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		//UserCommand->ViewAngles[0] = ANGLE2SHORT(72.0f);
		UserCommand->ViewAngles[0] = ANGLE2SHORT(-69.99 - LocalClient_AW->SpawnAngles.x);
	}

	// Anti-Aim Y 180 Flip From Enemy (Riot Shield)
	if (GetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:") == "Riot Shield")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "Y Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		if (ValidAATarget)
		{
			//UserCommand->ViewAngles[1] = ANGLE2SHORT(InverseNorm(AAX - 175.0f));
			UserCommand->ViewAngles[1] = ANGLE2SHORT(FinalAngles.y - 180.0f);
		}
		else
		{
			UserCommand->ViewAngles[1] = ANGLE2SHORT(rand() % 360);
		}
	}

	// X Spin Direction
	if (GetMenuOptionString("Spinbot", "X Spin Direction:") == "Look Down")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "X Anti-Aim Spinbot:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		//UserCommand->ViewAngles[0] = ANGLE2SHORT(ffAngleNormalize(60.0f + ShortToAngle(UserCommand->ViewAngles[0]) - RefDefAngles.x));
		UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_AW->SpawnAngles.x);
	}
	else if (GetMenuOptionString("Spinbot", "X Spin Direction:") == "Look Up")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "X Anti-Aim Spinbot:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		//UserCommand->ViewAngles[0] = ANGLE2SHORT(ffAngleNormalize(-60.0f + ShortToAngle(UserCommand->ViewAngles[0]) - RefDefAngles.x));
		UserCommand->ViewAngles[0] = ANGLE2SHORT(-69.99 - LocalClient_AW->SpawnAngles.x);
	}

	// Y Spin Direction
	if (GetMenuOptionString("Spinbot", "Y Spin Direction:") == "Left")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);


		UserCommand->ViewAngles[1] = ANGLE2SHORT(SpinbotSpinFlip);
	}
	else if (GetMenuOptionString("Spinbot", "Y Spin Direction:") == "Right")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);


		UserCommand->ViewAngles[1] = ANGLE2SHORT(SpinbotSpin);
	}

	// Z Spin Direction
	if (GetMenuOptionString("Spinbot", "Z Spin Direction:") == "Right")
	{
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		UserCommand->ViewAngles[2] = ANGLE2SHORT(SpinbotSpinZR);
	}
	else if (GetMenuOptionString("Spinbot", "Z Spin Direction:") == "Left")
	{
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		UserCommand->ViewAngles[2] = ANGLE2SHORT(SpinbotSpinZ);
	}


	// Random Z Spinning When Anti-Aim Is Selected
	if (GetMenuOptionString("Spinbot", "X Anti-Aim Spinbot:") != "Off" && GetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:") != "Off")
		UserCommand->ViewAngles[2] = ANGLE2SHORT(SpinbotSpin);

	LocalClient_AW->ViewAngles.z = -LocalClient_AW->SpawnAngles.z;
}

void CL_CreateNewCommandsHook(int r3)
{
	LocalClient_AW = (pclientActive_t_AW)GetPointer(0x82B0D308);//tu17
	Entities_AW = (pcentity_s_AW)GetPointer(0x82ACFE30);//tu17
	cg_AW = (pcg_s_AW)GetPointer(0x82AD0298);//tu17
	cgs_AW = (pcgs_t_AW)GetPointer(0x82ACFE34);//tu17
	XINPUT_STATE State;

	// Call the original 1st
	CL_CreateNewCommandsDetour->CallOriginal(r3);

	// If we are ingame, start our aimbot
	if (Dvar_GetBool_AW("cl_ingame") && (GetMenuOptionValue("Ragebot", "Enable Ragebot") || GetMenuOptionValue("Legitbot", "Enable Legitbot") || GetMenuOptionValue("Spinbot", "Enable Spinbot")))
	{
		pusercmd_s_AW OldUserCommand = LocalClient_AW->GetUserCommand(LocalClient_AW->GetUserCommandNumber() - 1);
		pusercmd_s_AW CurrentUserCommand = LocalClient_AW->GetUserCommand(LocalClient_AW->GetUserCommandNumber());
		pusercmd_s_AW NextUserCommand = LocalClient_AW->GetUserCommand(LocalClient_AW->GetUserCommandNumber() + 1);
		*NextUserCommand = *CurrentUserCommand;
		CurrentUserCommand->ServerTime -= 1;
		LocalClient_AW->CommandNumber += 1;

		// Save our angle for movement fix
		float MovementSave = SHORT2ANGLE(CurrentUserCommand->ViewAngles[1]);

		// Reset our player info structure when we enter a game
		if (!DroppedAllTargets)
		{
			for (int Target = 0; Target < cgs_AW->MaxClients; Target++)
			{
				DropTarget(PlayerInfo[Target]);
			}

			DroppedAllTargets = TRUE;
		}

		// If Ragebot aimbot mode
		if (GetMenuOptionValue("Ragebot", "Enable Ragebot"))
			Ragebot(CurrentUserCommand, NextUserCommand, OldUserCommand, MovementSave);

		// If Legitbot aimbot mode
		if (GetMenuOptionValue("Legitbot", "Enable Legitbot"))
			Legitbot(CurrentUserCommand);

		// Rapid Fire
		if (GetMenuOptionValue("Ragebot", "Rapid Fire") || GetMenuOptionValue("Legitbot", "Rapid Fire"))
		{
			if (FAILED(XInputGetState(0, &State)))
				goto Return;

			if (TriggerValue > 25)
			{
				if (RapidFireCtr > 2)
					RapidFireCtr = 0;

				if (RapidFireCtr == 0)
					CurrentUserCommand->Buttons |= 1;
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
	LocalClient_AW = (pclientActive_t_AW)GetPointer(0x82B0D308);//tu17
	Entities_AW = (pcentity_s_AW)GetPointer(0x82ACFE30);//tu17
	cg_AW = (pcg_s_AW)GetPointer(0x82AD0298);//tu17
	cgs_AW = (pcgs_t_AW)GetPointer(0x82ACFE34);//tu17

	// Update our input state
	if (!menuReady)
		goto Return;

	if (Dvar_GetBool_AW("cl_ingame"))
	{
		// Do our server time 
		pusercmd_s_AW OldUserCommand = LocalClient_AW->GetUserCommand(LocalClient_AW->GetUserCommandNumber() - 1);
		pusercmd_s_AW CurrentUserCommand = LocalClient_AW->GetUserCommand(LocalClient_AW->GetUserCommandNumber());
		pusercmd_s_AW NextUserCommand = LocalClient_AW->GetUserCommand(LocalClient_AW->GetUserCommandNumber() + 1);

		// Get Ragebot status
		BOOL Ragebot = GetMenuOptionValue("Ragebot", "Enable Ragebot");

		// Do our spinbot
		if (GetMenuOptionValue("Spinbot", "Enable Spinbot"))
			Spinbot(CurrentUserCommand, Ragebot);


		// Handle fake lag
		if (GetMenuOptionString("Main", "Fake Lag Amount:") != "Off")
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

int PlayLocalSoundAliasByNameHook(int localClientNum, char *aliasname, int a0)
{
	//DbgPrint("Name: %s", aliasname);

	// Disable main menu music
	if (GetMenuOptionValue("Settings", "Disable Main Menu Music"))
	{
		if (!strcmp(aliasname, "music_mainmenu_mp"))
			return 0;
	}

	return PlayLocalSoundAliasByNameDetour->CallOriginal(localClientNum, aliasname, a0);
}

VOID DrawMenuTextHook(PCHAR text, DWORD font, float x, float y, float f3, float f4, float f5, float f6)
{
	if (strcmp(text, "Store") == 0)
	{
		text = "^6Made by Team NiNJA\0";
	}
	else if (strcmp(text, "MULTIPLAYER") == 0)
	{
		text = "^1NiNJA\0";

		if (startX == 0.0f)
		{
			goingForward = TRUE;
			currentX = x;
			startX = x;
			endX = (myEndX * startX) / myStartX;
		}

		if (currentX >= endX) goingForward = FALSE;
		else if (currentX <= startX) goingForward = TRUE;

		currentX = goingForward ? (currentX + 1) : (currentX - 1);
		x = currentX;
	}

	typedef VOID(*DrawMenuText)(IN OUT PCHAR r3, IN OUT DWORD r4, IN OUT float f3, IN OUT float f4, IN OUT float f5, IN OUT float f6, IN OUT float f7, IN OUT float f8);
	return ((DrawMenuText)DrawMenuTextDetour->SaveStub)(text, font, x, y, f3, f4, f5, f6);
}

DWORD RenderObjectColorHook(DWORD r3, DWORD r4, DWORD r5, DWORD r6, DWORD r7, DWORD r8, WORD r9, DWORD r10)
{
	if (GetMenuOptionValue("Drawing", "Draw Green Bodies"))
	{
		DWORD dwRenderFlags = *(DWORD*)(r4 + 0x6C);

		if (dwRenderFlags > 0x500400)
		{
			dwRenderFlags |= 0x100000;
		}

		*(DWORD*)(r4 + 0x6C) = dwRenderFlags;
	}

	return ((DWORD(*)(...))RenderObjectColorDetour->SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10);
}

int getStatIndex(int msgt)
{
	return *(int*)(*(int*)(msgt + 8) + 1);
}

BYTE* getStatValue(int msgt)
{
	return (BYTE*)(*(int*)(msgt + 8) + 6);
}

void LiveStats_WriteStatHook(int localClientNum, int msg_t)
{
	if (GetMenuOptionValue("Settings", "Anti Derank") && Dvar_GetBool_AW("cl_ingame"))
	{
		// Some hash
		if (getStatIndex(msg_t) < 8)
			return;

		// Prestige
		if (getStatIndex(msg_t) == 0x09)
			return;

		// Experience
		if (getStatIndex(msg_t) == 0xA5)
			return;

		// Level
		if (getStatIndex(msg_t) == 0xA9)
			return;

		// Wins
		if (getStatIndex(msg_t) == 0xFE)
			return;

		// Ranked Play
		if (getStatIndex(msg_t) == 0x10E)
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
		D3DDevice_SetRenderState_AW(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_AW(gDevice, D3DRS_ZENABLE, D3DZB_FALSE);
		R_DrawXModelSkinnedCachedDetour->CallOriginal(context, modelSurf, unk);
		D3DDevice_SetRenderState_AW(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_AW(gDevice, D3DRS_ZENABLE, D3DZB_TRUE);
	}

	else if (strstr(ModelName, "opforce") || (strstr(ModelName, "militia") || (strstr(ModelName, "airborne_") || (strstr(ModelName, "op_arctic") || (strstr(ModelName, "arctic_") || strstr(ModelName, "arab") || strstr(ModelName, "head_suren") || (strstr(ModelName, "op_ghille")))))))
	{
		drawPlayer = 2;
		D3DDevice_SetRenderState_AW(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_AW(gDevice, D3DRS_ZENABLE, D3DZB_FALSE);
		R_DrawXModelSkinnedCachedDetour->CallOriginal(context, modelSurf, unk);
		D3DDevice_SetRenderState_AW(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_AW(gDevice, D3DRS_ZENABLE, D3DZB_TRUE);
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
	if (GetMenuOptionString("Drawing", "Chams:") != "Off")
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
				if (GetMenuOptionString("Drawing", "Chams:") == "Wireframe")
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
				if (GetMenuOptionString("Drawing", "Chams:") == "Wireframe")
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

void AWHax()
{
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
	DrawMenuTextDetour = new Detour<void>;
	RenderObjectColorDetour = new Detour<DWORD>;
	LiveStats_WriteStatDetour = new Detour<void>;
	//NetDll_XNetGetTitleXnAddrDetour = new Detour<DWORD>;

	Menu_PaintAllDetour->SetupDetour(0x823884D8, (PVOID)Menu_PaintAllHook);
	XInputGetStateDetour->SetupDetour(0x82144840, (PVOID)XInputGetStateHook);
	CL_SendCmdDetour->SetupDetour(0x825B5420, (PVOID)CL_SendCmdHook);
	CL_CreateNewCommandsDetour->SetupDetour(0x825B79D8, (PVOID)CL_CreateNewCommandsHook);

	PlayLocalSoundAliasByNameDetour->SetupDetour(0x8221DC40, (PVOID)PlayLocalSoundAliasByNameHook);
	R_EndFrameDetour->SetupDetour(0x825A5900, (PVOID)R_EndFrameHook);
	CG_EntityEventDetour->SetupDetour(0x8261E5C8, (PVOID)CG_EntityEventHook);
	DrawMenuTextDetour->SetupDetour(0x82295B38, (PVOID)DrawMenuTextHook);
	RenderObjectColorDetour->SetupDetour(0x8228C2B8, (PVOID)RenderObjectColorHook);
	LiveStats_WriteStatDetour->SetupDetour(0x825FA478, (PVOID)LiveStats_WriteStatHook);
	//NetDll_XNetGetTitleXnAddrDetour->SetupDetour(0x823B6994, (PVOID)NetDll_XNetGetTitleXnAddrHook);
	//RB_SetTessTechniqueDetour->SetupDetour(0x82373AF8, (PVOID)RB_SetTessTechniqueHook);
	//R_DrawXModelSkinnedCachedDetour->SetupDetour(0x823982C0, (PVOID)R_DrawXModelSkinnedCachedHook);
	//DrawIndexedPrimitiveDetour->SetupDetour(0x8242E2E0, (PVOID)DrawIndexedPrimitive); // tu23
	//R_DrawIndexedPrimitiveDetour->SetupDetour(0x820AB1C0, (PVOID)R_DrawIndexedPrimitiveHook); // tu23


	// Patch GetTagPos so it doesn't throw errors
	*(int *)0x827746D0 = 0x60000000;

	// Remove breakpoints on devkits
	if (isDevkit)
	{
		if ((*(int*)0x8184CB8C == 0x0FE00019) || (*(int*)0x8184D1E0 == 0x0FE00019))
		{
			*(int*)0x8184CB8C = 0x60000000;
			*(int*)0x8184D1E0 = 0x60000000;
		}
	}
}

void UninitializeMw2Cheater()
{
	if (Menu_PaintAllDetour->isDetoured())
		Menu_PaintAllDetour->TakeDownDetour();

	if (XInputGetStateDetour->isDetoured())
		XInputGetStateDetour->TakeDownDetour();

	if (CL_SendCmdDetour->isDetoured())
		CL_SendCmdDetour->TakeDownDetour();

	if (CL_CreateNewCommandsDetour->isDetoured())
		CL_CreateNewCommandsDetour->TakeDownDetour();

	if (PlayLocalSoundAliasByNameDetour->isDetoured())
		PlayLocalSoundAliasByNameDetour->TakeDownDetour();

	if (R_EndFrameDetour->isDetoured())
		R_EndFrameDetour->TakeDownDetour();

	if (CG_EntityEventDetour->isDetoured())
		CG_EntityEventDetour->TakeDownDetour();

	if (R_DrawXModelSkinnedCachedDetour->isDetoured())
		R_DrawXModelSkinnedCachedDetour->TakeDownDetour();

	if (DrawIndexedPrimitiveDetour->isDetoured())
		DrawIndexedPrimitiveDetour->TakeDownDetour();

	if (R_DrawIndexedPrimitiveDetour->isDetoured())
		R_DrawIndexedPrimitiveDetour->TakeDownDetour();

	if (DrawMenuTextDetour->isDetoured())
		DrawMenuTextDetour->TakeDownDetour();

	if (RenderObjectColorDetour->isDetoured())
		RenderObjectColorDetour->TakeDownDetour();

	if (LiveStats_WriteStatDetour->isDetoured())
		LiveStats_WriteStatDetour->TakeDownDetour();

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
	delete DrawMenuTextDetour;
	delete RenderObjectColorDetour;
	delete LiveStats_WriteStatDetour;
	//delete NetDll_XNetGetTitleXnAddrDetour;
}