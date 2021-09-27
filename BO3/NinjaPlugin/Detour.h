// Made by Xx jAmes t xX
// This .h is used to edit functions to detour to your own custom one
// 
// DOWNSIDE: you can only use a max of 7 int and float args with Detour->CallOriginal
// if you want to use any amout then do (( < your class type> (*)(...))Detour->SaveStub)( "Your", "Args", "Here" );
// 

#pragma once
#ifndef _DETOUR_H
#define _DETOUR_H

#include "stdafx.h"
#pragma warning (disable: 4018)


typedef struct _DETOUR_XEX_IMPORT_DESCRIPTOR
{
	DWORD Size;
	DWORD NameTableSize;
	DWORD ModuleCount;
} DETOUR_XEX_IMPORT_DESCRIPTOR, *PDETOUR_XEX_IMPORT_DESCRIPTOR;


extern VOID PatchInJump(DWORD* Address, void* Dest, BOOL Linked);
extern DWORD hookExportOrd(PLDR_DATA_TABLE_ENTRY moduleHandle, DWORD ord, DWORD dstFun);

static int Int24ToInt32(int Value) {
	byte bValue = (Value >> 24) & 0xFF;
	Value &= 0x00FFFFFF;

	if (Value & 0x800000 && bValue != 0x48)
		Value |= 0xFF000000;
	if (Value & 1)
		Value -= 1;
	return Value;
}

static bool IsZero(PVOID Scr, DWORD Size) {

	bool result;
	byte *bZeroData = new byte[Size];
	ZeroMemory(bZeroData, Size);

	result = !memcmp(Scr, bZeroData, Size);
	delete[] bZeroData;
	return result;
}

// need to call this from the class because all the agrs are pushed up
// from r3 because it is the class pointer
static void __declspec(naked) SetupCaller()
{
	__asm
	{
		mr r3, r4
		mr r4, r5
			mr r5, r6
			mr r6, r7
			mr r7, r8
			mr r8, r9
			mr r9, r10

			fmr fr1, fr2
			fmr fr2, fr3
			fmr fr3, fr4
			fmr fr4, fr5
			fmr fr5, fr6
			fmr fr6, fr7
			fmr fr7, fr8
			fmr fr8, fr9
			fmr fr9, fr10
			blr
	}
}

static bool bCheckIfCMP(int ptr)
{
	byte b = *(byte *)ptr;
	byte b2 = *(byte *)(ptr + 1);

	if (b == 0x40 || b == 0x41)
	{
		if (b2 == 0x9A || b2 == 0x82 || b2 == 0x99
			|| b2 == 0x81 || b2 == 0x98 || b2 == 0x80)
			return true;
	}
	return false;
}

template<class _ClassType>
class Detour
{
private:
	BYTE OriginalAsm[0x10]; // 4 instructions
	DWORD DetourIndex;

	__int64 iArgs[8];
	double fArgs[8];

