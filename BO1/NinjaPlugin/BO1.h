
#ifndef BO1_H
#define BO1_H

#include "stdafx.h"
#include "Utilities.h"
#include "SecureStrings.h"
#include "Menu.h"
#include "GeoLocationParser.h"


typedef struct refdef_s_BO1
{
	__int32 Width;//0x00
	__int32 Height;//0x04
	char _0x0008[20];//0x08
	Vector2 Fov;//0x1C
	FLOAT TotalFov;//0x24
	Vector3 ViewOrg;//0x28
	char _0x0034[4];//0x34
	Vector3 ViewAxis[3];//0x38
	__int32 ServerTime;//0x5C
} refdef_s_BO1, *prefdef_s; //0x60

typedef struct
{
	__int32 Valid;//0x00
	BYTE _0x0004[8];//0x08
	CHAR Name[16];//0x0C
	BYTE _0x001C[16];//0x1C
	__int32 Team;//0x2C
	BYTE _0x0030[52];//0x30
	__int32 isAlive;//0x64
	BYTE _0x0068[0x450 - 0x68];//0x68
	Vector3 Angles;//0x450
	BYTE _0x045C[0x5D0 - 0x45C];//0x45C
}client_t_BO1;  //[Size: 0x5D0]

struct snapshot_s_BO1
{
	DWORD snapFlags;//0x00
	DWORD Ping;//0x04
	DWORD serverTime;//0x08
	DWORD physicsTime;//0x0C
};

struct weaponName_BO1
{
	CHAR weaponName[0x20];//0x00
};//0x20

typedef struct
{
	BYTE _0x0000[0x18];//0x00
	weaponName_BO1 *Name;//0x18
}weaponInfo_BO1, *p_weaponInfo_BO1;//0x18

typedef struct cg_s_BO1
{
	__int32 ClientNumber;//0x00
	DWORD LocalClientNumber;//0x04
	BYTE _0x0008[0x28 - 0x8];//0x08
	snapshot_s_BO1* snap;//0x28
	snapshot_s_BO1* nextSnap;//0x2C
	BYTE _0x0030[0xB4 - 0x30];//0x30
	Vector3 Origin;//0xB4
	BYTE _0x00C0[268616];//0xC0
	refdef_s_BO1 refdef;//0x41A04
	BYTE _0x41A64[114136];//0x41A64
	DWORD Sway;//0x5D840
	BYTE _0x5D844[692];//0x5D844
	client_t_BO1 clientInfo[18];//0x5DAF4
	BYTE _0x6666C[19092];//0x66678
} cg_s_BO1, *pcg_s_BO1; // size 0x6B100 tu11

typedef struct cgs_t_BO1
{
	BYTE _0x0000[7];//0x00
	__int32 Width;//0x08
	__int32 Height;//0x0C
	BYTE _0x0010[32];//0x10
	BYTE Gametype[32]; // 0x0030
	CHAR HostName[256]; // 0x0050
	DWORD MaxClients; // 0x0150
	DWORD PrivateClients; // 0x0154
	CHAR Mapname[64]; // 0x0158
	BYTE _0x0198[12600];//0x198
} cgs_t_BO1, *pcgs_t_BO1;// size 0x32D0 tu11

#pragma pack(push, 1)
typedef struct usercmd_s_BO1
{
	int serverTime; //0x0000 
	int Buttons; //0x0004 
	char _0x0008[4];//0x0008
	int ViewAngles[3]; //0x000C
	short weaponID;  //0x18
	int unk;//0x1A
	signed char forwardmove;//0x1E
	signed char sidemove;//0x1F
	char _0x0020[0x14];//0x20
} usercmd_s_BO1, *pusercmd_s_BO1; // size 0x34  updated to bo1 tu11
C_ASSERT(sizeof(usercmd_s_BO1) == 0x34);
#pragma pack(pop)

typedef struct clientActive_t
{
	BYTE _0x0000[76];//0x00
	Vector3 Origin;//0x4C
	BYTE _0x0058[84];//0x58
	Vector3 SpawnAngles;//0xAC
	BYTE _0x00B8[308];//0xB8
	__int32 Health;//0x1EC
	BYTE _0x01F0[9808];//0x1F0
	Vector3 ViewAngles;//0x2840
	BYTE _0x284C[262160];//0x284C
	usercmd_s_BO1 UserCommands[128];// 0x4285C
	DWORD CommandNumber; // 0x4425C

	inline DWORD GetUserCommandNumber()
	{
		return CommandNumber & 0x7F;
	}
	inline pusercmd_s_BO1 GetUserCommand(DWORD CommandNumber)
	{
		return &UserCommands[CommandNumber]; // # r10 = r11 & 0x7F
	}
} clientActive_t_BO1, *pclientActive_t_BO1; // size 0x19FF00  bo1 tu11

