#ifndef GHOSTS_H
#define GHOSTS_H

#include "stdafx.h"
#include "Utilities.h"
#include "SecureStrings.h"
#include "Menu.h"
#include "GeoLocationParser.h"


typedef struct refdef_s_Ghosts
{
	BYTE _0x0000[4];
	DWORD Width; // 0x0008
	DWORD Height; // 0x000C
	Vector2 Fov; // 0x0010
	Vector3 ViewOrigin; // 0x0018
	Vector3 ViewAxis[3]; // 0x0024
} refdef_s_Ghosts, *prefdef_s_Ghosts; // custom size 0x0048

typedef struct
{
	BYTE _0x0000[4];
	DWORD Team; // 0x0004
	BYTE _0x0008[56];
	CHAR Name[32]; // 0x0040
	BYTE _0x0060[0xF0 - 0x60];
}client_t_Ghosts; // custom size 0x00F0

struct snapshot_s_Ghosts
{
	BYTE _0x0000[0x3864];//0x0000
	DWORD Ping;//0x3864 
};

typedef struct TeamCheck_Ghosts
{
	DWORD Team;//0x0000
	BYTE _0x0004[0x10 - 0x04];//0x0004
	BYTE ValidCheck;//0x10
	BYTE _0x0011[0x59C - 0x11];//0x11
}TeamCheck_Ghosts, *pTeamCheck_Ghosts;//0x59C

typedef struct ClientName_Ghosts
{
	CHAR Name[32];//0x0000
	BYTE _0x0020[0x74 - 0x20];//0x0020
}ClientName_Ghosts, *pClientName_Ghosts;//0x74

typedef struct cg_s_Ghosts
{
	DWORD CommandTime; // 0x0000
	BYTE _0x0004[24];
	Vector3 Origin; // 0x001C
	Vector3 Velocity; // 0x0028
	BYTE _0x0034[328];
	DWORD ClientNumber; // 0x017C
	BYTE _0x0180[0x1C8 - 0x180];
	DWORD Health;//0x1C8
	DWORD PadFix;//0x1CC
	BYTE _0x01CC[0x2FD8 - 0x1CC];//0x1CC
	snapshot_s_Ghosts *snapShot;//0x2FD8
	BYTE _0x2FDC[0x6BAF0 - 0x2FDC];//0x2FDC
	client_t_Ghosts clientInfo[18];// 0x6BAF0
	BYTE _0x6CBD0[2412];
	refdef_s_Ghosts refdef; // 0x6D53C
	BYTE Padding[0xB943C - 0x6D584];//0x6D584
	TeamCheck_Ghosts ClientTeam[12];//0xB943C
	BYTE _0xBD78C[0xC5E24 - 0xBD78C];//0xBD78C
	ClientName_Ghosts ClientName[12];//0xC5E24
} cg_s_Ghosts, *pcg_s_Ghosts;// size 0x6ED0

typedef struct cgs_t_Ghosts
{
	BYTE _0x0000[8];//0x00
	__int32 Width;//0x08
	__int32 Height;//0x0C
	BYTE _0x0010[20];//0x10
	BYTE Gametype[32]; // 0x0024
	CHAR HostName[260]; // 0x0044
	DWORD MaxClients; // 0x0148
	DWORD PrivateClients; // 0x014C
	CHAR Mapname[64]; // 0x0150
	BYTE _0x0190[0x6ED0 - 0x190];//0x190
} cgs_t_Ghosts, *pcgs_t_Ghosts; // size 0x40B8

