#include "stdafx.h"
#include "Utilities.h"
#include "HvPeekPoke.h"
#include "Config.h"
#include "TitleSpecificHooks.h"
#include <time.h>
#include "Globals.h"
#include "MailMan.h"
#include "WAW.h"
#include "Detour.h"

#if _DEVKIT
DWORD supportedVersion = 17526;
#else
DWORD supportedVersion = 17526;
#endif

BOOL g_killThread = FALSE;
extern void InitializeWAWHooks();
extern void UninitializeWAWCheater();
PLIST_ENTRY PsLoadedModuleList = NULL;


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

void DoXNotifyUI(LPCWSTR msg) {
	XNotifyQueueUI(XNOTIFYUI_TYPE_COMPLAINT, XUSER_INDEX_ANY, XNOTIFYUI_PRIORITY_DEFAULT, (PWCHAR)msg, NULL);
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

void unloadSysDLL(HANDLE mHandle)
{
	//addToList(mHandle);
	*(WORD*)((DWORD)mHandle + 64) = 1;
	DWORD exitCode;
	GetExitCodeThread(GetCurrentThread(), &exitCode);
	XexUnloadImageAndExitThread(mHandle, (HANDLE)exitCode);
}

BOOL isStringEmpty(PCHAR String)
{
	if(String == NULL) return TRUE;

	if(String[0] == 0) return TRUE;

	return FALSE;
}

// Use this one!
BOOL IsTrayOpen() 
{
	unsigned char msg[0x10]; 
	unsigned char resp[0x10]; 
	memset(msg, 0x0, 0x10); 
	msg[0] = 0xA; 
	HalSendSMCMessage(msg, resp); 
	
	if (resp[1] == 0x60) return true; 

	return false; 
}


//Initialize [MSGR] SubSystem
BOOL InitMessaging() {
	if (Messenger.isReady())
		return FALSE;
	DbgPrint("Initializing [MSGR] SubSystem");
	
	//Find ptr to mailbox
	Messenger.SetMsgBox(MailManGetBox(Messenger.myIdent));

	return TRUE;
}


BOOL InitPatches() {
	DbgPrint("InitPatches: Function Start");

	if (!Messenger.isReady()) {
		DbgPrint("InitPatches: ERR - [MSGR] not ready");
		return FALSE;
	}

	if (CheatPatches->fHasPatches) {
		DbgPrint("InitPatches: ERR - Already init");
		return FALSE;
	}
	
	//Request patches to be filled from server
	if (!Messenger.SendRecvMsg(PATCHES)) {
		DbgPrint("InitPatches: ERR - Message exchange failed");
		return FALSE;
	}

	if (!CheatPatches->fHasPatches) {
		DbgPrint("InitPatches: ERR - MSG Exchange Success but patches not loaded");
		return FALSE;
	}

	DbgPrint("InitPatches: Success!");
	return TRUE;
}

void bailout() {
	DbgPrint("Unloading module\r\n");
	g_killThread = TRUE;
	unloadSysDLL(Handle.hME);
}

char* tmpStr = NULL;
DWORD tmpLen = 0;


BOOL isHooked = FALSE;
BOOL hasMainLoopExited = FALSE;
DWORD titleID = 0x4156081C;
void MainLoop(void) 
{
	while (XamGetCurrentTitleId() != titleID)
		Sleep(500);

	
	PLDR_DATA_TABLE_ENTRY entry;
	XexPcToFileHeader((PVOID)0x82000000, &entry);
	if (wcscmp(entry->BaseDllName.Buffer, L"default_mp.xex") == 0)
	{
		//setup a memory manager class to store our patches
		PBYTE buf = (PBYTE)XPhysicalAlloc(0x2000, MAXULONG_PTR, 128, MEM_LARGE_PAGES | PAGE_READWRITE);
		mManager = new MemoryManager((DWORD)buf, 0x2000);
		mManager->alloc(sizeof(Security::CoDCheats), &CheatPatches); //allocate some memory for our cheat patches

		//setup a second memory manager to store web requests and decrypted strings which need to remain decrypted in memory
		PBYTE buf2 = (PBYTE)XPhysicalAlloc(0x5000, MAXULONG_PTR, 128, MEM_LARGE_PAGES | PAGE_READWRITE);
		sManager = new MemoryManager((DWORD)buf2, 0x5000);

#ifdef PATCH_SVR
		Flags.canConnect = InitMessaging();
		if (!Flags.canConnect) {
			DbgPrint("MainLoop: ERR - Couldn't start up [MSGR] service");
			bailout();
			return;
		}

		if (!InitPatches()) {
			DbgPrint("MainLoop: ERR - Couldn't download patches");
			goto unload;
		}
#endif

		while (!g_killThread)
		{
			if (CheatPatches->fHasPatches)
			{
				if (!isHooked)
				{
					DbgPrint("XamGetCurrenTitleId(): %08X\r\n", XamGetCurrentTitleId());
					DbgPrint("Setting up detours");
					HANDLE h = GetModuleHandle("default_mp.xex");
					XNotifyUI(L"NiNJA Engine Initialized!");
					PXEX_EXECUTION_ID pXex;
					XamGetExecutionId(&pXex);
					// Initalize ur stuffz here
					InitializeWAWHooks();

					isHooked = TRUE;
				}
				else if (XamGetCurrentTitleId() != titleID)
				{
					break;
				}

				if (shouldSaveMenuOptions)
				{
					SaveMenuOptions();
					shouldSaveMenuOptions = false;
				}

				for (int i = 0; i < 18; i++)
				{
					if (PlayerInfo[i].GrabGeoIPInfo)
					{
						GeoIpInfo info;
						GeoIpParser::GeoIpParser parser(PlayerInfo[i].ip, &info);
						if (parser.success == true)
						{
							printf("Parser success!\n");
							memcpy(PlayerInfo[i].city, info.city, sizeof(info.city));
							memcpy(PlayerInfo[i].state, info.state, sizeof(info.state));
							memcpy(PlayerInfo[i].stateFull, info.stateFull, sizeof(info.stateFull));
							memcpy(PlayerInfo[i].country, info.country, sizeof(info.country));
							memcpy(PlayerInfo[i].isp, info.isp, sizeof(info.isp));
							memcpy(PlayerInfo[i].zip, info.zip, sizeof(info.zip));
							PlayerInfo[i].GrabGeoIPInfo = FALSE;
							DbgPrint("Success!");
						}
					}
				}

				for (int i = 0; i < DecryptedStringsTimed.size(); i++)
				{
					if (DecryptedStringsTimed[i].InitTime + DecryptedStringsTimed[i].Lifetime < GetTickCount())
					{
						sManager->free(DecryptedStringsTimed[i].Ptr);
						std::vector<TimedString>::iterator it = DecryptedStringsTimed.begin();
						std::advance(it, i);
						DecryptedStringsTimed.erase(it);
					}
				}
			}
			else
			{
#ifndef PATCH_SVR
				CheatPatches->setAddresses(NULL);
#endif

#ifdef DUMP_BINS
				CheatPatches->dumpPatchBinary();
#endif
			}
			Sleep(100);
		}

		for (int i = 0; i < DecryptedStringsTimed.size(); i++)
			sManager->free(DecryptedStringsTimed[i].Ptr);

		if (isHooked) {
			DbgPrint("Taking down detours\r\n");
			UninitializeWAWCheater();
		}
		Sleep(500);
		XPhysicalFree((PVOID)mManager->memory);
		XPhysicalFree((PVOID)sManager->memory);

		delete mManager;
		delete sManager;
	}

unload:
	DbgPrint("Main loop has exited successfully!");
	hasMainLoopExited = TRUE;

	if(!g_killThread)
		unloadSysDLL(Handle.hME);
}

DWORD flags = EX_CREATE_FLAG_SUSPENDED | EX_CREATE_FLAG_SYSTEM | EX_CREATE_FLAG_CORE2 | EX_CREATE_FLAG_PRIORITY1;
void HiddenThreadStartup()
{
	DbgPrint("Running on %s", isDevkit ? "Devkit" : "Retail");

	if (XboxKrnlVersion->Build != supportedVersion)
	{
		DbgPrint("%s%s", isDevkit ? "[WRN]" : "", "Kernel version not supported!");
	}

	//Init flags and global variables
	initVariables();

	std::string iniPath;

	Flags.isInit = TRUE;

	// Let's begin our update presence thread now
	HANDLE hThread;
	DWORD hThreadId;

#ifdef ANTIHACK 
	flags |= 0x18000424;
#endif

	ExCreateThread(&hThread, 0, &hThreadId, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)MainLoop, NULL, flags);
	if (hThread == NULL) return;
	ResumeThread(hThread);

	// Seed the random number generator
	srand((int)time(NULL));
}

