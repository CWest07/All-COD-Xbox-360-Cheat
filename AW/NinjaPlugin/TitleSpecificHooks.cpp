#include "stdafx.h"
#include "Config.h"
#include "Utilities.h"
#include "TitleSpecificHooks.h"
#include "Globals.h"



DWORD XexGetProcedureAddressHook(HANDLE hand, DWORD dwOrdinal, PVOID* pvAddress)
{	
	// Check our module
	if (hand == Handle.hXam) 
	{
		switch (dwOrdinal) 
		{
			case 0x9BB:
				//*pvAddress = XSecurityCreateProcessHook;
				return 0;
		}
	}

	// Call our real function if we aren't interested
	return XexGetProcedureAddress(hand, dwOrdinal, pvAddress);
}



VOID InitializeTitleSpecificHooksTest(PLDR_DATA_TABLE_ENTRY ModuleHandle)
{
	XEX_EXECUTION_ID* pExecutionId;
	if (XamGetExecutionId(&pExecutionId) != S_OK) return;

	


	switch (pExecutionId->TitleID)
	{
		
		case COD_BLACK_OPS_2:
		{
			//InitializeBlackOps2Hooks(pExecutionId, ModuleHandle);
			break;
		}

		
		default:
		{
			break;
		}
	}
}




