#ifndef MW2_H
#define MW2_H

#include "stdafx.h"
#include "Utilities.h"
#include "SecureStrings.h"
#include "Menu.h"
#include "GeoLocationParser.h"


typedef struct refdef_s_Mw2
{
	int Width;                                      //0x0 (0xA475B8)
	int Height;                                     //0x4 (0xA475BC)
	Vector2 Fov;									//0x8
	Vector3 ViewOrg;                                //0x10 (0xA475C8)
	Vector3 ViewAxis[3];                            //0x1C (0xA475D4)
} refdef_s_Mw2, *prefdef_s_Mw2;                     //custom size

typedef struct
{
	CHAR Name[16];                       //0x00
	BYTE _0x0010[16];                    //0x10
	__int32 Team;                        //0x20
	BYTE _0x0024[0x50 - 0x24];           //0x24
	BYTE ValidCheck;                     //0x50
	BYTE _0x0051[0x480 - 0x51];          //0x51
	__int32 isAlive2;                    //0x480 || 1 = dead ||
	BYTE _0x0484[12];                    //0x484
	__int32 Stance;                      //0x490
	BYTE _0x0494[92];                    //0x494
	__int32 isAlive;                     //0x4F0
	BYTE _0x04F4[80];                    //0x4F4
}client_t_Mw2;                           //[Size: 0x544]

struct snapshot_s_Mw2
{
	DWORD Servertime;             //0x00
	BYTE _0x0004[0x3184 - 0x04];  //0x04
	DWORD ping;                   //0x3184
	DWORD ServerTime2;            //0x3188
};

typedef struct cg_s_Mw2
{
	DWORD ServerTime;                 //0x00
	BYTE _0x0004[24];                 //0x04
	Vector3 Origin;                   //0x1C
	BYTE _0x0028[0x38];               //0x28
	Vector3 SpawnAngles;              //0x60
	BYTE _0x006C[0x45];               //0x6C
	BYTE Mantling;                    //0xB1
	BYTE _0x00B2;                     //0xB2
	BYTE Stance;                      //0xB3
	BYTE _0x00B4[0x50];               //0xB4
	__int32 ClientNumber;             //0x104
	BYTE _0x0108[72];                 //0x108
	__int32 Health;                   //0x150
	BYTE _0x0154[0x16C];              //0x154
	FLOAT ZoomState;                  //0x2C0
	FLOAT Spread;                     //0x2C4
	BYTE _0x02C8[0x33C8 - 0x2C8];     //0x2C8
	snapshot_s_Mw2* snap;             //0x33C8
	snapshot_s_Mw2* nextsnap;         //0x33CC
	BYTE _0x33D0[0x6AC2E - 0x33D0];   //0x33D0
	refdef_s_Mw2 refdef;              //0x6AC2E
	BYTE _0x6AC6E[573978];            //0x6AC6E
	client_t_Mw2 clientInfo[18];      //0xF6E88
	BYTE _0xFCD50[4272];              //0xFCD50
} cg_s_Mw2, *pcg_s_Mw2;               // size 0xFDE00 tu8

typedef struct cgs_t_Mw2
{
	BYTE _0x0000[8];         //0x00
	__int32 Width;           //0x08
	__int32 Height;          //0x0C
	BYTE _0x0010[16];        //0x10
	BYTE Gametype[32];       //0x0020
	CHAR HostName[260];      //0x0040
	DWORD MaxClients;        //0x0144
	DWORD PrivateClients;    //0x0148
	CHAR Mapname[64];        //0x014C
	BYTE _0x018C[13480];     //0x18C
} cgs_t_Mw2, *pcgs_t_Mw2;    // size 0x3634

typedef struct usercmd_s_Mw2
{
	int serverTime;		// 0x00-0x04
	int buttons;		// 0x04-0x08
	int angles[3];		// 0x08-0x14
	short weapon;		// 0x14-0x16
	short offHandIndex;	// 0x16-0x18
	char unkBuf[2];     // 0x18-0x1A
	signed char movement[3]; // 0x1A - 0x1C
	char Buf2[0xB];      // 0x1C-0x28
} usercmd_s_Mw2, *pusercmd_s; // size 0x28 tu8

