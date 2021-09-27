#include "Menu.h"

// Main Tab 
bool bRadar;
bool bNoRecoil;
bool bNoSway;
bool bStealGTs;
bool bRainbowGT;
bool bShowHost;
bool bShowPing;
bool bFastReload;
bool bEveryoneHearsMe;
bool bNeverHost;
int iFieldOfViewIndex;
bool bThirdPerson;
int iFakeLagAmount;

// Ragebot Tab
bool bRagebot;
int iRagebotType;
int iRBAimBone;
int iRBPrediction;
bool bNoSpread;
bool bNoFlinch;
bool bAutoWall;
bool bAutoShoot;
bool bAutoAim;
bool bRapidFire;
bool bTeleportKill;
bool bTargetShields;
bool bRagebotPreferred;

// Legitbot Tab
bool bLegitbot;
int iLBAimBone;
int iLBAimButton;
bool bLBAutoShoot;
bool bLBAutoAim;
int iReactionDelay;
int iFOVSize;
bool bDrawFOVBox;
int iSmoothScale;
bool bTriggerbot;
bool bLBRapidFire;
bool bLBTargetShields;
bool bLegitbotPreferred;

// Spinbot Tab
bool bSpinbot;
int iXSpinDirection;
int iYSpinDirection;
int iYSpinSpeed;
int iZSpinDirection;
int iZSpinSpeed;
int iFunnySpinbots;
int iXAntiAimSpinbot;
int iYAntiAimSpinbot;
bool bSpinbotPreferred;

// Drawing Tab
bool bDrawEnemies;
bool bDrawFriendlies;
int iESPBoxType;
bool bDrawName;
bool bDrawWeapon;
bool bDrawBones;
bool bDrawExplosives;
int iSnaplines;
bool bDrawKillstreaks;
bool bDirectionalArrows;
int iCrosshairType;
bool bDrawCustomRadar;
bool bDrawDroppedItems;
bool bDrawEnemyVis;
bool bDrawProximityWrn;
bool bDrawThroughWalls;
bool bCartoonMode;

// Settings Tab
bool bDisableMenuSounds;
bool bDisableMenuInstructions;
bool bDisableNiNJAEngineText;
bool bDisableMainMenuMusic;
bool bPrestigeDisco;
bool bAntiDerank;
bool bLogIPs;

bool shouldSaveMenuOptions = false;

