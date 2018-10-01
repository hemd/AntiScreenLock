// DisableLockScreen.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DisableLockScreen.h"
#include "mhook-lib/mhook.h"

PVOID g_pOrigenLockWorkStation = NULL;

BOOL WINAPI HookedLockWordStation()
{
	return TRUE;
}

BOOL HookLockScreen()
{
	g_pOrigenLockWorkStation = GetProcAddress(GetModuleHandle(L"user32.dll"), "LockWorkStation");
	if (g_pOrigenLockWorkStation)
	{
		return Mhook_SetHook(&g_pOrigenLockWorkStation, HookedLockWordStation);
	}
}

BOOL UnHookLockScreen()
{
	if (g_pOrigenLockWorkStation)
	{
		return Mhook_Unhook(&g_pOrigenLockWorkStation);
	}
}

BOOL DisableLockScreen()
{
	return HookLockScreen();
}

BOOL RestoreLockScreen()
{
	return UnHookLockScreen();
}
