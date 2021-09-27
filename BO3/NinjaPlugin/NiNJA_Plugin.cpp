#include "stdafx.h"
#include "Utilities.h"
#include "HvPeekPoke.h"
#include "Config.h"
#include "TitleSpecificHooks.h"
#include <time.h>
#include "Globals.h"
#include "MailMan.h"
#include "BO3.h"
#include "Detour.h"

int DisableBO2PublicCheater = FALSE;

#if _DEVKIT
DWORD supportedVersion = 17526;
#else
DWORD supportedVersion = 17526;
#endif

// Values for the Detour class
BYTE DetourAsm[0x1000] = { 0 };
DWORD DetourAsmIndex = 0;
RTL_CRITICAL_SECTION DetourAsmSection;


BOOL g_killThread = FALSE;
extern void BO3Hax();
extern void UninitializeMw2Cheater();


void DoXNotifyUI(LPCWSTR msg) {
	XNotifyQueueUI(XNOTIFYUI_TYPE_COMPLAINT, XUSER_INDEX_ANY, XNOTIFYUI_PRIORITY_DEFAULT, (PWCHAR)msg, NULL);
}
BOOL unloadSysDLL() {
	HANDLE mHandle = GetModuleHandle("BO3.Engine.xex");
	if (!mHandle) {
		DbgPrint("Failed to get handle to BO3.Engine.xex");
		return FALSE;
	}
	*(WORD*)((DWORD)mHandle + 64) = 1;
	
	DWORD exitCode;
	if (KeGetCurrentProcessType() != 1)
	{
		DWORD dwThreadId;
		LPCWSTR msg = L"NiNJA Engine [BO3] Unloading!";
		HANDLE th = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DoXNotifyUI, (LPVOID)msg, CREATE_SUSPENDED, &dwThreadId);
		SetThreadPriority(th, THREAD_PRIORITY_TIME_CRITICAL);
		ResumeThread(th);
		WaitForSingleObject(th, INFINITE);
		CloseHandle(th);
	}
	else {
		XNotifyUI_Title(L"NiNJA Engine [BO3] Unloading!");
	}
	Sleep(3000);
	GetExitCodeThread(GetCurrentThread(), &exitCode);
	XexUnloadImageAndExitThread(mHandle, (HANDLE)exitCode);
	return TRUE;
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

	if (CheatPatches.fHasPatches) {
		DbgPrint("InitPatches: ERR - Already init");
		return FALSE;
	}
	
	//Request patches to be filled from server
	if (!Messenger.SendRecvMsg(PATCHES)) {
		DbgPrint("InitPatches: ERR - Message exchange failed");
		return FALSE;
	}

	if (!CheatPatches.fHasPatches) {
		DbgPrint("InitPatches: ERR - MSG Exchange Success but patches not loaded");
		return FALSE;
	}

	DbgPrint("InitPatches: Success!");
	return TRUE;
}

void bailout() {
	DbgPrint("Unloading module\r\n");
	g_killThread = TRUE;
	unloadSysDLL();
}

char* tmpStr = NULL;
DWORD tmpLen = 0;


BOOL isHooked = FALSE;

DWORD XamUserGetNameHook(DWORD dwUserIndex, LPSTR pUserName, DWORD cchUserName)
{
	DWORD ret = XamUserGetName(dwUserIndex, pUserName, cchUserName);
	char* spoofedName = "xCoolCanadianx";
	sprintf(pUserName, spoofedName);
	return ret;
}

void MainLoop(void) {

	DbgPrint("Main thread started\r\n");

	/*Flags.canConnect = InitMessaging();
	if (!Flags.canConnect) {
		DbgPrint("MainLoop: ERR - Couldn't start up [MSGR] service");
		bailout();
		return;
	}

	if (!InitPatches()) {
		DbgPrint("MainLoop: ERR - Couldn't download patches");
		bailout();
		return;
	}*/


	XINPUT_STATE State;

	while (!g_killThread) {

		if (XInputGetState(0, &State) != ERROR_SUCCESS)
			continue;

		if ((State.Gamepad.wButtons & (WORD)((WORD)0 | (WORD)XINPUT_GAMEPAD_DPAD_LEFT | (WORD)XINPUT_GAMEPAD_RIGHT_THUMB)) == (WORD)((WORD)0 | (WORD)XINPUT_GAMEPAD_DPAD_LEFT | (WORD)XINPUT_GAMEPAD_RIGHT_THUMB))
		{
			if(!isHooked)
			{
				DbgPrint("XamGetCurrenTitleId(): %08X\r\n", XamGetCurrentTitleId());
				DbgPrint("Hooking BO3\n");
				HANDLE h = GetModuleHandle("default.xex");
				XNotifyUI(L"NiNJA Engine [BO3] Initialized!");
				PXEX_EXECUTION_ID pXex;
				XamGetExecutionId(&pXex);
				// Initalize ur stuffz here
				BO3Hax();

				PatchModuleImport((PLDR_DATA_TABLE_ENTRY)* XexExecutableModuleHandle, "xam.xex", 526, (DWORD)XamUserGetNameHook);
				//InitializeBO2Cheater(pXex, (PLDR_DATA_TABLE_ENTRY)XexExecutableModuleHandle, NULL);

				isHooked = TRUE;
			}
		}

		else if ((State.Gamepad.wButtons & (WORD)((WORD)0 | (WORD)XINPUT_GAMEPAD_DPAD_DOWN | (WORD)XINPUT_GAMEPAD_RIGHT_THUMB)) == (WORD)((WORD)0 | (WORD)XINPUT_GAMEPAD_DPAD_DOWN | (WORD)XINPUT_GAMEPAD_RIGHT_THUMB))
		{
			// Un-hook ur stuffs here (or where ever u want lol)
			if (isHooked) {
				UninitializeMw2Cheater();
			}
			DbgPrint("Unloading module\r\n");
			g_killThread = TRUE;
			unloadSysDLL();
			return;
		}

		Sleep(100);
	}
}