	// This function will get any 'b' or 'bl' function added to the stub that
	// it replaces and return the size of the stub in byte lengths
	virtual DWORD DetourFunctionStart(DWORD dwFunctionAddress, DWORD dwStubAddress, PVOID pDestFunc, BOOL isExport = FALSE)
	{
		DWORD dwLength = 0;
		DWORD dwTemp = 0;
		DWORD dwTempFuncAddr = 0;
		BOOL bTemp;

		for (int i = 0; i < 4 && !isExport; i++)
		{
			dwTempFuncAddr = dwFunctionAddress + (i * 4);
			byte b = *(byte *)dwTempFuncAddr;
			byte b2 = *(byte *)(dwTempFuncAddr + 1);

			// b or bl
			if (b == 0x48 || b == 0x4B)
			{
				// get the branch to address
				dwTemp = dwTempFuncAddr + Int24ToInt32(*(DWORD *)dwTempFuncAddr);
				bTemp = (*(DWORD *)dwTempFuncAddr & 1) != 0;
				PatchInJump((PDWORD)(dwStubAddress + dwLength), (PVOID)dwTemp, bTemp);
				dwLength += 0x10;

				// if it was a 'b loc_' call, we won't need to anything else to the stub
				if (!bTemp)
					goto DoHook;
			}

			// beq or bne, ble or bgt, bge or blt
			/*else if (bCheckIfCMP(dwTempFuncAddr))
			{
			dwTemp = *(DWORD *)dwTempFuncAddr & 0xFFFF;

			// if bTemp is true the op code is 'beq'
			bTemp = b == 0x41;

			// check if the branch location is within the stub
			if (dwTemp <= 0x10 && dwTemp > 0)
			{
			if (dwTemp <= (0x10 - (i * 4)))
			{
			*(DWORD *)(dwStubAddress + dwLength) = *(DWORD *)dwTempFuncAddr;
			dwLength += 4;
			}
			else
			goto branch_else;
			}
			else
			{
			branch_else:
			// make a jump past the call if the cmp != what it is checking
			*(DWORD *)(dwStubAddress + dwLength) = ((0x40000000 + (*(DWORD *)dwTempFuncAddr & 0x00FF0000) + 0x14) +
			bTemp ? 0 : 0x01000000);
			dwLength += 4;
			PatchInJump((PDWORD)(dwStubAddress + dwLength), (PVOID)(dwTempFuncAddr + dwTemp), FALSE);
			dwLength += 0x10;
			}
			}*/

			// if the function op code is null it is invalid
			else if (*(DWORD *)dwTempFuncAddr == 0)
				break;

			else
			{
				*(DWORD *)(dwStubAddress + dwLength) = *(DWORD *)dwTempFuncAddr;
				dwLength += 4;
			}
		}

		// make the stub call the orig function
		if (!isExport)
			PatchInJump((PDWORD)(dwStubAddress + dwLength), (PVOID)(dwFunctionAddress + 0x10), FALSE);
		else {
			PatchInJump((PDWORD)(dwStubAddress), (PVOID)(dwFunctionAddress), FALSE);
			return 0x10;
		}
		dwLength += 0x10;


	DoHook:
		// apply the hook
		PatchInJump((PDWORD)dwFunctionAddress, pDestFunc, FALSE);
		return dwLength;
	}


public:
	DWORD Addr;
	DWORD SaveStub;
	Detour() {
		Addr = 0;
		SaveStub = 0;
		DetourIndex = 0;
	};
	~Detour() {};

	virtual bool isDetoured() {
		return (Addr != 0 /*&& (*(DWORD)Addr == 0x7D8802A6)*/);
	}

	///Will hook the import of a module
	virtual BOOL SetupDetour(char* Module, char* ImportedModuleName, int Ordinal, PVOID Destination)
	{
		return SetupDetour((HANDLE)GetModuleHandle(Module), ImportedModuleName, Ordinal, Destination);
	}

	///Will hook the import of a module
	virtual BOOL SetupDetour(HANDLE hModule, char* ImportedModuleName, int Ordinal, PVOID Destination)
	{
		DWORD dwAddress;
		HANDLE hImportedModule = (HANDLE)GetModuleHandle(ImportedModuleName);

		if (hImportedModule == NULL)
			return FALSE;

		dwAddress = GetModuleImport(hModule, hImportedModule, Ordinal);

		if (dwAddress == 0)
			return FALSE;

		return SetupDetour(dwAddress, Destination);
	}

	///Will hook the import of a module
	virtual BOOL SetupDetour(HANDLE hModule, HANDLE ImportedModule, int Ordinal, PVOID Destination)
	{
		DWORD dwAddress;

		if (ImportedModule == NULL || !hModule)
			return FALSE;

		dwAddress = GetModuleImport(hModule, ImportedModule, Ordinal);

		if (dwAddress == 0)
			return FALSE;

		return SetupDetour(dwAddress, Destination);
	}

	virtual BOOL SetupDetour(HANDLE Module, int Ordinal, PVOID Destination, BOOL isExport = FALSE)
	{
		DWORD dwAddress;

		if (Module == NULL)
			return FALSE;
		if (!isExport) {
			dwAddress = (int)GetProcAddress((HMODULE)Module, (LPCSTR)Ordinal);
		}
		else {
			dwAddress = (int)hookExportOrd((PLDR_DATA_TABLE_ENTRY)Module, Ordinal, (DWORD)Destination);
		}
		if (dwAddress == NULL)
			return FALSE;

		return SetupDetour((DWORD)dwAddress, Destination, isExport);
	}

