// Made by Xx jAmes t xX
// This .h is used to edit functions to detour to your own custom one
// This class dosnt edit the '.text' section
// 

#ifndef __DETOUR_H
#define __DETOUR_H

#pragma once

#include "stdafx.h"
#include <iterator>
#include <vector>
#include "MemoryManager.h"

// Values for the class
extern BYTE DetourAsm[0x500];
extern MemoryManager DetourManager;
extern CRITICAL_SECTION DetourAsmSection;

//VOID PatchInJump(DWORD* Address, void* Dest, BOOL Linked);
DWORD DetourGetModuleImport(HANDLE HModule, HANDLE HImportedModule, DWORD Ordinal);

int Int24ToInt32(int Value);
int GetBranchCall(int Address);
void SetupCaller();

template<class _ClassType>
class Detour
{
private:
	BYTE OriginalAsm[0x10]; // 4 instructions

	// This function will get any 'b' or 'bl' function added to the stub that
	// it replaces and return the size of the stub in byte lengths
	virtual DWORD DetourFunctionStart(DWORD dwFunctionAddress, DWORD dwStubAddress, PVOID pDestFunc)
	{
		DWORD dwTemp;
		DWORD dwTempFuncAddr;
		BOOL bTemp;
		DWORD dwLength = 0;

		for (int i = 0; i < 4; i++)
		{
			dwTempFuncAddr = dwFunctionAddress + (i * 4);
			byte b = *(byte *)dwTempFuncAddr;

			/*if( b & 0x48 )
			{
			dwTemp = GetBranchCall(dwTempFuncAddr);
			PatchInJump( (PDWORD)( dwStubAddress + dwLength ), (PVOID)dwTemp, FALSE );
			dwLength += 0x10;
			goto DoHook;
			}*/

			// b or bl
			if (b == 0x48 || b == 0x4B)
			{
				// get the branch to address
				dwTemp = dwTempFuncAddr + Int24ToInt32(*(DWORD *)dwTempFuncAddr);
				bTemp = (*(DWORD *)dwTempFuncAddr & 1) != 0;
				PatchInJump((PDWORD)(dwStubAddress + dwLength), (PVOID)dwTemp, bTemp);
				dwLength += 0x10;

				// if it was a 'b loc_' call, we won't need to add anything else to the stub
				if (!bTemp)
					goto DoHook;
			}

			// if the function op code is null it is invalid
			else if (*(DWORD *)dwTempFuncAddr == 0)
				break;

			// just copy the op code
			else
			{
				*(DWORD *)(dwStubAddress + dwLength) = *(DWORD *)dwTempFuncAddr;
				dwLength += 4;
			}
		}

		// make the stub call the orig function
		PatchInJump((PDWORD)(dwStubAddress + dwLength), (PVOID)(dwFunctionAddress + 0x10), FALSE);
		dwLength += 0x10;

	DoHook:
		// apply the hook in the function
		PatchInJump((PDWORD)dwFunctionAddress, pDestFunc, FALSE);
		return dwLength;
	}

public:
	DWORD Addr;
	DWORD SaveStub;
	Detour()
	{
		Addr = 0;
		SaveStub = 0;
	};
	~Detour() {};

	///Will hook the import of a module
	virtual BOOL SetupDetour(DWORD dwModuleBaseAddr, char* szImportedModuleName, int Ordinal, PVOID Destination)
	{
		HANDLE hModule;
		XexPcToFileHeader((PVOID)dwModuleBaseAddr, (PLDR_DATA_TABLE_ENTRY*)&hModule);
		return SetupDetour(hModule, szImportedModuleName, Ordinal, Destination);
	}

	///Will hook the import of a module
	virtual BOOL SetupDetour(HANDLE hModule, char* szImportedModuleName, int Ordinal, PVOID Destination)
	{
		DWORD dwAddress;
		HANDLE hImportedModule = (HANDLE)GetModuleHandle(szImportedModuleName);

		if (hImportedModule == NULL)
			return FALSE;

		dwAddress = DetourGetModuleImport(hModule, hImportedModule, Ordinal);

		if (dwAddress == 0)
			return FALSE;

		return SetupDetour(dwAddress, Destination);
	}