typedef struct clientActive_t_Mw2
{
	bool AimingDownSight; // 0x00
	BYTE _0x0001[0x80 - 0x01];//0x01
	DWORD ServerTime;//0x80
	BYTE _0x0084[0x9C - 0x84];//0x84
	Vector3 Origin;//0x9C
	BYTE _0x00A8[0xE0 - 0xA8];//0xA8
	Vector3 SpawnAngles;//0xE0
	BYTE _0x00EC[0x3358 - 0xEC];//0xEC
	Vector3 ViewAngles;//0x3358
	usercmd_s_Mw2 UserCommands[128];// 0x3364
	DWORD CommandNumber; // 0x4764

	inline DWORD GetUserCommandNumber()
	{
		return CommandNumber & 0x7F;
	}
	inline pusercmd_s GetUserCommand(DWORD CommandNumber)
	{
		return &UserCommands[CommandNumber]; // # r10 = r11 & 0x7F
	}
} clientActive_t_Mw2, *pclientActive_t_Mw2; // size 0x6D00  all this is updated to tu8 mw2

#pragma pack(push, 2)
typedef struct centity_s_Mw2
{
	BYTE _0x0000[2];//0x00
	BYTE RealIsAlive;//0x02
	BYTE _0x0003[21];//0x03
	Vector3 Origin;//0x018
	BYTE _0x0024[61];//0x24
	BYTE isAiming;//0x61 || 4 = aimiming||
	BYTE isShooting;//0x62 ||2 = shooting||
	BYTE Stance;//0x63 ||8 or 0x0A = prone|| 4 or 6 = crouched||0 or 2 = standing
	BYTE _0x0064[20];//0x64
	Vector3 OldOrigin;//0x78
	BYTE _0x0084[0x9C - 0x84];//0x84
	Vector3 Angles;//0x9C
	BYTE _0x00A8[0xD0 - 0xA8];//0xA8
	__int32 ClientNumber;//0xD0
	__int32 Type;//0xD4
	BYTE _0x00D8[28];//0xD8
	Vector3 NewOrigin;//0xF4
	BYTE _0x0100[0x5A];//0x100
	__int16 droppedWeaponNum;//0x15A
	BYTE _0x015C[0x180 - 0x15C];//0x15C
	__int32 isAlivecheck1;//0x180 || 98 = dead
	__int32 isAlivecheck2;//0x184 || 98 = dead
	__int32 isAlivecheck3;//0x188 || 98 = dead
	BYTE _0x018C[16];//0x18C
	__int16 WeaponNum;//0x19C
	__int16 WeaponNum2;//0x19E
	BYTE _0x01A0[2];//0x1A0
	BYTE isAlive2;//0x1A2
	BYTE _0x01A3;//0x1A3
	DWORD unk;//0x1A4
	BYTE _0x01A8[40];//0x1A8
	__int32 isAlive4;//0x1D0 || 0x10 = dead||
	BYTE _0x01D4[36];//0x1D4
} centity_s_Mw2, *pcentity_s_Mw2; // size 0x1F8

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

struct ClientInformation_Mw2
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

	ClientInformation_Mw2()
	{
		reset();
	}
};
extern ClientInformation_Mw2 PlayerInfo[18];

struct trace_t
{
	float fraction;//0x00
	Vector3 normal;//0x04
	BYTE _0x0010[0x20];//0x10
};

struct BulletFireParams
{
	int weaponEntIndex;//0x00
	int ignoreEntIndex;//0x04
	float damageMultiplier;//0x08
	int methodOfDeath;//0x0C
	Vector3 origStart;//0x10
	Vector3 start;//0x1C
	Vector3 end;//0x28
	Vector3 dir;//0x34
}; // 0x40

typedef enum XAssetType_Mw2 : DWORD
{
	material_Mw2 = 0x05,
	font_Mw2 = 0x16
} XAssetType_Mw2;//tu8


