#ifndef UTILITIES_H
#define UTILITIES_H

#include "stdafx.h"

float AngleNormalize(float angle);

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];
#define PITCH 0
#define YAW 1
#define ROLL 2

typedef struct Vector2
{
	FLOAT x, y;
	Vector2()
	{
		x = y = 0;
	}
	Vector2(FLOAT x, FLOAT y)
	{
		this->x = x;
		this->y = y;
	}

} Vector2, *PVector2;

#define PITCH 0
#define YAW 1
#define ROLL 2

typedef struct Vector3
{
	FLOAT x, y, z;
	Vector3()
	{
		x = y = z = 0;
	}
	Vector3(FLOAT x, FLOAT y, FLOAT z)
	{
		this->x = x; this->y = y; this->z = z;
	}
	Vector3(float* inp) {
		this->x = inp[0];
		this->y = inp[1];
		this->z = inp[2];
	}
	Vector3 negate()
	{
		this->x = this->x*-1.0f;
		this->y = this->y*-1.0f;
		this->z = this->z*-1.0f;
		return *this;
	}
	void setAngles(float *in)
	{
		x = in[0];
		y = in[1];
		z = in[2];
	}
	void setAngles(Vector3 in)
	{
		x = in.x;
		y = in.y;
		z = in.z;
	}
	void setAngles(float xIn, float yIn, float zIn)
	{
		x = xIn;
		y = yIn;
		z = zIn;
	}
	BOOL operator== (Vector3 &Vector)
	{
		return (this->x == Vector.x && this->y == Vector.y && this->z == Vector.z);
	}
	Vector3 operator+ (Vector3 &Vector)
	{
		return Vector3(this->x + Vector.x, this->y + Vector.y, this->z + Vector.z);
	}
	Vector3 operator- (Vector3 &Vector)
	{
		return Vector3(this->x - Vector.x, this->y - Vector.y, this->z - Vector.z);
	}
	float *toFloatArray()
	{
		return &x;
	}
	FLOAT Distance(Vector3 &Vector)
	{
		return sqrt(DistanceEx(Vector));
	}
	FLOAT DistanceEx(Vector3 &Vector)
	{
		FLOAT _x = this->x - Vector.x, _y = this->y - Vector.y, _z = this->z - Vector.z;
		return ((_x * _x) + (_y * _y) + (_z * _z));
	}
	FLOAT DotProduct(Vector3 &Vector)
	{
		return (this->x * Vector.x) + (this->y * Vector.y) + (this->z * Vector.z);
	}

	inline float Length(void) const { return (float)sqrtf(x*x + y*y + z*z); }
	inline Vector3 Normalize(void) const
	{
		float flLen = Length();
		if (flLen == 0) return Vector3(0, 0, 1); // ????
		flLen = 1 / flLen;
		return Vector3(x * flLen, y * flLen, z * flLen);
	}

	bool checkEmpty()
	{
		if (x == 0.0f && y == 0.0f && z == 0.0f)
			return true;
		else
			return false;
	}
	bool isEqualTo(Vector3 check)
	{
		return (check.x == x && check.y == y && check.z == z);
	}
	bool isWithinRange(Vector3 check, float range)
	{
		return ((check.x >= x - range && check.x <= x + range) &&
			(check.y >= y - range && check.y <= y + range) &&
			(check.z >= z - range && check.z <= z + range));
	}
	FLOAT& operator[] (int inp) {
		if (inp == 0)
			return x;
		if (inp == 1)
			return y;
		if (inp == 2)
			return z;
		return x;
	}
	Vector3 RoundHalfUp()
	{
		return Vector3((float)floor(this->x + 0.5), (float)floor(this->y + 0.5), (float)floor(this->z + 0.5));
	}\
		Vector3 RoundHalfDown()
	{
		return Vector3((float)floor(this->x + 0.5), (float)floor(this->y + 0.5), (float)floor(this->z + 0.5));
	}
} Vector3, *PVector3;
typedef struct Vector4
{
	FLOAT x, y, z, a;
	Vector4()
	{
		x = y = z = a = 0;
	}
	Vector4(FLOAT x, FLOAT y, FLOAT z, FLOAT a)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->a = a;
	}

} Vector4, *PVector4;