	virtual BOOL SetupDetour(char* Module, int Ordinal, PVOID Destination, BOOL isExport = FALSE)
	{
		DWORD dwAddress;
		HMODULE mHandle = GetModuleHandle(Module);

		if (mHandle == NULL)
			return FALSE;

		dwAddress = (DWORD)GetProcAddress(mHandle, (LPCSTR)Ordinal);

		if (dwAddress == NULL)
			return FALSE;

		return SetupDetour(dwAddress, Destination, isExport);
	}

	///Will hook the address specified
	virtual BOOL SetupDetour(DWORD Address, PVOID Destination, BOOL isExport = FALSE)
	{
		if (IsZero(&DetourAsmSection, sizeof(DetourAsmSection)))
			InitializeCriticalSection(&DetourAsmSection);

		EnterCriticalSection(&DetourAsmSection);

		if (Addr != Address || SaveStub == 0) {

			DetourIndex = DetourAsmIndex;
			SaveStub = (DWORD)&DetourAsm[DetourIndex];

			// save the address incase we take-down the detour
			Addr = Address;


			// Copy the asm bytes before we replace it with the hook
			memcpy(OriginalAsm, (PVOID)Address, 0x10);


			// increment the index for the space we are using for the stub
			DetourAsmIndex += DetourFunctionStart(Address, SaveStub, Destination, isExport);
		}
		else
		{
			// if we have already got a stub and the address is the same just re use it
			DetourFunctionStart(Address, SaveStub, Destination, isExport);
		}

		LeaveCriticalSection(&DetourAsmSection);

		return TRUE;
	}

	virtual DWORD DetourGetModuleImport(HANDLE HModule, HANDLE HImportedModule, DWORD Ordinal)
	{
		DWORD dwModuleCountIndex = 0;
		DWORD dwModuleIndex = 0;
		CHAR szModuleName[0x100];
		CHAR szHImportedModule[0x100];

		// get the resolve address to compair it to
		DWORD dwImportAddress = (DWORD)GetProcAddress((HMODULE)HImportedModule, (LPCSTR)Ordinal);

		// check if it is valid
		if (dwImportAddress == NULL || HModule == NULL)
			return 0;

		// get the importTable from the module
		PDETOUR_XEX_IMPORT_DESCRIPTOR importDesc = (PDETOUR_XEX_IMPORT_DESCRIPTOR)
			RtlImageXexHeaderField((void*)*(DWORD *)((DWORD)HModule + 0x58), 0x000103FF);

		// see if it is valid
		if (importDesc == NULL)
			return 0;

		// get the module name from the handle
		WideCharToMultiByte(CP_UTF8, NULL, (wchar_t*)*(DWORD*)((DWORD)HImportedModule + 0x30), -1, szHImportedModule, 0x100, NULL, NULL);

		// go through all imports
		for (DWORD dwModuleCount = 0; dwModuleCount < importDesc->ModuleCount; dwModuleCount++)
		{
			// save the index so we can copy the string
			DWORD i = dwModuleCountIndex;
			// get the pointer to the table
			DWORD dwPointer = (DWORD)importDesc + 0x0C + importDesc->NameTableSize + dwModuleIndex;
			// get the size
			DWORD dwModuleSectionSize = *(DWORD *)dwPointer;
			// go to the data in the table
			dwPointer += 0x24 + 4;

			// update the index
			dwModuleIndex += dwModuleSectionSize;

			// align the size for the update we did
			dwModuleSectionSize -= 0x24 + 4;

			// update the count
			dwModuleCountIndex += strlen((char*)((DWORD)importDesc + 0x0C + i));
			// get the import section's module name
			strcpy(szModuleName, (char*)((DWORD)importDesc + 0x0C + i));

			// compair if it is the one we are trying to get the address for
			if (!strcmp(szModuleName, szHImportedModule))
			{
				// scan through the table to find the import
				for (DWORD dwSize = 0; dwSize < dwModuleSectionSize; dwSize += 8)
				{
					DWORD dwTempAddr = *(DWORD *)*(DWORD *)(dwPointer + dwSize);
					if (dwTempAddr == dwImportAddress)
						return *(DWORD *)(dwPointer + dwSize + 4);
				}
			}
		}
		return 0;
	}

	virtual void TakeDownDetour()
	{
		if (Addr && MmIsAddressValid((PVOID)Addr))
			memcpy((PVOID)Addr, OriginalAsm, 0x10);
	}

	virtual _ClassType CallOriginal(...)
	{
		SetupCaller();
		return ((_ClassType(*)(...))SaveStub)();
	}
};


#endif