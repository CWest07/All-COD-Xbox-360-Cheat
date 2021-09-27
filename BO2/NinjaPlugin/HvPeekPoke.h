#pragma once

#ifndef _HV_PEEKPOKE_H
#define _HV_PEEKPOKE_H

typedef unsigned __int64 QWORD;
#define HvxCall QWORD _declspec(naked)

#define FREEBOOT_SYSCALL_KEY	0x72627472 
static HvxCall HvxRunCode(DWORD key, QWORD type, QWORD dest, UINT64 buf, UINT64 len){

	__asm {
		li      r0, 0x0
		sc
		blr
	}
}


typedef enum _PEEK_POKE_TYPE {
	PEEK_BYTE  = 0,
	PEEK_WORD  = 1,
	PEEK_DWORD = 2,
	PEEK_QWORD = 3,
	PEEK_BYTES = 4,
	POKE_BYTE  = 5,
	POKE_WORD  = 6,
	POKE_DWORD = 7,
	POKE_QWORD = 8,
	POKE_BYTES = 9,
	PEEK_SPR   = 10
} PEEK_POKE_TYPE;
	
#define HvPeekPokeExpID 0x48565050
	
	
BYTE    HvPeekBYTE(QWORD Address);
WORD    HvPeekWORD(QWORD Address);
DWORD   HvPeekDWORD(QWORD Address);
QWORD   HvPeekQWORD(QWORD Address);
HRESULT HvPeekBytes(QWORD Address, PVOID Buffer, DWORD Size);

HRESULT HvPokeBYTE(QWORD Address, BYTE Value);
HRESULT HvPokeWORD(QWORD Address, WORD Value);
HRESULT HvPokeDWORD(QWORD Address, DWORD Value);
HRESULT HvPokeQWORD(QWORD Address, QWORD Value);
HRESULT HvPokeBytes(QWORD Address, const void* Buffer, DWORD Size);
QWORD HvGetFuseLine(BYTE fuseIndex);

#define SYSCALL_KEY	0x72627472 // rbtr

QWORD HvxPeekCall(DWORD key, QWORD type, QWORD SourceAddress, QWORD DestAddress, QWORD lenInBytes);
//QWORD HvxMemCpy(QWORD DestAddress, QWORD SourceAddress, QWORD lenInBytes);


#endif // _HV_PEEKPOKE_H