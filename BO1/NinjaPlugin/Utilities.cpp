#include "stdafx.h"
#include "Utilities.h"
#include "Config.h"
#include "Globals.h"
#include "HvPeekPoke.h"

typedef float vec_t;
typedef vec_t vec3_t[3];
#define M_2PI 6.28318530718f
PLIST_ENTRY PsLoadedModuleList = NULL;

float AngleNormalize(float angle)
{
	while (angle < -180) angle += 360;
	while (angle > 180) angle -= 360;
	return angle;
}

void AxisToAngles(Vector3* axis, float* angles)
{
	float	length1;
	float	yaw, pitch, roll = 0.0f;

	if (axis[0][1] == 0.0f && axis[0][0] == 0.0f)
	{
		yaw = 0.0f;
		if (axis[0][2] > 0.0f)
			pitch = 90.0f;
		else
			pitch = 270.0f;
	}
	else
	{
		if (axis[0][0])
			yaw = (atan2(axis[0][1], axis[0][0]) * 180 / 3.14159265358979323846f);
		else if (axis[0][1] > 0.0f)
			yaw = 90.0f;
		else
			yaw = 270.0f;
		if (yaw < 0.0f)
			yaw += 360.0f;

		length1 = sqrt(axis[0][0] * axis[0][0] + axis[0][1] * axis[0][1]);
		pitch = (atan2(axis[0][2], length1) * 180 / 3.14159265358979323846f);
		if (pitch < 0.0f)
			pitch += 360.0f;

		roll = (atan2(axis[1][2], axis[2][2]) * 180 / 3.14159265358979323846f);
		if (roll < 0.0f)
			roll += 360.0f;
	}
	angles[PITCH] = -pitch;
	angles[YAW] = yaw;
	angles[ROLL] = roll;
}

Vector3 VectorToAngles(Vector3 Delta)
{
	FLOAT Forward, Yaw, Pitch;

	if (Delta.x == 0.0f && Delta.y == 0.0f)
	{
		Yaw = 0.0f;
		if (Delta.z > 0.0f)
		{
			Pitch = 90.0f;
		}
		else { Pitch = 270.0f; }
	}
	else
	{
		if (Delta.x != -1.0f && Delta.x != 0.0f)
		{
			Yaw = (FLOAT)(atan2(Delta.y, Delta.x) * 180.0f / 3.14159265358979323846f);
		}
		else if (Delta.y > 0.0f)
		{
			Yaw = 90.0f;
		}
		else { Yaw = 270.0f; }
		if (Yaw < 0.0f)
		{
			Yaw += 360.0f;
		}

		Forward = sqrt((Delta.x * Delta.x) + (Delta.y * Delta.y));
		Pitch = (FLOAT)(atan2(Delta.z, Forward) * 180.0f / 3.14159265358979323846f);
		if (Pitch < 0.0f)
		{
			Pitch += 360.0f;
		}
	}

	return Vector3(-Pitch, Yaw, 0.0f);
}

void AngleVectors(Vector3 angles, Vector3 *forward, Vector3 *right, Vector3 *up) {

	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles.y * (M_2PI / 360.0f);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles.x * (M_2PI / 360.0f);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles.z * (M_2PI / 360.0f);
	sr = sin(angle);
	cr = cos(angle);


	if (forward) {
		forward->x = cp*cy;
		forward->y = cp*sy;
		forward->z = -sp;
	}
	if (right) {
		right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->z = -1 * sr*cp;
	}

	if (up) {
		up->x = (cr*sp*cy + -sr*-sy);
		up->y = (cr*sp*sy + -sr*cy);
		up->z = cr*cp;
	}
}