typedef struct usercmd_s_Ghosts
{
	int serverTime;								//0x00-0x04
	int buttons;								//0x04-0x08
	int angles[3];								//0x08-0x14
	int weapon;									//0x14-0x18
	int offHand;								//0x18-0x1C
	char forwardmove;							//0x1C-0x1D
	char rightmove;								//0x1D-0x1E
	unsigned __int16 airburstMarkDistance;		//0x1E-0x20
	unsigned __int16 meleeChargeEnt;			//0x20-0x22
	char meleeChargeDist;						//0x22-0x23
	char selectedLoc[2];						//0x23-0x25
	char selectedLocAngle;						//0x25-0x26
	char remoteControlAngles[2];				//0x26-0x28
	char remoteControlMove[3];					//0x28-0x2B
	unsigned int sightedClientsMask;			//0x2C-0x30
	unsigned __int16 spawnTraceEntIndex;		//0x30-0x32
	unsigned int sightedSpawnsMask;				//0x34-0x38
	unsigned int partialSightedSpawnsMask;		//0x38-0x3C
} usercmd_s_Ghosts, *pusercmd_s_Ghosts;// 0x003C

typedef struct clientActive_t_Ghosts
{
	BYTE _0x0000[156];
	Vector3 Origin; // 0x009C
	Vector3 Velocity; // 0x00A8
	BYTE _0x00B4[48];
	Vector3 SpawnAngles; // 0x00E4
	BYTE _0x00F0[12768];
	Vector3 ViewAngles; // 0x32D0
	usercmd_s_Ghosts UserCommands[128]; // 0x32DC
	DWORD CommandNumber; // 0x50DC
	BYTE _0x50E0[35992];

	inline DWORD GetUserCommandNumber()
	{
		return CommandNumber & 0x7F;
	}
	inline pusercmd_s_Ghosts GetUserCommand(DWORD CommandNumber)
	{
		return &UserCommands[CommandNumber]; // # r10 = r11 & 0x7F
	}
} clientActive_t_Ghosts, *pclientActive_t_Ghosts; // 0xDD78

struct EntityEvent
{
	int eventType;//0x00
	int eventParam;//0x04
};//0x08

#pragma pack(push, 2)
typedef struct centity_s_Ghosts
{
	BYTE _0x0000[2];//0x00
	BYTE isAliveBYTE1;//0x02 || 0 = Dead
	BYTE isAliveBYTE2;//0x03
	BYTE _0x0004[0x10];//0x04
	Vector3 Origin;//0x14
	BYTE _0x0020[0x5D - 0x20];//0x20
	BYTE isShootingAiming;//0x5D
	BYTE Padding;//0x5E
	BYTE Stance;//0x5F
	BYTE _0x0060[0xC];//0x60
	Vector3 Origin2;//0x6C
	BYTE _0x0078[0xC8 - 0x78];//0x78
	__int32 Type;//0xC8
	DWORD Status;//0xCC
	BYTE _0x00D0[0xC];//0xD0
	Vector3 Origin3;//0xDC
	BYTE _0x00E8[0x100 - 0xE8];//0xE8
	Vector3 Angles;//0x100
	BYTE _0x010C[0x154 - 0x10C]; //0x10C
	DWORD ClientNum;//0x154
	DWORD unkShit;//0x158
	int solid;//0x15C
	unsigned int eventParm;//0x160
	int eventSequence;//0x164
	EntityEvent events[4];//0x168
	DWORD WeaponID;//0x188
	BYTE _0x018C[0x1C7 - 0x18C];//0x18C
	BYTE isAlive2;//0x1C7
	BYTE _0x01C8[0x1EC - 0x1C8];//0x1C8
} centity_s_Ghosts, *pcentity_s_Ghosts; // size 0x1EC

struct boundingBox
{
	Vector3 crnr2,
		crnr3,
		crnr4,
		crnr5,
		crnr6,
		crnr7,
		mins,
		maxes;
	float minsY;
};

