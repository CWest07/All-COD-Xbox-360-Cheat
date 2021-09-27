#ifndef MW3_H
#define MW3_H

#include "stdafx.h"
#include "Utilities.h"
#include "SecureStrings.h"
#include "Menu.h"
#include "GeoLocationParser.h"

typedef struct refdef_s_Mw3
{
	char _0x0000[0x8];                            //0x0
	int Width;                                    //0x8 (0xA475B8)
	int Height;                                   //0xC (0xA475BC)
	Vector2 Fov;									//0x10
	Vector3 ViewOrg;                               //0x18 (0xA475C8)
	Vector3 ViewAxis[3];                           //0x24 (0xA475D4)
	char _0x0048[0x24];                           //0x48
	float ZoomProgress;                           //0x6C (0xA4761C)
	char _0x0070[0x4ABC];                         //0x70
	float RefViewAngleY;                          //0x4B2C (0xA4C0DC)
	float RefViewAngleX;                          //0x4B30 (0xA4C0E0)
	char _0x4B34[0x34];                           //0x4B34
	float WeaponViewAngleX;                       //0x4B68 (0xA4C118)
	float WeaponViewAngleY;                       //0x4B6C (0xA4C11C)
} refdef_s_Mw3, *prefdef_s; // custom size

typedef struct
{
	__int32 Valid;//0x00
	BYTE _0x0004[4];//0x04
	CHAR Name[16];//0x08
	BYTE _0x0018[16];//0x18
	__int32 Team;//0x28
	BYTE _0x002C[4];//0x2C
	__int32 Rank;//0x30
	BYTE _0x0034[0x50 - 0x34];//0x34
	__int32 Score;//0x50
	BYTE _0x0054[0x60 - 0x54];//0x54
	BYTE GodClassCheck;//0x60
	BYTE _0x0061[0xA0 - 0x61];//0x61
	BYTE NoHeadCheck;//0xA0
	BYTE _0x00A1[0x424 - 0xA1];//0xA1
	BYTE _0x0424[216];//0x424
	__int32 WeaponNum1;//0x4FC
	BYTE _0x0500[24];//0x500
	__int32 WeaponNum2;//0x518
	BYTE _0x051C[120];//0x51C
}client_t_Mw3; //[Size: 0x594]

struct snapshot_s_Mw3
{
	DWORD Servertime;             //0x00
	BYTE _0x0004[0x3184 - 0x04];  //0x04
	DWORD ping;                   //0x3184
	DWORD ServerTime2;            //0x3188
};

typedef struct cg_s_Mw3
{
	__int32 ServerTime;//0x00
	BYTE _0x0004[0x84 - 0x04];//0x04
	DWORD Flags;//0x84
	BYTE _0x0088[200];//0x88
	__int32 ClientNumber;//0x150
	BYTE _0x0154[4];//0x154
	Vector3 NoSpreadAngles;//0x158
	BYTE _0x0164[0x19C - 0x164];//0x164
	DWORD Health;//0x19C
	BYTE _0x1A0[0x378 - 0x1A0];//0x1A0
	FLOAT ZoomState;//0x378
	FLOAT Spread;//0x37C
	BYTE _0x380[0x3000]; // 0x380
	__int32 OldServerTime;//0x3380
	BYTE _0x3384[427816];//0x3384
	refdef_s_Mw3 refdef;//0x6BAAC
	BYTE _0x70618[579952];//0x70618
	client_t_Mw3 clientInfo[18];//0xFDF88
	BYTE _0x1043F0[0x105520 - 0x1043F4];//0x1043F0
	DWORD NoSway;//0x105520
} cg_s_Mw3, *pcg_s_Mw3; // size 0x105A80 tu23

typedef struct cgs_t_Mw3
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
	BYTE _0x0190[16168];//0x190
} cgs_t_Mw3, *pcgs_t_Mw3; // size 0x40B8

typedef struct usercmd_s_Mw3
{
	__int32 serverTime; //0x0000 
	__int32 Buttons; //0x0004 
	__int32 ViewAngles[3]; //0x0008 
	char _0x0014[8];
	signed char movement[2];
	__int16 unk;//0x1E
	char _0x0020[0xC];//0x20
} usercmd_s_Mw3, *pusercmd_s_Mw3; // size 0x2C  updated to mw3 tu23