void VectorToAngle(const float * Vec, float * angles)
{
	float forward, yaw, pitch, PiDiv = (180.0f / XM_PI);

	if (Vec[1] == 0 && Vec[0] == 0)
	{
		yaw = 0;
		if (Vec[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		if (Vec[0])
			yaw = (atan2(Vec[1], Vec[0]) * PiDiv);
		else if (Vec[1] > 0)
			yaw = 90.0f;
		else
			yaw = 270.0f;

		if (yaw < 0.0f)
			yaw += 360.0f;

		forward = sqrt((Vec[0] * Vec[0]) + (Vec[1] * Vec[1]));
		pitch = (atan2(Vec[2], forward) * PiDiv);

		if (pitch < 0.0f)
			pitch += 360;
	}

	angles[0] = 360.0f - pitch;
	angles[1] = yaw;
	angles[2] = 0.0f;
	for (int i = 0; i < 2; i++) {
		if (angles[i] > 360.0f)
			angles[i] -= 360.0f;
		if (angles[i] < 0.0f)
			angles[i] += 360.0f;
	}
}

// Resolve set memory
pDmSetMemory DevSetMemory = NULL;

extern BOOL RunningFromUSB;

#if ( defined(DBG_PRINT) || defined(_DEBUG) )

CRITICAL_SECTION dbgLock;
BOOL dbgInit = FALSE;

VOID DbgPrint(const CHAR* strFormat, ...)
{
	if (dbgInit == FALSE)
	{
		RtlInitializeCriticalSection(&dbgLock);
		dbgInit = TRUE;
	}

	CHAR buffer[1000];

	va_list pArgList;
	va_start(pArgList, strFormat);
	RtlVsnprintf(buffer, 1000, strFormat, pArgList);
	va_end(pArgList);

	printf("[PLUGIN] %s\r\n", buffer);

	RtlEnterCriticalSection(&dbgLock);
	ofstream writeLog;
	writeLog.open("HDD:\\Engine.log", ofstream::app);
	if (writeLog.is_open())
	{
		writeLog.write(buffer, strlen(buffer));
		writeLog.write("\r\n", 2);
	}
	writeLog.close();
	RtlLeaveCriticalSection(&dbgLock);
}
#endif

#ifdef DBG_PRINT
void printBytes(PBYTE bytes, DWORD len) {
	if (!bytes) return;
	for (DWORD i = 0; i<len; i++) {
		if (i % 16 == 0 && i != 0) printf("\n");
		printf("%02X", bytes[i]);
	}
	printf("\n");
}
#else
void printBytes(PBYTE bytes, DWORD len) {
}
#endif

#pragma optimize("", off)
VOID __declspec(naked) GLPR(VOID)
{
	__asm
	{
		std     r14, -0x98(sp)
		std     r15, -0x90(sp)
			std     r16, -0x88(sp)
			std     r17, -0x80(sp)
			std     r18, -0x78(sp)
			std     r19, -0x70(sp)
			std     r20, -0x68(sp)
			std     r21, -0x60(sp)
			std     r22, -0x58(sp)
			std     r23, -0x50(sp)
			std     r24, -0x48(sp)
			std     r25, -0x40(sp)
			std     r26, -0x38(sp)
			std     r27, -0x30(sp)
			std     r28, -0x28(sp)
			std     r29, -0x20(sp)
			std     r30, -0x18(sp)
			std     r31, -0x10(sp)
			stw     r12, -0x8(sp)
			blr
	}
}

VOID PatchInJump(DWORD* Address, DWORD Destination, BOOL Linked)
{
	DWORD val = 0x3D600000 + ((Destination >> 16) & 0xFFFF);
	SetMemory(Address, &val, 4);
	if (Destination & 0x8000)
	{
		val = Address[0] + 1;
		SetMemory(Address, &val, 4);
	}
	val = 0x396B0000 + (Destination & 0xFFFF);
	SetMemory(Address + 1, &val, 4);
	val = 0x7D6903A6;
	SetMemory(Address + 2, &val, 4);
	val = Linked ? 0x4E800421 : 0x4E800420;
	SetMemory(Address + 3, &val, 4);
}


VOID PatchInJump(DWORD* Address, void* Dest, BOOL Linked) {

	DWORD Bytes[4];
	DWORD Destination = (DWORD)Dest;

	Bytes[0] = 0x3C000000 + ((Destination >> 16) & 0xFFFF);// lis 	%r0, dest>>16
	Bytes[1] = 0x60000000 + (Destination & 0xFFFF); // ori	%r0, %r0, dest&0xFFFF
	Bytes[2] = 0x7C0903A6; // mtctr	%r0
	Bytes[3] = 0x4E800420; // bctr

	if (Linked)
		Bytes[3] += 1; // bctrl

					   //memcpy(Address, Bytes, 0x10);
	*(__int64 *)((DWORD)Address + 0) = *(__int64 *)&Bytes[0];
	*(__int64 *)((DWORD)Address + 8) = *(__int64 *)&Bytes[2];
	__dcbst(0, Address);
	__sync();
	__isync();
}

/*VOID PatchInJump(DWORD* Address, void* Dest, BOOL Linked) {

DWORD Bytes[4];
DWORD Destination = (DWORD)Dest;

Bytes[0] = 0x3D600000 + ((Destination >> 16) & 0xFFFF);// lis 	%r11, dest>>16

if (Destination & 0x8000) // If bit 16 is 1
Bytes[0] += 1;

Bytes[1] = 0x396B0000 + (Destination & 0xFFFF); // addi	%r11, %r11, dest&0xFFFF
Bytes[2] = 0x7D6903A6; // mtctr	%r11

Bytes[3] = 0x4E800420; // bctr

if (Linked)
Bytes[3] += 1; // bctrl

*(__int64 *)((DWORD)Address + 0) = *(__int64 *)&Bytes[0];
*(__int64 *)((DWORD)Address + 8) = *(__int64 *)&Bytes[2];
//memcpy(Address, Bytes, 0x10);
__dcbst(0, Address);
__sync();
__isync();
}*/

#pragma optimize("", on)

DWORD RelinkGPLR(DWORD SFSOffset, PDWORD SaveStubAddress, PDWORD OriginalAddress)
{
	DWORD Instruction = 0, Replacing;
	PDWORD Saver = (PDWORD)GLPR;
	if (SFSOffset & 0x2000000)
	{
		SFSOffset = SFSOffset | 0xFC000000;
	}
	Replacing = OriginalAddress[SFSOffset / 4];
	for (int i = 0; i < 20; i++)
	{
		if (Replacing == Saver[i])
		{
			DWORD NewOffset = (DWORD)&Saver[i] - (DWORD)SaveStubAddress;
			Instruction = 0x48000001 | (NewOffset & 0x3FFFFFC);
		}
	}
	return Instruction;
}

DWORD resolveFunct(PCHAR modname, DWORD ord)
{
	DWORD ptr2 = 0;
	HANDLE hand;
	if (NT_SUCCESS(XexGetModuleHandle(modname, &hand)))
	{
		XexGetProcedureAddress(hand, ord, &ptr2);
	}
	return ptr2; // function not found
}

void __declspec(naked) GLPR_FUN(void)
{
	__asm {
		std     r14, -0x98(sp)
			std     r15, -0x90(sp)
			std     r16, -0x88(sp)
			std     r17, -0x80(sp)
			std     r18, -0x78(sp)
			std     r19, -0x70(sp)
			std     r20, -0x68(sp)
			std     r21, -0x60(sp)
			std     r22, -0x58(sp)
			std     r23, -0x50(sp)
			std     r24, -0x48(sp)
			std     r25, -0x40(sp)
			std     r26, -0x38(sp)
			std     r27, -0x30(sp)
			std     r28, -0x28(sp)
			std     r29, -0x20(sp)
			std     r30, -0x18(sp)
			std     r31, -0x10(sp)
			stw     r12, -0x8(sp)
			blr
	}
}

DWORD relinkGPLR(int offset, PDWORD saveStubAddr, PDWORD orgAddr)
{
	DWORD inst = 0, repl;
	int i;
	PDWORD saver = (PDWORD)GLPR_FUN;
	// if the msb is set in the instruction, set the rest of the bits to make the int negative
	if (offset & 0x2000000)
		offset = offset | 0xFC000000;
	//DbgPrint("frame save offset: %08x\n", offset);
	repl = orgAddr[offset / 4];
	//DbgPrint("replacing %08x\n", repl);
	for (i = 0; i < 20; i++)
	{
		if (repl == saver[i])
		{
			int newOffset = (int)&saver[i] - (int)saveStubAddr;
			inst = 0x48000001 | (newOffset & 0x3FFFFFC);
			//DbgPrint("saver addr: %08x savestubaddr: %08x\n", &saver[i], saveStubAddr);
		}
	}
	//DbgPrint("new instruction: %08x\n", inst);
	return inst;
}

void patchInJump(PDWORD addr, DWORD dest, BOOL linked)
{
	if (dest & 0x8000) // If bit 16 is 1
		addr[0] = 0x3D600000 + (((dest >> 16) & 0xFFFF) + 1); // lis %r11, dest>>16 + 1
	else
		addr[0] = 0x3D600000 + ((dest >> 16) & 0xFFFF); // lis %r11, dest>>16

	addr[1] = 0x396B0000 + (dest & 0xFFFF); // addi %r11, %r11, dest&0xFFFF
	addr[2] = 0x7D6903A6; // mtctr %r11

	if (linked)
		addr[3] = 0x4E800421; // bctrl
	else
		addr[3] = 0x4E800420; // bctr
	doSync(addr);
}

void HookFunctionStart(PDWORD addr, PDWORD saveStub, DWORD dest) {

	hookFunctionStart(addr, saveStub, 0, dest);
}

void hookFunctionStart(PDWORD addr, PDWORD saveStub, PDWORD oldData, DWORD dest)
{
	if ((saveStub != NULL) && (addr != NULL))
	{
		int i;
		DWORD addrReloc = (DWORD)(&addr[4]);// replacing 4 instructions with a jump, this is the stub return address
											//DbgPrint("hooking addr: %08x savestub: %08x dest: %08x addreloc: %08x\n", addr, saveStub, dest, addrReloc);
											// build the stub
											// make a jump to go to the original function start+4 instructions
		if (addrReloc & 0x8000) // If bit 16 is 1
			saveStub[0] = 0x3D600000 + (((addrReloc >> 16) & 0xFFFF) + 1); // lis %r11, dest>>16 + 1
		else
			saveStub[0] = 0x3D600000 + ((addrReloc >> 16) & 0xFFFF); // lis %r11, dest>>16

		saveStub[1] = 0x396B0000 + (addrReloc & 0xFFFF); // addi %r11, %r11, dest&0xFFFF
		saveStub[2] = 0x7D6903A6; // mtctr %r11
								  // instructions [3] through [6] are replaced with the original instructions from the function hook
								  // copy original instructions over, relink stack frame saves to local ones
		if (oldData != NULL)
		{
			for (i = 0; i<4; i++)
				oldData[i] = addr[i];
		}
		for (i = 0; i<4; i++)
		{
			if ((addr[i] & 0x48000003) == 0x48000001) // branch with link
			{
				//DbgPrint("relink %08x\n", addr[i]);
				saveStub[i + 3] = relinkGPLR((addr[i] & ~0x48000003), &saveStub[i + 3], &addr[i]);
			}
			else
			{
				//DbgPrint("copy %08x\n", addr[i]);
				saveStub[i + 3] = addr[i];
			}
		}
		saveStub[7] = 0x4E800420; // bctr
		doSync(saveStub);
		//DbgPrint("savestub:\n");
		//for(i = 0; i < 8; i++)
		//{
		//	DbgPrint("PatchDword(0x%08x, 0x%08x);\n", &saveStub[i], saveStub[i]);
		//}
		// patch the actual function to jump to our replaced one
		patchInJump(addr, dest, FALSE);
	}
}

PDWORD hookFunctionStartOrd(char* modName, DWORD ord, PDWORD saveStub, PDWORD oldData, DWORD dest)
{
	PDWORD addr = (PDWORD)resolveFunct(modName, ord);
	if (addr != NULL)
	{
		hookFunctionStart(addr, saveStub, oldData, dest);
	}
	return addr;
}

VOID PatchInBranch(DWORD* Address, DWORD Destination, BOOL Linked)
{
	DWORD val = (0x48000000 + ((Destination - (DWORD)Address) & 0x3FFFFFF));
	SetMemory(Address, &val, 4);
	if (Linked)
	{
		val = Address[0] + 1;
		SetMemory(Address, &val, 4);
	}
}


typedef struct _LDR_DATA_TABLE_ENTRY_2 {
	LIST_ENTRY     InLoadOrderLinks;
	LIST_ENTRY     InClosureOrderLinks;
	LIST_ENTRY     InInitializationOrderLinks;
	VOID*          NtHeadersBase;
	VOID*          ImageBase;
	DWORD          SizeOfNtImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	DWORD          Flags;
	DWORD          SizeOfFullImage;
	VOID*          EntryPoint;
	WORD           LoadCount;
	WORD           ModuleIndex;
	VOID*          DllBaseOriginal;
	DWORD          CheckSum;
	DWORD          ModuleLoadFlags;
	DWORD          TimeDateStamp;
	VOID*          LoadedImports;
	VOID*          XexHeaderBase;
	union {
		ANSI_STRING               LoadFileName;
		struct {
			_LDR_DATA_TABLE_ENTRY* ClosureRoot;
			_LDR_DATA_TABLE_ENTRY* TraversalParent;
		} asEntry;
	};
} LDR_DATA_TABLE_ENTRY_2, *PLDR_DATA_TABLE_ENTRY_2;

typedef struct _XEX_IMPORT_TABLE_2 {
	DWORD                 TableSize;
	HV_IMAGE_IMPORT_TABLE ImportTable;
} XEX_IMPORT_TABLE_2, *PXEX_IMPORT_TABLE_2;

DWORD GetModuleImport(HANDLE HModule, HANDLE HImportedModule, DWORD Ordinal) {

	PLDR_DATA_TABLE_ENTRY_2 Module = (PLDR_DATA_TABLE_ENTRY_2)HModule;

	DWORD address = (DWORD)GetProcAddress((HMODULE)HImportedModule, (LPCSTR)Ordinal);

	if (address == NULL || HModule == NULL)
		return 0;

	// Get our header field from this module
	VOID* headerBase = Module->XexHeaderBase;
	PXEX_IMPORT_DESCRIPTOR importDesc = (PXEX_IMPORT_DESCRIPTOR)
		RtlImageXexHeaderField(headerBase, 0x000103FF);
	if (importDesc == NULL)
		return 0;

	// Get our string table position
	CHAR* stringTable = (CHAR*)(importDesc + 1);

	// Get our first entry
	PXEX_IMPORT_TABLE_2 importTable = (PXEX_IMPORT_TABLE_2)
		(stringTable + importDesc->NameTableSize);

	// Loop through our table
	for (DWORD x = 0; x < importDesc->ModuleCount; x++) {

		// Go through and search all addresses for something that links
		DWORD* importAdd = (DWORD*)(importTable + 1);
		for (DWORD y = 0; y < importTable->ImportTable.ImportCount; y++) {

			// Check the address of this import
			DWORD value = *((DWORD*)importAdd[y]);
			if (value == address) {

				return importAdd[y + 1];
			}
		}

		// Goto the next table
		importTable = (PXEX_IMPORT_TABLE_2)(((BYTE*)importTable) +
			importTable->TableSize);
	}

	return 0;
}

FARPROC ResolveFunction(CHAR* ModuleName, DWORD Ordinal)
{
	HMODULE mHandle = GetModuleHandle(ModuleName);
	if (mHandle == NULL) {
		DbgPrint("ResolveFunction - GetModuleHandle returned NULL on %s", ModuleName);
		return NULL;
	}
	return GetProcAddress(mHandle, (LPCSTR)Ordinal);
}

int ResolveFunction_(CHAR* ModuleName, DWORD Ordinal) {

	// Get our module handle
	HMODULE mHandle = GetModuleHandle(ModuleName);
	if (mHandle == NULL)
		return NULL;

	// Return our address
	return (int)GetProcAddress(mHandle, (LPCSTR)Ordinal);
}

DWORD PatchModuleImport(CHAR* Module, CHAR* ImportedModuleName, DWORD Ordinal, DWORD PatchAddress)
{
	LDR_DATA_TABLE_ENTRY* moduleHandle = (LDR_DATA_TABLE_ENTRY*)GetModuleHandle(Module);
	return (moduleHandle == NULL) ? S_FALSE : PatchModuleImport(moduleHandle, ImportedModuleName, Ordinal, PatchAddress);
}

DWORD PatchModuleImport(PLDR_DATA_TABLE_ENTRY Module, CHAR* ImportedModuleName, DWORD Ordinal, DWORD PatchAddress)
{
	// First resolve this imports address
	DWORD address = (DWORD)ResolveFunction(ImportedModuleName, Ordinal);
	if (address == NULL) {
		DbgPrint("PatchModuleImport - Failed to resolve function %u", Ordinal);
		return S_FALSE;
	}

	// Get our header field from this module
	VOID* headerBase = Module->XexHeaderBase;
	PXEX_IMPORT_DESCRIPTOR importDesc = (PXEX_IMPORT_DESCRIPTOR)RtlImageXexHeaderField(headerBase, 0x000103FF);
	if (importDesc == NULL)
		return S_FALSE;

	// Our result
	DWORD result = 2; // No occurances patched

					  // Get our string table position
	CHAR* stringTable = (CHAR*)(importDesc + 1);

	// Get our first entry
	XEX_IMPORT_TABLE_ORG* importTable = (XEX_IMPORT_TABLE_ORG*)(stringTable + importDesc->NameTableSize);

	// Loop through our table
	for (DWORD x = 0; x < importDesc->ModuleCount; x++)
	{
		// Go through and search all addresses for something that links
		DWORD* importAdd = (DWORD*)(importTable + 1);
		for (DWORD y = 0; y < importTable->ImportTable.ImportCount; y++)
		{
			// Check the address of this import
			DWORD value = *((DWORD*)importAdd[y]);
			if (value == address)
			{
				// We found a matching address address
				SetMemory((DWORD*)importAdd[y], &PatchAddress, 4);
				DWORD newCode[4];
				PatchInJump(newCode, PatchAddress, FALSE);
				SetMemory((DWORD*)importAdd[y + 1], newCode, 16);

				// We patched at least one occurence
				result = S_OK;
			}
		}

		// Goto the next table
		importTable = (XEX_IMPORT_TABLE_ORG*)(((BYTE*)importTable) + importTable->TableSize);
	}

	// Return our result
	return result;
}

DWORD GetModuleImportAddr(HANDLE HModule, HANDLE HImportedModule, DWORD Ordinal) {

	PLDR_DATA_TABLE_ENTRY_2 Module = (PLDR_DATA_TABLE_ENTRY_2)HModule;

	DWORD address = (DWORD)GetProcAddress((HMODULE)HImportedModule, (LPCSTR)Ordinal);

	if (address == NULL || HModule == NULL)
		return 0;

	// Get our header field from this module
	VOID* headerBase = Module->XexHeaderBase;
	PXEX_IMPORT_DESCRIPTOR importDesc = (PXEX_IMPORT_DESCRIPTOR)
		RtlImageXexHeaderField(headerBase, 0x000103FF);
	if (importDesc == NULL)
		return 0;

	// Get our string table position
	CHAR* stringTable = (CHAR*)(importDesc + 1);

	// Get our first entry
	PXEX_IMPORT_TABLE_2 importTable = (PXEX_IMPORT_TABLE_2)
		(stringTable + importDesc->NameTableSize);

	// Loop through our table
	for (DWORD x = 0; x < importDesc->ModuleCount; x++) {

		// Go through and search all addresses for something that links
		DWORD* importAdd = (DWORD*)(importTable + 1);
		for (DWORD y = 0; y < importTable->ImportTable.ImportCount; y++) {

			// Check the address of this import
			DWORD value = *((DWORD*)importAdd[y]);
			if (value == address) {
				return (DWORD)(&importAdd[y + 1]);
				//return importAdd[y + 1];
			}
		}

		// Goto the next table
		importTable = (PXEX_IMPORT_TABLE_2)(((BYTE*)importTable) +
			importTable->TableSize);
	}

	return 0;
}

DWORD GetModuleImportAddress(LDR_DATA_TABLE_ENTRY* moduleHandle, CHAR* ImportedModuleName, DWORD Ordinal)
{
	DWORD address = (DWORD)ResolveFunction(ImportedModuleName, Ordinal);
	if (address == NULL)
		return S_FALSE;

	// Get our header field from this module
	VOID* headerBase = moduleHandle->XexHeaderBase;
	PXEX_IMPORT_DESCRIPTOR importDesc = (PXEX_IMPORT_DESCRIPTOR)RtlImageXexHeaderField(headerBase, 0x000103FF);
	if (importDesc == NULL)
		return S_FALSE;

	// Our result
	DWORD result = 2; // No occurances found

					  // Get our string table position
	CHAR* stringTable = (CHAR*)(importDesc + 1);

	// Get our first entry
	XEX_IMPORT_TABLE_ORG* importTable = (XEX_IMPORT_TABLE_ORG*)(stringTable + importDesc->NameTableSize);

	// Loop through our table
	for (DWORD x = 0; x < importDesc->ModuleCount; x++)
	{
		// Go through and search all addresses for something that links
		DWORD* importAdd = (DWORD*)(importTable + 1);
		for (DWORD y = 0; y < importTable->ImportTable.ImportCount; y++)
		{
			// Check the address of this import
			DWORD value = *((DWORD*)importAdd[y]);
			if (value == address)
			{
				if (result != 2)
				{
					HalReturnToFirmware(HalPowerDownRoutine);
					return S_FALSE;
				}

				// We found a matching address address
				result = importAdd[y];
			}
		}

		// Goto the next table
		importTable = (XEX_IMPORT_TABLE_ORG*)(((BYTE*)importTable) + importTable->TableSize);
	}

	// Return our result
	return result;
}

DWORD GetModuleImportCallAddress(LDR_DATA_TABLE_ENTRY* moduleHandle, CHAR* ImportedModuleName, DWORD Ordinal)
{
	DWORD address = (DWORD)ResolveFunction(ImportedModuleName, Ordinal);
	if (address == NULL)
		return S_FALSE;

	// Get our header field from this module
	VOID* headerBase = moduleHandle->XexHeaderBase;
	PXEX_IMPORT_DESCRIPTOR importDesc = (PXEX_IMPORT_DESCRIPTOR)RtlImageXexHeaderField(headerBase, 0x000103FF);
	if (importDesc == NULL)
		return S_FALSE;

	// Our result
	DWORD result = 2; // No occurances found

					  // Get our string table position
	CHAR* stringTable = (CHAR*)(importDesc + 1);

	// Get our first entry
	XEX_IMPORT_TABLE_ORG* importTable = (XEX_IMPORT_TABLE_ORG*)(stringTable + importDesc->NameTableSize);

	// Loop through our table
	for (DWORD x = 0; x < importDesc->ModuleCount; x++)
	{
		// Go through and search all addresses for something that links
		DWORD* importAdd = (DWORD*)(importTable + 1);
		for (DWORD y = 0; y < importTable->ImportTable.ImportCount; y++)
		{
			// Check the address of this import
			DWORD value = *((DWORD*)importAdd[y]);
			if (value == address)
			{
				if (result != 2)
				{
					HalReturnToFirmware(HalPowerDownRoutine);
					return S_FALSE;
				}

				// We found a matching address address
				result = importAdd[y + 1];
			}
		}

		// Goto the next table
		importTable = (XEX_IMPORT_TABLE_ORG*)(((BYTE*)importTable) + importTable->TableSize);
	}

	// Return our result
	return result;
}

HRESULT mountPath(std::string &device, std::string &link)
{
	DbgPrint("%s to %s", device.c_str(), link.c_str());

	STRING deviceName;
	STRING linkName;
	RtlInitAnsiString(&deviceName, device.c_str());
	RtlInitAnsiString(&linkName, link.c_str());
	ObDeleteSymbolicLink(&linkName);
	return ObCreateSymbolicLink(&linkName, &deviceName);
}

static STRING CdRomString = { 0xE, 0xF, "\\Device\\Cdrom0" };



#define DVD_INQUIRY_RESPONSE_SIZE	0x24

NTSTATUS dvdGetInquiryData(BYTE** resp)
{
	PDEVICE_OBJECT pdev;
	NTSTATUS ret = STATUS_SUCCESS; // returned from function
	NTSTATUS rret; // placed into response struct
	rret = ObReferenceObjectByName(&CdRomString, 0, 0, 0, (PVOID*)&pdev);

	DWORD respSize = DVD_INQUIRY_RESPONSE_SIZE;
	*resp = (PBYTE)XPhysicalAlloc(0x1000, MAXULONG_PTR, 128, MEM_LARGE_PAGES | PAGE_READWRITE);
	if (NT_SUCCESS(rret))
	{
		SCSI_PASS_THROUGH_DIRECT cmd;
		DbgPrint("dvd object referenced ok, making ioctl\n");
		memset(&cmd, 0, sizeof(SCSI_PASS_THROUGH_DIRECT));
		memset(*resp, 0xFF, respSize);
		cmd.DataBuffer = *resp;
		cmd.DataTransferLength = sizeof(SCSI_PASS_THROUGH_DIRECT);
		cmd.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
		cmd.DataIn = 1;
		cmd.Cdb[0] = SCSI_CMD_FORMAT_INQUIRY;  //0x12
		cmd.Cdb[4] = sizeof(SCSI_PASS_THROUGH_DIRECT);
		cmd.Cdb[5] = 0xC0;
		rret = IoSynchronousDeviceIoControlRequest(0x4D014, pdev, &cmd, sizeof(SCSI_PASS_THROUGH_DIRECT), NULL, 0, NULL);
		//DbgPrint("ioctl returned 0x%x\n", rret);
		ObDereferenceObject(pdev);
	}
	else
	{
		//DbgPrint("dvd object ref fail 0x%x\n", rret);
		ret = E_FAIL;
	}
	//resp->DvdInqResp = rret;
	return ret;
}

HRESULT CreateSymbolicLink(CHAR* szDrive, CHAR* szDeviceName, BOOL System)
{
	// Setup our path
	CHAR szDestinationDrive[MAX_PATH];
	RtlSnprintf(szDestinationDrive, MAX_PATH, System ? "\\System??\\%s" : "\\??\\%s", szDrive);

	// Setup our strings
	ANSI_STRING linkname, devicename;
	RtlInitAnsiString(&linkname, szDestinationDrive);
	RtlInitAnsiString(&devicename, szDeviceName);

	// Create finally
	NTSTATUS status = ObCreateSymbolicLink(&linkname, &devicename);
	return (status >= 0) ? S_OK : S_FALSE;
}

HRESULT DeleteSymbolicLink(CHAR* szDrive, BOOL System)
{
	// Setup our path
	CHAR szDestinationDrive[MAX_PATH];
	RtlSnprintf(szDestinationDrive, MAX_PATH, System ? "\\System??\\%s" : "\\??\\%s", szDrive);

	// Setup our string
	ANSI_STRING linkname;
	RtlInitAnsiString(&linkname, szDestinationDrive);

	// Delete finally
	NTSTATUS status = ObDeleteSymbolicLink(&linkname);
	return (status >= 0) ? S_OK : S_FALSE;
}

BOOL IsBufferEmpty(BYTE* Buffer, DWORD Length)
{
	for (DWORD i = 0; i < Length; i++)
	{
		if (Buffer[i] != 0) return FALSE;
	}
	return TRUE;
}


BOOL FileExists(LPCSTR lpFileName)
{
	// Try and get the file attributes
	if (GetFileAttributes(lpFileName) == -1)
	{
		DWORD lastError = GetLastError();
		if (lastError == ERROR_FILE_NOT_FOUND || lastError == ERROR_PATH_NOT_FOUND)
			return FALSE;
	}

	// The file must exist if we got this far...
	return TRUE;
}

static BOOL pfShow = (BOOL)0xDEADBEEF;  // flag to init values
static BOOL pfShowMovie;
static BOOL pfPlaySound;
static BOOL pfShowIPTV;

VOID ToggleNotifications(BOOL on)
{
	if ((int)pfShow == 0xDEADBEEF) // init values
		XNotifyUIGetOptions(&pfShow, &pfShowMovie, &pfPlaySound, &pfShowIPTV);

	if (!on)
	{
		XNotifyUISetOptions(pfShow, pfShowMovie, pfPlaySound, pfShowIPTV);  // set back original values
	}
	else
	{
		XNotifyUISetOptions(true, true, true, true);  // turn on notifications so our messages always show
	}
}

VOID XNotifyUI_Title(LPCWSTR pwszStringParam)
{
	// Used for title threads

	// Turn on notifications
	ToggleNotifications(TRUE);

	// Send message
	XNotifyQueueUI(XNOTIFYUI_TYPE_PREFERRED_REVIEW, XUSER_INDEX_ANY, XNOTIFYUI_PRIORITY_HIGH, (PWCHAR)pwszStringParam, NULL);

	// Turn notifications to their default state
	ToggleNotifications(FALSE);
}


VOID XNotifyUI_Title_ThreadStarter(VOID* params)
{
	DWORD* p = (DWORD*)params;
	LPCWSTR msg = (LPCWSTR)p[0];

	if (p[1])
		Sleep(p[1]);

	XNotifyUI_Title(msg);
	Sleep(3000);
	delete[] params;
}

// Use this one!
VOID XNotifyUI(LPCWSTR pwszStringParam, DWORD Delayms)
{
	// Turn on notifications
	ToggleNotifications(TRUE);

	// Send message
	XNotifyQueueUI(XNOTIFYUI_TYPE_PREFERRED_REVIEW, XUSER_INDEX_ANY, XNOTIFYUI_PRIORITY_HIGH, (PWCHAR)pwszStringParam, NULL);

	// Turn notifications to their default state
	ToggleNotifications(FALSE);
	// //RtlEnterCriticalSection(&CritSections.xuiNotify);

	// if (KeGetCurrentProcessType() != 1)
	// {
		// DWORD *params = new DWORD[2];
		// params[0] = (DWORD)pwszStringParam;
		// params[1] = Delayms;
		// DWORD dwThreadId;
		// HANDLE th = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)XNotifyUI_Title_ThreadStarter, (LPVOID)params, CREATE_SUSPENDED, &dwThreadId);
		// XSetThreadProcessor(th, 4);
		// SetThreadPriority(th, THREAD_PRIORITY_TIME_CRITICAL);
		// ResumeThread(th);
		// CloseHandle(th);
		// return;
	// }

	// if (Delayms)
		// Sleep(Delayms);
	// XNotifyUI_Title(pwszStringParam);
	// //RtlLeaveCriticalSection(&CritSections.xuiNotify);
}

HRESULT SetMemory(VOID* Destination, VOID* Source, DWORD Length)
{
	// Try to resolve our function
	if (DevSetMemory == NULL && isDevkit)
	{
		DevSetMemory = (pDmSetMemory)ResolveFunction("xbdm.xex", 40);
	}

	// Now lets try to set our memory
	if (DevSetMemory == NULL)
	{
		memcpy(Destination, Source, Length);
		return ERROR_SUCCESS;
	}
	else if (isDevkit)
	{
		if (DevSetMemory(Destination, Length, Source, NULL) == MAKE_HRESULT(0, 0x2DA, 0))
		{
			return ERROR_SUCCESS;
		}
	}

	// We have a problem...
	return E_FAIL;
}

VOID ShowMessageBox(LPCWSTR title, LPCWSTR msg, DWORD dwFlags)
{
	LPCWSTR* text;
	text = new LPCWSTR[1];
	text[0] = L"Okay";
	PXOVERLAPPED over = new XOVERLAPPED();
	memset(over, 0, sizeof(XOVERLAPPED));
	PMESSAGEBOX_RESULT result = new MESSAGEBOX_RESULT();
	memset(result, 0, sizeof(MESSAGEBOX_RESULT));

	WCHAR retSeq[] = L"\\r\\n";
	int len = lstrlenW(msg);
	WCHAR *str = new WCHAR[len + 1];
	lstrcpyW(str, msg);
	for (int i = 0; i <= len + 4; i += 1) {
		if ((*(BYTE*)&str[i]) + 1 == 0)
			break;

		if (memcmp(&str[i], retSeq, 8) == 0) {
			str[i] = '\r';
			str[i + 1] = '\n';
			memcpy((void*)(&str[i + 1] + 1), &str[i + 3] + 1, ((len * 2) - (i * 2)) + 2);
		}
	}

	while (XShowMessageBoxUI(0, title, str, 1, text, 0, dwFlags, result, over) == ERROR_ACCESS_DENIED)
		free(str);
	XINPUT_STATE mars;
	XInputGetState(0, &mars);

	while (!XHasOverlappedIoCompleted(over) && (!(result->dwButtonPressed & XINPUT_GAMEPAD_B) & !(mars.Gamepad.wButtons & XINPUT_GAMEPAD_START))) {
		XInputGetState(0, &mars);
		Sleep(500);
	}
	Sleep(5000);
}

BYTE char2byte(char input)
{
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	if (input >= 'a' && input <= 'f')
		return input - 'a' + 10;
	return 0;
}

WORD ReverseWORD(WORD in)
{
	WORD ni = 0;

	DWORD size = sizeof(WORD);

	DWORD c = 0;
	for (; c < (size * 8); ++c)
	{
		int olp = ((size - (c / 8) - 1) * 8) + c % 8;
		if ((1 << olp) & in)
		{
			ni |= 1 << c;
		}
	}
	return ni;
}

DWORD ReverseDWORD(DWORD in)
{
	DWORD ni = 0;

	DWORD size = sizeof(DWORD);

	DWORD c = 0;
	for (; c < (size * 8); ++c)
	{
		int olp = ((size - (c / 8) - 1) * 8) + c % 8;
		if ((1 << olp) & in)
		{
			ni |= 1 << c;
		}
	}
	return ni;
}

IMAGE_SECTION_HEADER* FindNtSection(IMAGE_SECTION_HEADER* Sections, WORD SectionCount, CHAR* SectionName)
{
	// Go through and search for our section
	for (WORD x = 0; x < SectionCount; x++)
	{
		if (strcmp((CHAR*)Sections[x].Name, SectionName) == 0)
			return &Sections[x];
	}

	return NULL;
}

WCHAR* strtowchar(const CHAR* str)
{
	WCHAR* retVal;

	int Length = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	retVal = new WCHAR[Length + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, retVal, Length);

	return retVal;
}

CHAR* wchartostr(const WCHAR* str, int mLength)
{
	int Length = WideCharToMultiByte(CP_UTF8, 0, str, mLength, NULL, 0, NULL, NULL);
	CHAR* retVal = new char[Length + 1];
	WideCharToMultiByte(CP_UTF8, 0, str, mLength, retVal, Length, NULL, NULL);

	return retVal;
}


VOID launchSysThread2(LPTHREAD_START_ROUTINE func, LPVOID parm) {
	HANDLE hThread;
	DWORD dwThreadId;
	//ExCreateThread(&hThread, 0, &dwThreadId, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)func, parm, EX_CREATE_FLAG_SUSPENDED | EX_CREATE_FLAG_SYSTEM | EX_CREATE_FLAG_HIDDEN | EX_CREATE_FLAG_CORE4);
	ExCreateThread(&hThread, 0, &dwThreadId, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)func, parm, 0x2);
	XSetThreadProcessor(hThread, 4);
	ResumeThread(hThread);
	CloseHandle(hThread);
}

PLDR_DATA_TABLE_ENTRY getModuleFromAddr(DWORD LoadAddr) {
	PLDR_DATA_TABLE_ENTRY Ninja = NULL;
	XexPcToFileHeader((PVOID)LoadAddr, &Ninja);
	if (Ninja == NULL)
		return NULL;
	return Ninja;
}

VOID returnToDashThread(VOID) {
	Sleep(3000);
	XSetLaunchData(NULL, 0);
	XamLoaderLaunchTitleEx(XLAUNCH_KEYWORD_DEFAULT_APP, NULL, NULL, 0);
}

bool Compare(char *str1, char *str2)
{
	if (str1 != NULL && str2 != NULL)
		if (strlen(str1) == strlen(str2))
			if (_strnicmp(str1, str2, strlen(str1)) == 0)
				return true;
	return false;
}

VOID returnToDash(WCHAR* msg) {
	if (KeGetCurrentProcessType() != PROC_USER)
	{
		HANDLE hThread;
		DWORD dwThreadId;
		hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)returnToDashThread, 0, CREATE_SUSPENDED, &dwThreadId);
		XSetThreadProcessor(hThread, 4);
		SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
		ResumeThread(hThread);
		CloseHandle(hThread);
	}
	else {
		returnToDashThread();
	}
	if (msg) launchSysMsg(msg, 300);
	Sleep(500);
}