struct ClientInformation_Ghosts
{
	BOOL Alive;
	BOOL Visible;
	BOOL VisibleESP;
	BOOL Wallbangable;
	BOOL GVisible;
	BOOL GVisibleESP;
	char* SelectedBone;
	Vector3 TagPosistion;
	FLOAT Distance;
	PCHAR Name;
	DWORD Team;
	BOOL OnTeam;
	DWORD ThreatLevel;
	DWORD ClientNumber;
	BOOL Whitelisted;
	BOOL Prioritized;
	DWORD EntType;
	BOOL CurrentTarget;
	BOOL PrimaryShield;
	FLOAT AutowallPower;
	BOOL ShouldTriggerbot;
	BOOL IsExplosive;
	boundingBox bBox;
	BOOL GrabGeoIPInfo;
	IN_ADDR ip;
	char city[40];
	char state[40];
	char stateFull[40];
	char country[40];
	char isp[40];
	char zip[40];

	void reset()
	{
		GrabGeoIPInfo = FALSE;
		memset(city, 0, sizeof(city));
		memset(state, 0, sizeof(state));
		memset(stateFull, 0, sizeof(stateFull));
		memset(country, 0, sizeof(country));
		memset(isp, 0, sizeof(isp));
		memset(zip, 0, sizeof(zip));
	}

	ClientInformation_Ghosts()
	{
		reset();
	}
};
extern ClientInformation_Ghosts PlayerInfo[18];

enum TraceHitType
{
	TRACE_HITTYPE_NONE = 0,
	TRACE_HITTYPE_ENTITY = 1,
	TRACE_HITTYPE_DYNENT_MODEL = 2,
	TRACE_HITTYPE_DYNENT_BRUSH = 3,
	TRACE_HITTYPE_DYNENT_UNKNOWN = 4
};

struct trace_t
{
	float Fraction;//0x00
	Vector3 normal;//0x04
	BYTE _0x0010[0x20];//0x10
};

struct BulletFireParams
{
	__int32 worldEntNum; //0x0000
	__int32 skipNum; //0x0004
	float power; //0x0008
	__int32 bulletType; //0x000C
	__int32 noRicochet;
	Vector3 vieworg; //0x0010
	Vector3 start; //0x001C
	Vector3 end; //0x0028
	Vector3 viewDirection; //0x0034
};

typedef enum XAssetType_Ghosts : DWORD
{
	material_Ghosts = 0x05,
	font_Ghosts = 0x18
} XAssetType_Ghosts;//tu8

typedef enum ThreatTypes : DWORD
{
	Blacklisted = 0x200, // If blacklisted, bigest priority
	AntiAim = 0x100, // If the user has anti-aim, they should be targeted over others
	ThreatDistance = 0x80, // Closest Client
	AimingAtUs = 0x40, // If they are aiming at us 
	ThreatWeapon = 0x20, // If they have a LMG
	ThreatAttachment = 0x10, // If they have FMJ/Extended Mags
	AimingDownSight = 0x8, // If they are aiming down their sights
	PlayerNotProne = 0x4 // If they are not laying down
} ThreatTypes;

typedef struct
{
	float x; //0x0000
	float y; //0x0004
	float w; //0x0008
	float h; //0x000C
	DWORD unk;//0x0010
} CRadarHud;


static enum eMenuOptions
{
	//Main
	OPTION_RADAR,
	OPTION_NORECOIL,
	OPTION_NOSWAY,
	OPTION_CHANGETEAM,
	OPTION_STEALGTS,
	OPTION_RAINBOWGT,
	OPTION_SPOOFGT,
	OPTION_SHOWHOST,
	OPTION_SHOWPING,
	OPTION_FASTRELOAD,
	OPTION_EVERYONEHEARSME,
	OPTION_NEVERHOST,
	OPTION_FOV,
	OPTION_THIRDPERSON,
	OPTION_FAKELAGAMOUNT,
	OPTION_JUGGCLASS,
	OPTION_ENDGAME,
	OPTION_LEAVEGAME,

	//Ragebot
	OPTION_ENABLERAGEBOT,
	OPTION_RAGEBOTTYPE,
	OPTION_AIMBONE,
	OPTION_PREDICTION,
	OPTION_NOSPREAD,
	OPTION_NOFLINCH,
	OPTION_AUTOWALL,
	OPTION_AUTOSHOOT,
	OPTION_AUTOAIM,
	OPTION_RAPIDFIRE,
	OPTION_TARGETSHIELDS,
	OPTION_PREFERREDSETTINGS,