struct vec3
{
	float x;
	float y;
	float z;
	vec3() {}
	vec3(float xIn, float yIn, float zIn)
	{
		x = xIn;
		y = yIn;
		z = zIn;
	}
	vec3(float *in)
	{
		x = in[0];
		y = in[1];
		z = in[2];
	}
	vec3(Vector3& in)
	{
		x = in.x;
		y = in.y;
		z = in.z;
	}
	void setAngles(float *in)
	{
		x = in[0];
		y = in[1];
		z = in[2];
	}
	void setAngles(vec3 in)
	{
		x = in.x;
		y = in.y;
		z = in.z;
	}
	void setAngles(float xIn, float yIn, float zIn)
	{
		x = xIn;
		y = yIn;
		z = zIn;
	}
	vec3 add(vec3 val)
	{
		x += val.x;
		y += val.y;
		z += val.z;
		return *this;
	}
	vec3 subtract(vec3 val)
	{
		x -= val.x;
		y -= val.y;
		z -= val.z;
		return *this;
	}
	float *toFloatArray()
	{
		return &x;
	}
	void to_vec3_t(vec3_t *out)
	{
		if (out)
		{
			*out[0] = x;
			*out[1] = y;
			*out[2] = z;
		}
	}
	bool checkEmpty()
	{
		if (x == 0.0f && y == 0.0f && z == 0.0f)
			return true;
		else
			return false;
	}
	bool isEqualTo(vec3 check)
	{
		return (check.x == x && check.y == y && check.z == z);
	}
	bool isWithinRange(vec3 check, float range)
	{
		return ((check.x >= x - range && check.x <= x + range) &&
			(check.y >= y - range && check.y <= y + range) &&
			(check.z >= z - range && check.z <= z + range));
	}
	vec3 negate()
	{
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}
	vec3 normalize()
	{
		x = AngleNormalize(x);
		y = AngleNormalize(y);
		z = AngleNormalize(z);
		return *this;
	}
	void clear()
	{
		x = 0;
		y = 0;
		z = 0;
	}
};

struct CRectDef
{
	float x; //0x0
	float y; //0x4
	float w; //0x8
	float h; //0xC
	DWORD horizAlign; //0x10
	DWORD vertAlign;
	CRectDef()
	{
		x = 0.0f;
		y = 0.0f;
		w = 0.0f;
		h = 0.0f;
		horizAlign = 0;
		vertAlign = 0;
	}
	CRectDef(float xIn, float yIn, float wIn, float hIn, DWORD horizAlignIn, DWORD vertAlignIn = 0)
	{
		x = xIn;
		y = yIn;
		w = wIn;
		h = hIn;
		horizAlign = horizAlignIn;
		vertAlign = vertAlignIn;
	}
};

Vector3 VectorToAngles(Vector3 Delta);
void AngleVectors(Vector3 angles, Vector3 *forward, Vector3 *right, Vector3 *up);
void AxisToAngles(Vector3* axis, float* angles);
void VectorToAngle(const float * Vec, float * angles);

static float fixAngleRanges(float Angle) {
	if (Angle < 0.0f)
		Angle += 360.0f;
	else if (Angle > 360.0f)
		Angle -= 360.0f;
	return Angle;
}

#define AngleToShort(X) ((DWORD)((X) * 65536 / 360) & 65535)
#define	ShortToAngle(X) ((X) * (360.0f / 65536))

#define GetPointer(X) *(DWORD*)(X)
#define InsertTailList(ListHead,Entry) {\
	PLIST_ENTRY _EX_Blink;\
	PLIST_ENTRY _EX_ListHead;\
	_EX_ListHead = (ListHead);\
	_EX_Blink = _EX_ListHead->Blink;\
	(Entry)->Flink = _EX_ListHead;\
	(Entry)->Blink = _EX_Blink;\
	_EX_Blink->Flink = (Entry);\
	_EX_ListHead->Blink = (Entry);\
}

