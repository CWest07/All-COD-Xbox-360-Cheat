#ifndef WAW_H
#define WAW_H

#include "stdafx.h"
#include "Utilities.h"
#include "SecureStrings.h"
#include "Menu.h"
#include "GeoLocationParser.h"


typedef struct refdef_s_WAW
{
	__int32 Width;//0x00
	__int32 Height;//0x04
	BYTE _0x0008[16];//0x08
	Vector2 Fov;//0x18
	FLOAT TotalFov;//0x20
	Vector3 ViewOrg;//0x24
	BYTE _0x0030[4];//0x30
	Vector3 ViewAxis[3];//0x34
} refdef_s_WAW, *prefdef_s; // custom size 0x58
typedef struct
{
	CHAR Name[16];//0x00
	BYTE _0x0010[16];//0x10
	__int32 Team;//0x20
	BYTE _0x0024[952];//0x24
	__int32 isAlive3;//0x3DC
	BYTE _0x03E0[152];//0x3E0
	__int32 isAlive2;//0x478 || 1 = dead || DOESNT WORK
	BYTE _0x047C[20];//0x484
	__int32 Stance;//0x490
	BYTE _0x0494[200];//494
}client_t_WAW;   //[Size: 0x55C] tu7

struct snapshot_s_WAW
{
	DWORD unk;
	DWORD ping;
	DWORD physicsTime;
	DWORD serverTime;
};

struct weaponName_WAW
{
	CHAR weaponName[0x20];//0x00
};//0x20

typedef struct
{
	BYTE _0x0000[0x18];//0x00
	weaponName_WAW *Name;//0x18
}weaponInfo_WAW, *p_weaponInfo_WAW;//0x18

typedef struct cg_s_WAW
{
	BYTE _0x0000[0x20];//0x00
	snapshot_s_WAW* snap;//0x20
	snapshot_s_WAW* nextSnap;//0x24
	BYTE _0x0028[0x58 - 0x28];//0x28
	Vector3 Origin;//0x58
	BYTE _0x0064[204];//0x64
	__int32 ClientNumber;//0x130
	BYTE _0x0134[104];//0x134
	__int32 Health;//0x19C
	BYTE _0x01A0[342888];//0x1A0
	refdef_s_WAW refdef;//0x53D08
	BYTE _0x53D60[588256];//0x53D5C
	client_t_WAW clientInfo[18];//0xE3740
	BYTE _0xE97B8[15432];//0xE97B8
} cg_s_WAW, *pcg_s_WAW;// size 0xED400 tu8

typedef struct cgs_t_WAW
{
	BYTE _0x0000[8];//0x00
	__int32 Width;//0x08
	__int32 Height;//0x0C
	BYTE _0x0010[32];//0x10
	BYTE Gametype[32]; // 0x0030
	CHAR HostName[256]; // 0x0050
	DWORD MaxClients; // 0x0150
	DWORD PrivateClients; // 0x0154
	CHAR Mapname[64]; // 0x0158
	BYTE _0x0198[17712];//0x198
} cgs_t_WAW, *pcgs_t_WAW;// size 0x46C8 TU7 

#pragma pack(push, 1)
typedef struct usercmd_s_WAW
{
	__int32 serverTime; //0x00
	__int32 Buttons; //0x04
	__int32 ViewAngles[3]; //0x08
	char _0x0014[0x16 - 0x14];
	char movement[2]; // 0x16
	char _0x0018[0x2C - 0x18]; // 0x18
} usercmd_s_WAW, *pusercmd_s_WAW; // size 0x2C tu7
C_ASSERT(sizeof(usercmd_s_WAW) == 0x2C);
#pragma pack(pop)