static WCHAR sysMsgBuffer[100];
VOID sysMsgThreadDelay(WCHAR* msg) {
	Sleep(5000);
	XNotifyUI(msg);
}

VOID sysMsgThread(WCHAR* msg) {
	XNotifyUI(msg);
}

VOID launchSysMsg(WCHAR* msg, int delay) {
	memcpy(sysMsgBuffer, msg, (wcslen(msg)*sizeof(WCHAR)) + 2);
	HANDLE hThread;
	DWORD dwThreadId;
	if (delay != 60000 && delay>0) {
		Sleep(delay);
		ExCreateThread(&hThread, 0, &dwThreadId, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)sysMsgThread, sysMsgBuffer, 0x2);
	}
	else
		ExCreateThread(&hThread, 0, &dwThreadId, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)sysMsgThreadDelay, sysMsgBuffer, 0x2);

	XSetThreadProcessor(hThread, 4);
	ResumeThread(hThread);
	CloseHandle(hThread);
}

void fileToBin(const char* fname, char* dPtr, ULONG len)
{
	FILE* fp;
	fopen_s(&fp, fname, "rb");
	fread(dPtr, len, 1, fp);
	fclose(fp);
}


ULONG fileSize(const char* fname) {
	FILE* fp;
	fopen_s(&fp, fname, "rb");
	if (!fp)return 0;
	long lSize;
	fseek(fp, 0, SEEK_END);
	lSize = ftell(fp);
	rewind(fp);
	fclose(fp);
	return (ULONG)lSize;
}