char EngineAscii[] = { 0x3B, 0x20, 0x5F, 0x20, 0x20, 0x20, 0x5F, 0x20, 0x5F, 0x20, 0x5F, 0x20, 0x20, 0x20, 0x5F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0D, 0x0A, 0x3B, 0x7C, 0x20, 0x5C, 0x20, 0x7C, 0x20, 0x28, 0x5F, 0x29, 0x20, 0x5C, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x20, 0x20, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x20, 0x2F, 0x5C, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7C, 0x20, 0x20, 0x5F, 0x5F, 0x5F, 0x5F, 0x7C, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x28, 0x5F, 0x29, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0D, 0x0A, 0x3B, 0x7C, 0x20, 0x20, 0x5C, 0x7C, 0x20, 0x7C, 0x5F, 0x7C, 0x20, 0x20, 0x5C, 0x7C, 0x20, 0x7C, 0x20, 0x20, 0x20, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x2F, 0x20, 0x20, 0x5C, 0x20, 0x20, 0x20, 0x20, 0x7C, 0x20, 0x7C, 0x5F, 0x5F, 0x20, 0x20, 0x20, 0x5F, 0x20, 0x5F, 0x5F, 0x20, 0x20, 0x20, 0x5F, 0x5F, 0x20, 0x5F, 0x20, 0x5F, 0x20, 0x5F, 0x20, 0x5F, 0x5F, 0x20, 0x20, 0x20, 0x5F, 0x5F, 0x5F, 0x20, 0x0D, 0x0A, 0x3B, 0x7C, 0x20, 0x2E, 0x20, 0x60, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x2E, 0x20, 0x60, 0x20, 0x7C, 0x5F, 0x20, 0x20, 0x20, 0x7C, 0x20, 0x7C, 0x2F, 0x20, 0x2F, 0x5C, 0x20, 0x5C, 0x20, 0x20, 0x20, 0x7C, 0x20, 0x20, 0x5F, 0x5F, 0x7C, 0x20, 0x7C, 0x20, 0x27, 0x5F, 0x20, 0x5C, 0x20, 0x2F, 0x20, 0x5F, 0x60, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x27, 0x5F, 0x20, 0x5C, 0x20, 0x2F, 0x20, 0x5F, 0x20, 0x5C, 0x0D, 0x0A, 0x3B, 0x7C, 0x20, 0x7C, 0x5C, 0x20, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x7C, 0x5C, 0x20, 0x20, 0x7C, 0x20, 0x7C, 0x5F, 0x5F, 0x7C, 0x20, 0x2F, 0x20, 0x5F, 0x5F, 0x5F, 0x5F, 0x20, 0x5C, 0x20, 0x20, 0x7C, 0x20, 0x7C, 0x5F, 0x5F, 0x5F, 0x5F, 0x7C, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x28, 0x5F, 0x7C, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x20, 0x5F, 0x5F, 0x2F, 0x0D, 0x0A, 0x3B, 0x7C, 0x5F, 0x7C, 0x20, 0x5C, 0x5F, 0x7C, 0x5F, 0x7C, 0x5F, 0x7C, 0x20, 0x5C, 0x5F, 0x7C, 0x5C, 0x5F, 0x5F, 0x5F, 0x5F, 0x2F, 0x5F, 0x2F, 0x20, 0x20, 0x20, 0x20, 0x5C, 0x5F, 0x5C, 0x20, 0x7C, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x7C, 0x5F, 0x7C, 0x20, 0x7C, 0x5F, 0x7C, 0x5C, 0x5F, 0x5F, 0x2C, 0x20, 0x7C, 0x5F, 0x7C, 0x5F, 0x7C, 0x20, 0x7C, 0x5F, 0x7C, 0x5C, 0x5F, 0x5F, 0x5F, 0x7C, 0x0D, 0x0A, 0x3B, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5F, 0x5F, 0x2F, 0x20, 0x7C, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0D, 0x0A, 0x3B, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7C, 0x5F, 0x5F, 0x5F, 0x2F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 };
#define ENGINE_CONFIG_VER 1
void CreateFreshConfig()
{
	CSimpleIniA ini(true, false, true);
	ini.Reset();

	if (FileExists(INI_PATH))
		DeleteFile(INI_PATH);

	DbgPrint("Wiping old Engine.ini!");
	binToFile(INI_PATH, EngineAscii, sizeof(EngineAscii));
	if (ini.LoadFile(INI_PATH) < 0) {
		DbgPrint("Failed to create fresh Engine.ini!");
		return;
	}

	ini.SetLongValue("DO NOT CHANGE!", "Engine Config Version", ENGINE_CONFIG_VER, NULL, FALSE, TRUE);
	ini.SaveFile(INI_PATH);

	DbgPrint("Success creating fresh Engine.ini!");
}

void PromptUserForUpdate()
{
	LPCWSTR g_strButtons[3] = { L"No", L"Yes" };

	XOVERLAPPED over;
	MESSAGEBOX_RESULT result;
	XShowMessageBoxUI(0, L"Reset Engine Config?", L"An outdated engine configuration has been detected. It is STRONGLY recommended that you read through the new options, and reset your configuration. Would you like to reset Engine.ini now?", 2, g_strButtons, 0, XMB_ALERTICON, &result, &over);

	XINPUT_STATE mars;
	XInputGetState(0, &mars);

	while (!XHasOverlappedIoCompleted(&over)) {
		XInputGetState(0, &mars);
		Sleep(500);
	}

	if (result.dwButtonPressed == 1)
		CreateFreshConfig();
	else if (result.dwButtonPressed == 0)
	{
		CSimpleIniA ini(true, false, true);
		ini.Reset();

		if (ini.LoadFile(INI_PATH) < 0) {
			DbgPrint("Failed to load Engine.ini!");
			return;
		}
		ini.SetLongValue("DO NOT CHANGE!", "Engine Config Version", ENGINE_CONFIG_VER, NULL, FALSE, TRUE);
		ini.SaveFile(INI_PATH);
	}
}

