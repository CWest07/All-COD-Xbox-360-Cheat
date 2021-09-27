#include "stdafx.h"
#include "Utilities.h"


typedef struct cgs_t_AW
{
	BYTE _0x0000[36];
	BYTE Gametype[32]; // 0x0024
	CHAR HostName[260]; // 0x0044
	DWORD MaxClients; // 0x0148
	DWORD PrivateClients; // 0x014C
	CHAR Mapname[64]; // 0x0150
	BYTE _0x0190[54100];
} cgs_t_AW, *pcgs_t_AW; // size 0xD4E4

typedef struct usercmd_s_AW
{
	DWORD ServerTime;		// 0x00-0x04
	DWORD Buttons;		    // 0x04-0x08
	DWORD ViewAngles[3];	// 0x08-0x14
	DWORD weapon;			// 0x14-0x18
	DWORD offHandIndex;	    // 0x18-0x1C
	CHAR forwardmove;	    // 0x1C-0x1D
	CHAR rightmove;		    // 0x1D-0x1E
	CHAR Buf[0x22];		    // 0x1E-0x40

} usercmd_s_AW, *pusercmd_s_AW; // size 0x40
typedef struct clientActive_t_AW
{
	BYTE _0x0000[248];
	Vector3 Origin; // 0x00F8
	BYTE _0x0104[12];
	Vector3 SpawnAngles; // 0x0110
	BYTE _0x011C[0x3568 - 0x11C];
	Vector3 ViewAngles;//0x3568
	usercmd_s_AW UserCommands[128];//0x3574
	DWORD CommandNumber; // 0x5574
	BYTE _0x56F8[42120];

	inline DWORD GetUserCommandNumber()
	{
		return CommandNumber & 0x7F;
	}
	inline pusercmd_s_AW GetUserCommand(DWORD CommandNumber)
	{
		return &UserCommands[CommandNumber]; // # r10 = r11 & 0x7F
	}
} clientActive_t_AW, *pclientActive_t_AW; // size 0x10440

typedef struct centity_s_AW
{
	BYTE _0x0000[20];
	Vector3 Origin; // 0x0014
	Vector3 Angles; // 0x0020
	BYTE _0x002C[64];
	DWORD Flags; // 0x006C
	BYTE _0x0070[0xD8 - 0x70];
	WORD ClientNumber; // 0x00D8
	BYTE _0x00DA[10];
	BYTE EntityType; // 0x00E4
	BYTE _0x00E5[0x124 - 0xE5];//0xE5
	DWORD weaponNum;//0x124
	BYTE _0x0128[0x1DB - 0x128];//0x128
	BYTE Alive; // 0x01DB
	BYTE _0x01DC[48];
} centity_s_AW, *pcentity_s_AW; // size 0x020C

typedef struct refdef_s_AW
{
	BYTE _0x0000[4];
	WORD Width; // 0x0004
	WORD Height; // 0x0006
	BYTE _0x0008[8];
	Vector2 Fov; // 0x0010
	Vector3 ViewOrigin; // 0x0018
	Vector3 ViewAxis[3]; // 0x0024
	BYTE _0x0048[4];
	FLOAT ZoomProgress; // 0x004C
	Vector3 _ViewOrigin; // 0x0050
} refdef_s_AW, *prefdef_s_AW; // custom size 0x5C
struct playerName_t
{
	CHAR PlayerName[32]; // 0x0000
	BYTE _0x0020[276];
}; // custom size 0x134
struct playerNames2
{
	CHAR PlayerName[32];//0x00
	BYTE _0x0020[0x74 - 0x20];
};//Custom Size 0x74
typedef struct clientInfo_t_AW
{
	BYTE _0x0000[4];//0x00
	DWORD Team;//0x04
	BYTE _0x0008[3116];
} clientInfo_t_AW, *pclientInfo_t_AW; // size 0x0C34
typedef struct cg_s_AW
{
	BYTE ClientNumber; // 0x0000
	BYTE _0x0001[75];
	DWORD ServerTime; // 0x004C
	BYTE _0x0050[8];
	DWORD Flags; // 0x0058
	BYTE _0x005C[28];
	Vector3 Origin; // 0x0078
	Vector3 Velocity; // 0x0084
	BYTE _0x0090[336];
	DWORD Health; // 0x01E0
	BYTE _0x01E4[0x1B030 - 0x1E4];
	playerName_t playerNames[18]; // 0x1B030
	BYTE _0x1C5D8[0x377C0 - 0x1C5D8];
	refdef_s_AW refdef; // 0x377C0
	BYTE _0x3781C[0xA05A0 - 0x3781C];
	clientInfo_t_AW clientInfo[18]; // 0xA05A0
	BYTE _0xAE148[0xBBCE9 - 0xAE148];
	playerNames2 playerNames2[18];//0xBBCE9
} cg_s_AW, *pcg_s_AW; // size 0xBF600
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