typedef struct centity_s_BO1
{
	BYTE _0x0000[28];//0x00
	__int32 GodmodeCheck;
	BYTE _0x0020[16];//0x20
	Vector3 Origin;//0x30
	BYTE _0x003C[301];//0x3C
	BYTE State1;//0x169     || 2 = sprinting || 8 = aiming
	BYTE type;//0x16A not actually type just a place holder
	BYTE State2;//0x16B     || 0x40 = shooting || 0x04 or 0x06 or 0x44 or 0x46 = crouched || 0x0A or 0x4A or 0x8 or 0x48 = prone
	BYTE _0x016C[112];//0x16C
	__int32 ClientNumber;//0x1DC
	BYTE _0x01E0[134];//0x1E0
	BYTE isAlive;//0x266    || 0 = dead
	BYTE _0x0267[0x2A4 - 0x267];//0x267
	DWORD WeaponID;//0x2A4
	BYTE _0x02A8[0x318 - 0x2A8];//0x2A8
	BYTE _0x0318;//0x318
	BYTE _0x0319;//0x319
	__int16 _0x031A;//0x31A
} centity_s_BO1, *pcentity_s_BO1; // size 0x374

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

struct ClientInformation_BO1
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

	ClientInformation_BO1()
	{
		reset();
	}
};
extern ClientInformation_BO1 PlayerInfo[18];

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

typedef enum XAssetType_BO1 : DWORD
{
	material_BO1 = 0x06,
	font_BO1 = 0x15
} XAssetType_BO1;//tu18

typedef enum HitLocationIndexs : DWORD
{
	HITLOC_NONE = 0,
	HITLOC_HELMET = 1,
	HITLOC_HEAD = 2,
	HITLOC_NECK = 3,
	HITLOC_TORSO_UPR = 4,
	HITLOC_TORSO_MID = 5,
	HITLOC_TORSO_LWR = 6,
	HITLOC_R_ARM_UPR = 7,
	HITLOC_L_ARM_UPR = 8,
	HITLOC_R_ARM_LWR = 9,
	HITLOC_L_ARM_LWR = 0x0A,
	HITLOC_R_HAND = 0x0B,
	HITLOC_L_HAND = 0x0C,
	HITLOC_R_LEG_UPR = 0x0D,
	HITLOC_L_LEG_UPR = 0x0E,
	HITLOC_R_LEG_LWR = 0x0F,
	HITLOC_L_LEG_LWR = 0x10,
	HITLOC_R_FOOT = 0x11,
	HITLOC_L_FOOT = 0x12,
	HITLOC_GUN = 0x13,
	HITLOC_SHIELD = 0x14,
	HITLOC_NUM = 0x15
} HitLocationIndexs;

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
#define colorRedRadar			Vector4(0, 255, 0, 191.25f)
#define colorGreenRadar			Vector4(255, 0, 0, 191.25f)
#define GetColorCode(color)     Vector4(color.x / 255, color.y / 255, color.z / 255, color.a / 255)

static CONST PCHAR(__cdecl *va_BO1)(CONST PCHAR Format, ...);
static BOOL(__cdecl *Dvar_GetBool_BO1)(CONST PCHAR Dvar);
static void(__cdecl *CG_LocationalTraceBO1)(void *trace, Vector3 *start, Vector3 *end, int passEntityNum, int contMask, bool checkropes, DWORD context);
static DWORD(__cdecl *DB_FindXAssetHeader_BO1)(XAssetType_BO1 Type, CONST PCHAR Name, DWORD r5);
static DWORD(__cdecl *R_TextWidth_BO1)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font);
static VOID(__cdecl *CL_DrawTextPhysical_BO1)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font, FLOAT X, FLOAT Y, FLOAT XScale, FLOAT YScale, FLOAT Angle, Vector4& Color, DWORD Style);
static VOID(__cdecl *R_AddCmdDrawStretchPic_BO1)(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, FLOAT XScale, FLOAT YScale, FLOAT Xay, FLOAT Yay, Vector4& Color, DWORD Material);
static VOID(__cdecl* CG_DrawRotatedPicPhysical_BO1)(DWORD ScreenPlacement, FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, FLOAT Angle, Vector4& Color, DWORD Material);
static PDWORD(__cdecl *BG_GetWeaponDef_BO1_2)(DWORD WeaponIndex);
static DWORD(__cdecl *BG_GetWeaponDef_BO1)(DWORD WeaponIndex);
static void(__cdecl *Cbuf_AddText_BO1)(DWORD LocalClientNum, char *Message);
static void(__cdecl *CL_AddReliableCommand)(DWORD LocalClientNum, char *Message);
static short(__cdecl *SL_GetString_BO1)(PCHAR String, DWORD aZero, DWORD aZeroAgain);
static DWORD(__cdecl *Com_GetClientDObj_BO1)(DWORD LocalClientNum, DWORD unk1);
static DWORD(__cdecl *CG_GetWeaponIndexForName_BO1)(CONST PCHAR WeaponName);
static void(__cdecl *PlayLocalSoundAliasByName)(char *aliasname, int a0, float soundLevel, int r6, int r7, int r8, int r9);
static int(__cdecl *R_NormalizedTextScale_BO1)(int font, float scale);
static bool(__cdecl *CG_CalcNamePosition)(int unk, Vector3* origin, float *xOut, float *yOut);
static VOID(__cdecl *CL_SetStance)(int localClientNum, int stance);
static FLOAT(__cdecl* G_GetWeaponHitLocationMultiplier_BO1)(DWORD hitLocation, DWORD weaponNum);