typedef HRESULT(*pDmSetMemory)(LPVOID lpbAddr, DWORD cb, LPCVOID lpbBuf, LPDWORD pcbRet);

#pragma pack(push, 1)
typedef struct _ORDINALS {
	// xam
	DWORD NetDll_connect;
	DWORD NetDll_recv;
	DWORD NetDll_send;
	// kernel
	DWORD HalReturnToFirmware;
	DWORD MmGetPhysicalAddress;
	DWORD XeCryptSha;
	DWORD XeCryptRc4;
	DWORD XexLoadImageFromMemory;
	DWORD XexLoadImage;
	DWORD XexLoadExecutable;
	DWORD XeCryptRandom;
	DWORD XeCryptShaInit;
	DWORD XeCryptShaUpdate;
	DWORD XeCryptShaFinal;
	DWORD MmAllocatePhysicalMemoryEx;
	DWORD MmFreePhysicalMemory;
} ORDINALS, *PORDINALS;
#pragma pack(pop)

class MemoryBuffer
{
public:

	MemoryBuffer(DWORD dwSize = 512)
	{
		m_pBuffer = NULL;
		m_dwDataLength = 0;
		m_dwBufferSize = 0;

		if ((dwSize < UINT_MAX) && (dwSize != 0))
		{
			m_pBuffer = (BYTE*)malloc(dwSize + 1); // one more char, in case when using string funcions
			if (m_pBuffer)
			{
				m_dwBufferSize = dwSize;
				m_pBuffer[0] = 0;
			}
		}
	};

	~MemoryBuffer()
	{
		if (m_pBuffer)
			free(m_pBuffer);

		m_pBuffer = NULL;
		m_dwDataLength = 0;
		m_dwBufferSize = 0;
	};