HRESULT Initialize() 
{
	// Running on devkit?
	isDevkit =  *(DWORD*)0x8E038610 & 0x8000 ? FALSE : TRUE;
	DbgPrint("Running on %s", isDevkit ? "Devkit" : "Retail");

	if (XboxKrnlVersion->Build != supportedVersion)	
	{
		DbgPrint("%s%s", isDevkit ? "[WRN]" : "", "Kernel version not supported!");
	}

	//Init flags and global variables
	initVariables();

	std::string iniPath;

	// Get our module handles
	Handle.hXam = GetModuleHandle(NAME_XAM);
	Handle.hKernel = GetModuleHandle(NAME_KERNEL);
	Handle.hXBDM = GetModuleHandle(NAME_XBDM);
	if (Handle.hME == 0 || Handle.hXam == 0 || Handle.hKernel == 0)
	{
		DbgPrint("Failed to get system module handles");
		return E_FAIL;
	}


	Flags.isInit = TRUE;

	// Let's begin our update presence thread now
	HANDLE hThread;
	DWORD hThreadId;

	//ExCreateThread(&hThread, 0, &threadId, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)MainLoop, NULL, CREATE_SUSPENDED);
	ExCreateThread(&hThread, 0, &hThreadId, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)MainLoop, NULL, 0x2);
	if (hThread == NULL) return E_FAIL;
	ResumeThread(hThread);

	// Seed the random number generator
	srand((int)time(NULL));
	
	// All done
	return ERROR_SUCCESS;
}

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

			// Make sure it's the correct game
			if (XamGetCurrentTitleId() != 0x4156091D)
			{
				DbgPrint("NiNJA Engine [BO3] Incorrect Title Id, Unloading!");
				*(WORD*)((DWORD)hModule + 64) = 1;
				return FALSE;
			}

			// Initialize our module
			if (Initialize() == ERROR_SUCCESS)
			{
				DbgPrint("NiNJA Engine [BO3] is ready!");
			}
			else
			{
				DbgPrint("NiNJA Engine [BO3] Failed To Start, Unloading!");
				*(WORD*)((DWORD)hModule + 64) = 1;
				return FALSE;
			}
		}
		else
		{
			DbgPrint("NiNJA Engine [BO3] Load Aborted, Tray Open...");
		}
		break;
	}
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
	{
		g_killThread = TRUE;
		return TRUE;
	}
	default:
		break;
	}

	return TRUE;
}

extern "C" int XapiInitProcess();
extern "C" int XapiCallThreadNotifyRoutines(int);
extern "C" int XapiPAL50Incompatible();
extern "C" int XamTerminateTitle();
extern "C" int _mtinit();
extern "C" int _rtinit();
extern "C" int _cinit(int);
extern "C" int _cexit(int);
extern "C" int _CRT_INIT(...);
extern "C" int __CppXcptFilter(...);

extern "C" static int __proc_attached;

// This function is for flushing the instruction cache. If we dont do this the cpu runs the breakpoints
extern "C" VOID KeSweepIcacheRange(PVOID Address, DWORD cbBytes);

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved);

// This stuff is done all in its own section so it can unobfuscate the text section.
#pragma code_seg(push, r1, ".ptext")

// ripped off microsofts dllcrt0.c for this
__declspec(noinline) BOOL __cdecl realEntryPoint(
		HANDLE  hDllHandle,
		DWORD   dwReason,
		LPVOID  lpreserved
		)
{
		BOOL retcode = TRUE;

		

		/*
		 * If this is a process detach notification, check that there has
		 * has been a prior process attach notification.
		 */
		if ( (dwReason == DLL_PROCESS_DETACH) && (__proc_attached == 0) )
			/*
			 * no prior process attach notification. just return
			 * without doing anything.
			 */
			return FALSE;

		__try {
			if ( dwReason == DLL_PROCESS_ATTACH || dwReason == DLL_THREAD_ATTACH )
			{
				retcode = _CRT_INIT(hDllHandle, dwReason, lpreserved);

				if ( !retcode )
					return FALSE;
			}

			retcode = DllMain(hDllHandle, dwReason, lpreserved);

			if ( (dwReason == DLL_PROCESS_ATTACH) && !retcode )
			{
				/*
				 * The user's DllMain routine returned failure, the C runtime
				 * needs to be cleaned up. Do this by calling _CRT_INIT again,
				 * this time imitating DLL_PROCESS_DETACH. Note this will also
				 * clear the __proc_attached flag so the cleanup will not be
				 * repeated upon receiving the real process detach notification.
				 */
				DllMain(hDllHandle, DLL_PROCESS_DETACH, lpreserved);
				_CRT_INIT(hDllHandle, DLL_PROCESS_DETACH, lpreserved);
			}

			if ( (dwReason == DLL_PROCESS_DETACH) ||
				 (dwReason == DLL_THREAD_DETACH) )
			{
				if ( _CRT_INIT(hDllHandle, dwReason, lpreserved) == FALSE )
					retcode = FALSE ;

			}
		} __except ( __CppXcptFilter(GetExceptionCode(), GetExceptionInformation()) ) {
			return FALSE;
		}

		return retcode ;
}

// end of this section. everything else you want protected should be in the text section.
#pragma code_seg(pop, r1)