	//Legitbot
	OPTION_ENABLELEGITBOT,
	OPTION_AIMKEY,
	OPTION_REACTIONDELAY,
	OPTION_FOVSIZE,
	OPTION_DRAWFOVBOX,
	OPTION_SMOOTHSCALE,
	OPTION_TRIGGERBOT,

	//Spinbot
	OPTION_ENABLESPINBOT,
	OPTION_XSPINDIRECTION,
	OPTION_YSPINDIRECTION,
	OPTION_YSPINSPEED,
	OPTION_ZSPINDIRECTION,
	OPTION_ZSPINSPEED,
	OPTION_XANTIAIMSPINBOT,
	OPTION_YANTIAIMSPINBOT,

	//Drawing
	OPTION_DRAWENEMIES,
	OPTION_DRAWFRIENDLIES,
	OPTION_ESPBOXTYPE,
	OPTION_DRAWNAME,
	OPTION_DRAWWEAPON,
	OPTION_DRAWBONES,
	OPTION_DRAWEXPLOSIVES,
	OPTION_DRAWSNAPLINES,
	OPTION_DRAWKILLSTREAKS,
	OPTION_DIRECTIONALARROWS,
	OPTION_CROSSHAIRTYPE,
	OPTION_DRAWCUSTOMRADAR,
	OPTION_DRAWDROPPEDITEMS,
	OPTION_DRAWENEMYVISIBLE,
	OPTION_DRAWPROXIMITYWARNING,
	OPTION_CARTOONMODE,

	//Clients
	OPTION_NAMEDEFAULT,

	//Settings
	OPTION_DISABLEMENUSOUNDS,
	OPTION_DISABLEMENUINSTRUCTIONS,
	OPTION_DISABLENINJATEXT,
	OPTION_DISABLEMENUMUSIC,
	OPTION_PRESTIGEDISCO,
	OPTION_PRESTIGE,
	OPTION_LEVEL60,
	OPTION_SQUADPOINTS,
	OPTION_UNLOCKALL,
	OPTION_ANTIDERANK,
	OPTION_INVERTTEAMCHECK,
	OPTION_LOGIPS
};
static vector<PCHAR> MenuOptionNames;

static enum eMenuNames
{
	MENU_MAIN,
	MENU_RAGEBOT,
	MENU_LEGITBOT,
	MENU_SPINBOT,
	MENU_DRAWING,
	MENU_CLIENTS,
	MENU_SETTINGS,
	MENU_NAMES_SIZE
};
static vector<PCHAR> MenuNames;

int GetMenuOptionValue(eMenuNames SubMenu, eMenuOptions Name);
int GetMenuOptionArrayIndex(eMenuNames SubMenu, eMenuOptions Name);


#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define DotProduct(a, b) ((a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]))
#define	ANGLE2SHORT(x)	((int)((x)*65536/360) & 65535)
#define	SHORT2ANGLE(x)	((x)*(360.0/65536))
#define PI 3.14159265358979323846
#define DEG2RAD(DEG) ((DEG)*((PI)/(180.0)))

#define colorRed                Vector4(255, 0, 153, 255)
#define colorOrange             Vector4(230, 126, 34, 175) //127
#define colorBlue               Vector4(52, 152, 219, 255)
#define colorGreen              Vector4(46, 204, 113, 255)
#define colorPurple             Vector4(128, 0, 255, 255)
#define colorBrightPurple       Vector4(255, 0, 255, 255)
#define colorYellow             Vector4(255, 255, 0, 255)
#define colorWhite              Vector4(255, 255, 255, 255)
#define colorVioletFlower       Vector4(191, 95, 255, 255)
#define GetColorCode(color)     Vector4(color.x / 255, color.y / 255, color.z / 255, color.a / 255)