void binToFile(const char* fname, PVOID dPtr, ULONG len)
{
	FILE* fp;
	fopen_s(&fp, fname, "wb");
	fwrite(dPtr, len, 1, fp);
	fflush(fp);
	fclose(fp);
}

DWORD ReadFlash(DWORD offset, PBYTE buf, DWORD len, DWORD readSize, PDWORD pbRead)
{
	HANDLE hFile;
	OBJECT_ATTRIBUTES atFlash;
	IO_STATUS_BLOCK ioFlash;
	DWORD bRead = 0, ttlRead = 0, dwPos, sta;
	DWORD rdsz = readSize;
	STRING nFlash;
	RtlInitAnsiString(&nFlash, "\\Device\\Flash");
	atFlash.RootDirectory = 0;
	atFlash.ObjectName = &nFlash;
	atFlash.Attributes = FILE_ATTRIBUTE_DEVICE;
	sta = NtOpenFile(&hFile, GENERIC_WRITE | GENERIC_READ | SYNCHRONIZE, &atFlash, &ioFlash, OPEN_EXISTING, FILE_SYNCHRONOUS_IO_NONALERT);
	if (sta != 0)
	{
		DbgPrint("NtOpenFile(flash): %08x (err: %08x)\n", sta, GetLastError());
		return 0;
	}

	dwPos = SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
	if (dwPos != INVALID_SET_FILE_POINTER)
	{
		ZeroMemory(buf, len);
		while (ttlRead < len)
		{
			if ((len - ttlRead)< readSize)
				rdsz = len - ttlRead;
			ReadFile(hFile, &buf[ttlRead], rdsz, &bRead, NULL);
			ttlRead += bRead;
		}
		if (pbRead != NULL)
		{
			*pbRead = ttlRead;
		}
	}
	else
		DbgPrint("SetFilePointer(flash) invalid! %08x\n", sta, GetLastError());
	NtClose(hFile);

	return ttlRead;
}

