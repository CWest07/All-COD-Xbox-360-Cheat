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
		bRadar = ini.GetBoolValue("MW3 Main", "Radar", false);
		bNoRecoil = ini.GetBoolValue("MW3 Main", "No Recoil", false);
		bNoSway = ini.GetBoolValue("MW3 Main", "No Sway", false);
		bStealGTs = ini.GetBoolValue("MW3 Main", "Steal GTs", false);
		bRainbowGT = ini.GetBoolValue("MW3 Main", "Rainbow GT", false);
		bShowHost = ini.GetBoolValue("MW3 Main", "Show Host", false);
		bShowPing = ini.GetBoolValue("MW3 Main", "Show Ping", false);
		bFastReload = ini.GetBoolValue("MW3 Main", "Fast Reload", false);
		bEveryoneHearsMe = ini.GetBoolValue("MW3 Main", "Everyone Hears Me", false);
		bNeverHost = ini.GetBoolValue("MW3 Main", "Never Host", false);
		iFieldOfViewIndex = ini.GetLongValue("MW3 Main", "Field of View", 0);
		bThirdPerson = ini.GetBoolValue("MW3 Main", "Third Person", false);
		iFakeLagAmount = ini.GetLongValue("MW3 Main", "Fake Lag Amount", 0);

		// Ragebot Tab
		bRagebot = ini.GetBoolValue("MW3 Ragebot", "Enable Ragebot", false);
		iRagebotType = ini.GetLongValue("MW3 Ragebot", "Ragebot Type", 0);
		iRBAimBone = ini.GetLongValue("MW3 Ragebot", "Aim Bone", 0);
		iRBPrediction = ini.GetLongValue("MW3 Ragebot", "Prediction Scale", 0);
		bNoSpread = ini.GetBoolValue("MW3 Ragebot", "No Spread", false);
		bNoFlinch = ini.GetBoolValue("MW3 Ragebot", "No Flinch", false);
		bAutoWall = ini.GetBoolValue("MW3 Ragebot", "Auto Wall", false);
		bAutoShoot = ini.GetBoolValue("MW3 Ragebot", "Auto Shoot", false);
		bAutoAim = ini.GetBoolValue("MW3 Ragebot", "Auto Aim", false);
		bRapidFire = ini.GetBoolValue("MW3 Ragebot", "Rapid Fire", false);
		bTargetShields = ini.GetBoolValue("MW3 Ragebot", "Target Shields", false);
		bRagebotPreferred = ini.GetBoolValue("MW3 Ragebot", "Preferred Settings", false);

		// Legitbot Tab
		bLegitbot = ini.GetBoolValue("MW3 Legitbot", "Enable Legitbot", false);
		iLBAimBone = ini.GetLongValue("MW3 Legitbot", "Aim Bone", 0);
		iLBAimButton = ini.GetLongValue("MW3 Legitbot", "Aim Button", 0);
		bLBAutoShoot = ini.GetBoolValue("MW3 Legitbot", "Auto Shoot", false);
		bLBAutoAim = ini.GetBoolValue("MW3 Legitbot", "Auto Aim", false);
		iReactionDelay = ini.GetLongValue("MW3 Legitbot", "Reaction Delay", 0);
		iFOVSize = ini.GetLongValue("MW3 Legitbot", "FOV Size", 0);
		bDrawFOVBox = ini.GetBoolValue("MW3 Legitbot", "Draw FOV Box", false);
		iSmoothScale = ini.GetLongValue("MW3 Legitbot", "Smooth Scale", 0);
		bTriggerbot = ini.GetBoolValue("MW3 Legitbot", "Triggerbot", false);
		bRapidFire = ini.GetBoolValue("MW3 Legitbot", "Rapid Fire", false);
		bLBTargetShields = ini.GetBoolValue("MW3 Legitbot", "Target Shields", false);
		bLegitbotPreferred = ini.GetBoolValue("MW3 Legitbot", "Preferred Settings", false);

		// Spinbot Tab
		bSpinbot = ini.GetBoolValue("MW3 Spinbot", "Enable Spinbot", false);
		iXSpinDirection = ini.GetLongValue("MW3 Spinbot", "X Spin Direction", 0);
		iYSpinDirection = ini.GetLongValue("MW3 Spinbot", "Y Spin Direction", 0);
		iYSpinSpeed = ini.GetLongValue("MW3 Spinbot", "Y Spin Speed", 0);
		iZSpinDirection = ini.GetLongValue("MW3 Spinbot", "Z Spin Direction", 0);
		iZSpinSpeed = ini.GetLongValue("MW3 Spinbot", "Z Spin Speed", 0);
		iFunnySpinbots = ini.GetLongValue("MW3 Spinbot", "Funny Spinbots", 0);
		iXAntiAimSpinbot = ini.GetLongValue("MW3 Spinbot", "X Anti-Aim Spinbot", 0);
		iYAntiAimSpinbot = ini.GetLongValue("MW3 Spinbot", "Y Anti-Aim Spinbot", 0);
		bSpinbotPreferred = ini.GetBoolValue("MW3 Spinbot", "Preferred Settings", false);

		// Drawing Tab
		bDrawEnemies = ini.GetBoolValue("MW3 Drawing", "Draw Enemies", false);
		bDrawFriendlies = ini.GetBoolValue("MW3 Drawing", "Draw Friendlies", false);
		iESPBoxType = ini.GetLongValue("MW3 Drawing", "ESP Box Type", 1);
		bDrawName = ini.GetBoolValue("MW3 Drawing", "Draw Name", false);
		bDrawWeapon = ini.GetBoolValue("MW3 Drawing", "Draw Weapon", false);
		bDrawBones = ini.GetBoolValue("MW3 Drawing", "Draw Bones", false);
		bDrawExplosives = ini.GetBoolValue("MW3 Drawing", "Draw Explosives", false);
		iSnaplines = ini.GetLongValue("MW3 Drawing", "Draw Snaplines", 0);
		bDrawKillstreaks = ini.GetBoolValue("MW3 Drawing", "Draw Killstreaks", false);
		bDirectionalArrows = ini.GetBoolValue("MW3 Drawing", "Directional Arrows", false);
		iCrosshairType = ini.GetLongValue("MW3 Drawing", "Crosshair Type", 0);
		bDrawCustomRadar = ini.GetBoolValue("MW3 Drawing", "Draw Custom Radar", false);
		bDrawDroppedItems = ini.GetBoolValue("MW3 Drawing", "Draw Dropped Items", false);
		bDrawEnemyVis = ini.GetBoolValue("MW3 Drawing", "Draw Enemy Visible Text", false);
		bDrawProximityWrn = ini.GetBoolValue("MW3 Drawing", "Draw Proximity Warning", false);
		bCartoonMode = ini.GetBoolValue("MW3 Drawing", "Cartoon Mode", false);

		// Settings Tab
		bDisableMenuSounds = ini.GetBoolValue("MW3 Settings", "Disable Menu Sounds", false);
		bDisableMenuInstructions = ini.GetBoolValue("MW3 Settings", "Disable Menu Instructions", false);
		bDisableNiNJAEngineText = ini.GetBoolValue("MW3 Settings", "Disable NiNJA Engine Text", false);
		bDisableMainMenuMusic = ini.GetBoolValue("MW3 Settings", "Disable Main Menu Music", false);
		bPrestigeDisco = ini.GetBoolValue("MW3 Settings", "Prestige Disco", false);
		bAntiDerank = ini.GetBoolValue("MW3 Settings", "Anti Derank", false);
		bLogIPs = ini.GetBoolValue("MW3 Settings", "Log IPs", false);

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
		ini.SetBoolValue("MW3 Main", "Radar", GetMenuOptionValue(MENU_MAIN, OPTION_RADAR));
		ini.SetBoolValue("MW3 Main", "No Recoil", GetMenuOptionValue(MENU_MAIN, OPTION_NORECOIL));
		ini.SetBoolValue("MW3 Main", "No Sway", GetMenuOptionValue(MENU_MAIN, OPTION_NOSWAY));
		ini.SetBoolValue("MW3 Main", "Steal GTs", GetMenuOptionValue(MENU_MAIN, OPTION_STEALGTS));
		ini.SetBoolValue("MW3 Main", "Rainbow GT", GetMenuOptionValue(MENU_MAIN, OPTION_RAINBOWGT));
		ini.SetBoolValue("MW3 Main", "Show Host", GetMenuOptionValue(MENU_MAIN, OPTION_SHOWHOST));
		ini.SetBoolValue("MW3 Main", "Show Ping", GetMenuOptionValue(MENU_MAIN, OPTION_SHOWPING));
		ini.SetBoolValue("MW3 Main", "Fast Reload", GetMenuOptionValue(MENU_MAIN, OPTION_FASTRELOAD));
		ini.SetBoolValue("MW3 Main", "Everyone Hears Me", GetMenuOptionValue(MENU_MAIN, OPTION_EVERYONEHEARSME));
		ini.SetBoolValue("MW3 Main", "Never Host", GetMenuOptionValue(MENU_MAIN, OPTION_NEVERHOST));
		ini.SetLongValue("MW3 Main", "Field of View", GetMenuOptionArrayIndex(MENU_MAIN, OPTION_FOV));
		ini.SetBoolValue("MW3 Main", "Third Person", GetMenuOptionValue(MENU_MAIN, OPTION_THIRDPERSON));
		ini.SetLongValue("MW3 Main", "Fake Lag Amount", GetMenuOptionArrayIndex(MENU_MAIN, OPTION_FAKELAGAMOUNT));

		// Ragebot Tab
		ini.SetBoolValue("MW3 Ragebot", "Enable Ragebot", GetMenuOptionValue(MENU_RAGEBOT, OPTION_ENABLERAGEBOT));
		ini.SetLongValue("MW3 Ragebot", "Ragebot Type", GetMenuOptionArrayIndex(MENU_RAGEBOT, OPTION_RAGEBOTTYPE));
		ini.SetLongValue("MW3 Ragebot", "Aim Bone", GetMenuOptionArrayIndex(MENU_RAGEBOT, OPTION_AIMBONE));
		ini.SetLongValue("MW3 Ragebot", "Prediction Scale", GetMenuOptionArrayIndex(MENU_RAGEBOT, OPTION_PREDICTION));
		ini.SetBoolValue("MW3 Ragebot", "No Spread", GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOSPREAD));
		ini.SetBoolValue("MW3 Ragebot", "No Flinch", GetMenuOptionValue(MENU_RAGEBOT, OPTION_NOFLINCH));
		ini.SetBoolValue("MW3 Ragebot", "Auto Wall", GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOWALL));
		ini.SetBoolValue("MW3 Ragebot", "Auto Shoot", GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOSHOOT));
		ini.SetBoolValue("MW3 Ragebot", "Auto Aim", GetMenuOptionValue(MENU_RAGEBOT, OPTION_AUTOAIM));
		ini.SetBoolValue("MW3 Ragebot", "Rapid Fire", GetMenuOptionValue(MENU_RAGEBOT, OPTION_RAPIDFIRE));
		ini.SetBoolValue("MW3 Ragebot", "Target Shields", GetMenuOptionValue(MENU_RAGEBOT, OPTION_TARGETSHIELDS));
		ini.SetBoolValue("MW3 Ragebot", "Preferred Settings", GetMenuOptionValue(MENU_RAGEBOT, OPTION_PREFERREDSETTINGS));

		// Legitbot Tab
		ini.SetBoolValue("MW3 Legitbot", "Enable Legitbot", GetMenuOptionValue(MENU_LEGITBOT, OPTION_ENABLELEGITBOT));
		ini.SetLongValue("MW3 Legitbot", "Aim Bone", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_AIMBONE));
		ini.SetLongValue("MW3 Legitbot", "Aim Button", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_AIMKEY));
		ini.SetBoolValue("MW3 Legitbot", "Auto Shoot", GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOSHOOT));
		ini.SetBoolValue("MW3 Legitbot", "Auto Aim", GetMenuOptionValue(MENU_LEGITBOT, OPTION_AUTOAIM));
		ini.SetLongValue("MW3 Legitbot", "Reaction Delay", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_REACTIONDELAY));
		ini.SetLongValue("MW3 Legitbot", "FOV Size", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_FOVSIZE));
		ini.SetBoolValue("MW3 Legitbot", "Draw FOV Box", GetMenuOptionValue(MENU_LEGITBOT, OPTION_DRAWFOVBOX));
		ini.SetLongValue("MW3 Legitbot", "Smooth Scale", GetMenuOptionArrayIndex(MENU_LEGITBOT, OPTION_SMOOTHSCALE));
		ini.SetBoolValue("MW3 Legitbot", "Triggerbot", GetMenuOptionValue(MENU_LEGITBOT, OPTION_TRIGGERBOT));
		ini.SetBoolValue("MW3 Legitbot", "Rapid Fire", GetMenuOptionValue(MENU_LEGITBOT, OPTION_RAPIDFIRE));
		ini.SetBoolValue("MW3 Legitbot", "Target Shields", GetMenuOptionValue(MENU_LEGITBOT, OPTION_TARGETSHIELDS));
		ini.SetBoolValue("MW3 Legitbot", "Preferred Settings", GetMenuOptionValue(MENU_LEGITBOT, OPTION_PREFERREDSETTINGS));

		// Spinbot Tab
		ini.SetBoolValue("MW3 Spinbot", "Enable Spinbot", GetMenuOptionValue(MENU_SPINBOT, OPTION_ENABLESPINBOT));
		ini.SetLongValue("MW3 Spinbot", "X Spin Direction", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_XSPINDIRECTION));
		ini.SetLongValue("MW3 Spinbot", "Y Spin Direction", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_YSPINDIRECTION));
		ini.SetLongValue("MW3 Spinbot", "Y Spin Speed", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_YSPINSPEED));
		ini.SetLongValue("MW3 Spinbot", "Z Spin Direction", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_ZSPINDIRECTION));
		ini.SetLongValue("MW3 Spinbot", "Z Spin Speed", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_ZSPINSPEED));
		ini.SetLongValue("MW3 Spinbot", "X Anti-Aim Spinbot", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_XANTIAIMSPINBOT));
		ini.SetLongValue("MW3 Spinbot", "Y Anti-Aim Spinbot", GetMenuOptionArrayIndex(MENU_SPINBOT, OPTION_YANTIAIMSPINBOT));
		ini.SetBoolValue("MW3 Spinbot", "Preferred Settings", GetMenuOptionValue(MENU_SPINBOT, OPTION_PREFERREDSETTINGS));

		// Drawing Tab
		ini.SetBoolValue("MW3 Drawing", "Draw Enemies", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMIES));
		ini.SetBoolValue("MW3 Drawing", "Draw Friendlies", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWFRIENDLIES));
		ini.SetLongValue("MW3 Drawing", "ESP Box Type", GetMenuOptionArrayIndex(MENU_DRAWING, OPTION_ESPBOXTYPE));
		ini.SetBoolValue("MW3 Drawing", "Draw Name", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWNAME));
		ini.SetBoolValue("MW3 Drawing", "Draw Weapon", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWWEAPON));
		ini.SetBoolValue("MW3 Drawing", "Draw Bones", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWBONES));
		ini.SetBoolValue("MW3 Drawing", "Draw Explosives", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWEXPLOSIVES));
		ini.SetLongValue("MW3 Drawing", "Draw Snaplines", GetMenuOptionArrayIndex(MENU_DRAWING, OPTION_DRAWSNAPLINES));
		ini.SetBoolValue("MW3 Drawing", "Draw Killstreaks", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWKILLSTREAKS));
		ini.SetBoolValue("MW3 Drawing", "Directional Arrows", GetMenuOptionValue(MENU_DRAWING, OPTION_DIRECTIONALARROWS));
		ini.SetLongValue("MW3 Drawing", "Crosshair Type", GetMenuOptionArrayIndex(MENU_DRAWING, OPTION_CROSSHAIRTYPE));
		ini.SetBoolValue("MW3 Drawing", "Draw Custom Radar", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWCUSTOMRADAR));
		ini.SetBoolValue("MW3 Drawing", "Draw Dropped Items", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWDROPPEDITEMS));
		ini.SetBoolValue("MW3 Drawing", "Draw Enemy Visible Text", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWENEMYVISIBLE));
		ini.SetBoolValue("MW3 Drawing", "Draw Proximity Warning", GetMenuOptionValue(MENU_DRAWING, OPTION_DRAWPROXIMITYWARNING));
		ini.SetBoolValue("MW3 Drawing", "Cartoon Mode", GetMenuOptionValue(MENU_DRAWING, OPTION_CARTOONMODE));

		// Settings Tab
		ini.SetBoolValue("MW3 Settings", "Disable Menu Sounds", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUSOUNDS));
		ini.SetBoolValue("MW3 Settings", "Disable Menu Instructions", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUINSTRUCTIONS));
		ini.SetBoolValue("MW3 Settings", "Disable NiNJA Engine Text", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLENINJATEXT));
		ini.SetBoolValue("MW3 Settings", "Disable Main Menu Music", GetMenuOptionValue(MENU_SETTINGS, OPTION_DISABLEMENUMUSIC));
		ini.SetBoolValue("MW3 Settings", "Prestige Disco", GetMenuOptionValue(MENU_SETTINGS, OPTION_PRESTIGEDISCO));
		ini.SetBoolValue("MW3 Settings", "Anti Derank", GetMenuOptionValue(MENU_SETTINGS, OPTION_ANTIDERANK));
		ini.SetBoolValue("MW3 Settings", "Log IPs", GetMenuOptionValue(MENU_SETTINGS, OPTION_LOGIPS));

		ini.SaveFile(INI_PATH);

		Sleep(500);
		DbgPrint("Saved ini values!");

		return true;
	}
	return false;
}