	///Will hook the import of a module
	virtual BOOL SetupDetour(char* Module, char* szImportedModuleName, int Ordinal, PVOID Destination)
	{
		HANDLE hModule = (HANDLE)GetModuleHandle(Module);

		if (hModule == NULL)
			return FALSE;

		return SetupDetour(hModule, szImportedModuleName, Ordinal, Destination);
	}

	///Will hook the export of a module
	virtual BOOL SetupDetour(HANDLE hModule, int Ordinal, PVOID Destination)
	{
		DWORD dwAddress;

		if (hModule == NULL)
			return FALSE;

		dwAddress = (int)GetProcAddress((HMODULE)hModule, (LPCSTR)Ordinal);

		if (dwAddress == NULL)
			return FALSE;

		return SetupDetour(dwAddress, Destination);
	}

	///Will hook the export of a module
	virtual BOOL SetupDetour(char* szModule, int Ordinal, PVOID Destination)
	{
		DWORD dwAddress;
		HMODULE mHandle = GetModuleHandle(szModule);

		if (mHandle == NULL)
			return FALSE;

		dwAddress = (DWORD)GetProcAddress(mHandle, (LPCSTR)Ordinal);

		if (dwAddress == NULL)
			return FALSE;

		return SetupDetour(dwAddress, Destination);
	}

	virtual DWORD GetSizeOfDetour(DWORD dwFunctionAddress, PVOID pDestFunc)
	{
		DWORD dwTemp;
		DWORD dwTempFuncAddr;
		BOOL bTemp;
		DWORD dwLength = 0;
		for (int i = 0; i < 4; i++)
		{
			dwTempFuncAddr = dwFunctionAddress + (i * 4);
			byte b = *(byte *)dwTempFuncAddr;
			if (b == 0x48 || b == 0x4B)
			{
				dwTemp = dwTempFuncAddr + Int24ToInt32(*(DWORD *)dwTempFuncAddr);
				bTemp = (*(DWORD *)dwTempFuncAddr & 1) != 0;
				dwLength += 0x10;
				if (!bTemp)
					goto DoHook;
			}
			else if (*(DWORD *)dwTempFuncAddr == 0)
				break;
			else
			{
				dwLength += 4;
			}
		}
		dwLength += 0x10;

	DoHook:
		return dwLength;
	}

	///Will hook the address specified
	virtual BOOL SetupDetour(DWORD Address, PVOID Destination)
	{
		if (DetourAsmSection.Synchronization.RawEvent[0] == 0)
			InitializeCriticalSection(&DetourAsmSection);

		EnterCriticalSection(&DetourAsmSection);

		DWORD EstimatedDetourSize = GetSizeOfDetour(Address, Destination);
		//printf("Detour is estimated to be 0x%X bytes!\n", EstimatedDetourSize);

		//try to find a spot in the global DetourAsm buffer to put the current detour stub
		BOOL success = DetourManager.alloc(EstimatedDetourSize, &SaveStub);
		if (success)
		{
			if (!MmIsAddressValid((PVOID)SaveStub))
			{
				//printf("DetourManager::alloc returned an invalid address! Cannot continue!\n");
				LeaveCriticalSection(&DetourAsmSection);
				return FALSE;
			}

			// save the address incase we take-down the detour
			Addr = Address;

			// Copy the asm bytes before we replace it with the hook
			memcpy(OriginalAsm, (PVOID)Address, 0x10);

			//detour the function :D
			DetourFunctionStart(Address, SaveStub, Destination);
		}

		LeaveCriticalSection(&DetourAsmSection);
		return success;
	}

	virtual void TakeDownDetour(BOOL reset = FALSE)
	{
		if (Addr && MmIsAddressValid((PVOID)Addr))
			memcpy((PVOID)Addr, OriginalAsm, 0x10);

		// reset the values
		Detour();

		DetourManager.free(SaveStub, FALSE);
	}

#pragma region Calling

