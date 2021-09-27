#include "stdafx.h"
#include "Utilities.h"


typedef struct cgs_t_BO3
{
	BYTE _0x0000[8];//0x00
	__int32 Width;//0x08
	__int32 Height;//0x0C
	BYTE _0x0010[32];//0x10
	CHAR Gametype[32]; // 0x0030
	CHAR HostName[256]; // 0x0050
	DWORD MaxClients; // 0x0150
	DWORD PrivateClients; // 0x0154
	CHAR Mapname[64]; // 0x0158
	CHAR CleanMapName[64];//0x198
} cgs_t_BO3, *pcgs_t_BO3;

typedef struct usercmd_s_BO3
{
	DWORD ServerTime;		 // 0x00-0x04
	DWORD Buttons;		     // 0x04-0x08
	CHAR Buf08[0x08];        // 0x08-0x10
	DWORD ViewAngles[3];     // 0x10-0x1C
	DWORD weapon;            // 0x1C-0x20
	DWORD offHandIndex;      // 0x20-0x24
	CHAR Buf24[0x28 - 0x24]; // 0x25-0x28
	CHAR forwardmove;        // 0x28-0x29
	CHAR rightmove;          // 0x29-0x30
	CHAR Buf2A[0x40 - 0x2A]; // 0x2A-0x40
} usercmd_s_BO3, *pusercmd_s_BO3; // size 0x40
C_ASSERT(sizeof(usercmd_s_BO3) == 0x40);

typedef struct clientActive_t_BO3
{
	BYTE _0x0000[0xD0];//0x00
	Vector3 Origin;//0xD0
	BYTE _0x00DC[0x15C - 0xDC];//0xDC
	Vector3 SpawnAngles;//0x15C
	BYTE _0x0168[0x370 - 0x168];//0x160
	DWORD Health;//0x370
	BYTE _0x0374[0x31A0 - 0x374];//0x374
	Vector3 ViewAngles;//0x31A0
	BYTE _0x31AC[0x231BC - 0x31AC];//0x31AC
	usercmd_s_BO3 UserCommands[128];// 0x231BC
	DWORD CommandNumber; // 0x251BC

	inline DWORD GetUserCommandNumber()
	{
		return CommandNumber & 0x7F;
	}
	inline pusercmd_s_BO3 GetUserCommand(DWORD CommandNumber)
	{
		return &UserCommands[CommandNumber]; // # r10 = r11 & 0x7F
	}
} clientActive_t_BO3, *pclientActive_t_BO3;

typedef struct centity_s_BO3
{
	BYTE _0x0000[0x28];
	Vector3 Origin; // 0x0028
	BYTE _0x0034[0x1F8 - 0x34];//0x34
	DWORD weaponNum;//0x1F8
	BYTE _0x01FC[0x258 - 0x1FC];//0x1FC
	DWORD ClientNumber;//0x258
	BYTE _0x025C[0x3BF - 0x25C];//0x208
	BYTE Type;//0x3BF;
	BYTE _0x03C0[0x638 - 0x3C0];//0x3C0
	DWORD Flags;//0x638
	BYTE _0x063C[4];//0x63C

	inline BOOL IsAlive()
	{
		return (Flags & 0x40000000);
	}
} centity_s_BO3, *pcentity_s_BO3; // size 0x640

typedef struct refdef_s_BO3
{
	BYTE _0x0000[8];//0x00
	__int32 Width;//0x08
	__int32 Height;//0x0C
	char _0x0010[0x74 - 0x10];//0x10
	Vector2 Fov;//0x74
	DWORD Padding;//0x7C
	FLOAT TotalFov;//0x80
	Vector3 ViewOrg;//0x84
	char _0x0090[0x10];//0x90
	Vector3 ViewAxis[3];//0xA0
} refdef_s_BO3, *prefdef_s_BO3; //0xC4

struct snapshot_s
{
	int snapFlags;
	int ping;
	int serverTime;
	int physicsTime;
	int serverTime2;
};

typedef struct
{
	DWORD Valid;//0x00
	BYTE _0x0004[8];//0x04
	CHAR Name[0x20];//0x0C
	DWORD Team;//0x2C
	DWORD Team2;//0x30
	BYTE _0x0034[0x58 - 0x34];//0x34
	QWORD XUID;//0x58
	BYTE _0x0060[0xC8 - 0x60];//0x60
	DWORD Health;//0xC8
	BYTE _0x00CC[0x980 - 0xCC];//0xCC
}client_t_BO3; //[Size: 0x980]

