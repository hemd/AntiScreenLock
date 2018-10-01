// Inject.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include "IInjectHelper.h"
#include "TargetProcess.h"

#define DISABLELOCKSCREEN L"DisableLockScreen.dll"

extern "C" __declspec(dllexport) BOOL __stdcall Inject()
{
	IInject* pInject = nullptr;
	if (SUCCEEDED(CreateInstance(&pInject)))
	{
		CTargetProcess TargetProc;
		vector<DWORD> dwExplorer;
		TargetProc.GetTargetPid(CTargetProcess::en_csrss, dwExplorer);
		if (!dwExplorer.empty())
		{
			WCHAR wszDllFullPath[MAX_PATH] = { 0 };
			GetModuleFileName(NULL, wszDllFullPath, MAX_PATH);
			PathRemoveFileSpec(wszDllFullPath);
			PathAppend(wszDllFullPath, DISABLELOCKSCREEN);
			return pInject->Inject(dwExplorer[0], wszDllFullPath);
		}
	}
	return FALSE;
}

extern "C" __declspec(dllexport) BOOL __stdcall UnInject()
{
	IInject* pInject = nullptr;
	if (SUCCEEDED(CreateInstance(&pInject)))
	{
		CTargetProcess TargetProc;
		vector<DWORD> dwExplorer;
		TargetProc.GetTargetPid(CTargetProcess::en_csrss, dwExplorer);
		if (!dwExplorer.empty())
		{
			WCHAR wszDllFullPath[MAX_PATH] = { 0 };
			GetModuleFileName(NULL, wszDllFullPath, MAX_PATH);
			PathRemoveFileSpec(wszDllFullPath);
			PathAppend(wszDllFullPath, DISABLELOCKSCREEN);
			return pInject->UnInject(dwExplorer[0], wszDllFullPath);
		}
	}
	return FALSE;
}