BOOL XeKeysPkcs1Verify(const unsigned char* pbHash, const unsigned char* pbSig, XECRYPT_RSA *pRsa)
{
	BOOL ret = FALSE;
	DWORD cnt = pRsa->cqw;
	if ((cnt != 0) && (cnt <= 0x40))
	{
		u8 buf[0x200];

		XeCryptBnQw_SwapDwQwLeBe((PQWORD)pbSig, (PQWORD)buf, cnt);
		XeCryptBnQwNeRsaPubCrypt((u64*)buf, (u64*)buf, pRsa);
		XeCryptBnQw_SwapDwQwLeBe((PQWORD)buf, (PQWORD)buf, cnt);

		ret = XeCryptBnDwLePkcs1Verify((PBYTE)pbHash, buf, cnt << 3);
	}
	return ret;
}

void RC4(void* Key, int ccKey, void* Data, int ccData)
{

	byte s[256];
	byte k[256];
	byte temp;
	byte* Key2 = (byte*)Key;
	byte* Data2 = (byte*)Data;
	int i, j;

	if (!Key || !Data || !ccData)
		return;

	for (i = 0; i < 256; i++)
	{
		s[i] = (byte)i;
		k[i] = Key2[i % ccKey];
	}

	j = 0;
	for (i = 0; i < 256; i++)
	{
		j = (j + s[i] + k[i]) % 256;
		temp = s[i];
		s[i] = s[j];
		s[j] = temp;
	}

	i = j = 0;
	for (int x = 0; x < ccData; x++)
	{
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		temp = s[i];
		s[i] = s[j];
		s[j] = temp;
		int t = (s[i] + s[j]) % 256;
		Data2[x] ^= s[t];
	}
}


