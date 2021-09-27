#include "stdafx.h"
#include "HvPeekPoke.h"
#include "Utilities.h"
#pragma warning(push)
#pragma warning(disable:4826) // Get rid of the sign-extended warning


static HvxCall HvxExpansionCall(DWORD ExpansionId, QWORD Param1 = 0, QWORD Param2 = 0, QWORD Param3 = 0, QWORD Param4 = 0) 
{
	__asm 
	{
		li			r0, 0x71
		sc
		blr
	}
}
	
	
BYTE HvPeekBYTE(QWORD Address) 
{
	return (BYTE)HvxExpansionCall(HvPeekPokeExpID, PEEK_BYTE, Address);
}

WORD HvPeekWORD(QWORD Address) 
{
	return (WORD)HvxExpansionCall(HvPeekPokeExpID, PEEK_WORD, Address);
}

DWORD HvPeekDWORD(QWORD Address) 
{
	return (DWORD)HvxExpansionCall(HvPeekPokeExpID, PEEK_DWORD, Address);
}

QWORD HvPeekQWORD(QWORD Address) 
{
	return HvxExpansionCall(HvPeekPokeExpID, PEEK_QWORD, Address);
}

HRESULT HvPeekBytes(QWORD Address, PVOID Buffer, DWORD Size) 
{		
	// Create a physical buffer to peek into
	VOID* data = XPhysicalAlloc(Size, MAXULONG_PTR, 0, PAGE_READWRITE);
	if(data == NULL)
	{
		DbgPrint("error allocating buffer!\n");
		HalReturnToFirmware(HalResetSMCRoutine);
	}
	ZeroMemory(data, Size);
	
	HRESULT result = (HRESULT)HvxExpansionCall(HvPeekPokeExpID, PEEK_BYTES, Address, (QWORD)MmGetPhysicalAddress(data), Size);

	// If its successful copy it back
	if(result == S_OK) memcpy(Buffer, data, Size);

	// Free our physical data and return our result
	XPhysicalFree(data);
	return result;
}

QWORD __declspec(naked) HvxPeekCall(DWORD key, QWORD type, QWORD SourceAddress, QWORD DestAddress, QWORD lenInBytes)
{ 
	__asm {
		li      r0, 0x0
		sc
		blr
	}
}

//QWORD HvxMemCpy(QWORD DestAddress, QWORD SourceAddress, QWORD lenInBytes)
//{

//	return HvxPeekCall(SYSCALL_KEY, 5, SourceAddress, DestAddress, lenInBytes);
//}

HRESULT HvPokeBYTE(QWORD Address, BYTE Value) 
{
	return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_BYTE, Address, Value);
}

HRESULT HvPokeWORD(QWORD Address, WORD Value) 
{
	return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_WORD, Address, Value);
}

HRESULT HvPokeDWORD(QWORD Address, DWORD Value) 
{
	return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_DWORD, Address, Value);
}

HRESULT HvPokeQWORD(QWORD Address, QWORD Value) 
{
	return (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_QWORD, Address, Value);
}

HRESULT HvPokeBytes(QWORD Address, const void* Buffer, DWORD Size) 
{
	// Create a physical buffer to poke from
	VOID* data = XPhysicalAlloc(Size, MAXULONG_PTR, 0, PAGE_READWRITE);
	if(data == NULL)
	{
		DbgPrint("error allocating buffer!\n");
		HalReturnToFirmware(HalResetSMCRoutine);
	}
	memcpy(data, Buffer, Size);
	
	HRESULT result = (HRESULT)HvxExpansionCall(HvPeekPokeExpID, POKE_BYTES, Address, (QWORD)MmGetPhysicalAddress(data), Size);

	// Free our physical data and return our result
	XPhysicalFree(data);
	return result;
}

// 0 = production data
// 1 = console type
// 2 = console sequence (2BL/CB LDV)
// 3 = CPU key 1 p1
// 4 = CPU key 2 p1
// 5 = CPU key 1 p2
// 6 = CPU key 2 p2
// 7-11 = update sequence

QWORD HvGetFuseLine(BYTE fuseIndex) 
{
	if (fuseIndex > 11) return 0;
	return HvPeekQWORD(0x8000020000020000 + (fuseIndex * 0x200));
}
#pragma warning(pop)