static CONST PCHAR(__cdecl *va_Ghosts)(CONST PCHAR Format, ...);
static BOOL(__cdecl *Dvar_GetBool_Ghosts)(CONST PCHAR Dvar);
static void(__cdecl *CG_LocationalTraceGhosts)(void *results, Vector3 *start, Vector3 *end, int passEntityNum, int contMask);
static DWORD(__cdecl *DB_FindXAssetHeader_Ghosts)(XAssetType_Ghosts Type, CONST PCHAR Name, DWORD r5);
static DWORD(__cdecl *R_TextWidth_Ghosts)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font);
static VOID(__cdecl *CL_DrawTextPhysical_Ghosts)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font, FLOAT X, FLOAT Y, FLOAT XScale, FLOAT YScale, FLOAT Angle, Vector4& Color, DWORD Style);
static VOID(__cdecl *R_AddCmdDrawStretchPic_Ghosts)(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, FLOAT XScale, FLOAT YScale, FLOAT Xay, FLOAT Yay, Vector4& Color, DWORD Material);
static VOID(__cdecl* CG_DrawRotatedPicPhysical_Ghosts)(DWORD ScreenPlacement, FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, FLOAT Angle, Vector4& Color, DWORD Material);
static VOID(__cdecl *UI_FillRectPhysical_Ghosts)(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color);
static void(__cdecl *Cbuf_AddText_Ghosts)(DWORD LocalClientNum, char *Message);
static short(__cdecl *SL_GetString_Ghosts)(PCHAR String, DWORD aZero);
static int(__cdecl *CG_GetWeaponIndexForName)(char* name);
static int(__cdecl *PlayLocalSoundAliasByName)(int localClientNum, char *aliasname, int a0);
static void(__cdecl *CG_GetEntityBModelBounds)(pcentity_s_Ghosts centity_s, float* mins, float* maxs);
static int(__cdecl *R_NormalizedTextScale_Ghosts)(int font, float scale);
static bool(__cdecl *CG_CalcNamePosition)(int unk, Vector3* origin, float *xOut, float *yOut);
static float(__cdecl *G_GetWeaponHitLocationMultiplier)(int unk, int WeaponNum);
static CONST PCHAR(__cdecl *GetWeaponHUDName_Ghosts)(DWORD WeaponIndex, DWORD nullr4, PCHAR Buffer, DWORD MaxChars);

// No Spread Functions
static VOID(__cdecl *BG_GetSpreadForWeapon_Ghosts)(pcg_s_Ghosts cg_Ghosts, DWORD weaponNum, float* min, float* max);
static VOID(__cdecl *RandomBulletDir_Ghosts)(int* Seed, float zero, float threesixty, float* x, float* y);
static PDWORD(__cdecl *BG_GetWeaponDef_Ghosts)(DWORD WeaponIndex);
static VOID(__cdecl *CG_GetPlayerViewOrigin_Ghosts)(int LocalClientNum, pcg_s_Ghosts cg_Ghosts, Vector3& Origin);
static float(__cdecl *sub_8222EC70)(int* Seed);

// Radar Functions
static VOID(__cdecl* DrawEngineRadar_Ghosts)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, DWORD shader, Vector4& Color);
static VOID(__cdecl* DrawEnemies_Ghosts)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color);
static VOID(__cdecl* DrawFriendlies_Ghosts)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color);
static VOID(__cdecl* DrawVehicles_Ghosts)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color);
static VOID(__cdecl* DrawPlayer_Ghosts)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color);
static BOOL(__cdecl* CG_WorldPosToCompass_Ghosts)(DWORD CompassType, pcg_s_Ghosts cg_Ghosts, Vector4& Size, Vector2& Yaw, Vector3& WorldPos, Vector3& WorldPos2, Vector2& Out, Vector2& OutClipped);
static BOOL(__cdecl* CG_CompassCalcDimensions_Ghosts)(DWORD CompassType, pcg_s_Ghosts cg_Ghosts, Vector2& Size, CRadarHud* RadarHud, float* x, float* y, float* w, float* h);
static void(__cdecl* CG_DrawRotatedPic_Ghosts)(DWORD scrPlace, float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, Vector4& color, DWORD material);
static void(__cdecl* CG_CompassUpYawVector_Ghosts)(pcg_s_Ghosts cg_Ghosts, Vector2& Yaw);
static float(__cdecl* AngleNormalize360_Ghosts)(float unk);