bool LoadMenuOptions()
{
	CSimpleIniA ini(true, false, true);
	ini.Reset();

	//if engine.ini doesn't exist, create it!
	if (ini.LoadFile(INI_PATH) < 0)
		SaveMenuOptions();

	if (ini.LoadFile(INI_PATH) >= 0)
	{
		DbgPrint("Engine ini exists, loading values!");

		if (ini.GetLongValue("DO NOT CHANGE!", "Engine Config Version", 0) < ENGINE_CONFIG_VER)
		{
			// Wipe their Engine.ini if we increment this value :)
			PromptUserForUpdate();
			ini.Reset();
			if (ini.LoadFile(INI_PATH) < 0) {
				DbgPrint("An error occured while trying to reset Engine.ini! Aborting!");
				return false;
			}
		}

		// Main Tab
		bRadar = ini.GetBoolValue("WAW Main", "Radar", false);
		bNoRecoil = ini.GetBoolValue("WAW Main", "No Recoil", false);
		bNoSway = ini.GetBoolValue("WAW Main", "No Sway", false);
		bStealGTs = ini.GetBoolValue("WAW Main", "Steal GTs", false);
		bRainbowGT = ini.GetBoolValue("WAW Main", "Rainbow GT", false);
		bShowHost = ini.GetBoolValue("WAW Main", "Show Host", false);
		bShowPing = ini.GetBoolValue("WAW Main", "Show Ping", false);
		bFastReload = ini.GetBoolValue("WAW Main", "Fast Reload", false);
		bEveryoneHearsMe = ini.GetBoolValue("WAW Main", "Everyone Hears Me", false);
		bNeverHost = ini.GetBoolValue("WAW Main", "Never Host", false);
		iFieldOfViewIndex = ini.GetLongValue("WAW Main", "Field of View", 0);
		bThirdPerson = ini.GetBoolValue("WAW Main", "Third Person", false);
		iFakeLagAmount = ini.GetLongValue("WAW Main", "Fake Lag Amount", 0);

		// Ragebot Tab
		bRagebot = ini.GetBoolValue("WAW Ragebot", "Enable Ragebot", false);
		iRagebotType = ini.GetLongValue("WAW Ragebot", "Ragebot Type", 0);
		iRBAimBone = ini.GetLongValue("WAW Ragebot", "Aim Bone", 0);
		iRBPrediction = ini.GetLongValue("WAW Ragebot", "Prediction Scale", 0);
		bNoSpread = ini.GetBoolValue("WAW Ragebot", "No Spread", false);
		bNoFlinch = ini.GetBoolValue("WAW Ragebot", "No Flinch", false);
		bAutoWall = ini.GetBoolValue("WAW Ragebot", "Auto Wall", false);
		bAutoShoot = ini.GetBoolValue("WAW Ragebot", "Auto Shoot", false);
		bAutoAim = ini.GetBoolValue("WAW Ragebot", "Auto Aim", false);
		bRapidFire = ini.GetBoolValue("WAW Ragebot", "Rapid Fire", false);
		bTeleportKill = ini.GetBoolValue("WAW Ragebot", "Teleport Kill", false);
		bRagebotPreferred = ini.GetBoolValue("WAW Ragebot", "Preferred Settings", false);

		// Legitbot Tab
		bLegitbot = ini.GetBoolValue("WAW Legitbot", "Enable Legitbot", false);
		iLBAimBone = ini.GetLongValue("WAW Legitbot", "Aim Bone", 0);
		iLBAimButton = ini.GetLongValue("WAW Legitbot", "Aim Button", 0);
		bLBAutoShoot = ini.GetBoolValue("WAW Legitbot", "Auto Shoot", false);
		bLBAutoAim = ini.GetBoolValue("WAW Legitbot", "Auto Aim", false);
		iReactionDelay = ini.GetLongValue("WAW Legitbot", "Reaction Delay", 0);
		iFOVSize = ini.GetLongValue("WAW Legitbot", "FOV Size", 0);
		bDrawFOVBox = ini.GetBoolValue("WAW Legitbot", "Draw FOV Box", false);
		iSmoothScale = ini.GetLongValue("WAW Legitbot", "Smooth Scale", 0);
		bTriggerbot = ini.GetBoolValue("WAW Legitbot", "Triggerbot", false);
		bRapidFire = ini.GetBoolValue("WAW Legitbot", "Rapid Fire", false);
		bLegitbotPreferred = ini.GetBoolValue("WAW Legitbot", "Preferred Settings", false);

		// Spinbot Tab
		bSpinbot = ini.GetBoolValue("WAW Spinbot", "Enable Spinbot", false);
		iXSpinDirection = ini.GetLongValue("WAW Spinbot", "X Spin Direction", 0);
		iYSpinDirection = ini.GetLongValue("WAW Spinbot", "Y Spin Direction", 0);
		iYSpinSpeed = ini.GetLongValue("WAW Spinbot", "Y Spin Speed", 0);
		iZSpinDirection = ini.GetLongValue("WAW Spinbot", "Z Spin Direction", 0);
		iZSpinSpeed = ini.GetLongValue("WAW Spinbot", "Z Spin Speed", 0);
		iFunnySpinbots = ini.GetLongValue("WAW Spinbot", "Funny Spinbots", 0);
		iXAntiAimSpinbot = ini.GetLongValue("WAW Spinbot", "X Anti-Aim Spinbot", 0);
		iYAntiAimSpinbot = ini.GetLongValue("WAW Spinbot", "Y Anti-Aim Spinbot", 0);
		bSpinbotPreferred = ini.GetBoolValue("WAW Spinbot", "Preferred Settings", false);

		// Drawing Tab
		bDrawEnemies = ini.GetBoolValue("WAW Drawing", "Draw Enemies", false);
		bDrawFriendlies = ini.GetBoolValue("WAW Drawing", "Draw Friendlies", false);
		iESPBoxType = ini.GetLongValue("WAW Drawing", "ESP Box Type", 1);
		bDrawName = ini.GetBoolValue("WAW Drawing", "Draw Name", false);
		bDrawWeapon = ini.GetBoolValue("WAW Drawing", "Draw Weapon", false);
		bDrawBones = ini.GetBoolValue("WAW Drawing", "Draw Bones", false);
		bDrawExplosives = ini.GetBoolValue("WAW Drawing", "Draw Explosives", false);
		iSnaplines = ini.GetLongValue("WAW Drawing", "Draw Snaplines", 0);
		bDrawKillstreaks = ini.GetBoolValue("WAW Drawing", "Draw Killstreaks", false);
		bDirectionalArrows = ini.GetBoolValue("WAW Drawing", "Directional Arrows", false);
		iCrosshairType = ini.GetLongValue("WAW Drawing", "Crosshair Type", 0);
		bDrawCustomRadar = ini.GetBoolValue("WAW Drawing", "Draw Custom Radar", false);
		bDrawDroppedItems = ini.GetBoolValue("WAW Drawing", "Draw Dropped Items", false);
		bDrawEnemyVis = ini.GetBoolValue("WAW Drawing", "Draw Enemy Visible Text", false);
		bDrawProximityWrn = ini.GetBoolValue("WAW Drawing", "Draw Proximity Warning", false);
		bDrawThroughWalls = ini.GetBoolValue("WAW Drawing", "Draw Through Walls", false);
		bCartoonMode = ini.GetBoolValue("WAW Drawing", "Cartoon Mode", false);

		// Settings Tab
		bDisableMenuSounds = ini.GetBoolValue("WAW Settings", "Disable Menu Sounds", false);
		bDisableMenuInstructions = ini.GetBoolValue("WAW Settings", "Disable Menu Instructions", false);
		bDisableNiNJAEngineText = ini.GetBoolValue("WAW Settings", "Disable NiNJA Engine Text", false);
		bDisableMainMenuMusic = ini.GetBoolValue("WAW Settings", "Disable Main Menu Music", false);
		bPrestigeDisco = ini.GetBoolValue("WAW Settings", "Prestige Disco", false);
		bAntiDerank = ini.GetBoolValue("WAW Settings", "Anti Derank", false);
		bLogIPs = ini.GetBoolValue("WAW Settings", "Log IPs", false);

		Sleep(500);
		DbgPrint("Loaded ini values!");

		return true;
	}
	return false;
}