PIMAGE_EXPORT_ADDRESS_TABLE getModuleEat(PLDR_DATA_TABLE_ENTRY moduleHandle)
{
	if (moduleHandle != NULL)
	{
		PIMAGE_DATA_DIRECTORY pddir;
		PIMAGE_XEX_HEADER xhead = (PIMAGE_XEX_HEADER)moduleHandle->XexHeaderBase;
		pddir = (PIMAGE_DATA_DIRECTORY)RtlImageXexHeaderField(xhead, XEX_HEADER_PE_EXPORTS);
		if (pddir == NULL)
		{
			PXEX_SECURITY_INFO xs = (PXEX_SECURITY_INFO)(xhead->SecurityInfo);
			return (PIMAGE_EXPORT_ADDRESS_TABLE)xs->ImageInfo.ExportTableAddress;
		}
	}
	return NULL;
}


DWORD hookExportOrd(PLDR_DATA_TABLE_ENTRY moduleHandle, DWORD ord, DWORD dstFun)
{
	PIMAGE_EXPORT_ADDRESS_TABLE expbase = getModuleEat(moduleHandle);
	if (expbase != NULL)
	{
		DWORD modOffset = (expbase->ImageBaseAddress) << 16;
		DWORD origOffset = (expbase->ordOffset[ord - 1]) + modOffset;
		expbase->ordOffset[ord - 1] = dstFun - modOffset;
		__dcbst(0, &expbase->ordOffset[ord - 1]);
		__sync();
		__isync();
		return origOffset;
	}
	return 0;
}