//AutoWall Functions
static VOID(__cdecl *FireBulletPenetrate_Ghosts)(DWORD LocalClientNum, BulletFireParams* bp, DWORD weaponIndex, DWORD unk1, pcentity_s_Ghosts centity_s, DWORD neg1, Vector3& Origin, bool drawTracer);

static char* TeamNames[] =
{
	"spectator",
	"axis",
	"allies"
};

static char* VisCheckBones[8] =
{
	"j_head","j_helmet", "j_spinelower", "j_spineupper",
	"j_shoulder_le", "j_shoulder_ri", "j_ankle_le", "j_ankle_ri"
};

static char* HitboxVisCheckBones[13] =
{
	"j_head", "j_neck", "j_spineupper", "j_mainroot", "j_spinelower", "j_shoulder_ri", "j_shoulder_le",
	"j_wrist_ri", "j_wrist_le", "j_knee_ri", "j_knee_le", "j_ankle_ri", "j_ankle_le"
};

static char* AutoWallBones[7] =
{
	"j_head", "j_hip_le", "j_hip_ri", "j_mainroot",
	"j_spineupper", "j_shoulder_le", "j_shoulder_ri",
};

static char* ThreeDBoxesBones[] =
{
	"j_helmet", "j_neck", "j_head", "j_ankle_le",
	"j_ankle_ri", "j_wrist_le", "j_wrist_ri", 
	"j_mainroot", "j_knee_le", "j_knee_ri", 
	"j_elbow_le", "j_elbow_ri", "j_shoulder_le",
	"j_shoulder_ri"
};

static char* bonesESPMW2[] =
{
	"j_helmet", "j_head", "j_neck",
	"j_shoulder_le", "j_elbow_le", "j_wrist_le", "j_elbow_le", "j_shoulder_le", "j_neck",
	"j_shoulder_ri", "j_elbow_ri", "j_wrist_ri", "j_elbow_ri", "j_shoulder_ri", "j_neck",
	"j_spineupper", "j_spinelower",
	"j_hip_le", "j_knee_le", "j_ankle_le", "j_knee_le", "j_hip_le",
	"j_spinelower",
	"j_hip_ri", "j_knee_ri", "j_ankle_ri"
};

static char* BoneCache[] = {
	"j_helmet", "j_head", "j_neck", "j_shoulder_le", "j_shoulder_ri",
	"j_elbow_le", "j_elbow_ri",  "j_wrist_le", "j_wrist_ri", "j_spineupper",
	"j_spinelower", "j_mainroot", "j_hip_le", "j_hip_ri", "j_knee_ri", "j_knee_le",
	"j_ankle_le", "j_ankle_ri", "pelvis", "j_spine4", "tag_eye"
};

static char *TriggerBotBones[14] = {
	"j_head",
	"j_neck",
	"j_spine4",
	"pelvis",
	"j_knee_le",
	"j_knee_ri",
	"j_ankle_le",
	"j_ankle_ri",
	"j_shoulder_le",
	"j_shoulder_ri",
	"j_elbow_le",
	"j_elbow_ri",
	"j_wrist_le",
	"j_wrist_ri"
};

static int RagebotTypes_Size = 2;
static int RagebotTypes_Start = 0;
static char *RagebotTypes[] =
{
	"Silent",
	"Normal",
};

static int RagebotLogics_Size = 2;
static int RagebotLogics_Start = 0;
static char *RagebotLogics[] =
{
	"Distance",
	"Threat",
};