// D3D Functions
//static void(__cdecl *D3DDevice_SetRenderState_BO1)(D3DDevice *pDevice, _D3DRENDERSTATETYPE State, unsigned int Value) = (void(__cdecl*)(D3DDevice *, _D3DRENDERSTATETYPE, unsigned int))0x823873C0;//TU8 // 823992C8
//static void(__cdecl *D3DDevice_SetTexture_BO1)(D3DDevice *pDevice, unsigned int Sampler, D3DBaseTexture *pTexture, unsigned __int64 PendingMask3) = (void(__cdecl*)(D3DDevice *, unsigned int, D3DBaseTexture*, unsigned __int64))0x820B13D8;//TU8
//static VOID(__cdecl *D3DDevice_DrawIndexedVertices_BO1)(D3DDevice* device, DWORD unk, DWORD unk2, DWORD unk3, DWORD unk4) = (VOID(__cdecl*)(D3DDevice*, DWORD, DWORD, DWORD, DWORD))0x820AF1D0;//TU8

// No Spread Functions
static VOID(__cdecl *CG_GetPlayerViewOrigin_BO1)(DWORD LocalClientNum, DWORD PlayerStatePTR, Vector3& Origin);
static VOID(__cdecl *R_GetSpread_BO1)(DWORD PlayerStatePTR, DWORD WeaponDef, float* BaseSpread, float* WeaponSpread);
static DWORD(__cdecl* TransformSeed_BO1)(int* seed);
static FLOAT(__cdecl *RandomBulletDir_BO1)(int* Seed);

// Radar Functions
static VOID(__cdecl* DrawEngineRadar_BO1)(DWORD unk, DWORD disablerotation, DWORD ScreenPlacement, unkRadarStruct* radarHud, float unkFloat, DWORD material);
static VOID(__cdecl* DrawEnemies_BO1)(DWORD unk, DWORD disablerotation, DWORD ScreenPlacement, unkRadarStruct* radarHud, float unkFloat, DWORD material);
static VOID(__cdecl* DrawFriendlies_BO1)(DWORD unk, DWORD disablerotation, DWORD ScreenPlacement, unkRadarStruct* radarHud, float unkFloat, DWORD material);
static VOID(__cdecl* DrawVehicles_BO1)(DWORD unk, DWORD disablerotation, DWORD ScreenPlacement, unkRadarStruct* radarHud, float unkFloat, DWORD material);
static VOID(__cdecl* DrawPlayer_BO1)(DWORD unk, DWORD disablerotation, DWORD ScreenPlacement, unkRadarStruct* radarHud, float unkFloat, DWORD material);
static BOOL(__cdecl* CG_WorldPosToCompass_BO1)(DWORD CompassType, pcg_s_BO1 cg_BO1, float RadarSizeZ, float RadarSizeA, Vector2& Yaw, Vector3& WorldPos, Vector3& WorldPos2, Vector2& Out, Vector2& OutClipped);
static BOOL(__cdecl* CG_CompassCalcDimensions_BO1)(DWORD CompassType, pcg_s_BO1 cg_BO1, Vector2& Size, unkRadarStruct* RadarHud, float* x, float* y, float* w, float* h);
static void(__cdecl *CG_DrawRotatedPic_BO1)(DWORD scrPlace, float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, Vector4& color, DWORD material);
static VOID(__cdecl* CG_CompassUpYawVector_BO1)(pcg_s_BO1 cg_BO1, Vector2& Yaw);
static float(__cdecl* AngleNormalize360_BO1)(float unk);

// AutoWall Functions
static VOID(__cdecl* FireBulletPenetrate_BO1)(DWORD LocalClientNum, BulletFireParams *bp, DWORD weaponNum, pcentity_s_BO1 centity_s, Vector3& Origin, DWORD one, bool unk, BulletTraceResults *br);

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
	OPTION_LEVEL50,
	OPTION_CODPOINTS,
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

static char* bonesESPBO1[] =
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

static int Prestiges_Size = 18;
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
	"12th",
	"13th",
	"14th",
	"15th",
	"16th",
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