struct ClientInformation_AW
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
};

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
	short worldEntNum; //0x0000
	short skipNum; //0x0004
	float power; //0x0008
	__int32 bulletType; //0x000C
	__int32 noRicochet;
	Vector3 vieworg; //0x0010
	Vector3 start; //0x001C
	Vector3 end; //0x0028
	Vector3 viewDirection; //0x0034
};

typedef enum XAssetType_AW : DWORD
{
	material = 0x05,
	font = 0x1C,
	rawfile = 0x2B,
	scriptfile = 0x2C,
	structureddatadef = 0x2F
} XAssetType_AW;//tu17

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
#define GetColorCode(color)     Vector4(color.x / 255, color.y / 255, color.z / 255, color.a / 255)

static CONST PCHAR(__cdecl *va_AW)(CONST PCHAR Format, ...) = (CONST PCHAR(__cdecl *)(CONST PCHAR, ...))0x8235DC18;//TU17
static BOOL(__cdecl *Dvar_GetBool_AW)(CONST PCHAR Dvar) = (BOOL(__cdecl *)(CONST PCHAR))0x82364D40;//TU17
//static void(__cdecl *CG_LocationalTraceAW)(void *results, Vector3 *start, Vector3 *end, Vector3* nullz, int passEntityNum, int contMask, bool atrue, bool btrue, int a00) = (void(_cdecl*)(void *, Vector3 *, Vector3 *, Vector3 *, int, int, bool, bool, int))0x825C1D20;//tu17
static void(__cdecl *CG_LocationalTrace_AW)(void *results, Vector3 *start, Vector3 *end, int passEntityNum, int contMask) = (void(_cdecl*)(void *, Vector3 *, Vector3 *, int, int))0x825C2238;//tu17
static DWORD(__cdecl *DB_FindXAssetHeader_AW)(XAssetType_AW Type, CONST PCHAR Name, DWORD r5) = (DWORD(__cdecl *)(XAssetType_AW, CONST PCHAR, DWORD))0x82544098;//TU17
static DWORD(__cdecl *R_TextWidth_AW)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font) = (DWORD(__cdecl *)(CONST PCHAR, DWORD, DWORD))0x822AC770;//TU17
static VOID(__cdecl *CL_DrawTextPhysical_AW)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font, FLOAT X, FLOAT Y, FLOAT XScale, FLOAT YScale, FLOAT Angle, Vector4& Color, DWORD Style) = (VOID(__cdecl*)(CONST PCHAR, DWORD, DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))0x82295F60;//TU17
static VOID(__cdecl *CL_DrawTextPhysical)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font, FLOAT X, FLOAT Y, FLOAT XScale, FLOAT YScale, FLOAT Angle, Vector4& Color, DWORD Style) = (VOID(__cdecl*)(CONST PCHAR, DWORD, DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))0x82294E08;
static VOID(__cdecl *R_AddCmdDrawStretchPic_AW)(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, FLOAT XScale, FLOAT YScale, FLOAT Xay, FLOAT Yay, Vector4& Color, DWORD Material) = (VOID(__cdecl*)(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))0x82295118;//TU17
static VOID(__cdecl* CG_DrawRotatedPicPhysical_AW)(DWORD ScreenPlacement, FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, FLOAT Angle, Vector4& Color, DWORD Material) = (VOID(__cdecl*)(DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))0x826289C0;//TU17
static void(__cdecl *Cbuf_AddText_AW)(DWORD LocalClientNum, char *Message) = (void(__cdecl*)(DWORD, char*))0x82472B80;//tu17
static short(__cdecl *SL_GetString_AW)(PCHAR String, DWORD aZero) = (short(__cdecl*)(PCHAR, DWORD))0x824934A8;//tu17
static int(__cdecl *CG_GetWeaponIndexForName)(char* name) = (int(__cdecl*)(char*))0x8248CF08;//tu17
static int(__cdecl *PlayLocalSoundAliasByName)(int localClientNum, char *aliasname, int a0) = (int(__cdecl*)(int, char*, int))0x8221DC40;//tu17
static void(__cdecl *CG_GetEntityBModelBounds)(pcentity_s_AW centity_s, float* mins, float* maxs) = (void(__cdecl*)(pcentity_s_AW, float*, float*))0x822C9728;//tu17
static VOID(__cdecl *AimAssist_GetTagPos)(DWORD LocalClientNumber, pcentity_s_AW centity_s, WORD Tag, Vector3& Position) = (VOID(__cdecl *)(DWORD, pcentity_s_AW, WORD, Vector3&))0x82774628;//TU17
static bool(__cdecl *CG_CalcNamePosition)(int unk, Vector3* origin, float *xOut, float *yOut) = (bool(__cdecl*)(int, Vector3*, float *, float *))0x8262C140;//tu17
static float(__cdecl *G_GetWeaponHitLocationMultiplier)(int unk, int WeaponNum) = (float(__cdecl*)(int, int))0x821DD100;//TU8
static CONST PCHAR(__cdecl *GetWeaponHUDName_AW)(DWORD weaponNum, DWORD unk, PCHAR Buffer) = (CONST PCHAR(__cdecl*)(DWORD, DWORD, PCHAR))0x825D4268;//TU17