typedef struct clientActive_t
{
	BYTE _0x0000[120];//0x00
	Vector3 Origin;//0x78
	BYTE _0x0084[32];//0x84
	Vector3 SpawnAngles;//0xA4
	BYTE _0x00B0[220];//0xB0
	__int32 Health;//0x18C
	BYTE _0x0190[157936];//0x190
	Vector3 ViewAngles;//0x26A80
	BYTE _0x26A8C[0x66AA0 - 0x26A8C];//0x26A8C
	usercmd_s_WAW UserCommands[128];//0x66AA0
	DWORD CommandNumber;//0x680A0
	inline DWORD GetUserCommandNumber()
	{
		return CommandNumber & 0x7F;
	}
	inline pusercmd_s_WAW GetUserCommand(DWORD CommandNumber)
	{
		return &UserCommands[CommandNumber]; // # r10 = r11 & 0x7F
	}
} clientActive_t_WAW, *pclientActive_t_WAW; // size 0x1FF348 tu7

typedef struct centity_s_WAW
{
	BYTE _0x0000[40];//0x00
	Vector3 Origin;//0x28
	BYTE _0x0034[157];//0x34
	BYTE isAiming;//0xD1
	BYTE _0x00D2;//0xD2
	BYTE Status;//0xD3 || 0x40 or 0x42 or 0x44 or 0x46 or 0x48 or 0x4A = shooting|| 0x04 or 0x06 or 0x44 or 0x46 = crouched || 0x08 or 0x0A or 0x48 or 0x4A = prone||
	__int32 _0x00D4;//0xD4
	BYTE _0x00D8[32];//0xD8
	__int32 Type;//0xF8
	BYTE _0x00FC[0x20C - 0xFC];//0xFC
	__int32 ClientNum; //0x20C
	BYTE _0x0210[0x244 - 0x210]; // 0x210
	__int32 weaponNum; //0x244
	BYTE _0x0248[0x2AC - 0x248]; //0x248
	__int32 isAlive;//0x2AC
} centity_s_WAW, *pcentity_s_WAW; // size 0x2B0 tu7
C_ASSERT(sizeof(centity_s_WAW) == 0x2B0);

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

struct ClientInformation_WAW
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

	ClientInformation_WAW()
	{
		reset();
	}
};
extern ClientInformation_WAW PlayerInfo[18];

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
	char Buf00[0x10];
	float Fraction;
	char Buf04[0x60];
};

struct BulletTraceResults
{
	trace_t trace;//0x00
	int* hitEnt; //0x40- gentity_s*
	float hitPos[3];//0x44
	int ignoreHitEnt;//0x50
	int hitSurfaceType;//0x54
}; // 0x60

struct BulletFireParams
{
	int weaponEntIndex;
	int ignoreEntIndex;
	float damageMultiplier;
	int methodOfDeath;
	Vector3 origStart;
	Vector3 start;
	Vector3 end;
	Vector3 dir;
}; // 0x40

typedef enum XAssetType_WAW : DWORD
{
	material_WAW = 0x06,
	font_WAW = 0x15,
	rawfile_WAW = 0x21
} XAssetType_WAW;//tu4

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

struct unkRadarStruct
{
	DWORD materialName;//0x00
	DWORD fontName;//0x04
	BYTE _0x0008[0x14];//0x08
	Vector2 Resolution;//0x1C
	BYTE _0x0024[8];//0x24
	Vector2 Resolution2;//0x2C
};//0x34

#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define DotProduct(a, b) ((a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]))
#define	ANGLE2SHORT(x)	((int)((x)*65536/360) & 65535)
#define	SHORT2ANGLE(x)	((x)*(360.0/65536))
#define PI 3.14159265358979323846
#define DEG2RAD(DEG) ((DEG)*((PI)/(180.0)))

#define colorRed                Vector4(255, 0, 153, 255)
#define colorOrange             Vector4(230, 126, 34, 255) //127
#define colorBlue               Vector4(52, 152, 219, 255)
#define colorGreen              Vector4(46, 204, 113, 255)
#define colorPurple             Vector4(128, 0, 255, 255)
#define colorBrightPurple       Vector4(255, 0, 255, 255)
#define colorYellow             Vector4(255, 255, 0, 255)
#define colorWhite              Vector4(255, 255, 255, 255)
#define colorVioletFlower       Vector4(191, 95, 255, 255)
#define GetColorCode(color)     Vector4(color.x / 255, color.y / 255, color.z / 255, color.a / 255)