typedef struct cg_s_BO3
{
	DWORD ClientNumber;//0x00
	BYTE _0x0004[0x1C];//0x04
	snapshot_s *snap;//0x20
	snapshot_s *nextsnap;//0x24
	BYTE _0x0028[0x84 - 0x28];//0x28
	DWORD Ping;//0x84
	BYTE _0x0088[0xC0 - 0x88];//0x88
	Vector3 Origin;//0xC0
	BYTE _0x00CC[0x2E8 - 0xCC];//0xCC
	DWORD weaponNum;//0x2E8
	BYTE _0x02EC[0x32550 - 0x2EC];//0x2EC
	refdef_s_BO3 refdef;//0x32550
	BYTE _0x32614[0x54E10 - 0x32614];//0x32614
	client_t_BO3 clientInfo[12];//0x54E10
} cg_s_BO3, *pcg_s_BO3;
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

struct ClientInformation_BO3
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
	char Buf00[0x10];
	float Fraction;
	char Buf04[0x60];
};

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
	BYTE _0x40[12]; //three floats
};

typedef enum XAssetType_BO3 : DWORD
{
	material = 0x06,
	font = 0x14,
} XAssetType_BO3;//tu17

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

static CONST PCHAR(__cdecl *va_BO3)(CONST PCHAR Format, ...) = (CONST PCHAR(__cdecl *)(CONST PCHAR, ...))0x826BECE0;//TU7
static BOOL(__cdecl *Dvar_GetBool_BO3)(CONST PCHAR Dvar) = (BOOL(__cdecl *)(CONST PCHAR))0x82364D40;//TU17
static void(__cdecl *CG_LocationalTraceBO3)(void *results, Vector3 *start, Vector3 *end, int passEntityNum, int contMask, bool afalse, int a0) = (void(_cdecl*)(void *, Vector3 *, Vector3 *, int, int, bool, int))0x82285838;//tu7
static DWORD(__cdecl *DB_FindXAssetHeader_BO3)(XAssetType_BO3 Type, CONST PCHAR Name, DWORD imageTrack, int neg1) = (DWORD(__cdecl *)(XAssetType_BO3, CONST PCHAR, DWORD, int))0x82328110;//TU7
static DWORD(__cdecl *R_TextWidth_BO3)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font, DWORD a0) = (DWORD(__cdecl *)(CONST PCHAR, DWORD, DWORD, DWORD))0x824A1000;//TU7
static VOID(__cdecl *CL_DrawTextPhysical_BO3)(CONST PCHAR Text, DWORD MaxCharacters, DWORD Font, FLOAT X, FLOAT Y, FLOAT XScale, FLOAT YScale, Vector4& Color, DWORD Style) = (VOID(__cdecl*)(CONST PCHAR, DWORD, DWORD, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))0x824AE7C0;//TU7
static VOID(__cdecl *R_AddCmdDrawStretchPic_BO3)(FLOAT X, FLOAT Y, FLOAT W, FLOAT H, FLOAT XScale, FLOAT YScale, FLOAT Xay, FLOAT Yay, Vector4& Color, DWORD Material) = (VOID(__cdecl*)(FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD))0x824AE300;//TU7
static VOID(__cdecl* CG_DrawRotatedPicPhysical_BO3)(DWORD ScreenPlacement, FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height, FLOAT Angle, Vector4& Color, DWORD Material, DWORD a0) = (VOID(__cdecl*)(DWORD, FLOAT, FLOAT, FLOAT, FLOAT, FLOAT, Vector4&, DWORD, DWORD))0x82210468;//TU7
static void(__cdecl *Cbuf_AddText_BO3)(DWORD LocalClientNum, char *Message) = (void(__cdecl*)(DWORD, char*))0x8263A6A0;//TU7
static int(__cdecl *CG_GetWeaponIndexForName)(char* name) = (int(__cdecl*)(char*))0x8248CF08;//tu17
static void(__cdecl *PlayLocalSoundAliasByName)(char *aliasname, int a0, float soundLevel, int r6, int r7, int r8, int r9) = (void(__cdecl*)(char *, int, float, int, int, int, int))0x82699610;//TU7
static VOID(__cdecl *AimAssist_GetTagPos_BO3)(pcentity_s_BO3 centity_s, WORD Tag, Vector3& Position) = (VOID(__cdecl *)(pcentity_s_BO3, WORD, Vector3&))0x82163C90;//TU7
static bool(__cdecl *CG_CalcNamePosition)(int unk, Vector3* origin, float *xOut, float *yOut) = (bool(__cdecl*)(int, Vector3*, float *, float *))0x82212AA0;//TU7
static bool(__cdecl *World2Screen)(Vector3* WorldPos, Vector3* ScreenPos) = (bool(__cdecl*)(Vector3*, Vector3*))0x825F9AB0;//TU7
static float(__cdecl *G_GetWeaponHitLocationMultiplier)(int unk, int WeaponNum) = (float(__cdecl*)(int, int))0x821DD100;//TU8