	// No more virtual :(

	template<typename R3>
	_ClassType CallOriginal(R3 r3)
	{
		return ((_ClassType(*)(...))SaveStub)(r3);
	}

	template<typename R3, typename R4>
	_ClassType CallOriginal(R3 r3, R4 r4)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4);
	}

	template<typename R3, typename R4, typename R5>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5);
	}

	template<typename R3, typename R4, typename R5, typename R6>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
		S3 s3)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
		S3 s3, S4 s4)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
		S3 s3, S4 s4, S5 s5)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
		S3 s3, S4 s4, S5 s5, S6 s6)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
		S3 s3, S4 s4, S5 s5, S6 s6, S7 s7)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
		S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9>
	_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
		S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9, typename S10>
		_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
			S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9, S10 s10)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9, typename S10, typename S11>
		_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
			S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9, S10 s10, S11 s11)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
			s11);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9, typename S10, typename S11, typename S12>
		_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
			S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9, S10 s10, S11 s11, S12 s12)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
			s11, s12);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9, typename S10, typename S11, typename S12, typename S13>
		_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
			S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9, S10 s10, S11 s11, S12 s12, S13 s13)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
			s11, s12, s13);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9, typename S10, typename S11, typename S12, typename S13, typename S14>
		_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
			S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9, S10 s10, S11 s11, S12 s12, S13 s13, S14 s14)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
			s11, s12, s13, s14);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9, typename S10, typename S11, typename S12, typename S13, typename S14, typename S15>
		_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
			S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9, S10 s10, S11 s11, S12 s12, S13 s13, S14 s14,
			S15 s15)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
			s11, s12, s13, s14, s15);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9, typename S10, typename S11, typename S12, typename S13, typename S14, typename S15, typename S16>
		_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
			S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9, S10 s10, S11 s11, S12 s12, S13 s13, S14 s14,
			S15 s15, S16 s16)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
			s11, s12, s13, s14, s15, s16);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9, typename S10, typename S11, typename S12, typename S13, typename S14, typename S15, typename S16,
		typename S17>
		_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
			S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9, S10 s10, S11 s11, S12 s12, S13 s13, S14 s14,
			S15 s15, S16 s16, S17 s17)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
			s11, s12, s13, s14, s15, s16, s17);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9, typename S10, typename S11, typename S12, typename S13, typename S14, typename S15, typename S16,
		typename S17, typename S18>
		_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
			S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9, S10 s10, S11 s11, S12 s12, S13 s13, S14 s14,
			S15 s15, S16 s16, S17 s17, S18 s18)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
			s11, s12, s13, s14, s15, s16, s17, s18);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9, typename S10, typename S11, typename S12, typename S13, typename S14, typename S15, typename S16,
		typename S17, typename S18, typename S19>
		_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
			S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9, S10 s10, S11 s11, S12 s12, S13 s13, S14 s14,
			S15 s15, S16 s16, S17 s17, S18 s18, S19 s19)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
			s11, s12, s13, s14, s15, s16, s17, s18, s19);
	}

	template<typename R3, typename R4, typename R5, typename R6, typename R7, typename R8, typename R9, typename R10,
		typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8,
		typename S9, typename S10, typename S11, typename S12, typename S13, typename S14, typename S15, typename S16,
		typename S17, typename S18, typename S19, typename S20>
		_ClassType CallOriginal(R3 r3, R4 r4, R5 r5, R6 r6, R7 r7, R8 r8, R9 r9, R10 r10, S1 s1, S2 s2,
			S3 s3, S4 s4, S5 s5, S6 s6, S7 s7, S8 s8, S9 s9, S10 s10, S11 s11, S12 s12, S13 s13, S14 s14,
			S15 s15, S16 s16, S17 s17, S18 s18, S19 s19, S20 s20)
	{
		return ((_ClassType(*)(...))SaveStub)(r3, r4, r5, r6, r7, r8, r9, r10, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10,
			s11, s12, s13, s14, s15, s16, s17, s18, s19, s20);
	}

#pragma endregion

};

#endif //__DETOUR_H_