// Svc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#define INJECT_DLL L"Inject.dll"
#define SVCNAME TEXT("Svc")

BOOL m_bDisableLockScreen = FALSE;

SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

VOID WINAPI SvcCtrlHandler(DWORD);
VOID WINAPI SvcMain(DWORD, LPTSTR *);

VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcInit(DWORD, LPTSTR *);
VOID SvcReportEvent(LPTSTR);

LRESULT DisableLockScreen();
LRESULT EnableLockScreen();

int main()
{
	// TO_DO: Add any additional services for the process to this table.
	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{ SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
		{ NULL, NULL }
	};

	// This call returns when the service has stopped. 
	// The process should simply terminate when the call returns.

	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		SvcReportEvent(TEXT("StartServiceCtrlDispatcher"));
	}


    return 0;
}


BOOL EnableSpecificPrivilege(TCHAR* szPrivilegeName)
{
	HANDLE hCurProc = GetCurrentProcess(); // 伪句柄不需要关闭
	HANDLE hCurProcToken = NULL;
	BOOL bSuc = OpenProcessToken(hCurProc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hCurProcToken);
	if (!bSuc)
	{
		return FALSE;
	}

	LUID luid = { 0 };
	bSuc = LookupPrivilegeValue(NULL, szPrivilegeName, &luid);
	if (!bSuc)
	{
		CloseHandle(hCurProcToken);
		return FALSE;
	}

	TOKEN_PRIVILEGES tp = { 0 };
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	bSuc = AdjustTokenPrivileges(hCurProcToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, 0);
	if (!bSuc)
	{
		CloseHandle(hCurProcToken);
		return FALSE;
	}
	return TRUE;

}

//
// Purpose: 
//   Entry point for the service
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None.
//
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// Register the handler function for the service

	Sleep(10000);
	
	EnableSpecificPrivilege(_T("SeDebugPrivilege"));
	EnableSpecificPrivilege(_T("SeSecurityPrivilege"));

	gSvcStatusHandle = RegisterServiceCtrlHandler(
		SVCNAME,
		SvcCtrlHandler);

	if (!gSvcStatusHandle)
	{
		SvcReportEvent(TEXT("RegisterServiceCtrlHandler"));
		return;
	}

	// These SERVICE_STATUS members remain as set here

	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;

	// Report initial status to the SCM

	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	// Perform service-specific initialization and work.

	SvcInit(dwArgc, lpszArgv);
}

//
// Purpose: 
//   The service code
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None
//
VOID SvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// TO_DO: Declare and set any required variables.
	//   Be sure to periodically call ReportSvcStatus() with 
	//   SERVICE_START_PENDING. If initialization fails, call
	//   ReportSvcStatus with SERVICE_STOPPED.

	// Create an event. The control handler function, SvcCtrlHandler,
	// signals this event when it receives the stop control code.

	ghSvcStopEvent = CreateEvent(
		NULL,    // default security attributes
		TRUE,    // manual reset event
		FALSE,   // not signaled
		NULL);   // no name

	if (ghSvcStopEvent == NULL)
	{
		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}

	// Report running status when initialization is complete.

	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

	// TO_DO: Perform work until service stops.
	if (dwArgc > 1)
	{
		if (wcsnicmp(lpszArgv[1], L"Disable", MAX_PATH) == 0)
		{
			DisableLockScreen();
		}
		else if (wcsnicmp(lpszArgv[1], L"Enable", MAX_PATH) == 0)
		{
			EnableLockScreen();
		}
	}


	while (1)
	{
		// Check whether to stop the service.

		WaitForSingleObject(ghSvcStopEvent, INFINITE);

		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}
}

//
// Purpose: 
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation, 
//     in milliseconds
// 
// Return value:
//   None
//
VOID ReportSvcStatus(DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure.

	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED))
		gSvcStatus.dwCheckPoint = 0;
	else gSvcStatus.dwCheckPoint = dwCheckPoint++;

	// Report the status of the service to the SCM.
	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

//
// Purpose: 
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
// 
// Return value:
//   None
//
VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
	// Handle the requested control code. 

	switch (dwCtrl)
	{
	case SERVICE_CONTROL_STOP:
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		// Signal the service to stop.

		SetEvent(ghSvcStopEvent);

		return;

	case SERVICE_CONTROL_INTERROGATE:
		// Fall through to send current status.
		break;

	default:
		break;
	}

	ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
}

//
// Purpose: 
//   Logs messages to the event log
//
// Parameters:
//   szFunction - name of function that failed
// 
// Return value:
//   None
//
// Remarks:
//   The service must have an entry in the Application event log.
//
VOID SvcReportEvent(LPTSTR szFunction)
{
	HANDLE hEventSource;
	LPCTSTR lpszStrings[2];
	TCHAR Buffer[80];

	hEventSource = RegisterEventSource(NULL, SVCNAME);

	if (NULL != hEventSource)
	{
		StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

		lpszStrings[0] = SVCNAME;
		lpszStrings[1] = Buffer;

		ReportEvent(hEventSource,        // event log handle
			EVENTLOG_ERROR_TYPE, // event type
			0,                   // event category
			1,           // event identifier
			NULL,                // no security identifier
			2,                   // size of lpszStrings array
			0,                   // no binary data
			lpszStrings,         // array of strings
			NULL);               // no binary data

		DeregisterEventSource(hEventSource);
	}
}



LRESULT DisableLockScreen()
{
	// TODO: Add your control notification handler code here
	HMODULE hInjectDll = LoadLibrary(INJECT_DLL);
	if (hInjectDll)
	{
		typedef BOOL(__stdcall * INJECT)();
		typedef BOOL(__stdcall * UNINJECT)();

		INJECT pInject = (INJECT)GetProcAddress(hInjectDll, "Inject");
		UNINJECT pUnInject = (UNINJECT)GetProcAddress(hInjectDll, "UnInject");

		if (pInject && pUnInject)
		{
			m_bDisableLockScreen = pInject();
		}

		FreeLibrary(hInjectDll);
	}

	return 0;
}


LRESULT EnableLockScreen()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bDisableLockScreen)
	{
		return 0;
	}

	HMODULE hInjectDll = LoadLibrary(INJECT_DLL);
	if (hInjectDll)
	{
		typedef BOOL(__stdcall * INJECT)();
		typedef BOOL(__stdcall * UNINJECT)();

		INJECT pInject = (INJECT)GetProcAddress(hInjectDll, "Inject");
		UNINJECT pUnInject = (UNINJECT)GetProcAddress(hInjectDll, "UnInject");

		if (pInject && pUnInject)
		{
			m_bDisableLockScreen = !pUnInject();
		}

		FreeLibrary(hInjectDll);
	}

	return 0;
}
