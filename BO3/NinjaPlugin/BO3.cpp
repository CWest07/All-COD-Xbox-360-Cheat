#include "stdafx.h"
#include "Utilities.h"
#include "Config.h"
#include <time.h>
#include "Globals.h"
#include "BO3.h"
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

pclientActive_t_BO3 LocalClient_BO3;
pcentity_s_BO3 Entities_BO3;
pcgs_t_BO3 cgs_BO3;
pcg_s_BO3 cg_BO3;
prefdef_s_BO3 refdef_BO3;

ClientInformation_BO3 PlayerInfo[18];

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
short BoneIndexs[22];

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
	AddMenuOption("Main", "Steal GT's", 3);
	AddMenuOption("Main", "Rainbow GT", 4);
	AddMenuOption("Main", "Spoof GT?", 5);
	AddMenuOption("Main", "Show Host", 6);
	AddMenuOption("Main", "Show Ping", 7);
	AddMenuOption("Main", "Fast Reload", 8);
	//AddMenuOption("Main", "Everyone Hears Me", 9);
	AddMenuOption("Main", "Never Host", 9);
	AddMenuOption("Main", "Field Of View:", 10, 0, "Normal", FOVS, FOVS_Size, FOVSizes_Start);
	AddMenuOption("Main", "Third Person", 11);
	AddMenuOption("Main", "Fake Lag Amount:", 12, 0, "Off", FakeLagTypes, FakeLagTypes_Size, FakeLagTypes_Start);
	AddMenuOption("Main", "Jugg Class?", 13);
	AddMenuOption("Main", "End Game?", 14);
	AddMenuOption("Main", "Leave Game?", 15);


	AddMenuOption("Ragebot", "Enable Ragebot", 0);
	AddMenuOption("Ragebot", "Ragebot Type:", 1, 0, "Silent", RagebotTypes, RagebotTypes_Size, RagebotTypes_Start);
	//AddMenuOption("Ragebot", "Ragebot Logic:", 2, 0, "Distance", RagebotLogics, RagebotLogics_Size, RagebotLogics_Start);
	AddMenuOption("Ragebot", "Aim Bone:", 2, 0, "Best", AimBones, AimBones_Size, AimBones_Start);
	AddMenuOption("Ragebot", "No Spread", 3);
	AddMenuOption("Ragebot", "No Flinch", 4);
	AddMenuOption("Ragebot", "Auto Wall", 5);
	AddMenuOption("Ragebot", "Auto Shoot", 6);
	AddMenuOption("Ragebot", "Auto Aim", 7);
	AddMenuOption("Ragebot", "Rapid Fire", 8);
	AddMenuOption("Ragebot", "Target Shields", 9);
	//AddMenuOption("Ragebot", "Anti-Blacklist", 11);
	AddMenuOption("Ragebot", "Preferred Settings", 10);



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
	AddMenuOption("Spinbot", "Funny Spinbots:", 6, 0, "Off", FunnySpinbots, FunnySpinbots_Size, FunnySpinbots_Start);
	AddMenuOption("Spinbot", "X Anti-Aim Spinbot:", 7, 0, "Off", XAASpinbots, XAASpinbots_Size, XAASpinbots_Start);
	AddMenuOption("Spinbot", "Y Anti-Aim Spinbot:", 8, 0, "Off", YAASpinbots, YAASpinbots_Size, YAASpinbots_Start);
	AddMenuOption("Spinbot", "Preferred Settings", 9);


	AddMenuOption("Drawing", "Draw Enemies", 0);
	AddMenuOption("Drawing", "Draw Friendlies", 1);
	AddMenuOption("Drawing", "ESP Box Type:", 2, 0, "Off", ESPBoxes, ESPBoxes_Size, ESPBoxes_Start);
	AddMenuOption("Drawing", "Chams:", 3, 0, "Off", ChamTypes, ChamTypes_Size, ChamTypes_Start);
	AddMenuOption("Drawing", "Draw Name", 4);
	AddMenuOption("Drawing", "Draw Weapon", 5);
	AddMenuOption("Drawing", "Draw Bones", 6);
	AddMenuOption("Drawing", "Draw Explosives", 7);
	AddMenuOption("Drawing", "Draw Snaplines:", 8, 0, "Off", SnaplinesTypes, SnaplinesTypes_Size, SnaplinesTypes_Start);
	AddMenuOption("Drawing", "Draw Killstreaks", 9);
	AddMenuOption("Drawing", "Directional Arrows", 10);
	AddMenuOption("Drawing", "Crosshair Type:", 11, 0, "Off", CrosshairTypes, CrosshairTypes_Size, CrosshairTypess_Start);
	AddMenuOption("Drawing", "Draw Custom Radar", 12);
	AddMenuOption("Drawing", "Draw Dropped Items", 13);
	AddMenuOption("Drawing", "Draw Enemy Visible Text", 14);
	AddMenuOption("Drawing", "Draw Proximity Warning", 15);
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
	//AddMenuOption("Settings", "Set Legit Stats?", 11);
	AddMenuOption("Settings", "Invert Team Check", 7);
	AddMenuOption("Settings", "Log IP's", 8);
	//AddMenuOption("Settings", "Freeze All Consoles?", 9);

	HudSmallFont = DB_FindXAssetHeader_BO3(font, "fonts/hudsmallfont", 1, -1);
	BigDevFont = DB_FindXAssetHeader_BO3(font, "fonts/bigDevFont", 1, -1);
	SmallFont = DB_FindXAssetHeader_BO3(font, "fonts/smallfont", 1, -1);
	WhiteMaterial = DB_FindXAssetHeader_BO3(material, "white", 1, -1);
	ScavMaterial = DB_FindXAssetHeader_BO3(material, "scavenger_pickup", 1, -1);
	CompassPingMaterial = DB_FindXAssetHeader_BO3(material, "compassping_player", 1, -1);

	//memcpy(OriginalGT, (PVOID)(0x83F0A358 + 4), 0x1E);
	//memcpy(OriginalXUID, (PVOID)(0x838BA853), 0xE);

	menuReady = TRUE;
}

bool UserInGame()
{
	int Result;
	int *IntStack[10];
	IntStack[9] = &Result;
	((void(*)(...))0x825D5A70)(IntStack);
	//return (Result != 0 && ((bool(*)(...))0x8253F6C0)(0));
	return Result != 0;
}

