#ifndef MENU_H
#define MENU_H

#include "stdafx.h"
#include "Utilities.h"
#include "SimpleIni.h"
#include <vector>
#include "COD4.h"

using namespace std;

// Main Tab 
extern bool bRadar;
extern bool bNoRecoil;
extern bool bNoSway;
extern bool bStealGTs;
extern bool bRainbowGT;
extern bool bShowHost;
extern bool bShowPing;
extern bool bFastReload;
extern bool bEveryoneHearsMe;
extern bool bNeverHost;
extern int iFieldOfViewIndex;
extern bool bThirdPerson;
extern int iFakeLagAmount;

// Ragebot Tab
extern bool bRagebot;
extern int iRagebotType;
extern int iRBAimBone;
extern int iRBPrediction;
extern bool bNoSpread;
extern bool bNoFlinch;
extern bool bAutoWall;
extern bool bAutoShoot;
extern bool bAutoAim;
extern bool bRapidFire;
extern bool bTargetShields;
extern bool bRagebotPreferred;
extern bool bTeleportKill;

// Legitbot Tab
extern bool bLegitbot;
extern int iLBAimBone;
extern int iLBAimButton;
extern bool bLBAutoShoot;
extern bool bLBAutoAim;
extern int iReactionDelay;
extern int iFOVSize;
extern bool bDrawFOVBox;
extern int iSmoothScale;
extern bool bTriggerbot;
extern bool bLBRapidFire;
extern bool bLBTargetShields;
extern bool bLegitbotPreferred;

// Spinbot Tab
extern bool bSpinbot;
extern int iXSpinDirection;
extern int iYSpinDirection;
extern int iYSpinSpeed;
extern int iZSpinDirection;
extern int iZSpinSpeed;
extern int iFunnySpinbots;
extern int iXAntiAimSpinbot;
extern int iYAntiAimSpinbot;
extern bool bSpinbotPreferred;

// Drawing Tab
extern bool bDrawEnemies;
extern bool bDrawFriendlies;
extern int iESPBoxType;
extern bool bDrawName;
extern bool bDrawWeapon;
extern bool bDrawBones;
extern bool bDrawExplosives;
extern int iSnaplines;
extern bool bDrawKillstreaks;
extern bool bDirectionalArrows;
extern int iCrosshairType;
extern bool bDrawCustomRadar;
extern bool bDrawDroppedItems;
extern bool bDrawEnemyVis;
extern bool bDrawProximityWrn;
extern bool bDrawThroughWalls;
extern bool bCartoonMode;

// Settings Tab
extern bool bDisableMenuSounds;
extern bool bDisableMenuInstructions;
extern bool bDisableNiNJAEngineText;
extern bool bDisableMainMenuMusic;
extern bool bPrestigeDisco;
extern bool bAntiDerank;
extern bool bLogIPs;


extern bool shouldSaveMenuOptions;

#define INI_PATH "XBLIVE:\\Engine.ini"
bool LoadMenuOptions();
bool SaveMenuOptions();

#endif