bool SaveMenuOptions()
{
	CSimpleIniA ini(true, false, true);
	ini.Reset();

	if (ini.LoadFile(INI_PATH) < 0)
	{
		CreateFreshConfig();
		DbgPrint("Engine ini does not exist! Creating!");
	}

	if (ini.LoadFile(INI_PATH) >= 0)
	{
		// Main Tab
		ini.SetBoolValue("WAW Main", "Radar", GetMenuOptionValue(MENU_MAIN, OPTION_RADAR));
		ini.SetBoolValue("WAW Main", "No Recoil", GetMenuOptionValue(MENU_MAIN, OPTION_NORECOIL));
		ini.SetBoolValue("WAW Main", "No Sway", GetMenuOptionValue(MENU_MAIN, OPTION_NOSWAY));
		ini.SetBoolValue("WAW Main", "Steal GTs", GetMenuOptionValue(MENU_MAIN, OPTION_STEALGTS));
		ini.SetBoolValue("WAW Main", "Rainbow GT", GetMenuOptionValue(MENU_MAIN, OPTION_RAINBOWGT));
		ini.SetBoolValue("WAW Main", "Show Host", GetMenuOptionValue(MENU_MAIN, OPTION_SHOWHOST));
		ini.SetBoolValue("WAW Main", "Show Ping", GetMenuOptionValue(MENU_MAIN, OPTION_SHOWPING));
		ini.SetBoolValue("WAW Main", "Fast Reload", GetMenuOptionValue(MENU_MAIN, OPTION_FASTRELOAD));
		ini.SetBoolValue("WAW Main", "Everyone Hears Me", GetMenuOptionValue(MENU_MAIN, OPTION_EVERYONEHEARSME));
		ini.SetBoolValue("WAW Main", "Never Host", GetMenuOptionValue(MENU_MAIN, OPTION_NEVERHOST));
		ini.SetLongValue("WAW Main", "Field of View", GetMenuOptionArrayIndex(MENU_MAIN, OPTION_FOV));
		ini.SetBoolValue("WAW Main", "Third Person", GetMenuOptionValue(MENU_MAIN, OPTION_THIRDPERSON));
		ini.SetLongValue("WAW Main", "Fake Lag Amount", GetMenuOptionArrayIndex(MENU_MAIN, OPTION_FAKELAGAMOUNT));

		// Ragebot Tab
		ini.SetBoolValue("WAW Ragebot", "Enable Ragebot", GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT));
		ini.SetLongValue("WAW Ragebot", "Ragebot Type", GetMenuOptionArrayIndex(MENU_RAGEBOT, OPTION_RAGEBOTTYPE));
		ini.SetLongValue("WAW Ragebot", "Aim Bone", GetMenuOptionArrayIndex(MENU_RAGEBOT, OPTION_AIMBONE));
		ini.SetLongValue("WAW Ragebot", "Prediction Scale", GetMenuOptionArrayIndex(MENU_RAGEBOT, OPTION_PREDICTION));
		ini.SetBoolValue("WAW Ragebot", "No Spread", GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD));
		ini.SetBoolValue("WAW Ragebot", "No Flinch", GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOFLINCH));
		ini.SetBoolValue("WAW Ragebot", "Auto Wall", GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL));
		ini.SetBoolValue("WAW Ragebot", "Auto Shoot", GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT));
		ini.SetBoolValue("WAW Ragebot", "Auto Aim", GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM));
		ini.SetBoolValue("WAW Ragebot", "Rapid Fire", GetMenuOptionValue(MENU_RAGEBOT, OPTION_RAPIDFIRE));
		ini.SetBoolValue("WAW Ragebot", "Teleport Kill", GetMenuOptionValue(MENU_RAGEBOT, OPTION_TELEPORTKILL));
		ini.SetBoolValue("WAW Ragebot", "Preferred Settings", GetMenuOptionValue(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS));

		// Legitbot Tab
		ini.SetBoolValue("WAW Legitbot", "Enable Legitbot", GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT));
		ini.SetLongValue("WAW Legitbot", "Aim Bone", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_AIMBONE));
		ini.SetLongValue("WAW Legitbot", "Aim Button", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_AIMKEY));
		ini.SetBoolValue("WAW Legitbot", "Auto Shoot", GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOSHOOT));
		ini.SetBoolValue("WAW Legitbot", "Auto Aim", GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOAIM));
		ini.SetLongValue("WAW Legitbot", "Reaction Delay", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_REACTIONDELAY));
		ini.SetLongValue("WAW Legitbot", "FOV Size", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_FOVSIZE));
		ini.SetBoolValue("WAW Legitbot", "Draw FOV Box", GetMenuOptionValue(MENU_LEGITBOT, OPTION_DRAWFOVBOX));
		ini.SetLongValue("WAW Legitbot", "Smooth Scale", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_SMOOTHSCALE));
		ini.SetBoolValue("WAW Legitbot", "Triggerbot", GetMenuOptionValue(MENU_LEGITBOT, OPTION_TRIGGERBOT));
		ini.SetBoolValue("WAW Legitbot", "Rapid Fire", GetMenuOptionValue(MENU_LEGITBOT, OPTION_RAPIDFIRE));
		ini.SetBoolValue("WAW Legitbot", "Preferred Settings", GetMenuOptionValue(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS));

		// Spinbot Tab
		ini.SetBoolValue("WAW Spinbot", "Enable Spinbot", GetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT));
		ini.SetLongValue("WAW Spinbot", "X Spin Direction", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_XSPINDIRECTION));
		ini.SetLongValue("WAW Spinbot", "Y Spin Direction", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_YSPINDIRECTION));
		ini.SetLongValue("WAW Spinbot", "Y Spin Speed", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_YSPINSPEED));
		ini.SetLongValue("WAW Spinbot", "Z Spin Direction", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_ZSPINDIRECTION));
		ini.SetLongValue("WAW Spinbot", "Z Spin Speed", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_ZSPINSPEED));
		ini.SetLongValue("WAW Spinbot", "X Anti-Aim Spinbot", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT));
		ini.SetLongValue("WAW Spinbot", "Y Anti-Aim Spinbot", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT));
		ini.SetBoolValue("WAW Spinbot", "Preferred Settings", GetMenuOptionValue(MENU_SPINBOT, OPTION_PREFERREDSETTINGS));

		// Drawing Tab
		ini.SetBoolValue("WAW Drawing", "Draw Enemies", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMIES));
		ini.SetBoolValue("WAW Drawing", "Draw Friendlies", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWFRIENDLIES));
		ini.SetLongValue("WAW Drawing", "ESP Box Type", GetMenuOptionArrayIndex(MENU_DRAWING, OPTION_ESPBOXTYPE));
		ini.SetBoolValue("WAW Drawing", "Draw Name", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWNAME));
		ini.SetBoolValue("WAW Drawing", "Draw Weapon", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWWEAPON));
		ini.SetBoolValue("WAW Drawing", "Draw Bones", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWBONES));
		ini.SetBoolValue("WAW Drawing", "Draw Explosives", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWEXPLOSIVES));
		ini.SetLongValue("WAW Drawing", "Draw Snaplines", GetMenuOptionArrayIndex(MENU_DRAWING, OPTION_DRAWSNAPLINES));
		ini.SetBoolValue("WAW Drawing", "Draw Killstreaks", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWKILLSTREAKS));
		ini.SetBoolValue("WAW Drawing", "Directional Arrows", GetMenuOptionValue(MENU_DRAWING, OPTION_DIRECTIONALARROWS));
		ini.SetLongValue("WAW Drawing", "Crosshair Type", GetMenuOptionArrayIndex(MENU_DRAWING, OPTION_CROSSHAIRTYPE));
		ini.SetBoolValue("WAW Drawing", "Draw Custom Radar", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR));
		ini.SetBoolValue("WAW Drawing", "Draw Dropped Items", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS));
		ini.SetBoolValue("WAW Drawing", "Draw Enemy Visible Text", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMYVISIBLE));
		ini.SetBoolValue("WAW Drawing", "Draw Proximity Warning", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWPROXIMITYWARNING));
		ini.SetBoolValue("WAW Drawing", "Draw Through Walls", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWTHROUGHWALLS));
		ini.SetBoolValue("WAW Drawing", "Cartoon Mode", GetMenuOptionValue(MENU_DRAWING, OPTION_CARTOONMODE));

		// Settings Tab
		ini.SetBoolValue("WAW Settings", "Disable Menu Sounds", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUSOUNDS));
		ini.SetBoolValue("WAW Settings", "Disable Menu Instructions", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUINSTRUCTIONS));
		ini.SetBoolValue("WAW Settings", "Disable NiNJA Engine Text", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLENINJATEXT));
		ini.SetBoolValue("WAW Settings", "Disable Main Menu Music", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUMUSIC));
		ini.SetBoolValue("WAW Settings", "Prestige Disco", GetMenuOptionValue(MENU_SETTINGS, OPTION_PRESTIGEDISCO));
		ini.SetBoolValue("WAW Settings", "Anti Derank", GetMenuOptionValue(MENU_SETTINGS, OPTION_ANTIDERANK));
		ini.SetBoolValue("WAW Settings", "Log IPs", GetMenuOptionValue(MENU_SETTINGS, OPTION_LOGIPS));

		ini.SaveFile(INI_PATH);

		Sleep(500);
		DbgPrint("Saved ini values!");

		return true;
	}
	return false;
}