static CONST PCHAR(__cdecl *va_WAW)(CONST PCHAR Format, ...);
static BOOL(__cdecl *Dvar_GetBool_WAW)(CONST PCHAR Dvar);
static PCHAR(__cdecl *Dvar_GetString_WAW)(CONST PCHAR Dvar);
static void(__cdecl *CG_LocationalTraceWAW)(void *trace, Vector3 *start, Vector3 *end, int passEntityNum, int contMask);
static DWORD(__cdecl *DB_FindXAssetHeader_WAW)(XAssetType_WAW Type, CONST PCHAR Name, DWORD r5);
static DWORD(__cdecl *R_TextWidth_WAW)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font);
static VOID(__cdecl *CL_DrawTextPhysical_WAW)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font, FLOAT X, FLOAT Y, FLOAT XScale, FLOAT YScale, FLOAT Angle, Vector4& Color, DWORD Style);
static VOID(__cdecl *R_AddCmdDrawStretchPic_WAW)(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, FLOAT XScale, FLOAT YScale, FLOAT Xay, FLOAT Yay, Vector4& Color, DWORD Material);
static VOID(__cdecl* CG_DrawRotatedPicPhysical_WAW)(DWORD ScreenPlacement, FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, FLOAT Angle, Vector4& Color, DWORD Material);
static DWORD(__cdecl *BG_GetWeaponDef_WAW)(DWORD WeaponIndex);
static void(__cdecl *Cbuf_AddText_WAW)(DWORD LocalClientNum, char *Message);
static short(__cdecl *sub_8222D368)(PCHAR String);
static DWORD(__cdecl *Com_GetClientDObj_WAW)(DWORD unk1, DWORD ClientNum);
static DWORD(__cdecl *CG_GetWeaponIndexForName_WAW)(CONST PCHAR WeaponName, int unk);
static int(__cdecl *PlayLocalSoundAliasByName)(int localClient, char* soundName, int aa0);
static void(__cdecl *CG_GetEntityBModelBounds)(pcentity_s_WAW centity_s, float* mins, float* maxs);
static int(__cdecl *R_NormalizedTextScale_WAW)(int font, float scale);
static bool(__cdecl *CG_CalcNamePosition)(int unk, Vector3* origin, float *xOut, float *yOut);
static VOID(__cdecl *CL_SetStance)(int localClientNum, int stance);
static FLOAT(__cdecl* G_GetWeaponHitLocationMultiplier_WAW)(DWORD hitLocation, DWORD weaponNum);
static VOID(__cdecl *SV_SetClientStat_WAW)(DWORD clientNumber, DWORD statIndex, DWORD statValue);
static int(__cdecl *Dvar_GetInt)(char *Message);

// No Spread Functions
static VOID(__cdecl *CG_GetPlayerViewOrigin_WAW)(DWORD localclientnum, DWORD playerState, Vector3& Origin);
static VOID(__cdecl *BG_GetSpreadForWeapon_WAW)(DWORD playerState, DWORD weaponDef, float* min, float* max);
static float(__cdecl *CG_GoodRandomFloat_WAW)(int *idum);

