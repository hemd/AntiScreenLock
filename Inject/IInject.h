#pragma once

#include <windows.h>

class IInject 
{
public:
	virtual BOOL __stdcall Inject(DWORD dwPid, WCHAR* wszDllFullPath) = 0;
	virtual BOOL __stdcall UnInject(DWORD dwPid, WCHAR* wszDllFullPath) = 0;
};