static int AimKeys_Size = 16;
static int AimKeys_Start = 0;
static char *AimKeys[] =
{
	"None",
	"LT",
	"RT",
	"LT + RT",
	"DPAD UP",
	"DPAD RIGHT",
	"DPAD DOWN",
	"DPAD LEFT",
	"A",
	"B",
	"X",
	"Y",
	"RB",
	"LB",
	"BACK",
	"RS",
	"LS"
};

static int AimBones_Size = 21;
static int AimBones_Start = 0;
static char *AimBones[] =
{
	"Best", "j_head", "tag_eye", "j_neck", "j_helmet", "j_spineupper",
	"j_spinelower", "j_spine4", "j_shoulder_ri",  "j_shoulder_le",
	"j_mainroot", "pelvis", "j_hip_ri", "j_hip_le",
	"j_knee_ri", "j_knee_le", "j_elbow_ri", "j_elbow_le",
	"j_wrist_ri", "j_wrist_le", "back_low"
};

static int AimBonesLegit_Size = 19;
static int AimBonesLegit_Start = 0;
static char *AimBonesLegit[] =
{
	"j_head", "tag_eye", "j_neck", "j_helmet", "j_spineupper",
	"j_spinelower", "j_spine4", "j_shoulder_ri",  "j_shoulder_le",
	"j_mainroot", "pelvis", "j_hip_ri", "j_hip_le",
	"j_knee_ri", "j_knee_le", "j_elbow_ri", "j_elbow_le",
	"j_wrist_ri", "j_wrist_le",
};

static int ESPBoxes_Size = 6;
static int ESPBoxes_Start = 0;
static char *ESPBoxes[] =
{
	"Off",
	"3D",
	"Corners",
	"2D",
	"2D Filled",
	"Pyramid"
};

static int SnaplinesTypes_Size = 4;
static int SnaplinesTypes_Start = 0;
static char *SnaplinesTypes[] =
{
	"Off",
	"Bottom",
	"Center",
	"Top"
};

static int CrosshairTypes_Size = 4;
static int CrosshairTypess_Start = 0;
static char *CrosshairTypes[] =
{
	"Off",
	"+",
	"x",
	"o"
};

static int FOVSizes_Size = 7;
static int FOVSizes_Start = 0;
static char *FOVSizes[] =
{
	"80.0",
	"90.0",
	"100.0",
	"120.0",
	"140.0",
	"180.0",
	"220.0"
};

static int SmoothScales_Size = 6;
static int SmoothScales_Start = 0;
static char *SmoothScales[] =
{
	"5.0",
	"10.0",
	"15.0",
	"20.0",
	"25.0",
	"30.0",
};


static int XAASpinbots_Size = 6;
static int XAASpinbots_Start = 0;
static char *XAASpinbots[] =
{
	"Off",
	"Look Down",
	"Up & Down",
	"Fake Up",
	"Detection",
	"Riot Shield"
};

static int YAASpinbots_Size = 10;
static int YAASpinbots_Start = 0;
static char *YAASpinbots[] =
{
	"Off",
	"Flip Flop",
	"Face Away",
	"Jitter",
	"Jitter Synced",
	"Sideways",
	"Detection",
	"Angel Backwards",
	"Static",
	"Riot Shield"
};

static int MenuControls_Size = 2;
static int MenuControls_Start = 0;
static char *MenuControls[] =
{
	"\x0016 & \x0012", // Left Dpad & LT
	"\x0016",
};

static int FOVS_Size = 7;
static int FOVS_Start = 0;
static char *FOVS[] =
{
	"Normal",
	"70.0",
	"75.0",
	"80.0",
	"90.0",
	"100.0",
	"110.0",
};


static int XSpinDirects_Size = 3;
static int XSpinDirects_Start = 0;
static char *XSpinDirects[] =
{
	"Off",
	"Look Down",
	"Look Up"
};

static int YSpinDirects_Size = 3;
static int YSpinDirects_Start = 0;
static char *YSpinDirects[] =
{
	"Off",
	"Left",
	"Right"
};

