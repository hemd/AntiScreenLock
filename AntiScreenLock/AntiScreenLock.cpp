// AntiScreenLock.cpp : main source file for AntiScreenLock.exe
//

#include "stdafx.h"

#include "resource.h"

#include "PayDlg.h"
#include "MainDlg.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	
	if ( CMainDlg::GetInstance().Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	CMainDlg::GetInstance().ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

BOOL SingleInstance()
{
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, _T("99E35E29-D7F1-45FF-ADB7-3D03DDB36DFC"));
	if (hEvent && (GetLastError() == ERROR_ALREADY_EXISTS))
	{
		return FALSE;
	}
	return TRUE;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	if (!SingleInstance())
	{
		return -1;
	}

	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