static vector<PCHAR> MenuNames;
enum eMenuNames
{
	MENU_MAIN,
	MENU_RAGEBOT,
	MENU_LEGITBOT,
	MENU_SPINBOT,
	MENU_DRAWING,
	MENU_CLIENTS,
	MENU_SETTINGS
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
	OPTION_LEVEL70,
	OPTION_ANTIDERANK,
	OPTION_INVERTTEAMCHECK,
	OPTION_LOGIPS,
	OPTION_FREEZEALLCONSOLES
};
static vector<PCHAR> MenuOptionNames;

int GetMenuOptionValue(eMenuNames SubMenu, eMenuOptions Name);
int GetMenuOptionArrayIndex(eMenuNames SubMenu, eMenuOptions Name);


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
#define colorOrange             Vector4(230, 126, 34, 127)
#define colorBlue               Vector4(52, 152, 219, 255)
#define colorGreen              Vector4(46, 204, 113, 255)
#define colorPurple             Vector4(128, 0, 255, 255)
#define colorBrightPurple       Vector4(255, 0, 255, 255)
#define colorYellow             Vector4(255, 255, 0, 255)
#define colorWhite              Vector4(255, 255, 255, 255)
#define colorDarkViolet         Vector4(148, 0, 211, 255)
#define colorMediumOrchid       Vector4(186, 85, 211, 255)
#define colorBlueViolet         Vector4(138, 43, 226, 255)
#define colorVioletFlower       Vector4(191, 95, 255, 255)
#define GetColorCode(color)     Vector4(color.x / 255, color.y / 255, color.z / 255, color.a / 255)

static CONST PCHAR(__cdecl *va_Mw2)(CONST PCHAR Format, ...);
static BOOL(__cdecl *Dvar_GetBool_Mw2)(CONST PCHAR Dvar);
static void(__cdecl *CG_LocationalTraceMw2)(trace_t* trace, Vector3 *start, Vector3 *end, int passEntityNum, int contMask);
static DWORD(__cdecl *DB_FindXAssetHeader_Mw2)(XAssetType_Mw2 Type, CONST PCHAR Name, DWORD r5);
static DWORD(__cdecl *R_TextWidth_Mw2)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font);
static VOID(__cdecl *CL_DrawTextPhysical_Mw2)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font, FLOAT X, FLOAT Y, FLOAT XScale, FLOAT YScale, FLOAT Angle, Vector4& Color, DWORD Style);
static VOID(__cdecl *R_AddCmdDrawStretchPic_Mw2)(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, FLOAT XScale, FLOAT YScale, FLOAT Xay, FLOAT Yay, Vector4& Color, DWORD Material);
static VOID(__cdecl* CG_DrawRotatedPicPhysical_Mw2)(DWORD ScreenPlacement, FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, FLOAT Angle, Vector4& Color, DWORD Material);
static VOID(__cdecl *UI_FillRectPhysical_Mw2)(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, Vector4& Color);
static void(__cdecl *Cbuf_AddText_Mw2)(DWORD LocalClientNum, char *Message);
static short(__cdecl *SL_GetString_Mw2)(PCHAR String, DWORD aZero);
static DWORD(__cdecl *Com_GetClientDObj_Mw2)(DWORD r3, DWORD r4);
static int(__cdecl *CG_GetWeaponIndexForName)(char* name);
static int(__cdecl *PlayLocalSoundAliasByName)(int localClientNum, char *aliasname, int a0);
static void(__cdecl *CG_GetEntityBModelBounds)(pcentity_s_Mw2 centity_s, float* mins, float* maxs);
static int(__cdecl *R_NormalizedTextScale_Mw2)(int font, float scale);
static bool(__cdecl *CG_CalcNamePosition)(int unk, Vector3* origin, float *xOut, float *yOut);
static float(__cdecl *G_GetWeaponHitLocationMultiplier)(int unk, int WeaponNum);
static void(__cdecl *CL_SetStance)(int localClient, int stance);
static int(__cdecl *AnglesToAxis)(float* ViewAnglesIn, float* ViewAxisOut);

