#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#pragma once

#include <xtl.h>
#include <vector>
#include <stdio.h>

struct MemoryRegion
{
	DWORD address;
	DWORD size;
	MemoryRegion(DWORD Address, DWORD Size)
	{
		address = Address;
		size = Size;
	}
};

class MemoryManager
{
public:
	DWORD memory; //pointer to the memory
	DWORD memorySize; //size of the memory being pointed to
	std::vector<MemoryRegion> allocated; //list of memory regions, which contain the address and size of all memory which is currently allocated in this context

	BOOL alloc(DWORD Size, DWORD *Out, BOOL zeroMemory = TRUE);
	BOOL alloc(DWORD Size, PVOID Out, BOOL zeroMemory = TRUE);
	BOOL free(DWORD Address, BOOL clearMemory = TRUE);
	BOOL free(PVOID Address, BOOL clearMemory = TRUE);

	VOID SetupManager(DWORD Buffer, DWORD Size);

	MemoryManager(DWORD Buffer, DWORD Size);
	MemoryManager();
	~MemoryManager();
};

#endif