// D3D Functions
static void(__cdecl *D3DDevice_SetRenderState_AW)(D3DDevice *pDevice, _D3DRENDERSTATETYPE State, unsigned int Value) = (void(__cdecl*)(D3DDevice *, _D3DRENDERSTATETYPE, unsigned int))0x823873C0;//TU8 // 823992C8
static void(__cdecl *D3DDevice_SetTexture_AW)(D3DDevice *pDevice, unsigned int Sampler, D3DBaseTexture *pTexture, unsigned __int64 PendingMask3) = (void(__cdecl*)(D3DDevice *, unsigned int, D3DBaseTexture*, unsigned __int64))0x820B13D8;//TU8
static VOID(__cdecl *D3DDevice_DrawIndexedVertices_AW)(D3DDevice* device, DWORD unk, DWORD unk2, DWORD unk3, DWORD unk4) = (VOID(__cdecl*)(D3DDevice*, DWORD, DWORD, DWORD, DWORD))0x820AF1D0;//TU8

// No Spread Functions
static VOID(__cdecl *BG_GetSpreadForWeapon_AW)(pcg_s_AW cg_AW, float* min, float* max) = (VOID(__cdecl*)(pcg_s_AW, float*, float*))0x82647658;//TU17
static VOID(__cdecl *RandomBulletDir_AW)(int* Seed, float zero, float threesixty, float* x, float* y) = (VOID(__cdecl*)(int*, float, float, float*, float*))0x825C45B8;//TU17
static float(__cdecl *BG_Random_AW)(int* seed) = (float(__cdecl*)(int*))0x82663EF0;
static VOID(__cdecl *CG_GetPlayerViewOrigin_AW)(int LocalClientNum, pcg_s_AW cg_AW, Vector3& Origin) = (VOID(__cdecl*)(int, pcg_s_AW, Vector3&))0x825D39C0;//TU17