HRESULT Initialize() 
{
	// Running on devkit?
	isDevkit = *(DWORD*)0x8E038610 & 0x8000 ? FALSE : TRUE;

	// Get our module handles
	Handle.hXam = GetModuleHandle(NAME_XAM);
	Handle.hKernel = GetModuleHandle(NAME_KERNEL);
	Handle.hXBDM = GetModuleHandle(NAME_XBDM);
	if (Handle.hME == 0 || Handle.hXam == 0 || Handle.hKernel == 0)
	{
		DbgPrint("Failed to get system module handles");
		return E_FAIL;
	}

	DWORD startFunc = GetModuleImportAddr(Handle.hME, Handle.hKernel, 25);
	//DbgPrint("StartFunc: %08X  %08X", startFunc, *(DWORD*)startFunc);
	if (!startFunc)
		return E_FAIL;

#ifdef PATCH_SVR
	*(DWORD*)startFunc = (DWORD)HiddenThreadStartup;
#else
	HiddenThreadStartup();
#endif
	
	// All done
	return ERROR_SUCCESS;
}

char SecurityData[0x300] = { 0x78, 0x62, 0x4F, 0x62, 0x66, 0x75, 0x00, 0x00,
(sizeof(SecurityData) >> 24) & 0xFF, (sizeof(SecurityData) >> 16) & 0xFF, (sizeof(SecurityData) >> 8) & 0xFF, sizeof(SecurityData) & 0xFF };
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	// Check how we want to enter
	switch (ul_reason_for_call)
	{
	case DLL_THREAD_ATTACH:
	case DLL_PROCESS_ATTACH:
	{
		if (!IsTrayOpen())
		{
			Handle.hME = hModule;

			// Initialize our module
			if (Initialize() == ERROR_SUCCESS)
			{
				DbgPrint("NiNJA Engine is ready!");
				if (!isDevkit)
					removeFromList(hModule);
			}
			else
			{
				DbgPrint("NiNJA Engine Failed To Start, Unloading!");
				*(WORD*)((DWORD)hModule + 64) = 1;
				return FALSE;
			}
		}
		else
		{
			DbgPrint("NiNJA Engine Load Aborted, Tray Open...");
		}
		break;
	}
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	{
		g_killThread = TRUE;
		while (!hasMainLoopExited)
			Sleep(10);
		if (!isDevkit)
			addToList(Handle.hME);
		return TRUE;
	}
	default:
		break;
	}

	return TRUE;
}