typedef struct clientActive_t
{
	BYTE _0x0000[224];//0x00
	Vector3 SpawnAngles;//0xE0
	BYTE _0x00EC[12956];//0xEC
	__int32 ServerTime;//0x3388
	BYTE _0x338C[136];//0x338C
	__int32 OldServerTime;//0x3414
	BYTE _0x3418[408];//0x3418
	Vector3 ViewAngles;//0x35B0   
	usercmd_s_Mw3 UserCommands[128];// 0x35BC
	DWORD CommandNumber; // 0x4BBC
	BYTE _0x4BC0[34880];//0x4BC0

	inline DWORD GetUserCommandNumber()
	{
		return CommandNumber & 0x7F;
	}
	inline pusercmd_s_Mw3 GetUserCommand(DWORD CommandNumber)
	{
		return &UserCommands[CommandNumber]; // # r10 = r11 & 0x7F
	}
} clientActive_t_Mw3, *pclientActive_t_Mw3; // size 0xD400  all this is updated to tu23 mw3

#pragma pack(push, 2)
typedef struct centity_s_Mw3
{
	BYTE _0x0000[20];//0x00
	Vector3 pos;//0x14
	Vector3 Angles;//0x20
	BYTE _0x002C[49];//0x2C
	BYTE status;//0x5D 0 = not aiming/shooting || 128 = shooting not aiming || 8 = aiming not shooting || 136 = aiming and shooting || 4 = dead
	BYTE isPaused;//0x5E 0 = not paused || 1 = paused
	BYTE pose;//0x5F 0/2 = standing || 4/6 = crouched || 8/10 = prone
	BYTE _0x0060[12];//0x60
	Vector3 pos2;//0x6C
	BYTE _0x0078[76];//0x78
	__int32 ClientNumber;//0xC4
	__int32 type;//0xC8
	BYTE _0x00CC[16];//0xCC
	Vector3 pos3;//0xDC
	BYTE _0x00E8[64];//0xE8
	__int16 CurrentWeapon;//0x128
	BYTE padding;//0x12A
	BYTE Weapon;//0x12B
	BYTE _0x0012C[96];//0x12C
	DWORD weaponNum;//0x18C
	BYTE _0x0190[52];//0x190
	BYTE testalive1;//0x1C4
	BYTE testalive2;//0x1C5
	BYTE testalive3;//0x1C6
	BYTE testalive4;//0x1C7
	BYTE _0x01C8[36];//0x1C8
} centity_s_Mw3, *pcentity_s_Mw3; // size 0x01EC

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

struct ClientInformation_Mw3
{
	BOOL Alive;
	BOOL GrabGeoIPInfo;
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

	ClientInformation_Mw3()
	{
		reset();
	}
};
extern ClientInformation_Mw3 PlayerInfo[18];

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

typedef enum XAssetType_Mw3 : DWORD
{
	material_Mw3 = 0x05,
	font_Mw3 = 0x16
} XAssetType_Mw3;//tu8

typedef enum HitLocationIndexs : DWORD
{
	HITLOC_NONE = 0,
	HITLOC_HELMET = 0x1,
	HITLOC_HEAD = 0x2,
	HITLOC_NECK = 0x3,
	HITLOC_TORSO_UPR = 0x4,
	HITLOC_TORSO_LWR = 0x5,
	HITLOC_R_ARM_UPR = 0x6,
	HITLOC_L_ARM_UPR = 0x7,
	HITLOC_R_ARM_LWR = 0x8,
	HITLOC_L_ARM_LWR = 0x9,
	HITLOC_R_HAND = 0x0A,
	HITLOC_L_HAND = 0x0B,
	HITLOC_R_LEG_UPR = 0x0C,
	HITLOC_L_LEG_UPR = 0x0D,
	HITLOC_R_LEG_LWR = 0x0E,
	HITLOC_L_LEG_LWR = 0x0F,
	HITLOC_R_FOOT = 0x10,
	HITLOC_L_FOOT = 0x11,
	HITLOC_GUN = 0x12,
	HITLOC_SHIELD = 0x13,
	HITLOC_ARMOR = 0x14,
	HITLOC_SOFT = 0x15,
	HITLOC_NUM = 0x16,
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


static CONST PCHAR(__cdecl *va_Mw3)(CONST PCHAR Format, ...);
static BOOL(__cdecl *Dvar_GetBool_Mw3)(CONST PCHAR Dvar);
static void(__cdecl *CG_LocationalTraceMw3)(trace_t* trace, Vector3 *start, Vector3 *end, int passEntityNum, int contMask);
static DWORD(__cdecl *DB_FindXAssetHeader_Mw3)(XAssetType_Mw3 Type, CONST PCHAR Name, DWORD r5);
static DWORD(__cdecl *R_TextWidth_Mw3)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font);
static VOID(__cdecl *CL_DrawTextPhysical_Mw3)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font, FLOAT X, FLOAT Y, FLOAT XScale, FLOAT YScale, FLOAT Angle, Vector4& Color, DWORD Style);
static VOID(__cdecl *R_AddCmdDrawStretchPic_Mw3)(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, FLOAT XScale, FLOAT YScale, FLOAT Xay, FLOAT Yay, Vector4& Color, DWORD Material);
static VOID(__cdecl* CG_DrawRotatedPicPhysical_Mw3)(DWORD ScreenPlacement, FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, FLOAT Angle, Vector4& Color, DWORD Material);
static void(__cdecl *Cbuf_AddText_Mw3)(DWORD LocalClientNum, char *Message);
static short(__cdecl *SL_GetString_Mw3)(PCHAR String, DWORD aZero);
static DWORD(__cdecl *Com_GetClientDObj_Mw3)(DWORD r3, DWORD r4);
static int(__cdecl *CG_GetWeaponIndexForName)(char* name);
static int(__cdecl *PlayLocalSoundAliasByName)(int localClientNum, char *aliasname, int a0);
static void(__cdecl *CG_GetEntityBModelBounds)(pcentity_s_Mw3 centity_s, float* mins, float* maxs);
static int(__cdecl *R_NormalizedTextScale_Mw3)(int font, float scale);
static bool(__cdecl *CG_CalcNamePosition)(int unk, Vector3* origin, float *xOut, float *yOut);
static float(__cdecl *G_GetWeaponHitLocationMultiplier)(int unk, int WeaponNum, int unk1);
static int(__cdecl *AnglesToAxis)(float* ViewAnglesIn, float* ViewAxisOut);
static void(__cdecl *CL_SetStance)(int localClient, int stance);