// Radar Functions
static VOID(__cdecl* DrawEngineRadar_AW)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, DWORD shader, Vector4& Color) = (VOID(__cdecl*)(DWORD, DWORD, Vector2&, CRadarHud*, DWORD, Vector4&))0x82260B50;//tu17
static VOID(__cdecl* DrawEnemies_AW)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color) = (VOID(__cdecl*)(DWORD, DWORD, Vector2&, CRadarHud*, Vector4&))0x8220DDB0;//tu17
static VOID(__cdecl* DrawFriendlies_AW)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color) = (VOID(__cdecl*)(DWORD, DWORD, Vector2&, CRadarHud*, Vector4&))0x82264A38;//tu17
static VOID(__cdecl* DrawVehicles_AW)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color) = (VOID(__cdecl*)(DWORD, DWORD, Vector2&, CRadarHud*, Vector4&))0x82266D70;//tu17
static VOID(__cdecl* DrawPlayer_AW)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color) = (VOID(__cdecl*)(DWORD, DWORD, Vector2&, CRadarHud*, Vector4&))0x82260E40;//tu17
static BOOL(__cdecl* CG_WorldPosToCompass_AW)(DWORD CompassType, pcg_s_AW cg_AW, Vector4& Size, Vector2& Yaw, Vector3& WorldPos, Vector3& WorldPos2, Vector2& Out, Vector2& OutClipped) = (BOOL(__cdecl*)(DWORD, pcg_s_AW, Vector4&, Vector2&, Vector3&, Vector3&, Vector2&, Vector2&))0x8225FD18;//tu17
static BOOL(__cdecl* CG_CompassCalcDimensions_AW)(DWORD CompassType, pcg_s_AW cg_AW, Vector2& Size, CRadarHud* RadarHud, float* x, float* y, float* w, float* h) = (BOOL(__cdecl*)(DWORD, pcg_s_AW, Vector2&, CRadarHud*, float*, float*, float*, float*))0x8225F3D8;//tu17
static void(__cdecl* CG_DrawRotatedPic_AW)(DWORD scrPlace, float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, Vector4& color, DWORD material) = (VOID(__cdecl*)(DWORD, float, float, float, float, int, int, float, Vector4&, DWORD))0x822702F0;//tu17
static void(__cdecl* CG_CompassUpYawVector_AW)(pcg_s_AW cg_AW, Vector2& Yaw) = (VOID(__cdecl*)(pcg_s_AW, Vector2&))0x82261428;//tu17
static float(__cdecl* AngleNormalize360_AW)(float unk) = (float(__cdecl*)(float))0x82552EC0;//tu17


//AutoWall Functions
static VOID(__cdecl *FireBulletPenetrate_AW)(DWORD LocalClientNum, BulletFireParams* bp, DWORD weaponIndex, DWORD unk1, DWORD unk2, pcentity_s_AW centity_s, DWORD neg1, Vector3& Origin, bool drawTracer) = (VOID(__cdecl*)(DWORD, BulletFireParams*, DWORD, DWORD, DWORD, pcentity_s_AW, DWORD, Vector3&, bool))0x825D2420;//TU17

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

static char* AutoWallBones[7] =
{
	"j_head", "j_hip_le", "j_hip_ri", "j_mainroot",
	"j_spineupper", "j_shoulder_le", "j_shoulder_ri",
};


static char* HitboxVisCheckBones[13] =
{
	"j_head", "j_neck", "j_spineupper", "j_mainroot", "j_spinelower", "j_shoulder_ri", "j_shoulder_le",
	"j_wrist_ri", "j_wrist_le", "j_knee_ri", "j_knee_le", "j_ankle_ri", "j_ankle_le"
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


static int XAASpinbots_Size = 4;
static int XAASpinbots_Start = 0;
static char *XAASpinbots[] =
{
	"Off",
	"Look Down",
	"Up & Down",
	"Riot Shield"
};

static int YAASpinbots_Size = 7;
static int YAASpinbots_Start = 0;
static char *YAASpinbots[] =
{
	"Off",
	"Flip Flop",
	"Face Away",
	"Jitter",
	"Sideways",
	"Detection",
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

static int Prestiges_Size = 33;
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
	"21st",
	"22nd",
	"23rd",
	"24th",
	"25th",
	"26th",
	"27th",
	"28th",
	"29th",
	"30th",
	"31st"
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