BOOL CReadFile(const CHAR* FileName, MemoryBuffer &pBuffer)
{
	HANDLE hFile; DWORD dwFileSize, dwNumberOfBytesRead;
	hFile = CreateFile(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DbgPrint("CReadFile - CreateFile failed");
		return FALSE;
	}

	dwFileSize = GetFileSize(hFile, NULL);
	PBYTE lpBuffer = (BYTE*)malloc(dwFileSize);
	if (lpBuffer == NULL)
	{
		CloseHandle(hFile);
		DbgPrint("CReadFile - malloc failed");
		return FALSE;
	}

	if (ReadFile(hFile, lpBuffer, dwFileSize, &dwNumberOfBytesRead, NULL) == FALSE)
	{
		free(lpBuffer);
		CloseHandle(hFile);
		DbgPrint("CReadFile - ReadFile failed");
		return FALSE;
	}

	else if (dwNumberOfBytesRead != dwFileSize)
	{
		free(lpBuffer);
		CloseHandle(hFile);
		DbgPrint("CReadFile - Failed to read all the bytes");
		return FALSE;
	}

	CloseHandle(hFile);
	pBuffer.Add(lpBuffer, dwFileSize);
	free(lpBuffer);
	return TRUE;
}


HRESULT GenerateModuleHash(BYTE* pbKey, DWORD cbKey, BYTE* pbDigest, DWORD cbDigest) {
	// TODO: Split this function up, make sure we process import data and export data
	// and update hash using page size once we have the others

	//return ERROR_SUCCESS;

	// Init our module hashing
	XECRYPT_HMACSHA_STATE shaState;
	XeCryptHmacShaInit(&shaState, pbKey, cbKey);

	// First we want to get our module handle
	LDR_DATA_TABLE_ENTRY* mHandle = (LDR_DATA_TABLE_ENTRY*)Handle.hME;
	IMAGE_DOS_HEADER* pDosHead = (IMAGE_DOS_HEADER*)mHandle->ImageBase;
	IMAGE_NT_HEADERS* pNtHead = (IMAGE_NT_HEADERS*)((DWORD)pDosHead + ReverseDWORD(pDosHead->e_lfanew));
	WORD sectionCount = ReverseWORD(pNtHead->FileHeader.NumberOfSections);
	IMAGE_SECTION_HEADER* sections = (IMAGE_SECTION_HEADER*)((DWORD)&pNtHead->OptionalHeader + ReverseWORD(pNtHead->FileHeader.SizeOfOptionalHeader));
	IMAGE_SECTION_HEADER* secText = FindNtSection(sections, sectionCount, ".text");

	// Update the hash
	DbgPrint("ImageBase: %08X Ptr to Data: %08X  Size: %08X", (DWORD)mHandle->ImageBase, (DWORD)mHandle->ImageBase + (DWORD)ReverseDWORD(secText->VirtualAddress), (DWORD)ReverseDWORD(secText->SizeOfRawData) - 0x2000);
	Sleep(100);
	XeCryptHmacShaUpdate(&shaState, (PBYTE)((DWORD)mHandle->ImageBase + ReverseDWORD(secText->VirtualAddress)), (DWORD)ReverseDWORD(secText->SizeOfRawData) - 0x2000);


	// Now we want to finish up
	XeCryptHmacShaFinal(&shaState, pbDigest, cbDigest);
	return ERROR_SUCCESS;
}