void PlayMenuSound(char* SoundName)
{
	if (GetMenuOptionValue("Settings", "Disable Menu Sounds"))
		return;

	return;

	// Read the sound level (1.0, not sure if this changes per user)
	float SoundLevel = *(float*)0x8209EEB8;

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

void DrawBox_BO3(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color)
{
	R_AddCmdDrawStretchPic_BO3(X, Y, W, H, 4, 4, 1, 1, Color, WhiteMaterial);
}

void DrawGradient_BO3(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color1, Vector4& Color2, bool horizontal)
{
	float fAdd = (horizontal ? 0 : W / 2 - H / 2);
	R_AddCmdDrawStretchPic_BO3(X, Y, W, H, 4, 4, 1, 1, Color2, WhiteMaterial);
	R_AddCmdDrawStretchPic_BO3(X + fAdd, Y - fAdd, (horizontal ? W : H), (horizontal ? H : W), 4, 4, 1, 1, Color1, GradientFadeIn);
}

void DrawText_BO3(CONST PCHAR Text, FLOAT X, FLOAT Y, DWORD index, FLOAT addy, DWORD Font, Vector4& Color, float fontXSize = 0.7, float fontYSize = 0.7)
{
	CL_DrawTextPhysical_BO3(Text, 0x7FFFFFFF, Font, X, Y + (index * addy), fontXSize, fontYSize, Color, 0);
}

DWORD GetViewOriginPointer()
{
	DWORD cg = *(int*)0x82E819D8;
	DWORD refDef = cg + 0x32554;
	return refDef + 0x1BD58;
}

bool WorldPosToScreenPos(float* In, Vector2* Out)
{
	Vector3 Position, Transform;

	VectorSubtract(In, cg_BO3->refdef.ViewOrg, Position);

	Transform.x = DotProduct(Position, cg_BO3->refdef.ViewAxis[1]);
	Transform.y = DotProduct(Position, cg_BO3->refdef.ViewAxis[2]);
	Transform.z = DotProduct(Position, cg_BO3->refdef.ViewAxis[0]);

	if (Transform.z < 0.1f)
		return false;

	Vector2 Center; Vector2 OutPut;
	Center.x = cg_BO3->refdef.Width * 0.5f; Center.y = cg_BO3->refdef.Height * 0.5f;
	OutPut.x = Center.x * (1 - (Transform.x / cg_BO3->refdef.Fov.x / Transform.z)); OutPut.y = Center.y * (1 - (Transform.y / cg_BO3->refdef.Fov.y / Transform.z));
	*Out = OutPut;
	return true;
}

void DrawLine_BO3(FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2, FLOAT Width, Vector4& Color)
{
	FLOAT X, Y, Angle, L1, L2, H1;
	H1 = Y2 - Y1;
	L1 = X2 - X1;
	L2 = sqrt(L1 * L1 + H1 * H1);
	X = X1 + ((L1 - L2) / 2);
	Y = Y1 + (H1 / 2);
	Angle = (FLOAT)atan(H1 / L1) * (180 / 3.14159265358979323846);
	CG_DrawRotatedPicPhysical_BO3(0x83088EC0, X, Y, L2, Width, Angle, Color, WhiteMaterial, 0);
}

void MakeLines(Vector3 Origin, FLOAT X1, FLOAT Y1, FLOAT Z1, FLOAT X2, FLOAT Y2, FLOAT Z2, Vector4& Color, int Size = 1)
{
	Vector2 Screen1, Screen2;
	Vector3 Origin1(Origin.x + X1, Origin.y + Y1, (Origin.z + Z1) - 4);
	Vector3 Origin2(Origin.x + X2, Origin.y + Y2, (Origin.z + Z2) - 4);
	if (WorldPosToScreenPos((float*)&Origin1, &Screen1) && WorldPosToScreenPos((float*)&Origin2, &Screen2)) {
		DrawLine_BO3(Screen1.x, Screen1.y, Screen2.x, Screen2.y, Size, Color);
	}
}

void DrawCheckBox(FLOAT X, FLOAT Y, BOOL Checked = FALSE)
{
	Vector4 Color;
	if (Checked)
		Color = Vector4(0, 1, 0, 1);
	else
		Color = Vector4(1, 0, 0, 1);
	DrawLine_BO3(X - 7.5, Y - 7.5, X + 7.5, Y - 7.5, 2, Color);//Top
	DrawLine_BO3(X - 7.5, Y - 7.5, X - 7.5, Y + 7.5, 2, Color);//Left Side
	DrawLine_BO3(X + 7.5, Y - 7.5, X + 7.5, Y + 7.5, 2, Color);//Right Side
	DrawLine_BO3(X - 7.5, Y + 7.5, X + 7.5, Y + 7.5, 2, Color);//Bottom

	if (Checked)
	{
		DrawLine_BO3(X - 7.5, Y - 7.5, X + 7.5, Y + 7.5, 1.5, Color);//Check
		DrawLine_BO3(X - 7.5, Y + 7.5, X + 7.5, Y - 7.5, 1.5, Color);
	}
}

void DrawMenu_BO3()
{
	FLOAT X, Y;
	X = *(DWORD*)0x83067894;
	Y = *(DWORD*)0x83067898;

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
	DrawBox_BO3(X * 0.331, Y * 0.25, menuWidth, menuHeight, Vector4(0, 0, 0, 0.420));
	DrawLine_BO3(X * 0.331, Y * 0.25, (X * 0.331) + menuWidth, Y * 0.25, 3.5, Vector4(1, 0, 0, 1));
	DrawLine_BO3(X * 0.331, Y * 0.25 + menuHeight, (X * 0.331) + menuWidth, (Y * 0.25) + menuHeight, 3.5, Vector4(1, 0, 0, 1));
	DrawLine_BO3((X * 0.331) + 115, (Y * 0.25), (X * 0.331) + 115, (Y * 0.25) + menuHeight, 3.5, Vector4(1, 0, 0, 1));

	// Wait for the animation to be done
	if (menuWidth == 420 && menuHeight == 400)
	{
		//Draw Menu Names
		for (int i = 0; i < MenuOptions; i++)
		{
			DrawText_BO3(MenuNames[i], X * 0.3328, Y * 0.2861, i, 22, HudSmallFont, (scroll == i) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25)); // Y * 0.2861
			DrawLine_BO3(X * 0.331, (Y * 0.28) + (i * 22), (X * 0.331) + 115, (Y * 0.28) + (22 * i), 3.5, Vector4(1, 0, 0, 1));
		}

		//Draw SubOptions
		for (int i = 0; i < SubMenuOptions[scroll]; i++)
		{
			if (subMenu == FALSE)
			{
				DrawText_BO3(SubMenuNames[i][scroll], X * 0.425, Y * 0.2861, i, 22, HudSmallFont, Vector4(1, 1, 1, 0.25));
			}
			else if (subMenu == TRUE && clientMenu == FALSE)
			{
				DrawText_BO3(SubMenuNames[i][scroll], X * 0.425, Y * 0.2861, i, 22, HudSmallFont, (subScroll == i) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
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

				DrawText_BO3(SubMenuNames[selectedClient][Tabindex], X * 0.425, (Y * 0.2861) + (0 * 22), 0, 0, HudSmallFont, (subMenu == TRUE) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				DrawText_BO3("Prioritize", X * 0.425, (Y * 0.2861), 1, 22, HudSmallFont, (subMenu == TRUE && subScroll == 1) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (1 * 22), ClientOptionEnabled[1][selectedClient]);

				DrawText_BO3("Whitelist", X * 0.425, (Y * 0.2861), 2, 22, HudSmallFont, (subMenu == TRUE && subScroll == 2) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (2 * 22), ClientOptionEnabled[2][selectedClient]);

				DrawText_BO3("Invert Team", X * 0.425, (Y * 0.2861), 3, 22, HudSmallFont, (subMenu == TRUE && subScroll == 3) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (3 * 22), ClientOptionEnabled[3][selectedClient]);


				DrawText_BO3("Set GT To Player's IP?", X * 0.425, (Y * 0.2861), 4, 22, HudSmallFont, (subMenu == TRUE && subScroll == 4) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));

				DrawText_BO3("Player Has Anti-Aim", X * 0.425, (Y * 0.2861), 5, 22, HudSmallFont, (subMenu == TRUE && subScroll == 5) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (5 * 22), ClientOptionEnabled[5][selectedClient]);

				BYTE* IP = GetIPFromNetInfo(selectedClient);
				DrawText_BO3(va_BO3("IP: %i.%i.%i.%i", IP[0], IP[1], IP[2], IP[3]), X * 0.425, (Y * 0.2861), 6, 22, HudSmallFont, (subMenu == TRUE && subScroll == 6) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				QWORD MachineID = GetMachineIDFromNetInfo(selectedClient);
				DWORD MachineIDPT1 = *(DWORD*)(DWORD)&MachineID;
				DWORD MachineIDPT2 = *(DWORD*)((DWORD)&MachineID + 0x04);
				DrawText_BO3(va_BO3("Machine ID: %08X%08X", MachineIDPT1, MachineIDPT2), X * 0.425, Y * 0.2861, 7, 22, HudSmallFont, (subMenu == TRUE && subScroll == 7) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
				BYTE* XUID = (BYTE*)GetXUIDFromNetInfo(selectedClient);

				DrawText_BO3(va_BO3("XUID: %08X%08X", *(DWORD*)(DWORD)&XUID[0], *(DWORD*)(DWORD)&XUID[4]), X * 0.425, Y * 0.2861, 8, 22, HudSmallFont, (subMenu == TRUE && subScroll == 8) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));


			}

			if (clientMenu)
				continue;

			if (strstr(SubMenuNames[i][scroll], "?"))
				continue;

			if (!strstr(SubMenuNames[i][scroll], ":") && scroll != 5)
				DrawCheckBox(X * 0.625, (Y * 0.2742) + (i * 22), OptionEnabled[i][scroll]);
			else if (strstr(SubMenuNames[i][scroll], ":"))
				DrawText_BO3(GetMenuOptionString(MenuNames[scroll], SubMenuNames[i][scroll]), X * 0.58, (Y * 0.2841) + (i * 22), 0, 0, HudSmallFont, (subMenu == TRUE && subScroll == i) ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.25));
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
	X = (FLOAT)cg_BO3->refdef.Width * 0.78125;
	Y = (FLOAT)cg_BO3->refdef.Height * 0.13888888888;

	RotateXY(cos(ToRadians(rotation)), sin(ToRadians(rotation)), X + (FLOAT)((FLOAT)R_TextWidth_BO3(Text, 0x7FFFFFFF, BigDevFont, 0) / 2), Y, X, Y, &X, &Y);

	rotation += 0.25;
	if (rotation >= 360.0)
		rotation -= 360.0;

	CL_DrawTextPhysical_BO3(Text, 0x7FFFFFFF, BigDevFont, X, Y, 1, 1, Vector4(1, 0, 0, 0.50), 0);
}

short SL_GetString(char* String)
{
	return ((short(*)(...))0x822D2248)(String, 0, strlen(String) + 1, 8);
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
	int DObj_s = *(short *)(0x83EC1440 + ((handle + (localClientNum * 0x4C1)) << 1));

	if (!DObj_s)
		return 0;

	return 0x83E71440 + (DObj_s * 0xA0);
}

bool GetTagPosistion(pcentity_s_BO3 centity_s, int ClientIndex, PCHAR SelectedTag, float* OutPos)
{
	//char Temp[0x100];
	short TagIndex;
	int Dobj;

	TagIndex = SL_GetString(SelectedTag);//GetTagIndex(SelectedTag, ShouldUpdateTagCache()); //SL_GetString(Temp);
	Dobj = Com_GetClientDObj(ClientIndex, 0);

	if (Dobj == 0)
		return false;

	return ((bool(*)(...))0x8221ACF0)(centity_s, Dobj, TagIndex, OutPos);
}

short TagToShort(PCHAR Tag)
{
	//char Temp[0x100];

	//for (int i = 0; i <= strlen(Tag); i++)
	//	Temp[i] = tolower(Tag[i]);

	return GetTagIndex(Tag, ShouldUpdateTagCache());
}

void DropTarget(ClientInformation_BO3 PlayerInfo)
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
	if (UserInGame() && (cg_BO3 != NULL))
	{
		if (strcmp(cg_BO3->clientInfo[RandomClient].Name, "") && (RandomClient != cg_BO3->ClientNumber) && (cg_BO3->clientInfo[RandomClient].Name != NULL))
		{
			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", cg_BO3->clientInfo[RandomClient].Name, 0x83F0A358 + 0x30);
			Cbuf_AddText_BO3(0, Output);
		}
	}
	else
	{
		if (strcmp(GetNameFromNetInfo(RandomClient), ""))
		{
			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", GetNameFromNetInfo(RandomClient), 0x83F0A358 + 0x30);
			Cbuf_AddText_BO3(0, Output);
			strcpy_s((char*)(0x83F0A358 + 4), 0x1F, GetNameFromNetInfo(RandomClient));
		}
	}
}

void RainbowName()
{
	char Output[0x200], Color[10], PreGameOutput[0x50];
	int RandomColor = rand() % 9;
	sprintf(Color, "^%i", RandomColor);

	if (UserInGame())
	{
		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", OriginalGT, Color, 0x83F0A358 + 0x30);
		Cbuf_AddText_BO3(0, Output);
	}
	else
	{
		sprintf(PreGameOutput, "^%i%s", RandomColor, OriginalGT);
		strcpy_s((char*)(0x83F0A358 + 4), 0x1F, PreGameOutput);
	}
}

void PrestigeDisco()
{
	int RandomPrestige = rand() % 11;
	if (!UserInGame())
	{
		*(BYTE*)0x83A6C151 = RandomPrestige;
	}
	else
	{
		//sprintf(Output, "userinfo \"\\name\\%s\\xuid\\%s\\natType\\1\rank\\%i\\prestige\\%i\"", OriginalGT, 0x83F0A358 + 0x30, RandomPrestige, RandomPrestige);
		//Cbuf_AddText_BO3(0, Output);
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

		sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", OriginalGT, 0x83F0A358 + 0x30);
		Cbuf_AddText_BO3(0, Output);
		strcpy_s((char*)(0x83F0A358 + 4), 0x1F, OriginalGT);
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

void HandleToggleOptions(pcgs_t_BO3 cgs_BO3)
{
	// Make sure the menu is ready
	if (!menuReady) return;

	// Update Player Names
	UpdateClientsNames();

	// Handle UAV
	if (GetMenuOptionValue("Main", "Radar"))
	{
		*(int*)0x8228BC78 = 0x60000000;
	}
	else
	{
		*(int*)0x8228BC78 = 0x419A004C;
	}

	// Handle No Recoil
	if (GetMenuOptionValue("Main", "No Recoil"))
	{
		*(int*)0x82279CB8 = 0x60000000;
	}
	else
	{
		*(int*)0x82279CB8 = 0x4BF79239;
	}

	// Handle No Sway
	if (GetMenuOptionValue("Main", "No Sway"))
	{
		*(int*)0x82201008 = 0x60000000;
		*(int*)0x822017C4 = 0x60000000;
	}
	else
	{
		*(int*)0x82201008 = 0x4BFFE659;
		*(int*)0x822017C4 = 0x4BFFFBA5;
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
	/*BOOL HearEveryoneValue = Dvar_GetBool_BO3("cg_everyoneHearsEveryone");
	if (GetMenuOptionValue("Main", "Everyone Hears Me") && (UserInGame()))
	{
		if (!HearEveryoneValue)
			Cbuf_AddText_BO3(0, "cg_everyoneHearsEveryone 1");
	}
	else if(!GetMenuOptionValue("Main", "Everyone Hears Me") && (UserInGame()))
	{
		if (HearEveryoneValue)
			Cbuf_AddText_BO3(0, "cg_everyoneHearsEveryone 0");
	}*/

	// Handle Never Host
	if (GetMenuOptionValue("Main", "Never Host"))
	{
		*(int*)0x844761A4 = 1000;
	}
	else
	{
		*(int*)0x844761A4 = 10000000;
	}

	// Handle Show Host
	if (GetMenuOptionValue("Main", "Show Host") && (UserInGame()))
	{
		if (cgs_BO3 == NULL)
			return;

		char* HostName = va_BO3("Host: %s", cgs_BO3->HostName);
		DrawText_BO3(HostName, 100, 25, 0, 0, HudSmallFont, Vector4(1, 1, 1, 1));
	}

	// Handle Custom Field Of View
	//*(float*)(*(int*)(0x829AAEB4) + 0xC) = FindFieldOfViewSize();

	// Handle End Game
	if (GetMenuOptionValue("Main", "End Game?") && (UserInGame()))
	{
		//char* EndGameString;
		//EndGameString = va_BO3("cmd lui 22 1 %i", *(int*)0x82B4F1D4);

		//Cbuf_AddText_BO3(0, EndGameString);
		SetMenuOptionValue("Main", "End Game?", 0);
	}

	// Handle Leave Game
	if (GetMenuOptionValue("Main", "Leave Game?"))
	{
		Cbuf_AddText_BO3(0, "disconnect");
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
	if (GetMenuOptionValue("Main", "Steal GT's") /*&& (UserInGame())*/)
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
	if (GetMenuOptionString("Drawing", "Crosshair Type:") != "Off" && (UserInGame()))
	{
		if (GetMenuOptionString("Drawing", "Crosshair Type:") == "+")
		{
			// + styled crosshair
			DrawLine_BO3((cg_BO3->refdef.Width / 2) - 10, cg_BO3->refdef.Height / 2, (cg_BO3->refdef.Width / 2) + 10, cg_BO3->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine_BO3(cg_BO3->refdef.Width / 2, (cg_BO3->refdef.Height / 2) - 10, cg_BO3->refdef.Width / 2, (cg_BO3->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString("Drawing", "Crosshair Type:") == "x")
		{
			// X styled crosshair
			DrawLine_BO3((cg_BO3->refdef.Width / 2) - 8, (cg_BO3->refdef.Height / 2) - 8, (cg_BO3->refdef.Width / 2) + 8, (cg_BO3->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine_BO3((cg_BO3->refdef.Width / 2) - 8, (cg_BO3->refdef.Height / 2) + 8, (cg_BO3->refdef.Width / 2) + 8, (cg_BO3->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
		}
		else if (GetMenuOptionString("Drawing", "Crosshair Type:") == "o")
		{
			DrawLine_BO3((cg_BO3->refdef.Width / 2) - 10, cg_BO3->refdef.Height / 2, (cg_BO3->refdef.Width / 2) + 10, cg_BO3->refdef.Height / 2, 2, Vector4(1, 1, 1, 1));
			DrawLine_BO3(cg_BO3->refdef.Width / 2, (cg_BO3->refdef.Height / 2) - 10, cg_BO3->refdef.Width / 2, (cg_BO3->refdef.Height / 2) + 10, 2, Vector4(1, 1, 1, 1));

			DrawLine_BO3((cg_BO3->refdef.Width / 2) - 8, (cg_BO3->refdef.Height / 2) - 8, (cg_BO3->refdef.Width / 2) + 8, (cg_BO3->refdef.Height / 2) + 8, 2, Vector4(1, 1, 1, 1));
			DrawLine_BO3((cg_BO3->refdef.Width / 2) - 8, (cg_BO3->refdef.Height / 2) + 8, (cg_BO3->refdef.Width / 2) + 8, (cg_BO3->refdef.Height / 2) - 8, 2, Vector4(1, 1, 1, 1));
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
	if (GetMenuOptionValue("Main", "Third Person") && UserInGame())
	{
		//*(BYTE*)(*(int*)(0x82A773A8) + 0xC) = 1;
	}
	else
	{
		//*(BYTE*)(*(int*)(0x82A773A8) + 0xC) = 0;
	}

	// Draw Legitbot FOV Box
	if (GetMenuOptionValue("Legitbot", "Draw FOV Box") && UserInGame() && !menuOpen && GetMenuOptionValue("Legitbot", "Enable Legitbot"))
	{
		int FOVSize = FindFOVSize();
		DrawBox_BO3((cg_BO3->refdef.Width / 2) - (FOVSize / 2), (cg_BO3->refdef.Height / 2) - (FOVSize / 2), FOVSize, FOVSize, Vector4(1, 1, 1, 0.50f));
	}

	// Handle No Flinch
	if (GetMenuOptionValue("Ragebot", "No Flinch"))
	{
		//*(int*)0x826019F4 = 0x60000000;
	}
	else
	{
		//*(int*)0x826019F4 = 0x4BFFF365;
	}

	// Handle Anti-Blacklist
	/*if (GetMenuOptionValue("Ragebot", "Anti-Blacklist") && UserInGame())
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
				char* IPName = va_BO3("%s's IP: %i.%i.%i.%i", ClientName, IP[0], IP[1], IP[2], IP[3]);
				sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", IPName, 0x83F0A358 + 0x30);
				Cbuf_AddText_BO3(0, Output);
				//strcpy_s((char*)(0x83F0A358 + 4), 0x1F, IPName);
				//strcpy((char*)(0x83F0A358 + 4), IPName);
			}
		}

		ClientOptionEnabled[4][selectedClient] = 0;
	}

	// Freeze All Consoles
	/*if (GetMenuOptionValue("Settings", "Freeze All Consoles?"))
	{
		if (UserInGame())
		{
			char Output[0x200];
			PCHAR Hax = "^\x0002NiNJA";

			sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\clanAbbrev\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", cg_BO3->clientInfo[cg_BO3->ClientNumber].Name, Hax, 0x83F0A358 + 0x30);
			Cbuf_AddText_BO3(0, Output);
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
		//*(BYTE*)0x83A6C151 = Prestige;
	}

	// Handle Jugg Class
	/*if (GetMenuOptionValue("Main", "Jugg Class?"))
	{
		char* JuggClass = va_BO3("cmd lui 5 1;wait 20;cmd mr %i 10 custom%i", *(int*)0x82B4F1D4, *(int*)0x82B4F1D4, 1);

		Cbuf_AddText_BO3(0, JuggClass);

		SetMenuOptionValue("Main", "Jugg Class?", 0);
	}*/

	// Correct Some Settings
	if (GetMenuOptionValue("Ragebot", "No Spread") && GetMenuOptionValue("Ragebot", "Auto Aim"))
	{
		SetMenuOptionValue("Ragebot", "Auto Aim", 0);
	}

	// Unlock All
	if (GetMenuOptionValue("Settings", "Unlock All?"))
	{
		// Set Unlock All Data
		//memset((LPVOID)0x83A71880, 0xFE, 0x1770);

		// Disable the menu option
		SetMenuOptionValue("Settings", "Unlock All?", 0);
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

	memcpy(viewaxis, cg_BO3->refdef.ViewAxis, sizeof(viewaxis));
	memcpy(vieworg, &cg_BO3->refdef.ViewOrg, sizeof(vieworg));
	width = (float)cg_BO3->refdef.Width;
	height = (float)cg_BO3->refdef.Height;
	tanHalfFovX = cg_BO3->refdef.Fov.x;
	tanHalfFovY = cg_BO3->refdef.Fov.y;

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

float GetBoxWidth(pcentity_s_BO3 Entity) {
	if (Entity->Flags & 0x08)
		return 35;
	else return 17;
}

float GetBoxHeight(pcentity_s_BO3 Entity) {
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
	//int weaponDef = GetWeaponDef(Weapon);
	//if (!weaponDef)
	//	return 0;
	//else
	//	return *(int*)(weaponDef + 0x778);

	return *(int*)(*(int*)(*(int*)(0x82DE5E60 + (4 * Weapon)) + 0x10) + 0x9F0);
}

void DrawSnapLine(Vector4 color, int Index, BOOL SameTeam)
{
	float xy2[2];
	Vector3 xy1, LinePos, LinePosOut;

	//Get center of screen coordinates
	xy2[0] = (float)cg_BO3->refdef.Width / 2.0f;

	if (GetMenuOptionString("Drawing", "Draw Snaplines:") == "Bottom")
		xy2[1] = (float)cg_BO3->refdef.Height;
	else if (GetMenuOptionString("Drawing", "Draw Snaplines:") == "Center")
		xy2[1] = (float)cg_BO3->refdef.Height / 2.0f;
	else if (GetMenuOptionString("Drawing", "Draw Snaplines:") == "Top")
		xy2[1] = (float)cg_BO3->refdef.Height / 10.0f - 65.0f;

	if (GetMenuOptionString("Drawing", "Draw Snaplines:") != "Top")
		LinePos = Entities_BO3[Index].Origin;
	else
	{
		GetTagPosistion(&Entities_BO3[Index], Index, "j_helmet", (float*)&LinePos);
	}

	//Get coordinates of person
	//if (WorldToScreen(LinePos, xy1) == TRUE)
	if (World2Screen(&LinePos, &xy1))
	{
		DrawLine_BO3((int)xy2[0], (int)xy2[1], (int)xy1[0], (int)xy1[1], 1.0f, color);
	}
}

void DrawArrows(float* color, int Index)
{
	Vector3 Output;
	VectorSubtract(Entities_BO3[Index].Origin, Entities_BO3[cg_BO3->ClientNumber].Origin, Output);

	//((void(*)(...))0x82211FD0)(0, cg_BO3->refdef.Width / 2.0f, cg_BO3->refdef.Height / 2.0f, &Output, color, 0.0f, 1.0f);
	((void(*)(...))0x82211FD0)(0, cg_BO3->refdef.Width / 2.0f, cg_BO3->refdef.Height / 2.0f, &Output, color, 1.0f);
}

void DrawBones(Vector4 color, int Index)
{
	Vector3 TagPos[2], TagScreen[2];
	for (int b = 0; b < ARRAYSIZE(bonesESPMW2) - 1; b++)
	{
		if (!GetTagPosistion(&Entities_BO3[Index], Index, bonesESPMW2[b], (float*)&TagPos[0]) || !GetTagPosistion(&Entities_BO3[Index], Index, bonesESPMW2[b + 1], (float*)&TagPos[1]))
			return;

		//if (WorldToScreen(TagPos[0], TagScreen[0]) && WorldToScreen(TagPos[1], TagScreen[1]))
		if (World2Screen(&TagPos[0], &TagScreen[0]) && World2Screen(&TagPos[1], &TagScreen[1]))
			DrawLine_BO3(TagScreen[0].x, TagScreen[0].y, TagScreen[1].x, TagScreen[1].y,  2.0f, color);
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
		GetTagPosistion(&Entities_BO3[index], index, ThreeDBoxesBones[t], (float*)&BonePos[t]);
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
		DrawLine_BO3(minsX, minsY, corner2X, corner2Y, 1.0f, color);
		DrawLine_BO3(minsX, minsY, corner4X, corner4Y, 1.0f, color);
		DrawLine_BO3(minsX, minsY, corner6X, corner6Y, 1.0f, color);
		//maxes to 3, 5, and 7
		DrawLine_BO3(maxesX, maxesY, corner3X, corner3Y, 1.0f, color);
		DrawLine_BO3(maxesX, maxesY, corner5X, corner5Y, 1.0f, color);
		DrawLine_BO3(maxesX, maxesY, corner7X, corner7Y, 1.0f, color);
		//from 2 to 3 and 7
		DrawLine_BO3(corner2X, corner2Y, corner3X, corner3Y, 1.0f, color);
		DrawLine_BO3(corner2X, corner2Y, corner7X, corner7Y, 1.0f, color);
		//from 4 to 3 and 5
		DrawLine_BO3(corner4X, corner4Y, corner3X, corner3Y, 1.0f, color);
		DrawLine_BO3(corner4X, corner4Y, corner5X, corner5Y, 1.0f, color);
		//from 6 to 5 and 7
		DrawLine_BO3(corner6X, corner6Y, corner5X, corner5Y, 1.0f, color);
		DrawLine_BO3(corner6X, corner6Y, corner7X, corner7Y, 1.0f, color);
	}
}

void DrawESPBox(Vector4 color, int Index)
{
	float fHeight, fWidth;
	Vector3 HeadPos, FeetLocation, HeadLocation, MainRootPos;

	if (GetMenuOptionString("Drawing", "ESP Box Type:") == "Corners")
	{
		if (!GetTagPosistion(&Entities_BO3[Index], Index, "j_head", (float*)&HeadPos))
			return;

		//if (WorldToScreen(Entities_BO3[Index].Origin, FeetLocation) && WorldToScreen(HeadPos, HeadLocation))
		if (World2Screen(&Entities_BO3[Index].Origin, &FeetLocation) && World2Screen(&HeadPos, &HeadLocation))
		{
			fHeight = FeetLocation.y - HeadLocation.y;
			if (Entities_BO3[Index].Flags & 0x8)
				fWidth = fHeight / 0.9;
			else
				fWidth = fHeight / 2;

			DrawBox_BO3((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawBox_BO3((FeetLocation.x - (fWidth / 2)), FeetLocation.y, (fWidth / 4), 1, color);
			DrawBox_BO3((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawBox_BO3((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - fHeight), 1, (fHeight / 4), color);
			DrawBox_BO3((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), (FeetLocation.y - fHeight), (fWidth / 4), 1, color);
			DrawBox_BO3((FeetLocation.x + ((fWidth / 2) - (fWidth / 4))), FeetLocation.y, (fWidth / 4), 1, color);
			DrawBox_BO3((FeetLocation.x - (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);
			DrawBox_BO3((FeetLocation.x + (fWidth / 2)), (FeetLocation.y - (fHeight / 4)), 1, (fHeight / 4), color);

		}
	}
	else if (GetMenuOptionString("Drawing", "ESP Box Type:") == "3D")
	{
		CalcBoundingBox(Index, color);
	}
	else if (GetMenuOptionString("Drawing", "ESP Box Type:") == "2D" || GetMenuOptionString("Drawing", "ESP Box Type:") == "2D Filled")
	{
	
		Vector3 MainRoot, MainRootOut, FeetOut, Feet;
		GetTagPosistion(&Entities_BO3[Index], Index, "j_mainroot", (float*)&MainRoot);
		MainRoot.z += 10;

		int BoxHeight = 55; // 8 or 0x0A = prone|| 4 or 6 = crouched||0 or 2 = standing
		if (Entities_BO3[Index].Flags & 0x8)
			BoxHeight = 17;
		else if (Entities_BO3[Index].Flags & 0x4)
			BoxHeight = 35;

		Feet.x = MainRoot.x; Feet.y = MainRoot.y; Feet.z = (MainRoot.z - -BoxHeight - 10);

		//if (WorldToScreen(MainRoot, MainRootOut) == TRUE && WorldToScreen(Feet, FeetOut) == TRUE)
		if (World2Screen(&MainRoot, &MainRootOut) == TRUE && World2Screen(&Feet, &FeetOut) == TRUE)
		{
			FLOAT Height = (MainRootOut.y - FeetOut.y);
			FLOAT Width = (Height / 3.0f);

			if (GetMenuOptionString("Drawing", "ESP Box Type:") != "2D Filled")
			{
				DrawLine_BO3((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), 1.0f, color); // Left side line
				DrawLine_BO3((int)(MainRootOut.x - Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), 1.0f, color); // Bottom line
				DrawLine_BO3((int)(MainRootOut.x + Width), (int)(MainRootOut.y - Height*0.5f), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Right side line
				DrawLine_BO3((int)(MainRootOut.x - Width), (int)(MainRootOut.y + Height), (int)(MainRootOut.x + Width), (int)(MainRootOut.y + Height), 1.0f, color); // Top line
			}
			else 
				R_AddCmdDrawStretchPic_BO3((MainRootOut.x - Width), (MainRootOut.y - (Height*0.5f)), Width*2.0f, Height*1.5f, 0.0f, 0.0f, 0.0f, 0.0f, color, WhiteMaterial);
		}
	}
	else if (GetMenuOptionString("Drawing", "ESP Box Type:") == "Pyramid")
	{
		Vector3 Center = Entities_BO3[Index].Origin;

		float W = GetBoxWidth(&Entities_BO3[Index]), H = GetBoxHeight(&Entities_BO3[Index]);

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
	if (!GetMenuOptionValue("Settings", "Disable Menu Instructions"))
	{
		if (!UserInGame())
		{
			FLOAT X, Y;
			X = *(DWORD*)0x83067894;//cgs_BO3->Width;
			Y = *(DWORD*)0x83067898;//cgs_BO3->Height;

			sprintf(Output, "Press ^BXENONButtondpadL^ & ^BXENONButtontrigL^ To %s ^7NiNJA Engine", menuOpen ? "^1Close" : "^2Open");
			DrawText_BO3(Output, (X - 380), Y - 20, 0, 0, HudSmallFont, Vector4(1, 1, 1, 0.75f));
		}
		else
		{
			sprintf(Output, "Press ^BXENONButtondpadL^ & ^BXENONButtontrigL^ To %s ^7NiNJA Engine", menuOpen ? "^1Close" : "^2Open");
			DrawText_BO3(Output, (cg_BO3->refdef.Width - 1250), cg_BO3->refdef.Height - 20, 0, 0, HudSmallFont, Vector4(1, 1, 1, 0.75f));
		}
	}
}

double Radians(float Degrees)
{
	return  0.01745329238474369 * Degrees;
}

int TransformSeed(int* time)
{
	/*int s = (*seed & 0xFF);
	for (int i = 0; i < 3; i++)
	{
		s = (s << 8);
		*seed ^= s;
	}*/

	int iSeed = *time, start;
	start = 0;
	__asm
	{
		clrlwi    start, iSeed, 24
	}
	for (int i = 0; i<3; i++)
	{
		__asm
		{
			slwi  start, start, 8
			xor iSeed, start, iSeed
		}
	}
	*time = iSeed;
	return (int)time;
}

double BG_random_BO3(int *seed)
{
	int ye = 0x343FD * *seed + 0x269EC3;
	*seed = ye;
	return (double)(ye >> 17) * 0.000030517578;
}

void RandomBulletDir_BO3(int *randSeed, float *x, float *y) 
{
	float theta = (BG_random_BO3(randSeed) * 360.0f) * 0.01745329251;
	float radius = BG_random_BO3(randSeed);
	*x = radius * cos(theta);
	*y = radius * sin(theta);
}

int GetWeaponDef_BO3(int weaponNum)
{
	int weapDef = *(int*)(0x82DE5E60 + (weaponNum * 4));
	DbgPrint("Weapon Def: 0x%08X", weapDef);

	if (weapDef != 0)
		return *(int*)(weapDef + 0x10);
	else
		return 0;
}

void GetSpread_BO3(int* seed, float* x, float* y)
{
	float min, max, spread, x2, y2;

	if (cg_BO3->weaponNum == 0)
		return;

	int unknownStruct = *(int*)0x844C1BC0;
	int weaponNum = *(int*)((DWORD)&Entities_BO3[cg_BO3->ClientNumber] + 0x1F8);
	//BG_GetSpreadForWeapon_BO3(unknownStruct, cg_BO3->weaponNum, &min, &max);//0x82272C18
	((void(*)(...))0x821DC870)(unknownStruct, weaponNum, &min, &max);
	spread = (max - min) * (*(FLOAT*)((DWORD)cg_BO3 + 0x675A0) * 0.00392156862) + min;

	TransformSeed(seed);
	RandomBulletDir_BO3(seed, &x2, &y2);
	*x = x2 * spread;
	*y = y2 * spread;
}

Vector3 SmoothAimbotAngles(Vector3 AimbotAngles, float SmoothValue)
{
	float FinishedAngles[3]; float smoothdiff[3];
	AimbotAngles[0] -= LocalClient_BO3->ViewAngles.x;
	AimbotAngles[1] -= LocalClient_BO3->ViewAngles.y;
	if (AimbotAngles[0] > 180)  AimbotAngles[0] -= 360;
	if (AimbotAngles[1] > 180)  AimbotAngles[1] -= 360;
	if (AimbotAngles[0] < -180) AimbotAngles[0] += 360;
	if (AimbotAngles[1] < -180) AimbotAngles[1] += 360;
	smoothdiff[0] = AimbotAngles[0] / SmoothValue;
	smoothdiff[1] = AimbotAngles[1] / SmoothValue;
	FinishedAngles[0] = LocalClient_BO3->ViewAngles.x + smoothdiff[0];
	FinishedAngles[1] = LocalClient_BO3->ViewAngles.y + smoothdiff[1];
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

float R_NormalizedTextScale_BO3(int font, float scale)
{
	return((scale * 48) / ((*(int*)(font + 0x4))));
}

void FixTextForDistance(CONST PCHAR Text, float* x, float* y, DWORD Font, float scale, Vector3 WorldPos, float* OutDist)
{
	float distanceSq = cg_BO3->refdef.ViewOrg.Distance(WorldPos);
	float distance = sqrt(distanceSq);
	float distFrac = (float)(distance - 64) / 448;
	float distScale = (float)(distFrac * scale) + (float)(1 - distFrac);
	*OutDist = distFrac;
	float x2 = (FLOAT)(floor(-((((FLOAT)R_TextWidth_BO3(Text, 0x7FFFFFFF, Font, 0) * R_NormalizedTextScale_BO3(Font, scale * distScale)) * 0.4) - *x) + 0.4));

	__asm
	{
		frsp x2, x2
	}
	*x = x2;
	*y = (FLOAT)(floor(*y + 0.4));
}

bool IsWeaponDualWield()
{
	return false;//return ((bool(*)(...))0x82641898)(Entities_BO3[cg_BO3->ClientNumber].weaponNum);
}

CONST PCHAR GetWeaponHUDName_BO3(int weaponNum)
{
	int weaponsArray = *(int*)0x82DFCF50;
	return (CONST PCHAR)(*(int*)(weaponsArray + (weaponNum * 0x1C) + 0x10));
}

bool Autowall(int Index, PCHAR TagToUse, BOOL UsingShield)
{
	Vector3 TagPos, Angles, dir, Hax[3], MyPos;
	BYTE bulletTraceResults[100];

	if (!GetMenuOptionValue("Ragebot", "Auto Wall"))
		return false;

	if (GetMenuOptionValue("Ragebot", "Target Shields") && UsingShield)
		TagToUse = "j_ankle_le";

	if (!GetTagPosistion(&Entities_BO3[Index], Index, TagToUse, (float*)&TagPos))
		return false;

	//CG_GetPlayerViewOrigin_BO3(0, cg_BO3, MyPos);
	//MyPos = cg_BO3->refdef.ViewOrg;
	MyPos = *(Vector3*)GetViewOriginPointer();


	BulletFireParams bulletFireParams;
	bulletFireParams.ignoreEntIndex = cg_BO3->ClientNumber;
	bulletFireParams.weaponEntIndex = 702;
	bulletFireParams.damageMultiplier = 1.0f;
	bulletFireParams.methodOfDeath = 2;
	VectorCopy(MyPos, bulletFireParams.origStart);
	VectorCopy(MyPos, bulletFireParams.start);
	VectorCopy(TagPos, bulletFireParams.end);
	VectorSubtract(TagPos, MyPos, dir);
	Angles = VectorToAngles(dir);
	
	AngleVectors(Angles, &Hax[0], NULL, NULL);
	VectorCopy(Hax[0], bulletFireParams.dir);

	// Disable visual effects
	*(short*)0x8269EBE0 = 0x4800;
	*(int*)0x8227165C = 0x48000084;

	// Allow 360 degree autowall checks
	DWORD restoreVal = *(DWORD*)0x82272960;
	*(DWORD*)0x82272960 = 0x38600001;

	FireBulletPenetrate_BO3(0, &bulletFireParams, Entities_BO3[cg_BO3->ClientNumber].weaponNum, 0, &MyPos, 0, &bulletTraceResults);

	// Unpatch 360 degree autowall check
	*(DWORD*)0x82272960 = restoreVal;

	// Restore visual effects
	*(short*)0x8269EBE0 = 0x4099;
	*(int*)0x8227165C = 0x41820040;

	return (bulletFireParams.ignoreEntIndex == Index);
}

void CorrectMovement(usercmd_s_BO3 *usercommand, Vector3 vec)
{
	Vector3 Fix = vec - (LocalClient_BO3->ViewAngles);

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
	Vector3 FeetOrigin = Entities_BO3[Index].Origin;
	GetTagPosistion(&Entities_BO3[Index], Index, "j_head", (float*)&HeadOrigin);
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
			GetTagPosistion(&Entities_BO3[Index], Index, TriggerBotBones[b], (float*)&Hitboxes);
			if (WorldPosToScreenPos((float*)&Hitboxes, &ScreenPos))
			{
				if (ScreenPos.x - ((fWidth / 5) / 2) < (cg_BO3->refdef.Width / 2) &&
					ScreenPos.x + ((fWidth / 5) / 2) > (cg_BO3->refdef.Width / 2) &&
					ScreenPos.y - ((fWidth / 5) / 2) < (cg_BO3->refdef.Height / 2) &&
					ScreenPos.y + ((fWidth / 5) / 2) > (cg_BO3->refdef.Height / 2))
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
	GetTagPosistion(&Entities_BO3[index], index, Tag, Output);
	if (WorldPosToScreenPos(Output, &ScreenPos))
	{
		if (ScreenPos.x > ((cg_BO3->refdef.Width / 2) - FOVSize) &&
			ScreenPos.x < ((cg_BO3->refdef.Width / 2) + FOVSize) &&
			ScreenPos.y >((cg_BO3->refdef.Height / 2) - FOVSize) &&
			ScreenPos.y < ((cg_BO3->refdef.Height / 2) + FOVSize))
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
		if ((cg_BO3->clientInfo[myClientNum].Team == cg_BO3->clientInfo[ClientIndex].Team) && (cg_BO3->clientInfo[myClientNum].Team != 0))
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
		if ((cg_BO3->clientInfo[myClientNum].Team == cg_BO3->clientInfo[ClientIndex].Team) && (cg_BO3->clientInfo[myClientNum].Team != 0))
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
	if (!Entities_BO3[ClientIndex].IsAlive())
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
	CG_CompassCalcDimensions_BO3(0, cg_BO3, size, radarHud, &RadarSize.x, &RadarSize.y, &RadarSize.z, &RadarSize.a);

	float x2 = (RadarSize.z * 0.5) + RadarSize.x;
	float y2 = (RadarSize.a * 0.5) + RadarSize.y;

	Vector2 out, outClipped, aw;

	CG_CompassUpYawVector_BO3(cg_BO3, aw);

	CG_WorldPosToCompass_BO3(0, cg_BO3, RadarSize, aw, Entities_BO3[cg_BO3->ClientNumber].Origin, World, out, outClipped);

	Compass.x = -((widthHeight.x * 0.5) - x2) + outClipped.x;
	Compass.y = -((widthHeight.y * 0.5) - y2) + outClipped.y;
}

void DrawRadar_BO3()
{
	CRadarHud radarHud;
	Vector2 size;
	DWORD compassPingFriendly = *(DWORD*)0x82ACD048;
	size.x = 0;//ScaleRadarX(12, cg_BO3->refdef.Width);
	size.y = 0;//ScaleRadarY(12, cg_BO3->refdef.Height);
	radarHud.x = ScaleRadarX(12, cg_BO3->refdef.Width);
	radarHud.y = ScaleRadarY(12, cg_BO3->refdef.Height);
	radarHud.w = 178.667;
	radarHud.h = 93.3333;
	radarHud.unk = 0x0C0D0000;

	DrawEngineRadar_BO3(0, 0, size, &radarHud, WhiteMaterial, Vector4(1, 1, 1, 1));

	// Draw Dropped Items
	for (int i = 100; i < 800; i++)
	{
		if (GetMenuOptionValue("Drawing", "Draw Dropped Items"))
		{
			if ((*(int*)((DWORD)&Entities_BO3[i] + 0x130) & 0x20) && Entities_BO3[i].Type == 3)
			{
				Vector2 Screen;
				Vector3 Origin = Entities_BO3[i].Origin;

				// Make sure it's not a scavenger pack
				if ((Entities_BO3[i].weaponNum != CG_GetWeaponIndexForName("scavenger_bag_mp")))
					continue;


				Vector2 Compass, widthHeight;
				widthHeight.x = 25;
				widthHeight.y = 15;
				WorldToCompass_Mw2(&radarHud, size, Entities_BO3[i].Origin, Compass, widthHeight);

				int horz = radarHud.unk >> 24;
				int vert = (radarHud.unk >> 16) & 0xFF;

				//float unk = sub_8228C068(*(FLOAT*)((DWORD)cg_Mw2 + 0x6EB8C) - *(FLOAT*)0x8259C334);

				CG_DrawRotatedPic_BO3(0x83088EC0, Compass.x, Compass.y, widthHeight.x, widthHeight.y, horz, vert, 0, Vector4(1, 1, 1, 0.75f), DB_FindXAssetHeader_BO3(material, "scavenger_pickup", 1, -1));
			}
		}
	}

	// Draw Enemies
	for (int client = 0; client < 18; client++)
	{
		if (client == cg_BO3->ClientNumber || Entities_BO3[client].Type != 1 || (*(int*)((DWORD)&Entities_BO3[client] + 0x130) & 0x20) || (*(int*)((DWORD)&Entities_BO3[client] + 0x1D8) & 0x1) == 0)
			continue;

		Vector2 Compass, widthHeight;
		widthHeight.x = 16;
		widthHeight.y = 16;
		WorldToCompass_Mw2(&radarHud, size, Entities_BO3[client].Origin, Compass, widthHeight);

		int horz = radarHud.unk >> 24;
		int vert = (radarHud.unk >> 16) & 0xFF;

		//float angle = AngleNormalize360_BO3(*(FLOAT*)((DWORD)cg_BO3 + 0x720BC) - Entities_BO3[client].Angles.y);
		//CG_DrawRotatedPic_BO3(0x83088EC0, Compass.x, Compass.y, widthHeight.x, widthHeight.y, horz, vert, angle, !IsPlayerEnemy(client, cg_BO3->ClientNumber) ? Vector4(0, 1, 0, 0.75f) : Vector4(1, 0, 0, 0.75f), compassPingFriendly);
	}

	//DrawEnemies_Mw2(0, 0, size, &radarHud, Vector4(1,0,0,1));
	//DrawFriendlies_BO3(0, 0, size, &radarHud, Vector4(0, 1, 0, 0.75f));
	DrawVehicles_BO3(0, 0, size, &radarHud, Vector4(0, 0, 1, 0.75f));
	DrawPlayer_BO3(0, 0, size, &radarHud, Vector4(1, 1, 0, 1));
}

void CG_EntityEventHook(int localClient, int entityState_t, int Event, int eventParam)
{
	if (GetMenuOptionValue("Main", "Fast Reload"))
	{
		if (Event == 19)
		{
			int ClientReloadIndex = *(int*)(entityState_t + 0x258);
			if (ClientReloadIndex == cg_BO3->ClientNumber)
				WasReloading = TRUE;
		}
		else if (Event == 23)
		{
			int ClientSwitchIndex = *(int*)(entityState_t + 0x258);
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
	if (GetMenuOptionValue("Main", "Fast Reload") && UserInGame())
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
	LocalClient_BO3 = (pclientActive_t_BO3)GetPointer(0x84475F74);//tu17
	Entities_BO3 = (pcentity_s_BO3)GetPointer(0x82E819EC);//tu17
	cg_BO3 = (pcg_s_BO3)GetPointer(0x82E819D8);//tu17
	cgs_BO3 = (pcgs_t_BO3)GetPointer(0x82E819C0);//tu17

	// If in-game, start our ESP
	if (UserInGame() && menuReady && (cgs_BO3 != NULL))
	{
		// NiNJA Engine v1.0
		if (!GetMenuOptionValue("Settings", "Disable NiNJA Engine Text"))
			DrawAd();

		// Loop for all ents (this 1st so our player ESP Draws over it if they over lap)
		for (int i = 18; i < 0x7FF; i++)
		{
			// Draw Explosives
			if (GetMenuOptionValue("Drawing", "Draw Explosives"))
			{
				// Make sure it's a explosive
				if (((*(int*)((DWORD)&Entities_BO3[i] + 0x638) & 0x40000000) == 0 && Entities_BO3[i].Type == 4))
				{
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					TopLocation = Entities_BO3[i].Origin;
					//if (WorldToScreen(TopLocation, TopLocationOut))
					if (World2Screen(&TopLocation, &TopLocationOut))
					{
						// Get the explosive id
						int ExplosiveID = Entities_BO3[i].weaponNum;

						// Make sure it's valid
						if (ExplosiveID != 0)
						{
							int ExplosiveIcon = GetWeaponIcon(ExplosiveID);
							if (ExplosiveIcon != 0 && MmIsAddressValid((DWORD*)ExplosiveIcon))
							{
								CG_DrawRotatedPicPhysical_BO3(0x83088EC0, TopLocationOut.x - 30, TopLocationOut.y - 10, 20, 20, 0.0, GetColorCode(colorYellow), ExplosiveIcon, 0);
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
				if (((*(int*)((DWORD)&Entities_BO3[i] + 0x638) & 0x40000000) == 0 && Entities_BO3[i].Type == 3))
				{
					Vector3 TopLocation, TopLocationOut, Mins, Maxs;
					TopLocation = Entities_BO3[i].Origin; 
					//if (WorldToScreen(TopLocation, TopLocationOut))
					if (World2Screen(&TopLocation, &TopLocationOut))
					{
						// Get the dropped item id
						int DroppedWeapon = Entities_BO3[i].weaponNum;

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
								CG_DrawRotatedPicPhysical_BO3(0x83088EC0, TopLocationOut.x - 30, TopLocationOut.y - 10, 25, 30, 0.0, color, DroppedIcon, 0);
							}
						}
						else
						{
							CG_DrawRotatedPicPhysical_BO3(0x83088EC0, TopLocationOut.x - 30, TopLocationOut.y - 10, 25, 30, 0.0, color, DB_FindXAssetHeader_BO3(material, "scavenger_pickup", 1, -1), 0);
						}
					}
				}
			}
		}

		// Loop for all clients
		for (int i = 0; i < cgs_BO3->MaxClients; i++)
		{
			// Make sure the client is not us
			if (Entities_BO3[i].ClientNumber != cg_BO3->ClientNumber && cg_BO3->ClientNumber != i)
			{
				// Alive and is player
				if (!Entities_BO3[i].IsAlive())
					continue;

				// Set some of the players info in our struct
				PlayerInfo[i].Alive = TRUE;
				PlayerInfo[i].ClientNumber = Entities_BO3[i].ClientNumber;
				PlayerInfo[i].Name = cg_BO3->clientInfo[i].Name;

				// Team check
				if (!GetMenuOptionValue("Settings", "Invert Team Check"))
				{
					if ((cg_BO3->clientInfo[cg_BO3->ClientNumber].Team == cg_BO3->clientInfo[i].Team) && (cg_BO3->clientInfo[cg_BO3->ClientNumber].Team != 0))
					{
						// Save players team
						PlayerInfo[i].Team = cg_BO3->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = TRUE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;

						// If we want to Draw friendlies
						if (!GetMenuOptionValue("Drawing", "Draw Friendlies"))
							continue;
					}
					else
					{
						// If we want to Draw enemies
						if (!GetMenuOptionValue("Drawing", "Draw Enemies"))
							continue;

						PlayerInfo[i].Team = cg_BO3->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = FALSE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
					}
				}
				else
				{
					// Reverse team check
					if ((cg_BO3->clientInfo[cg_BO3->ClientNumber].Team == cg_BO3->clientInfo[i].Team) && (cg_BO3->clientInfo[cg_BO3->ClientNumber].Team != 0))
					{
						// If we want to Draw enemies
						if (!GetMenuOptionValue("Drawing", "Draw Enemies"))
							continue;

						PlayerInfo[i].Team = cg_BO3->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = FALSE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;
					}
					else
					{
						// Save players team
						PlayerInfo[i].Team = cg_BO3->clientInfo[i].Team;
						PlayerInfo[i].OnTeam = TRUE;

						// Invert individual player team check
						if (ClientOptionEnabled[3][i])
							PlayerInfo[i].OnTeam = !PlayerInfo[i].OnTeam;

						// If we want to Draw friendlies
						if (!GetMenuOptionValue("Drawing", "Draw Friendlies"))
							continue;
					}
				}

				// Visible / Wallbang Check
				if (!PlayerInfo[i].OnTeam)
				{
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_BO3(0, &Entities_BO3[i], TagToShort("j_head"));

					// Get the players posistion for our visible check
					GetTagPosistion(&Entities_BO3[i], i, "j_head", (float*)&PlayerInfo[i].TagPosistion);

					// Run our trace
					trace_t Trace;
					CG_LocationalTraceBO3(&Trace, &cg_BO3->refdef.ViewOrg, &PlayerInfo[i].TagPosistion, i, 0x818011, false, 0);

					// If the trace fraction is >= 0.97 let's say they are visible
					PlayerInfo[i].VisibleESP = ((Trace.Fraction >= 0.97f) ? TRUE : FALSE);
					//PlayerInfo[i].Visible = AimAssist_IsTargetVisible_BO3(0, &Entities_BO3[i], TagToShort("j_head"));

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
					GetTagPosistion(&Entities_BO3[i], i, "j_head", (float*)&Pos[1]);
					FeetPos = Entities_BO3[i].Origin;

					//if (WorldToScreen(Pos[1], xy1[0]) && WorldToScreen(FeetPos, FeetPosOut))
					if (CG_CalcNamePosition(0, &Pos[1], &xy1[0].x, &xy1[0].y))
					{
						//int WeaponIcon = GetWeaponIcon(Entities_BO3[i].weaponNum);
						//if (WeaponIcon != 0 && MmIsAddressValid((DWORD*)WeaponIcon))
						//{
						//int test = DB_FindXAssetHeader_BO3(material, "t7_icon_weapon_ar_standard_kf", 1, -1);
						//DbgPrint("Test: 0x%08X", test);
						//	CG_DrawRotatedPicPhysical_BO3(0x83088EC0, xy1[0].x - 60 / 2.0f, xy1[0].y - 25, 50, 20, 0.0, Color, test, 0);
						//}

						//PCHAR WeaponName = GetWeaponHudName(Entities_BO3[i].CurrentWeapon);
						//if (MmIsAddressValid((PCHAR*)WeaponName))
						//	DrawText_BO3(WeaponName, (xy1[0].x - (R_TextWidth_BO3(WeaponName, 0x7FFFFFFF, SmallFont) / 2) * 0.5f), (xy1[0].y), 0, 0, SmallFont, Vector4(1, 1, 1, 0.50f), 0.5f, 0.5f);
					}
				}

				// ESP Name
				if (GetMenuOptionValue("Drawing", "Draw Name"))
				{
					Vector3 HeadPos, HeadPosOut, FeetPos, FeetPosOut;
					GetTagPosistion(&Entities_BO3[i], i, "j_head", (float*)&HeadPos);
					FeetPos = Entities_BO3[i].Origin;

					//if (WorldToScreen(HeadPos, HeadPosOut) && WorldToScreen(FeetPos, FeetPosOut))
					if (CG_CalcNamePosition(0, &HeadPos, &HeadPosOut.x, &HeadPosOut.y) && CG_CalcNamePosition(0, &FeetPos, &FeetPosOut.x, &FeetPosOut.y))
					{
						if (MmIsAddressValid((PCHAR*)cg_BO3->clientInfo[i].Name))
						{
							float DisanceOut;
							Vector3 FeetPosOutOG, HeadPosOutOG;
							VectorCopy(FeetPosOut, FeetPosOutOG);
							VectorCopy(HeadPosOut, HeadPosOutOG);

							FixTextForDistance(cg_BO3->clientInfo[i].Name, &FeetPosOut.x, &FeetPosOut.y, SmallFont, 1, FeetPos, &DisanceOut);
							//FeetPosOut.y -= ((HeadPosOutOG.y - FeetPosOutOG.y) * 0.750);
							FeetPosOut.y += (((FeetPosOutOG.y - HeadPosOutOG.y) * (DisanceOut * 0.0001f) + 15));

							DrawText_BO3(cg_BO3->clientInfo[i].Name, (FeetPosOutOG.x - (R_TextWidth_BO3(cg_BO3->clientInfo[i].Name, 0x7FFFFFFF, SmallFont, 0) / 2) * 0.4f), FeetPosOut.y, 0, 0, SmallFont, Color, 0.4f, 0.4f);
						}
					}
				}

				// Enemy Visible Text
				if (GetMenuOptionValue("Drawing", "Draw Enemy Visible Text"))
				{
					if (PlayerInfo[i].VisibleESP && !PlayerInfo[i].OnTeam)
					{
						DrawText_BO3("Visible Enemy!", ((cg_BO3->refdef.Width / 2) - (R_TextWidth_BO3("Visible Enemy!", 0x7FFFFFFF, HudSmallFont, 0) / 2) * 0.7f), (cg_BO3->refdef.Height / 2) - 120, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
					}
				}

				// Proximity Warning Text
				if (GetMenuOptionValue("Drawing", "Draw Proximity Warning"))
				{
					if (!PlayerInfo[i].OnTeam)
					{
						float OutPos[3];
						GetTagPosistion(&Entities_BO3[i], i, "j_head", OutPos);
						if ((GetDistance(cg_BO3->refdef.ViewOrg, OutPos) / 100 < 5))
						{
							DrawText_BO3("Proximity Warning!", ((cg_BO3->refdef.Width / 2) - (R_TextWidth_BO3("Proximity Warning!", 0x7FFFFFFF, HudSmallFont, 0) / 2) * 0.7f), (cg_BO3->refdef.Height / 2) - 170, 0, 0, HudSmallFont, Vector4(1, 0.128f, 0, 0.75f));
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
			//DrawRadar_BO3();
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
	LocalClient_BO3 = (pclientActive_t_BO3)GetPointer(0x84475F74);//tu17
	Entities_BO3 = (pcentity_s_BO3)GetPointer(0x82E819EC);//tu17
	cg_BO3 = (pcg_s_BO3)GetPointer(0x82E819D8);//tu17
	cgs_BO3 = (pcgs_t_BO3)GetPointer(0x82E819C0);//tu17

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
		DrawMenu_BO3();
	}
	else
	{
		menuWidth = 0;
		menuHeight = 0;
	}

	// Handles toggle options like no recoil, sway, uav, etc
	HandleToggleOptions(cgs_BO3);

	// Draw our NiNJA Engine v1.0 text at the top right of screen pre-game
	if (!UserInGame() && menuReady)
	{
		if (!GetMenuOptionValue("Settings", "Disable NiNJA Engine Text"))
		{
			FLOAT X, Y;
			X = *(DWORD*)0x83067894;//cgs_BO3->Width;
			Y = *(DWORD*)0x83067898;//cgs_BO3->Height;
			DrawText_BO3("NiNJA Engine v1.0", ((X - 130) - (R_TextWidth_BO3("NiNJA Engine v1.0", 0x7FFFFFFF, SmallFont, 0) / 2) * 0.7f), (Y / 2) - 330, 0, 0, SmallFont, Vector4(0.102f, 1, 0.102f, 0.50f), 0.7f, 0.7f);
		}
	}

	// Draw our menu controls
	DrawMenuControlsText();

	// Call the original function
	R_EndFrameDetour->CallOriginal();
}

int Menu_PaintAllHook(QWORD r3)
{
	LocalClient_BO3 = (pclientActive_t_BO3)GetPointer(0x84475F74);//tu17
	Entities_BO3 = (pcentity_s_BO3)GetPointer(0x82E819EC);//tu17
	cg_BO3 = (pcg_s_BO3)GetPointer(0x82E819D8);//tu17
	cgs_BO3 = (pcgs_t_BO3)GetPointer(0x82E819C0);//tu17

	// Call the original
	int Original = Menu_PaintAllDetour->CallOriginal(r3);

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
		GetTagPosistion(&Entities_BO3[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		//CG_GetPlayerViewOrigin_BO3(0, cg_BO3, MyPos);
		CG_LocationalTraceBO3(&Trace, &MyPos, &VisPos, EntIndex, 0x818011, false, 0);

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
		GetTagPosistion(&Entities_BO3[EntIndex], EntIndex, VisCheckBones[i], (float*)&VisPos);

		// Run our trace
		//CG_GetPlayerViewOrigin_BO3(0, cg_BO3, MyPos);
		CG_LocationalTraceBO3(&Trace, &MyPos, &VisPos, EntIndex, 0x818011, false, 0);

		// Check our trace fraction
		if (Trace.Fraction >= 0.95f)
		{
			// Get weapon def
			//PDWORD WeaponDef = BG_GetWeaponDef_BO3(Entities_BO3[cg_BO3->ClientNumber].weaponNum);

			// Get our temp damage from weaponDamage * HitLocationMultiplier
			//float tempDamage = (float)*(int*)(WeaponDef + 0x230) * G_GetWeaponHitLocationMultiplier(HitLocationIndexs[i], Entities_BO3[cg_BO3->ClientNumber].weaponNum);
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
		GetTagPosistion(&Entities_BO3[EntIndex], EntIndex, "j_ankle_le", (float*)&VisPos);

		// Run our trace
		//CG_GetPlayerViewOrigin_BO3(0, cg_BO3, MyPos);
		MyPos = *(Vector3*)GetViewOriginPointer();
		CG_LocationalTraceBO3(&Trace, &MyPos, &VisPos, EntIndex, 0x818011, false, 0);

		// Check our trace fraction
		if (Trace.Fraction >= 0.95f)
			return 6;

		return -1;
	}

	for (int i = 0; i < 8; i++)
	{
		// If the player has anti-aim, skip the head
		if (ClientOptionEnabled[5][EntIndex] && (i == 0))
			continue;

		// Get the players posistion for our visible check
		GetTagPosistion(&Entities_BO3[EntIndex], EntIndex, VisCheckBones[i], (float*)&VisPos);

		// Run our trace
		//CG_GetPlayerViewOrigin_BO3(0, cg_BO3, MyPos);
		//MyPos = cg_BO3->refdef.ViewOrg;
		MyPos = *(Vector3*)GetViewOriginPointer();
		CG_LocationalTraceBO3(&Trace, &MyPos, &VisPos, EntIndex, 0x818011, false, 0);

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
	//		if (!(Entities_BO3[ClientIndex].weaponNum == CG_GetWeaponIndexForName("riotshield_mp")))
	//		{
	//			return true;
	//		}
	//
	//		return false;
	//	}
	//}
	return false;
}

DWORD FindBestTarget(BOOL UsingRagebot)
{
	DWORD dwClosestClient = -1;
	FLOAT Closest = 900000;
	PCHAR AimbotMenu = UsingRagebot ? "Ragebot" : "Legitbot";

	// Loop for all players
	for (int i = 0; i < cgs_BO3->MaxClients; i++) //
	{
		BOOL Visible = FALSE, Wallbangable = FALSE, UsingShield = FALSE;
		//CHAR PlayerWeaponName[100];

		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_BO3->ClientNumber, UsingRagebot))
			continue;

		// Check the user for a shield
		//GetWeaponHUDName_BO3(Entities_BO3[i].weaponNum, 0, PlayerWeaponName);
		//if (strstr(PlayerWeaponName, "riotshield"))
		//	UsingShield = TRUE;

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
				AimTag = VisCheckBones[BoneIndex];
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
			GetTagPosistion(&Entities_BO3[i], i, AimTag, (float*)&VisPos);

			// Run our trace
			trace_t Trace;
			//MyPos = cg_BO3->refdef.ViewOrg;
			//CG_GetPlayerViewOrigin_BO3(0, cg_BO3, MyPos);
			MyPos = *(Vector3*)GetViewOriginPointer();
			CG_LocationalTraceBO3(&Trace, &MyPos, &VisPos, i, 0x818011, false, 0);

			// Check our trace fraction
			Visible = ((Trace.Fraction >= 0.95f) ? TRUE : FALSE);
			if (!Visible && UsingRagebot)
			{
				// If they are prioritized, check multiple bones
				if (ClientOptionEnabled[1][i] && (GetMenuOptionString("Ragebot", "Aim Bone:") == "Best"))
				{
					// Wallbang check 8 bones for prioritized users
					int BO3allBoneIndex = FindBestAutoWallBoneIndex(i, UsingShield);
					if (BO3allBoneIndex == -1)
					{
						Wallbangable = FALSE;
					}
					else
					{
						AimTag = AutoWallBones[BO3allBoneIndex];
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
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_BO3[i].Origin, (float*)&cg_BO3->refdef.ViewOrg));

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
	for (int i = 0; i < cgs_BO3->MaxClients; i++)
	{
		BOOL Visible = FALSE, Wallbangable = FALSE, UsingShield = FALSE;

		// Make sure it's a valid player
		if (!ValidPlayer(i, cg_BO3->ClientNumber, NULL))
			continue;

		// Grab our temp distance
		FLOAT TempDistance = sqrt(Vec3DistanceSq((float*)&Entities_BO3[i].Origin, (float*)&cg_BO3->refdef.ViewOrg));

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

VOID Ragebot(pusercmd_s_BO3 usercommand, pusercmd_s_BO3 nextusercommand, pusercmd_s_BO3 oldusercommand, float movementsave)
{
	Vector3 EnemyPosistion, MyOrigin, Angles, VectorAngles, FinalAngles, SpreadAngles;
	PCHAR AimbotTag = "j_head";
	//CHAR TargetWeaponName[100];
	int unknownStruct = *(int*)0x844C1BC0;

	// Get our best target
	DWORD BestTarget = FindBestTarget(TRUE);
	GClosestClient = BestTarget;
	if (BestTarget == -1)
		return;

	// Use Right Ankle for shield users
	//DbgPrint("Weapon Name: %s", GetWeaponHUDName_BO3(Entities_BO3[cg_BO3->ClientNumber].weaponNum));
	//if (strstr(TargetWeaponName, "shield") && (GetMenuOptionValue("Ragebot", "Target Shields") || GetMenuOptionValue("Legitbot", "Target Shields")))
	//	AimTag = "j_ankle_ri";

	// Use Left Ankle for anti-aim shield users
	if (IsSecondaryShield(BestTarget) && (ClientOptionEnabled[5][BestTarget]))
		AimTag = "j_ankle_le";

	// Get the best targets posistion
	GetTagPosistion(&Entities_BO3[BestTarget], BestTarget, AimTag, (float*)&EnemyPosistion);
	//AimAssist_GetTagPos_BO3(&Entities_BO3[BestTarget], TagToShort(AimTag), EnemyPosistion);

	// Get our view origin
	CG_GetPlayerViewOrigin_BO3(cg_BO3->ClientNumber, unknownStruct, MyOrigin);

	// Calculate aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles);
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_BO3->SpawnAngles, FinalAngles);
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
		LocalClient_BO3->ViewAngles[0] = FinalAngles.x;
		LocalClient_BO3->ViewAngles[1] = FinalAngles.y;
	}

	// Do our no spread
	if (GetMenuOptionValue("Ragebot", "No Spread"))
	{
		// Get our seed from the current server time
		int Seed = nextusercommand->ServerTime;//*(int*)unknownStruct;

		// If dual wield, +10 to the seed (thanks brian)
		//if (IsWeaponDualWield())
		//	Seed += 10;

		// Calculate zero spread
		GetSpread_BO3(&Seed, &SpreadAngles.x, &SpreadAngles.y);

		// Apply our no spread
		usercommand->ViewAngles[0] -= ANGLE2SHORT(-SpreadAngles.y);
		usercommand->ViewAngles[1] -= ANGLE2SHORT(-SpreadAngles.x);

	}

	// Autoshoot
	if (GetMenuOptionValue("Ragebot", "Auto Shoot"))
	{
		if (GetMenuOptionValue("Ragebot", "No Spread"))
		{
			nextusercommand->Buttons |= 0x80000000;
			if (usercommand->Buttons & 0x80000000)
			{
				usercommand->Buttons &= 0x7FFFFFFFu;
			}

			//if (IsWeaponDualWield())
			//	usercommand->Buttons |= 0x00080800;
		}
		else
		{
			// If not using no spread, wait until aimed in before shooting
			//if (*(float*)((DWORD)cg_BO3 + 0x424) >= 0.9f)
			//{
			
			nextusercommand->Buttons |= 0x80000000;
			if (usercommand->Buttons & 0x80000000)
			{
				usercommand->Buttons &= 0x7FFFFFFFu;
			}

			//	if (IsWeaponDualWield())
			//		usercommand->Buttons |= 0x00080800;
			//}
		}
	}
}

VOID Legitbot(pusercmd_s_BO3 usercommand, pusercmd_s_BO3 nextusercommand)
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
	GetTagPosistion(&Entities_BO3[BestTarget], BestTarget, GetMenuOptionString("Legitbot", "Aim Bone:"), (float*)&EnemyPosistion);

	// Get our view origin
	//CG_GetPlayerViewOrigin_BO3(0, cg_BO3, MyOrigin);

	// Calc our aimbot angles
	VectorSubtract(EnemyPosistion, MyOrigin, Angles); 
	VectorToAngle((float*)&Angles, (float*)&VectorAngles);
	VectorSubtract(VectorAngles, LocalClient_BO3->SpawnAngles, FinalAngles);
	//AngleNormalize(FinalAngles);

	// Get our smooth angles
	LegitAngles = SmoothAimbotAngles(FinalAngles, FindSmoothScale());

	// Apply our smooth angles if we are aimed in
	DWORD ReactionDelay = FindReactionDelayAmount();
	PCHAR AimKeyVal = GetMenuOptionString("Legitbot", "Aim Button:");
	if (GetMenuOptionString("Legitbot", "Reaction Delay:") != "Off")
	{
		// If we want a reaction delay
		if ((GReactionDelay > ReactionDelay) && *(float*)((DWORD)cg_BO3 + 0x424) >= 0.3f)
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
				LocalClient_BO3->ViewAngles[0] = LegitAngles.x;
				LocalClient_BO3->ViewAngles[1] = LegitAngles.y;
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
		if (*(float*)((DWORD)cg_BO3 + 0x424) >= 0.3f)
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
				LocalClient_BO3->ViewAngles[0] = LegitAngles.x;
				LocalClient_BO3->ViewAngles[1] = LegitAngles.y;
			}
		}
	}

	// Autoshoot
	if (GetMenuOptionValue("Legitbot", "Auto Shoot") || ((GetMenuOptionValue("Legitbot", "Triggerbot") && TriggerBot(BestTarget))))
	{
		// Wait until aimed in before shooting
		if (*(float*)((DWORD)cg_BO3 + 0x424) >= 0.3f)
		{
			nextusercommand->Buttons |= 0x80000000;
			if (usercommand->Buttons & 0x80000000)
			{
				usercommand->Buttons &= 0x7FFFFFFFu;
			}

			if (IsWeaponDualWield())
				usercommand->Buttons |= 0x00080800;
		}
	}
}

VOID Spinbot(usercmd_s_BO3 *UserCommand, BOOL RagebotEnabled)
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
	//if ((*(int*)((DWORD)&Entities_BO3[cg_BO3->ClientNumber] + 0x1C4) & 0x1) == 0 || UserCommand->weapon == 0 || Entities_BO3[cg_BO3->ClientNumber].Stance & 0x08 || (cg_BO3->Health <= 0) || (UserCommand->buttons & 0x8000) || (UserCommand->buttons & 0x4000))
	//	return;

	DWORD BestAAClient = FindBestAntiAimTarget();
	Vector3 EnemyPos, MyOrigin, VectorAngles, FinalAngles, Angles;

	if (BestAAClient != -1)
	{
		//CG_GetPlayerViewOrigin_BO3(0, cg_BO3, MyOrigin);
		MyOrigin = *(Vector3*)GetViewOriginPointer();
		GetTagPosistion(&Entities_BO3[BestAAClient], BestAAClient, "j_mainroot", (float*)&EnemyPos);

		// Calculate aimbot angles
		VectorSubtract(EnemyPos, MyOrigin, Angles);
		VectorToAngle((float*)&Angles, (float*)&VectorAngles);
		VectorSubtract(VectorAngles, LocalClient_BO3->SpawnAngles, FinalAngles);

		ValidAATarget = TRUE;
	}

	// Anti-Aim X Look down
	if (GetMenuOptionString("Spinbot", "X Anti-Aim Spinbot:") == "Look Down")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "X Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		//UserCommand->ViewAngles[0] = ANGLE2SHORT(ffAngleNormalize(60.0f + ShortToAngle(UserCommand->ViewAngles[0]) - RefDefAngles.x)); //ANGLE2SHORT(72.0f);
		UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_BO3->SpawnAngles.x);
	}
	else if (GetMenuOptionString("Spinbot", "X Anti-Aim Spinbot:") == "Up & Down")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "X Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		int RandomShouldFlip = rand() % 12;
		if (RandomShouldFlip > 5)
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_BO3->SpawnAngles.x);
			SpinbotFlipX = TRUE;
		}
		else
		{
			UserCommand->ViewAngles[0] = ANGLE2SHORT(-45 - LocalClient_BO3->SpawnAngles.x);
			SpinbotFlipX = FALSE;
		}

	}

	// Anti-Aim Y Flip Flop
	if (GetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:") == "Flip Flop")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "Y Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

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
	else if (GetMenuOptionString("Spinbot", "Y Anti-Aim Spinbot:") == "Face BO3ay")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "Y Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

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

	// Anti-Aim X Look Up (Riot Shield)
	if (GetMenuOptionString("Spinbot", "X Anti-Aim Spinbot:") == "Riot Shield")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "X Spin Direction:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		//UserCommand->ViewAngles[0] = ANGLE2SHORT(72.0f);
		UserCommand->ViewAngles[0] = ANGLE2SHORT(-69.99 - LocalClient_BO3->SpawnAngles.x);
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
		UserCommand->ViewAngles[0] = ANGLE2SHORT(69.99 - LocalClient_BO3->SpawnAngles.x);
	}
	else if (GetMenuOptionString("Spinbot", "X Spin Direction:") == "Look Up")
	{
		// Disable other spinbots
		SetMenuOptionString("Spinbot", "X Anti-Aim Spinbot:", 0);
		SetMenuOptionString("Spinbot", "Funny Spinbots:", 0);

		//UserCommand->ViewAngles[0] = ANGLE2SHORT(ffAngleNormalize(-60.0f + ShortToAngle(UserCommand->ViewAngles[0]) - RefDefAngles.x));
		UserCommand->ViewAngles[0] = ANGLE2SHORT(-69.99 - LocalClient_BO3->SpawnAngles.x);
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

	//LocalClient_BO3->ViewAngles.z = -LocalClient_BO3->SpawnAngles.z;
}

void CL_CreateNewCommandsHook(int r3)
{
	LocalClient_BO3 = (pclientActive_t_BO3)GetPointer(0x84475F74);//tu17
	Entities_BO3 = (pcentity_s_BO3)GetPointer(0x82E819EC);//tu17
	cg_BO3 = (pcg_s_BO3)GetPointer(0x82E819D8);//tu17
	cgs_BO3 = (pcgs_t_BO3)GetPointer(0x82E819C0);//tu17
	XINPUT_STATE State;

	// Call the original 1st
	CL_CreateNewCommandsDetour->CallOriginal(r3);

	// If we are ingame, start our aimbot
	if (UserInGame() && (GetMenuOptionValue("Ragebot", "Enable Ragebot") || GetMenuOptionValue("Legitbot", "Enable Legitbot") || GetMenuOptionValue("Spinbot", "Enable Spinbot")))
	{
		pusercmd_s_BO3 OldUserCommand = LocalClient_BO3->GetUserCommand(LocalClient_BO3->GetUserCommandNumber() - 1);
		pusercmd_s_BO3 CurrentUserCommand = LocalClient_BO3->GetUserCommand(LocalClient_BO3->GetUserCommandNumber());
		pusercmd_s_BO3 NextUserCommand = LocalClient_BO3->GetUserCommand(LocalClient_BO3->GetUserCommandNumber() + 1);
		*NextUserCommand = *CurrentUserCommand;
		//CurrentUserCommand->Buttons = OldUserCommand->Buttons;
		//NextUserCommand->Buttons = OldUserCommand->Buttons;
		LocalClient_BO3->CommandNumber += 1;
		CurrentUserCommand->ServerTime = OldUserCommand->ServerTime + 2;

		// Save our angle for movement fix
		float MovementSave = SHORT2ANGLE(CurrentUserCommand->ViewAngles[1]);

		// Reset our player info structure when we enter a game
		if (!DroppedAllTargets)
		{
			for (int Target = 0; Target < cgs_BO3->MaxClients; Target++)
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
			Legitbot(CurrentUserCommand, NextUserCommand);

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
					CurrentUserCommand->Buttons |= 0x80000000;
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
	LocalClient_BO3 = (pclientActive_t_BO3)GetPointer(0x84475F74);//TU7
	Entities_BO3 = (pcentity_s_BO3)GetPointer(0x82E819EC);//TU7
	cg_BO3 = (pcg_s_BO3)GetPointer(0x82E819D8);//TU7
	cgs_BO3 = (pcgs_t_BO3)GetPointer(0x82E819C0);//TU7

	// Update our input state
	if (!menuReady)
		goto Return;

	if (UserInGame())
	{
		// Do our server time 
		pusercmd_s_BO3 OldUserCommand = LocalClient_BO3->GetUserCommand(LocalClient_BO3->GetUserCommandNumber() - 1);
		pusercmd_s_BO3 CurrentUserCommand = LocalClient_BO3->GetUserCommand(LocalClient_BO3->GetUserCommandNumber());
		pusercmd_s_BO3 NextUserCommand = LocalClient_BO3->GetUserCommand(LocalClient_BO3->GetUserCommandNumber() + 1);


		// Get Ragebot status
		BOOL nRagebot = GetMenuOptionValue("Ragebot", "Enable Ragebot");

		// Do our spinbot
		if (GetMenuOptionValue("Spinbot", "Enable Spinbot"))
			Spinbot(CurrentUserCommand, nRagebot);

		// If Ragebot aimbot mode
		//if (GetMenuOptionValue("Ragebot", "Enable Ragebot"))
		//	Ragebot(CurrentUserCommand, NextUserCommand, OldUserCommand, NULL);


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

int PlayLocalSoundAliasByNameHook(char *aliasname, int a0, float soundLevel, int r6, int r7, int r8, int r9)
{
	DbgPrint("Name: %s", aliasname);

	// Disable main menu music
	if (GetMenuOptionValue("Settings", "Disable Main Menu Music"))
	{
		if (!strcmp(aliasname, "music_mainmenu_mp"))
			return 0;
	}

	return PlayLocalSoundAliasByNameDetour->CallOriginal(aliasname, a0, soundLevel, r6, r7, r8, r9);
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
	int DrawPlayer = 0;

	if (strstr(ModelName, "tf_141_") || (strstr(ModelName, "_ally_tf141_") || (strstr(ModelName, "udt") || (strstr(ModelName, "sniper_ghillie") || strstr(ModelName, "us_army_") || (strstr(ModelName, "seal_"))))))
	{
		DrawPlayer = 1;
		D3DDevice_SetRenderState_BO3(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_BO3(gDevice, D3DRS_ZENABLE, D3DZB_FALSE);
		R_DrawXModelSkinnedCachedDetour->CallOriginal(context, modelSurf, unk);
		D3DDevice_SetRenderState_BO3(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_BO3(gDevice, D3DRS_ZENABLE, D3DZB_TRUE);
	}

	else if (strstr(ModelName, "opforce") || (strstr(ModelName, "militia") || (strstr(ModelName, "airborne_") || (strstr(ModelName, "op_arctic") || (strstr(ModelName, "arctic_") || strstr(ModelName, "arab") || strstr(ModelName, "head_suren") || (strstr(ModelName, "op_ghille")))))))
	{
		DrawPlayer = 2;
		D3DDevice_SetRenderState_BO3(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_BO3(gDevice, D3DRS_ZENABLE, D3DZB_FALSE);
		R_DrawXModelSkinnedCachedDetour->CallOriginal(context, modelSurf, unk);
		D3DDevice_SetRenderState_BO3(gDevice, D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		D3DDevice_SetRenderState_BO3(gDevice, D3DRS_ZENABLE, D3DZB_TRUE);
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

void BO3Hax()
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
	//NetDll_XNetGetTitleXnAddrDetour = new Detour<DWORD>;

	Menu_PaintAllDetour->SetupDetour(0x822112F0, (PVOID)Menu_PaintAllHook);
	XInputGetStateDetour->SetupDetour(0x826EE8D8, (PVOID)XInputGetStateHook);
	CL_SendCmdDetour->SetupDetour(0x822E58F8, (PVOID)CL_SendCmdHook);
	CL_CreateNewCommandsDetour->SetupDetour(0x822E5800, (PVOID)CL_CreateNewCommandsHook); 

	//PlayLocalSoundAliasByNameDetour->SetupDetour(0x82699610, (PVOID)PlayLocalSoundAliasByNameHook);
	R_EndFrameDetour->SetupDetour(0x824CE670, (PVOID)R_EndFrameHook);
	CG_EntityEventDetour->SetupDetour(0x822201C0, (PVOID)CG_EntityEventHook);
	//NetDll_XNetGetTitleXnAddrDetour->SetupDetour(0x823B6994, (PVOID)NetDll_XNetGetTitleXnAddrHook);
	//RB_SetTessTechniqueDetour->SetupDetour(0x82373AF8, (PVOID)RB_SetTessTechniqueHook);
	//R_DrawXModelSkinnedCachedDetour->SetupDetour(0x823982C0, (PVOID)R_DrawXModelSkinnedCachedHook);
	//DrawIndexedPrimitiveDetour->SetupDetour(0x8242E2E0, (PVOID)DrawIndexedPrimitive); // tu23
	//R_DrawIndexedPrimitiveDetour->SetupDetour(0x820AB1C0, (PVOID)R_DrawIndexedPrimitiveHook); // tu23

	// Patch the anti-rapid fire
	*(short *)0x822F2540 = 0x4800;

	// Stop AimAssist_GetTagPos from throwing errors
	*(short*)0x82163CD4 = 0x4800;

	// Remove redbox perk check and respawn delay
	*(int *)(0x82610920 + 0x28) = 0x60000000;
	*(int *)0x8261073C = 0x60000000;// remove the FOF perk check
	*(int *)0x82610790 = 0x60000000;// remove the FOF respawn delay

	//((void(*)(int, char*))0x822F59A8)(0, "userinfo \"\\rate\\20000\\snaps\\20\\name\\xbLive 17544\\xuid\\000901FAAB270BDE\\natType\\1\rank\\1\\prestige\\0\"");
	//sprintf(Output, "userinfo \"\\rate\\20000\\snaps\\20\\name\\%s\\xuid\\%s\\natType\\1\rank\\1\\prestige\\0\"", cg_BO2->clientInfo[RandomClient].Name, xuid);
	//Cbuf_AddText_BO3(0, "userinfo \"\\rate\\20000\\snaps\\20\\name\\xbLive 17544\\xuid\\000901FAAB270BDE\\natType\\1\rank\\1\\prestige\\0\"");
	Cbuf_AddText_BO3(0, "userinfo \"\\rate\\20000\\snaps\\20\\name\\xbLive17544\\xuid\\000901FAAB270BDE\\natType\\1\rank\\1\\prestige\\0\"");
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
	//delete NetDll_XNetGetTitleXnAddrDetour;
}