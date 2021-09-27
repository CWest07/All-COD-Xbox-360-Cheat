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
		bRadar = ini.GetBoolValue("Ghosts Main", "Radar", false);
		bNoRecoil = ini.GetBoolValue("Ghosts Main", "No Recoil", false);
		bNoSway = ini.GetBoolValue("Ghosts Main", "No Sway", false);
		bStealGTs = ini.GetBoolValue("Ghosts Main", "Steal GTs", false);
		bRainbowGT = ini.GetBoolValue("Ghosts Main", "Rainbow GT", false);
		bShowHost = ini.GetBoolValue("Ghosts Main", "Show Host", false);
		bShowPing = ini.GetBoolValue("Ghosts Main", "Show Ping", false);
		bFastReload = ini.GetBoolValue("Ghosts Main", "Fast Reload", false);
		bEveryoneHearsMe = ini.GetBoolValue("Ghosts Main", "Everyone Hears Me", false);
		bNeverHost = ini.GetBoolValue("Ghosts Main", "Never Host", false);
		iFieldOfViewIndex = ini.GetLongValue("Ghosts Main", "Field of View", 0);
		bThirdPerson = ini.GetBoolValue("Ghosts Main", "Third Person", false);
		iFakeLagAmount = ini.GetLongValue("Ghosts Main", "Fake Lag Amount", 0);

		// Ragebot Tab
		bRagebot = ini.GetBoolValue("Ghosts Ragebot", "Enable Ragebot", false);
		iRagebotType = ini.GetLongValue("Ghosts Ragebot", "Ragebot Type", 0);
		iRBAimBone = ini.GetLongValue("Ghosts Ragebot", "Aim Bone", 0);
		iRBPrediction = ini.GetLongValue("Ghosts Ragebot", "Prediction Scale", 0);
		bNoSpread = ini.GetBoolValue("Ghosts Ragebot", "No Spread", false);
		bNoFlinch = ini.GetBoolValue("Ghosts Ragebot", "No Flinch", false);
		bAutoWall = ini.GetBoolValue("Ghosts Ragebot", "Auto Wall", false);
		bAutoShoot = ini.GetBoolValue("Ghosts Ragebot", "Auto Shoot", false);
		bAutoAim = ini.GetBoolValue("Ghosts Ragebot", "Auto Aim", false);
		bRapidFire = ini.GetBoolValue("Ghosts Ragebot", "Rapid Fire", false);
		bTargetShields = ini.GetBoolValue("Ghosts Ragebot", "Target Shields", false);
		bRagebotPreferred = ini.GetBoolValue("Ghosts Ragebot", "Preferred Settings", false);

		// Legitbot Tab
		bLegitbot = ini.GetBoolValue("Ghosts Legitbot", "Enable Legitbot", false);
		iLBAimBone = ini.GetLongValue("Ghosts Legitbot", "Aim Bone", 0);
		iLBAimButton = ini.GetLongValue("Ghosts Legitbot", "Aim Button", 0);
		bLBAutoShoot = ini.GetBoolValue("Ghosts Legitbot", "Auto Shoot", false);
		bLBAutoAim = ini.GetBoolValue("Ghosts Legitbot", "Auto Aim", false);
		iReactionDelay = ini.GetLongValue("Ghosts Legitbot", "Reaction Delay", 0);
		iFOVSize = ini.GetLongValue("Ghosts Legitbot", "FOV Size", 0);
		bDrawFOVBox = ini.GetBoolValue("Ghosts Legitbot", "Draw FOV Box", false);
		iSmoothScale = ini.GetLongValue("Ghosts Legitbot", "Smooth Scale", 0);
		bTriggerbot = ini.GetBoolValue("Ghosts Legitbot", "Triggerbot", false);
		bRapidFire = ini.GetBoolValue("Ghosts Legitbot", "Rapid Fire", false);
		bLBTargetShields = ini.GetBoolValue("Ghosts Legitbot", "Target Shields", false);
		bLegitbotPreferred = ini.GetBoolValue("Ghosts Legitbot", "Preferred Settings", false);

		// Spinbot Tab
		bSpinbot = ini.GetBoolValue("Ghosts Spinbot", "Enable Spinbot", false);
		iXSpinDirection = ini.GetLongValue("Ghosts Spinbot", "X Spin Direction", 0);
		iYSpinDirection = ini.GetLongValue("Ghosts Spinbot", "Y Spin Direction", 0);
		iYSpinSpeed = ini.GetLongValue("Ghosts Spinbot", "Y Spin Speed", 0);
		iZSpinDirection = ini.GetLongValue("Ghosts Spinbot", "Z Spin Direction", 0);
		iZSpinSpeed = ini.GetLongValue("Ghosts Spinbot", "Z Spin Speed", 0);
		iFunnySpinbots = ini.GetLongValue("Ghosts Spinbot", "Funny Spinbots", 0);
		iXAntiAimSpinbot = ini.GetLongValue("Ghosts Spinbot", "X Anti-Aim Spinbot", 0);
		iYAntiAimSpinbot = ini.GetLongValue("Ghosts Spinbot", "Y Anti-Aim Spinbot", 0);
		bSpinbotPreferred = ini.GetBoolValue("Ghosts Spinbot", "Preferred Settings", false);

		// Drawing Tab
		bDrawEnemies = ini.GetBoolValue("Ghosts Drawing", "Draw Enemies", false);
		bDrawFriendlies = ini.GetBoolValue("Ghosts Drawing", "Draw Friendlies", false);
		iESPBoxType = ini.GetLongValue("Ghosts Drawing", "ESP Box Type", 1);
		bDrawName = ini.GetBoolValue("Ghosts Drawing", "Draw Name", false);
		bDrawWeapon = ini.GetBoolValue("Ghosts Drawing", "Draw Weapon", false);
		bDrawBones = ini.GetBoolValue("Ghosts Drawing", "Draw Bones", false);
		bDrawExplosives = ini.GetBoolValue("Ghosts Drawing", "Draw Explosives", false);
		iSnaplines = ini.GetLongValue("Ghosts Drawing", "Draw Snaplines", 0);
		bDrawKillstreaks = ini.GetBoolValue("Ghosts Drawing", "Draw Killstreaks", false);
		bDirectionalArrows = ini.GetBoolValue("Ghosts Drawing", "Directional Arrows", false);
		iCrosshairType = ini.GetLongValue("Ghosts Drawing", "Crosshair Type", 0);
		bDrawCustomRadar = ini.GetBoolValue("Ghosts Drawing", "Draw Custom Radar", false);
		bDrawDroppedItems = ini.GetBoolValue("Ghosts Drawing", "Draw Dropped Items", false);
		bDrawEnemyVis = ini.GetBoolValue("Ghosts Drawing", "Draw Enemy Visible Text", false);
		bDrawProximityWrn = ini.GetBoolValue("Ghosts Drawing", "Draw Proximity Warning", false);
		bCartoonMode = ini.GetBoolValue("Ghosts Drawing", "Cartoon Mode", false);

		// Settings Tab
		bDisableMenuSounds = ini.GetBoolValue("Ghosts Settings", "Disable Menu Sounds", false);
		bDisableMenuInstructions = ini.GetBoolValue("Ghosts Settings", "Disable Menu Instructions", false);
		bDisableNiNJAEngineText = ini.GetBoolValue("Ghosts Settings", "Disable NiNJA Engine Text", false);
		bDisableMainMenuMusic = ini.GetBoolValue("Ghosts Settings", "Disable Main Menu Music", false);
		bPrestigeDisco = ini.GetBoolValue("Ghosts Settings", "Prestige Disco", false);
		bAntiDerank = ini.GetBoolValue("Ghosts Settings", "Anti Derank", false);
		bLogIPs = ini.GetBoolValue("Ghosts Settings", "Log IPs", false);

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
		ini.SetBoolValue("Ghosts Main", "Radar", GetMenuOptionValue(MENU_MAIN, OPTION_RADAR));
		ini.SetBoolValue("Ghosts Main", "No Recoil", GetMenuOptionValue(MENU_MAIN, OPTION_NORECOIL));
		ini.SetBoolValue("Ghosts Main", "No Sway", GetMenuOptionValue(MENU_MAIN, OPTION_NOSWAY));
		ini.SetBoolValue("Ghosts Main", "Steal GTs", GetMenuOptionValue(MENU_MAIN, OPTION_STEALGTS));
		ini.SetBoolValue("Ghosts Main", "Rainbow GT", GetMenuOptionValue(MENU_MAIN, OPTION_RAINBOWGT));
		ini.SetBoolValue("Ghosts Main", "Show Host", GetMenuOptionValue(MENU_MAIN, OPTION_SHOWHOST));
		ini.SetBoolValue("Ghosts Main", "Show Ping", GetMenuOptionValue(MENU_MAIN, OPTION_SHOWPING));
		ini.SetBoolValue("Ghosts Main", "Fast Reload", GetMenuOptionValue(MENU_MAIN, OPTION_FASTRELOAD));
		ini.SetBoolValue("Ghosts Main", "Everyone Hears Me", GetMenuOptionValue(MENU_MAIN, OPTION_EVERYONEHEARSME));
		ini.SetBoolValue("Ghosts Main", "Never Host", GetMenuOptionValue(MENU_MAIN, OPTION_NEVERHOST));
		ini.SetLongValue("Ghosts Main", "Field of View", GetMenuOptionArrayIndex(MENU_MAIN, OPTION_FOV));
		ini.SetBoolValue("Ghosts Main", "Third Person", GetMenuOptionValue(MENU_MAIN, OPTION_THIRDPERSON));
		ini.SetLongValue("Ghosts Main", "Fake Lag Amount", GetMenuOptionArrayIndex(MENU_MAIN, OPTION_FAKELAGAMOUNT));

		// Ragebot Tab
		ini.SetBoolValue("Ghosts Ragebot", "Enable Ragebot", GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT));
		ini.SetLongValue("Ghosts Ragebot", "Ragebot Type", GetMenuOptionArrayIndex(MENU_RAGEBOT, OPTION_RAGEBOTTYPE));
		ini.SetLongValue("Ghosts Ragebot", "Aim Bone", GetMenuOptionArrayIndex(MENU_RAGEBOT, OPTION_AIMBONE));
		ini.SetLongValue("Ghosts Ragebot", "Prediction Scale", GetMenuOptionArrayIndex(MENU_RAGEBOT, OPTION_PREDICTION));
		ini.SetBoolValue("Ghosts Ragebot", "No Spread", GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD));
		ini.SetBoolValue("Ghosts Ragebot", "No Flinch", GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOFLINCH));
		ini.SetBoolValue("Ghosts Ragebot", "Auto Wall", GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL));
		ini.SetBoolValue("Ghosts Ragebot", "Auto Shoot", GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT));
		ini.SetBoolValue("Ghosts Ragebot", "Auto Aim", GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM));
		ini.SetBoolValue("Ghosts Ragebot", "Rapid Fire", GetMenuOptionValue(MENU_RAGEBOT, OPTION_RAPIDFIRE));
		ini.SetBoolValue("Ghosts Ragebot", "Target Shields", GetMenuOptionValue(MENU_RAGEBOT, OPTION_TARGETSHIELDS));
		ini.SetBoolValue("Ghosts Ragebot", "Preferred Settings", GetMenuOptionValue(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS));

		// Legitbot Tab
		ini.SetBoolValue("Ghosts Legitbot", "Enable Legitbot", GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT));
		ini.SetLongValue("Ghosts Legitbot", "Aim Bone", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_AIMBONE));
		ini.SetLongValue("Ghosts Legitbot", "Aim Button", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_AIMKEY));
		ini.SetBoolValue("Ghosts Legitbot", "Auto Shoot", GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOSHOOT));
		ini.SetBoolValue("Ghosts Legitbot", "Auto Aim", GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOAIM));
		ini.SetLongValue("Ghosts Legitbot", "Reaction Delay", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_REACTIONDELAY));
		ini.SetLongValue("Ghosts Legitbot", "FOV Size", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_FOVSIZE));
		ini.SetBoolValue("Ghosts Legitbot", "Draw FOV Box", GetMenuOptionValue(MENU_LEGITBOT, OPTION_DRAWFOVBOX));
		ini.SetLongValue("Ghosts Legitbot", "Smooth Scale", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_SMOOTHSCALE));
		ini.SetBoolValue("Ghosts Legitbot", "Triggerbot", GetMenuOptionValue(MENU_LEGITBOT, OPTION_TRIGGERBOT));
		ini.SetBoolValue("Ghosts Legitbot", "Rapid Fire", GetMenuOptionValue(MENU_LEGITBOT, OPTION_RAPIDFIRE));
		ini.SetBoolValue("Ghosts Legitbot", "Target Shields", GetMenuOptionValue(MENU_LEGITBOT, OPTION_TARGETSHIELDS));
		ini.SetBoolValue("Ghosts Legitbot", "Preferred Settings", GetMenuOptionValue(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS));

		// Spinbot Tab
		ini.SetBoolValue("Ghosts Spinbot", "Enable Spinbot", GetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT));
		ini.SetLongValue("Ghosts Spinbot", "X Spin Direction", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_XSPINDIRECTION));
		ini.SetLongValue("Ghosts Spinbot", "Y Spin Direction", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_YSPINDIRECTION));
		ini.SetLongValue("Ghosts Spinbot", "Y Spin Speed", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_YSPINSPEED));
		ini.SetLongValue("Ghosts Spinbot", "Z Spin Direction", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_ZSPINDIRECTION));
		ini.SetLongValue("Ghosts Spinbot", "Z Spin Speed", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_ZSPINSPEED));
		ini.SetLongValue("Ghosts Spinbot", "X Anti-Aim Spinbot", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT));
		ini.SetLongValue("Ghosts Spinbot", "Y Anti-Aim Spinbot", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT));
		ini.SetBoolValue("Ghosts Spinbot", "Preferred Settings", GetMenuOptionValue(MENU_SPINBOT, OPTION_PREFERREDSETTINGS));

		// Drawing Tab
		ini.SetBoolValue("Ghosts Drawing", "Draw Enemies", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMIES));
		ini.SetBoolValue("Ghosts Drawing", "Draw Friendlies", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWFRIENDLIES));
		ini.SetLongValue("Ghosts Drawing", "ESP Box Type", GetMenuOptionArrayIndex(MENU_DRAWING, OPTION_ESPBOXTYPE));
		ini.SetBoolValue("Ghosts Drawing", "Draw Name", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWNAME));
		ini.SetBoolValue("Ghosts Drawing", "Draw Weapon", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWWEAPON));
		ini.SetBoolValue("Ghosts Drawing", "Draw Bones", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWBONES));
		ini.SetBoolValue("Ghosts Drawing", "Draw Explosives", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWEXPLOSIVES));
		ini.SetLongValue("Ghosts Drawing", "Draw Snaplines", GetMenuOptionArrayIndex(MENU_DRAWING, OPTION_DRAWSNAPLINES));
		ini.SetBoolValue("Ghosts Drawing", "Draw Killstreaks", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWKILLSTREAKS));
		ini.SetBoolValue("Ghosts Drawing", "Directional Arrows", GetMenuOptionValue(MENU_DRAWING, OPTION_DIRECTIONALARROWS));
		ini.SetLongValue("Ghosts Drawing", "Crosshair Type", GetMenuOptionArrayIndex(MENU_DRAWING, OPTION_CROSSHAIRTYPE));
		ini.SetBoolValue("Ghosts Drawing", "Draw Custom Radar", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR));
		ini.SetBoolValue("Ghosts Drawing", "Draw Dropped Items", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS));
		ini.SetBoolValue("Ghosts Drawing", "Draw Enemy Visible Text", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMYVISIBLE));
		ini.SetBoolValue("Ghosts Drawing", "Draw Proximity Warning", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWPROXIMITYWARNING));
		ini.SetBoolValue("Ghosts Drawing", "Cartoon Mode", GetMenuOptionValue(MENU_DRAWING, OPTION_CARTOONMODE));

		// Settings Tab
		ini.SetBoolValue("Ghosts Settings", "Disable Menu Sounds", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUSOUNDS));
		ini.SetBoolValue("Ghosts Settings", "Disable Menu Instructions", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUINSTRUCTIONS));
		ini.SetBoolValue("Ghosts Settings", "Disable NiNJA Engine Text", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLENINJATEXT));
		ini.SetBoolValue("Ghosts Settings", "Disable Main Menu Music", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUMUSIC));
		ini.SetBoolValue("Ghosts Settings", "Prestige Disco", GetMenuOptionValue(MENU_SETTINGS, OPTION_PRESTIGEDISCO));
		ini.SetBoolValue("Ghosts Settings", "Anti Derank", GetMenuOptionValue(MENU_SETTINGS, OPTION_ANTIDERANK));
		ini.SetBoolValue("Ghosts Settings", "Log IPs", GetMenuOptionValue(MENU_SETTINGS, OPTION_LOGIPS));

		ini.SaveFile(INI_PATH);

		Sleep(500);
		DbgPrint("Saved ini values!");

		return true;
	}
	return false;
}