DWORD ApplyPatches(CHAR* FilePath, const VOID* DefaultPatches)
{
	// Read our file
	DWORD patchCount = 0;
	MemoryBuffer mbPatches;
	DWORD* patchData = (DWORD*)DefaultPatches;

	// Check if we have our override patches
	if (FilePath != NULL && FileExists(FilePath))
	{
		if (!CReadFile(FilePath, mbPatches))
		{
			DbgPrint("ApplyPatches - CReadFile failed");
			return patchCount;
		}

		// Set our patch data now
		patchData = (DWORD*)mbPatches.GetData();
	}

	// Make sure we have patches...
	if (patchData == NULL)
	{
		return 0;
	}

	// Apply our patches	
	while (*patchData != 0xFFFFFFFF)
	{
		BOOL inHvMode = (patchData[0] < 0x40000);
		QWORD patchAddr = inHvMode ? (0x200000000 * (patchData[0] / 0x10000)) + patchData[0] : (QWORD)patchData[0];

		if (inHvMode)
		{
			HvPokeBytes(patchAddr, &patchData[2], patchData[1] * sizeof(DWORD));
		}
		else
		{
			SetMemory((VOID*)patchData[0], &patchData[2], patchData[1] * sizeof(DWORD));
		}

		patchData += (patchData[1] + 2);
		patchCount++;
	}

	return patchCount;
}

DWORD BytesRead;
BOOL WriteFileAlt(PCHAR Path, LPCVOID Buffer, DWORD dwBytesToWrite)
{
	HANDLE hFile = CreateFile(Path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile); return FALSE;
	}
	WriteFile(hFile, Buffer, dwBytesToWrite, &BytesRead, NULL);
	CloseHandle(hFile);
	return TRUE;
}


BOOL CWriteFile(const CHAR* FilePath, const VOID* Data, DWORD Size)
{
	// Open our file
	HANDLE fHandle = CreateFile(FilePath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fHandle == INVALID_HANDLE_VALUE)
	{
		DbgPrint("CWriteFile - CreateFile failed");
		return FALSE;
	}

	// Write our data and close
	DWORD writeSize = Size;
	if (WriteFile(fHandle, Data, writeSize, &writeSize, NULL) != TRUE)
	{
		DbgPrint("CWriteFile - WriteFile failed");
		return FALSE;
	}
	CloseHandle(fHandle);

	// All done
	return TRUE;
}

DWORD _resolve(HMODULE Module, DWORD Export, DWORD off1, DWORD off2)
{
	SHORT t1, t2;
	DWORD *f;

	if (FAILED(XexGetProcedureAddress(Module, Export, &f)))
		return 0;

	t1 = f[off1] & 0xFFFF;
	t2 = f[off2] & 0xFFFF;

	return (t1 << 16) + t2;
}


void addToList(HANDLE hModule)
{
	PLDR_DATA_TABLE_ENTRY Module;

	if (hModule == 0)
		return;

	Module = (PLDR_DATA_TABLE_ENTRY)hModule;

	// Add the module to the PsLoadedModuleList
	if (PsLoadedModuleList == 0)
		PsLoadedModuleList = (PLIST_ENTRY)_resolve(GetModuleHandle("xboxkrnl.exe"), 412, 9, 11);

	InsertTailList(PsLoadedModuleList, &Module->InLoadOrderLinks);
}

void removeFromList(HANDLE hModule)
{
	PLDR_DATA_TABLE_ENTRY Module;
	PLIST_ENTRY preMod, nextMod, ps;

	if (hModule == 0)
		return;

	Module = (PLDR_DATA_TABLE_ENTRY)hModule;

	// Remove the module from the PsLoadedModuleList
	if (PsLoadedModuleList == 0)
		PsLoadedModuleList = (PLIST_ENTRY)_resolve(GetModuleHandle("xboxkrnl.exe"), 412, 9, 11);

	ps = PsLoadedModuleList->Flink;
	while (ps != PsLoadedModuleList)
	{
		// if the base address is the same
		if (CONTAINING_RECORD(ps, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks)->NtHeadersBase == Module->NtHeadersBase)
		{
			preMod = ps->Blink;
			nextMod = ps->Flink;

			preMod->Flink = nextMod;
			nextMod->Blink = preMod;
		}
		ps = ps->Flink;
	}
}