// D3D Functions
static void(__cdecl *D3DDevice_SetRenderState_BO3)(D3DDevice *pDevice, _D3DRENDERSTATETYPE State, unsigned int Value) = (void(__cdecl*)(D3DDevice *, _D3DRENDERSTATETYPE, unsigned int))0x823873C0;//TU8 // 823992C8
static void(__cdecl *D3DDevice_SetTexture_BO3)(D3DDevice *pDevice, unsigned int Sampler, D3DBaseTexture *pTexture, unsigned __int64 PendingMask3) = (void(__cdecl*)(D3DDevice *, unsigned int, D3DBaseTexture*, unsigned __int64))0x820B13D8;//TU8
static VOID(__cdecl *D3DDevice_DrawIndexedVertices_BO3)(D3DDevice* device, DWORD unk, DWORD unk2, DWORD unk3, DWORD unk4) = (VOID(__cdecl*)(D3DDevice*, DWORD, DWORD, DWORD, DWORD))0x820AF1D0;//TU8

// No Spread Functions
static VOID(__cdecl *BG_GetSpreadForWeapon_BO3)(int unkStruct, int weaponDef, float* min, float* max) = (VOID(__cdecl*)(int, int, float*, float*))0x821DC870;//    0x82272C18;//TU7
static float(__cdecl *BG_Random_BO3)(int* seed) = (float(__cdecl*)(int*))0x82663EF0;
static VOID(__cdecl *CG_GetPlayerViewOrigin_BO3)(int myClientNum, int playerState, Vector3& Origin) = (VOID(__cdecl*)(int, int, Vector3&))0x822730E0;//TU7

// Radar Functions
static VOID(__cdecl* DrawEngineRadar_BO3)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, DWORD shader, Vector4& Color) = (VOID(__cdecl*)(DWORD, DWORD, Vector2&, CRadarHud*, DWORD, Vector4&))0x82260B50;//tu17
static VOID(__cdecl* DrawEnemies_BO3)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color) = (VOID(__cdecl*)(DWORD, DWORD, Vector2&, CRadarHud*, Vector4&))0x8220DDB0;//tu17
static VOID(__cdecl* DrawFriendlies_BO3)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color) = (VOID(__cdecl*)(DWORD, DWORD, Vector2&, CRadarHud*, Vector4&))0x82264A38;//tu17
static VOID(__cdecl* DrawVehicles_BO3)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color) = (VOID(__cdecl*)(DWORD, DWORD, Vector2&, CRadarHud*, Vector4&))0x82266D70;//tu17
static VOID(__cdecl* DrawPlayer_BO3)(DWORD unk, DWORD disablerotation, Vector2& Size, CRadarHud* radarHud, Vector4& Color) = (VOID(__cdecl*)(DWORD, DWORD, Vector2&, CRadarHud*, Vector4&))0x82260E40;//tu17
static BOOL(__cdecl* CG_WorldPosToCompass_BO3)(DWORD CompassType, pcg_s_BO3 cg_BO3, Vector4& Size, Vector2& Yaw, Vector3& WorldPos, Vector3& WorldPos2, Vector2& Out, Vector2& OutClipped) = (BOOL(__cdecl*)(DWORD, pcg_s_BO3, Vector4&, Vector2&, Vector3&, Vector3&, Vector2&, Vector2&))0x8225FD18;//tu17
static BOOL(__cdecl* CG_CompassCalcDimensions_BO3)(DWORD CompassType, pcg_s_BO3 cg_BO3, Vector2& Size, CRadarHud* RadarHud, float* x, float* y, float* w, float* h) = (BOOL(__cdecl*)(DWORD, pcg_s_BO3, Vector2&, CRadarHud*, float*, float*, float*, float*))0x8225F3D8;//tu17
static void(__cdecl* CG_DrawRotatedPic_BO3)(DWORD scrPlace, float x, float y, float width, float height, int horzAlign, int vertAlign, float angle, Vector4& color, DWORD material) = (VOID(__cdecl*)(DWORD, float, float, float, float, int, int, float, Vector4&, DWORD))0x822702F0;//tu17
static void(__cdecl* CG_CompassUpYawVector_BO3)(pcg_s_BO3 cg_BO3, Vector2& Yaw) = (VOID(__cdecl*)(pcg_s_BO3, Vector2&))0x82261428;//tu17
static float(__cdecl* AngleNormalize360_BO3)(float unk) = (float(__cdecl*)(float))0x82552EC0;//tu17


//AutoWall Functions
static VOID(__cdecl *FireBulletPenetrate_BO3)(DWORD LocalClientNum, BulletFireParams* bp, DWORD weaponIndex, DWORD unk0, Vector3 *tracerStart, DWORD unk1, void *bulletTraceResults) = (VOID(__cdecl*)(DWORD, BulletFireParams*, DWORD, DWORD, Vector3 *, DWORD, void *))0x82272708;//TU7


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
	"j_ankle_le", "j_ankle_ri", "pelvis", "j_spine4", "tag_eye", "tag_player"
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

static int YAASpinbots_Size = 4;
static int YAASpinbots_Start = 0;
static char *YAASpinbots[] =
{
	"Off",
	"Flip Flop",
	"Face BO3ay",
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
// j_jBO3
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