// No Spread Functions
static VOID(__cdecl *BG_GetSpreadForWeapon_Mw3)(pcg_s_Mw3 cg_Mw3, DWORD weaponDef, float* min, float* max);
static VOID(__cdecl *RandomBulletDir_Mw3)(int* Seed, float* flX, float* flY);
static PDWORD(__cdecl *BG_GetWeaponDef_Mw3)(DWORD WeaponIndex);
static DWORD(__cdecl *BG_GetWeaponDef2_Mw3)(DWORD weaponNum, DWORD unk);
static VOID(__cdecl *CG_GetPlayerViewOrigin)(DWORD LocalClientNum, pcg_s_Mw3 cg_s_Mw3, Vector3& Origin);
static int(__cdecl *CalculateRandomSeed)(int* time);

// Radar Functions
static VOID(__cdecl* DrawEngineRadar_Mw3)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, DWORD shader, Vector4& Color);
static VOID(__cdecl* DrawEnemies_Mw3)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color);
static VOID(__cdecl* DrawFriendlies_Mw3)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color);
static VOID(__cdecl* DrawVehicles_Mw3)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color);
static VOID(__cdecl* DrawPlayer_Mw3)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color);
static BOOL(__cdecl* CG_WorldPosToCompass_Mw3)(DWORD CompassType, pcg_s_Mw3 cg_Mw3, Vector4& Size, Vector2& Yaw, Vector3& WorldPos, Vector3& WorldPos2, Vector2& Out, Vector2& OutClipped);
static BOOL(__cdecl* CG_CompassCalcDimensions_Mw3)(DWORD CompassType, pcg_s_Mw3 cg_Mw3, Vector2& Size, CRadarHud* RadarHud, float* x, float* y, float* w, float* h);
static void(__cdecl* CG_DrawRotatedPic_Mw3)(DWORD scrPlace, float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, Vector4& color, DWORD material);
static void(__cdecl* CG_CompassUpYawVector_Mw3)(pcg_s_Mw3 cg_Mw3, Vector2& Yaw);
static float(__cdecl* sub_82317BF0)(float unk);

//AutoWall Functions
static VOID(__cdecl *FireBulletPenetrate_Mw3)(DWORD LocalClientNum, BulletFireParams* bp, DWORD weaponIndex, DWORD unk1, pcentity_s_Mw3 centity_s, DWORD neg1, Vector3& Origin, bool drawTracer);

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
	OPTION_LEVEL80,
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

static char* HitboxVisCheckBones[12] =
{
	"j_head", "j_neck", "j_spineupper", "j_spinelower", "j_shoulder_ri", "j_shoulder_le",
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

static char* bonesESPMw3[] =
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

static int Prestiges_Size = 23;
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
	"17th",
	"18th",
	"19th",
	"20th",
	"21st"
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