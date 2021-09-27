#ifndef SECURESTRINGS_H
#define SECURESTRINGS_H
#pragma once
#include <xtl.h>
#include <string>
#include <stdio.h>
#include <vector>
#include "MemoryManager.h"
#include <xkelib.h>
#include "Globals.h"

using namespace std;

#pragma warning(disable: 4129 4309)

struct TimedString
{
	DWORD Ptr; //ptr to the string//
	DWORD InitTime; //GetTickCount()
	DWORD Lifetime; //ms
};

static char xorKey[0x10] = { 0x86, 0x17, 0x18, 0x98, 0x3d, 0x07, 0x43, 0x91, 0x05, 0xfc, 0xea, 0x1f, 0x9a, 0xf9, 0x5d, 0x2d };

char *XOR(char *data);
char *XOR_Timed(char *data);
char *XOR_Alloc(char *data);

#define DECRYPTED_STRING_LIFETIME_MS 3000

#ifdef CRYPTED_STRINGS
	//Decrypt strings on the stack and return a temp stack ptr
	#define E(A) (XOR(A ## "\0\."))
	//Decrypt strings and copy the result into allocated memory, automatically freed after DECRYPTED_STRING_LIFETIME_MS
	#define E_TIMED(A) (XOR_Timed(A ## "\0\."))
	//Decrypt strings and copy the result into allocated memory
	#define E_ALLOC(A) (XOR_Alloc(A ## "\0\."))
#else
	//Define CRYPTED_STRINGS to enable
	#define E(A) A
	//Define CRYPTED_STRINGS to enable
	#define E_TIMED(A) A
	//Define CRYPTED_STRINGS to enable
	#define E_ALLOC(A) A
#endif

// Crashes the xbox by writing random bytes into the kernel in an infinite loop.
#pragma warning(disable: 4307)
#pragma optimize( "", off )
#ifndef DBG_PRINT
#define CRASH_XBOX_SNEAKILY \
{ \
int i = 0; \
while (true) \
{ \
	*(BYTE*)(0x10008000 * 8 + i) = (BYTE)(rand() & 0xff); \
	i++; \
} \
}
#else
#define CRASH_XBOX_SNEAKILY \
{ \
	DbgPrint("I AM TRYING TO CRASH YOU!"); \
}
#endif

#pragma optimize( "", on )

// Checks whether or not the function is being called from 
// within our own xex, if not crash the console.
#define SENSITIVE_FUNCTION_START \
{ \
	DWORD CallingAddr; \
	__asm \
	{ \
		mr CallingAddr, r12 \
	} \
	PLDR_DATA_TABLE_ENTRY pldte = (PLDR_DATA_TABLE_ENTRY)Handle.hME; \
	if ((CallingAddr < (DWORD)pldte->NtHeadersBase && CallingAddr > ((DWORD)pldte->NtHeadersBase + pldte->SizeOfNtImage))) \
	{ \
		CRASH_XBOX_SNEAKILY; \
	} \
} 

extern vector<TimedString> DecryptedStrings;
extern vector<TimedString> DecryptedStringsTimed;

extern MemoryManager *sManager;

#endif