// Radar Functions
static VOID(__cdecl* DrawEngineRadar_WAW)(DWORD unk, DWORD disablerotation, DWORD ScreenPlacement, unkRadarStruct* radarHud, float unkFloat, DWORD material);
static VOID(__cdecl* DrawEnemies_WAW)(DWORD unk, DWORD disablerotation, DWORD ScreenPlacement, unkRadarStruct* radarHud, float unkFloat, DWORD material);
static VOID(__cdecl* DrawFriendlies_WAW)(DWORD unk, DWORD disablerotation, DWORD ScreenPlacement, unkRadarStruct* radarHud, float unkFloat, DWORD material);
static VOID(__cdecl* DrawVehicles_WAW)(DWORD unk, DWORD disablerotation, DWORD ScreenPlacement, unkRadarStruct* radarHud, float unkFloat, DWORD material);
static VOID(__cdecl* DrawPlayer_WAW)(DWORD unk, DWORD disablerotation, DWORD ScreenPlacement, unkRadarStruct* radarHud, float unkFloat, DWORD material);
static BOOL(__cdecl* CG_WorldPosToCompass_WAW)(DWORD CompassType, pcg_s_WAW cg_WAW, float RadarSizeZ, float RadarSizeA, Vector2& Yaw, Vector3& WorldPos, Vector3& WorldPos2, Vector2& Out, Vector2& OutClipped);
static BOOL(__cdecl* CG_CompassCalcDimensions_WAW)(DWORD CompassType, pcg_s_WAW cg_WAW, Vector2& Size, unkRadarStruct* RadarHud, float* x, float* y, float* w, float* h);
static void(__cdecl *CG_DrawRotatedPic_WAW)(DWORD scrPlace, float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, Vector4& color, DWORD material);
static VOID(__cdecl* CG_CompassUpYawVector_WAW)(pcg_s_WAW cg_WAW, Vector2& Yaw) = (VOID(__cdecl*)(pcg_s_WAW, Vector2&))0x821B7D10;//tu18
static float(__cdecl* AngleNormalize360_WAW)(float unk);


// AutoWall Functions
static VOID(__cdecl *FireBulletPenetrate_WAW)(DWORD LocalClientNum, BulletFireParams* bulletFireParams, DWORD weaponDef, pcentity_s_WAW centity_s, Vector3& Origin, bool drawTracer, DWORD a1);

static DWORD PlayerColorTable[7][3] = 
{
	{ 0x3F800000, 0x3F800000, 0x3F800000 }, // Normal
	{ 0x4F800000, 0x3F800000, 0x3F800000 }, // Red
	{ 0x4F800000, 0x6F800000, 0x4F800000 }, // White Glow
	{ 0xAF800000, 0x5F800000, 0x2F800000 }, // Green
	{ 0x1F800000, 0x7F800000, 0x4F800000 }, // Blue
	{ 0x6F800000, 0x7F800000, 0x4F800000 }, // Pink
	{ 0x2F800000, 0x2F800000, 0x2F800000 }  // Black
};


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
	OPTION_TELEPORTKILL,
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
	OPTION_DRAWTHROUGHWALLS,
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
	OPTION_LEVEL65,
	OPTION_ANTIDERANK,
	OPTION_INVERTTEAMCHECK,
	OPTION_LOGIPS,
	OPTION_FREEZEALLCONSOLES,

	//Extras
	OPTION_GODMODE,
	OPTION_GIVEAMMO,
	OPTION_TOGGLENOCLIP,
	OPTION_GIVEDOGS
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
	MENU_EXTRA,
	MENU_NAMES_SIZE
};
static vector<PCHAR> MenuNames;

int GetMenuOptionValue(eMenuNames SubMenu, eMenuOptions Name);
int GetMenuOptionArrayIndex(eMenuNames SubMenu, eMenuOptions Name);

static char* TeamNames[] =
{
	"spectator",
	"axis",
	"allies"
};

static char* VisCheckBones[7] =
{
	"j_head", "j_spinelower", "j_spineupper",
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

static char* bonesESPWAW[] =
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

static int SmoothScales_Size = 12;
static int SmoothScales_Start = 0;
static char *SmoothScales[] =
{
	"Off",
	"0.5",
	"1.0",
	"2.0",
	"3.0",
	"4.0",
	"5.0",
	"10.0",
	"15.0",
	"20.0",
	"25.0",
	"30.0",
};


static int XAASpinbots_Size = 5;
static int XAASpinbots_Start = 0;
static char *XAASpinbots[] =
{
	"Off",
	"Look Down",
	"Up & Down",
	"Fake Up",
	"Detection"
};

static int YAASpinbots_Size = 9;
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
	"Static"
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

static int PlayerColors_Size = 7;
static int PlayerColors_Start = 0;
static char *PlayerColors[] =
{
	"Normal",
	"Red",
	"White",
	"Green",
	"Blue",
	"Pink",
	"Black"
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
	"11th",
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