static int ZSpinDirects_Size = 3;
static int ZSpinDirects_Start = 0;
static char *ZSpinDirects[] =
{
	"Off",
	"Left",
	"Right"
};

static int YSpinSpeeds_Size = 9;
static int YSpinSpeeds_Start = 0;
static char *YSpinSpeeds[] =
{
	"5.0",
	"10.0",
	"20.0",
	"30.0",
	"40.0",
	"50.0",
	"60.0",
	"70.0",
	"Random"
};

static int ZSpinSpeeds_Size = 9;
static int ZSpinSpeeds_Start = 0;
static char *ZSpinSpeeds[] =
{
	"5.0",
	"10.0",
	"20.0",
	"30.0",
	"40.0",
	"50.0",
	"60.0",
	"70.0",
	"Random"
};

static int FunnySpinbots_Size = 2;
static int FunnySpinbots_Start = 0;
static char *FunnySpinbots[] =
{
	"Off",
	"Crazy Head"
};

static int FakeLagTypes_Size = 8;
static int FakeLagTypes_Start = 0;
static char *FakeLagTypes[] =
{
	"Off",
	"5.0",
	"10.0",
	"15.0",
	"20.0",
	"25.0",
	"30.0",
	"Random"
};

static int ChamTypes_Size = 3;
static int ChamTypes_Start = 0;
static char *ChamTypes[] =
{
	"Off",
	"Solid",
	"Wireframe"
};

static int ReactionDelays_Size = 8;
static int ReactionDelays_Start = 0;
static char *ReactionDelays[] =
{
	"Off",
	"2.0",
	"3.0",
	"4.0",
	"5.0",
	"6.0",
	"7.0",
	"Random"
};

static int Prestiges_Size = 13;
static int Prestiges_Start = 0;
static char *Prestiges[] =
{
	"N/A",
	"0th",
	"1st",
	"2nd",
	"3rd",
	"4th",
	"5th",
	"6th",
	"7th",
	"8th",
	"9th",
	"10th",
	"11th"
};

static int PredictionScales_Size = 6;
static int PredictionScales_Start = 0;
static char *PredictionScales[] =
{
	"Off",
	"2.0",
	"5.0",
	"10.0",
	"15.0",
	"20.0"
};

// j_mainroot
// j_coatfront_le
// j_coatfront_ri
// j_coatrear_le
// j_coatrear_ri
// j_hip_le
// j_hip_ri
// j_spinelower
// j_hiptwist_le
// j_hiptwist_ri
// j_knee_le
// j_knee_ri
// j_shorts_le
// j_shorts_ri
// j_shorts_lift_le
// j_shorts_lift_ri
// j_spineupper
// j_ankle_le
// j_ankle_ri
// j_knee_bulge_le
// j_knee_bulge_ri
// j_spine4
// j_ball_le
// j_ball_ri
// j_clavicle_le
// j_clavicle_ri 
// j_collar
// j_neck
// j_shoulderraise_le
// j_shoulderraise_ri
// j_head
// tag_weapon_chest
// j_brow_le
// j_brow_ri
// j_cheek_le
// j_cheek_ri
// j_elbow_bulge_le
// j_elbow_bulge_ri
// j_eye_lid_bot_le
// j_eye_lid_bot_ri
// j_eye_lid_top_le
// j_eye_lid_top_ri
// j_eyeball_le
// j_eyeball_ri
// j_head_end
// j_jaw
// tag_eye
// j_levator_le
// j_levator_ri
// j_lip_top_le
// j_lip_top_ri
// j_mouth_le
// j_mouth_ri
// j_shouldertwist_le
// j_shouldertwist_ri
// j_chin_skinroll
// j_helmet
// j_lip_bot_le
// j_lip_bot_ri
// j_kneebuldgelow_le
// j_kneebuldgelow_ri
// j_kneebuldgeup_le
// j_kneebuldgeup_ri
// j_collar_front

#endif