#include "stdafx.h"
#include "MainDlg.h"

#define INJECT_DLL L"Inject.dll"


CMainDlg::CMainDlg()
	: m_bDisableLockScreen(FALSE)
{

}

LRESULT CMainDlg::OnBnClickedLockscreen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	LockWorkStation();
	return 0;
}


LRESULT CMainDlg::OnBnClickedDisablelockscreen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
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


LRESULT CMainDlg::OnBnClickedEnablelockscreen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
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