	// Add chunk of memory to buffer
	BOOL Add(const void* p, DWORD dwSize)
	{
		if (CheckSize(dwSize))
		{
			memcpy(m_pBuffer + m_dwDataLength, p, dwSize);
			m_dwDataLength += dwSize;
			*(m_pBuffer + m_dwDataLength) = 0; // fill end zero
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	};

	// Get the data in buffer
	BYTE* GetData() const
	{
		return m_pBuffer;
	};

	// Get the length of data in buffer
	DWORD GetDataLength() const
	{
		return m_dwDataLength;
	};

	// Rewind the data pointer to the begining
	void Rewind()
	{
		m_dwDataLength = 0;
		m_pBuffer[0] = 0;
	};

	// Automatically adjust increase buffer size if necessary
	BOOL CheckSize(DWORD dwSize)
	{
		if (m_dwBufferSize >= (m_dwDataLength + dwSize))
		{
			return TRUE; // Enough space
		}
		else
		{
			// Try to double it
			DWORD dwNewSize = max(m_dwDataLength + dwSize, m_dwBufferSize * 2);
			BYTE* pNewBuffer = (UCHAR*)realloc(m_pBuffer, dwNewSize + 1); // one more char
			if (pNewBuffer)
			{
				m_pBuffer = pNewBuffer;
				m_dwBufferSize = dwNewSize;
				return TRUE;
			}
			else
			{
				// Failed
				return FALSE;
			}
		}
	}

private:

	BYTE* m_pBuffer;

	DWORD m_dwDataLength;

	DWORD m_dwBufferSize;
};


#if defined(_DEBUG) || defined(DBG_PRINT)
VOID DbgPrint(const CHAR* fmt, ...);
#else
#define DbgPrint
#endif

void printBytes(PBYTE bytes, DWORD len);


#define MAX_SPINS_DEFAULT		65280				// I don't think a large number here is a good thing, drivers tend to use 4000 according to google



VOID ToggleNotifications(BOOL on);
VOID XNotifyUI(LPCWSTR pwszStringParam, DWORD Delayms = 0);
VOID launchSysMsg(WCHAR* msg, int delay);

HRESULT SetMemory(VOID* Destination, VOID* Source, DWORD Length);
HRESULT CreateSymbolicLink(CHAR* szDrive, CHAR* szDeviceName, BOOL System);
HRESULT DeleteSymbolicLink(CHAR* szDrive, BOOL System);

BOOL XeKeysPkcs1Verify(const unsigned char* pbHash, const unsigned char* pbSig, XECRYPT_RSA *pRsa);

BOOL CReadFile(const CHAR * FileName, MemoryBuffer &pBuffer);
VOID PatchInJump(DWORD* Address, void* Dest, BOOL Linked);
VOID PatchInJump(DWORD* Address, DWORD Destination, BOOL Linked);
VOID HookFunctionStart(PDWORD addr, PDWORD saveStub, DWORD dest);
VOID PatchInBranch(DWORD* Address, DWORD Destination, BOOL Linked);
FARPROC ResolveFunction(CHAR* ModuleName, DWORD Ordinal);
int ResolveFunction_(CHAR* ModuleName, DWORD Ordinal);
DWORD PatchModuleImport(CHAR* Module, CHAR* ImportedModuleName, DWORD Ordinal, DWORD PatchAddress);
DWORD PatchModuleImport(PLDR_DATA_TABLE_ENTRY Module, CHAR* ImportedModuleName, DWORD Ordinal, DWORD PatchAddress);
DWORD GetModuleImportAddress(LDR_DATA_TABLE_ENTRY* moduleHandle, CHAR* ImportedModuleName, DWORD Ordinal);
DWORD GetModuleImportCallAddress(LDR_DATA_TABLE_ENTRY* moduleHandle, CHAR* ImportedModuleName, DWORD Ordinal);
PDWORD hookFunctionStartOrd(char* modName, DWORD ord, PDWORD saveStub, PDWORD oldData, DWORD dest);
void hookFunctionStart(PDWORD addr, PDWORD saveStub, PDWORD oldData, DWORD dest);

BOOL IsBufferEmpty(BYTE* Buffer, DWORD Length);
BOOL FileExists(LPCSTR lpFileName);
VOID ShowMessageBox(LPCWSTR title, LPCWSTR msg, DWORD dwFlags = XMB_NOICON);
VOID returnToDash(WCHAR* msg);

//VOID XNotifyUI(DWORD dwType, DWORD dwUserIndex, DWORD dwPriority, LPCWSTR pwszStringParam, ULONGLONG qwParam);
VOID XNotifyUI_Title(LPCWSTR pwszStringParam);

WORD ReverseWORD(WORD in);
DWORD ReverseDWORD(DWORD in);
IMAGE_SECTION_HEADER* FindNtSection(IMAGE_SECTION_HEADER* Sections, WORD SectionCount, CHAR* SectionName);

WCHAR* strtowchar(const CHAR* str);
CHAR* wchartostr(const WCHAR* str, int mLength);
void fileToBin(const char* fname, char* dPtr, ULONG len);
ULONG fileSize(const char* fname);
void binToFile(const char* fname, PVOID dPtr, ULONG len);

DWORD ReadFlash(DWORD offset, PBYTE buf, DWORD len, DWORD readSize, PDWORD pbRead);

BYTE char2byte(char input);
void RC4(void* Key, int ccKey, void* Data, int ccData);

DWORD resolveFunct(PCHAR modname, DWORD ord);
HRESULT mountPath(std::string &device, std::string &link);
NTSTATUS dvdGetInquiryData(BYTE** resp);

bool Compare(char *str1, char *str2);

DWORD hookExportOrd(PLDR_DATA_TABLE_ENTRY moduleHandle, DWORD ord, DWORD dstFun);

DWORD GetModuleImport(HANDLE HModule, HANDLE HImportedModule, DWORD Ordinal);

HRESULT GenerateModuleHash(BYTE* pbKey, DWORD cbKey, BYTE* pbDigest, DWORD cbDigest);
DWORD ApplyPatches(CHAR* FilePath, const VOID* DefaultPatches = NULL);

PLDR_DATA_TABLE_ENTRY getModuleFromAddr(DWORD LoadAddr);
BOOL WriteFileAlt(PCHAR Path, LPCVOID Buffer, DWORD dwBytesToWrite);
BOOL CWriteFile(const CHAR* FilePath, const VOID* Data, DWORD Size);

DWORD GetModuleImportAddr(HANDLE HModule, HANDLE HImportedModule, DWORD Ordinal);
DWORD _resolve(HMODULE Module, DWORD Export, DWORD off1, DWORD off2);
void addToList(HANDLE hModule);
void removeFromList(HANDLE hModule);

//typedef BOOL(*_XeKeysPkcs1Verify)(const BYTE *pbHash, const BYTE *pbSig, const BYTE *pubKey);
//static _XeKeysPkcs1Verify XeKeysPkcs1Verify = (_XeKeysPkcs1Verify)0x80111550;  //


/*// mobileB and mobileC have xnet data (also in xconfig)
void CMobilesSection::DecryptXnetData(int mobNum)
{
unsigned char hashBuf[0x14];
unsigned char numBuf[4] = {0,0,0,4}; // mobileC.dat, XCONFIG_XNET_MACHINE_ACCOUNT_Drawing category is 4
unsigned char* buf = &m_DecryptedData[4]; // mobileC.dat at 4 byte offset for 0x1EC bytes
if(mobNum == MOBILEB_DAT)
{
numBuf[3] = 0x5; // mobileB.dat, XCONFIG_XNET_PARAMETERS_Drawing category is 0x5
buf = &m_DecryptedData[0x404]; // mobileB.dat at 0x404 byte offset for 0x1EC bytes
}
XeCryptHmacSha(cObfKey, 0x10, &buf[0x14], 8, numBuf, 4, NULL, 0, hashBuf, 0x14);
XeCryptRc4(hashBuf, 0x14, &buf[0x1C], 0x1D0);
XeCryptSha(&buf[0x14], 8, &buf[0x1C], 0x1D0, NULL, 0, hashBuf, 0x14);
if(memcmp(buf, hashBuf, 0x14) != 0)
{
delete []m_DecryptedData;
m_DecryptedData = NULL;
#ifdef _DEBUG
printf("xnet mob %d failed key %d: ", mobNum, numBuf[3]);
for(int i = 0; i < 0x10; i++)
printf("%02x ", cObfKey[i]&0xFF);
printf("\n");
#endif
}
#ifdef _DEBUG
else
{
printf("xnet mob %d decrypted OK\n", mobNum);
}
#endif
}
*/


namespace OPCodes
{
	const DWORD NOP = 0x60000000;
	const DWORD LIR3_0 = 0x38600000;
	const DWORD LIR3_1 = 0x38600001;
	const DWORD BRANCH = 0x48000000;
	const DWORD BRANCHL = 0x48000001;
	const DWORD BRANCHA = 0x48000002;
	const DWORD BLR = 0x4E800020;
}
#define makeBranch(a) (OPCodes::BRANCH | (DWORD)(a & 0x03FFFFFF))
#define makeBranchL(a) (OPCodes::BRANCHL | (DWORD)(a & 0x03FFFFFF))


static const DWORD kCrc32Table[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
}; // kCrc32Table

class Crc32
{
public:
	Crc32() { Reset(); }
	~Crc32() throw() {}
	void Reset() { _crc = (DWORD)~0; }
	void AddData(const BYTE* pData, const DWORD length)
	{
		BYTE* pCur = (BYTE*)pData;
		DWORD remaining = length;
		for (; remaining--; ++pCur)
			_crc = (_crc >> 8) ^ kCrc32Table[(_crc ^ *pCur) & 0xff];
	}
	const DWORD GetCrc32() { return ~_crc; }

private:
	DWORD _crc;
};

#endif