// Stat Functions
static int(__cdecl *sub_82229BC0)(int localClientNum);

// D3D Functions
//static void(__cdecl *D3DDevice_SetRenderState_Mw2)(D3DDevice *pDevice, _D3DRENDERSTATETYPE State, unsigned int Value);
//static void(__cdecl *D3DDevice_SetTexture_Mw2)(D3DDevice *pDevice, unsigned int Sampler, D3DBaseTexture *pTexture, unsigned __int64 PendingMask3);
//static VOID(__cdecl *D3DDevice_DrawIndexedVertices_Mw2)(D3DDevice* device, DWORD unk, DWORD unk2, DWORD unk3, DWORD unk4);

// No Spread Functions
static VOID(__cdecl *BG_GetSpreadForWeapon_Mw2)(pcg_s_Mw2 cg_Mw2, DWORD weaponDef, float* min, float* max);
static VOID(__cdecl *RandomBulletDir_Mw2)(int* Seed, float* flX, float* flY);

static DWORD(__cdecl *BG_GetWeaponDef_Mw2)(DWORD weaponNum);
static VOID(__cdecl *CG_GetPlayerViewOrigin_Mw2)(DWORD localclientnum, pcg_s_Mw2 cg_Mw2, Vector3& Origin);
static int(__cdecl *CalculateRandomSeed)(int* time);

// Radar Functions
static VOID(__cdecl* DrawEngineRadar_Mw2)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, DWORD shader, Vector4& Color);
static VOID(__cdecl* DrawEnemies_Mw2)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color);
static VOID(__cdecl* DrawFriendlies_Mw2)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color);
static VOID(__cdecl* DrawVehicles_Mw2)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color);
static VOID(__cdecl* DrawPlayer_Mw2)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, DWORD shader, Vector4& Color);
static BOOL(__cdecl* CG_WorldPosToCompass_Mw2)(DWORD CompassType, pcg_s_Mw2 cg_Mw2, Vector4& Size, Vector2& Yaw, Vector3& WorldPos, Vector3& WorldPos2, Vector2& Out, Vector2& OutClipped);
static BOOL(__cdecl* CG_CompassCalcDimensions_Mw2)(DWORD CompassType, pcg_s_Mw2 cg_Mw2, Vector2& Size, CRadarHud* RadarHud, float* x, float* y, float* w, float* h);
static void(__cdecl* CG_DrawRotatedPic_Mw2)(DWORD scrPlace, float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, Vector4& color, DWORD material);
static void(__cdecl* CG_CompassUpYawVector_Mw2)(pcg_s_Mw2 cg_Mw2, Vector2& Yaw);
static void(__cdecl* CG_GetCompassSize_Mw2)(DWORD CompassType, float* Width, float* Height);
static float(__cdecl* sub_8228C068)(float unk);


//AutoWall Functions
static VOID(__cdecl *FireBulletPenetrate_Mw2)(DWORD LocalClientNum, BulletFireParams* bulletFireParams, DWORD weaponNum, pcentity_s_Mw2 centity_s, DWORD neg1, Vector3& tracerstart, bool drawTracer) = (VOID(__cdecl*)(DWORD, BulletFireParams*, DWORD, pcentity_s_Mw2, DWORD, Vector3&, bool))0x82134648;//TU8
static char* TeamNames[] =
{
	"spectator",
	"axis",
	"allies"
};

static char* VisCheckBones[7] =
{
	"j_head", "j_shoulder_le", "j_shoulder_ri",
	"j_spinelower", "j_spineupper", "j_ankle_le", "j_ankle_ri"
};

static char* HitboxVisCheckBones[13] =
{
	"j_head", "j_neck", "j_spineupper", "j_spinelower", "j_shoulder_ri", "j_shoulder_le",
	"j_wrist_ri", "j_wrist_le", "j_knee_ri", "j_knee_le", "j_ankle_ri", "j_ankle_le"
};

static char* AutoWallBones[7] =
{
	"j_head", "j_mainroot", "j_hip_le", "j_hip_ri",
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