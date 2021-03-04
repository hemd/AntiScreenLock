// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "PayDlg.h"
#include "MainDlg.h"

CMainDlg::CMainDlg() : m_bAntiScreenLockSwitch(FALSE)
{

}

CMainDlg& CMainDlg::GetInstance()
{
	static CMainDlg MainDlg;
	return MainDlg;
}

void CMainDlg::Show()
{
	ShowWindow(SW_SHOWNORMAL);
	SetForegroundWindow(m_hWnd);
}

void CMainDlg::Hide()
{
	ShowWindow(SW_HIDE);
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	UIUpdateChildWindows();
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	m_Tray.Create(m_hWnd, IDR_MAINFRAME, IDR_MENU_TRAY, _T("∑¿À¯∆¡"));
	m_Tray.UpdateTip(_T("∑¿À¯∆¡ - Œ¥ø™∆Ù"));
	//Sleep(5000);
	//m_Tray.UpdateIcon(IDI_ENABLE);
	//Sleep(5000);
	//m_Tray.ShowBalloon(_T("Title"), _T("Text"));
	//Sleep(5000);
	//m_Tray.Delete();
	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
	return 0;
}

LRESULT CMainDlg::OnAppPay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CPayDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnHide(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Hide();
	m_Tray.ShowBalloon(_T("∑¿À¯∆¡“—ø™∆Ù"), _T(""));
	return 0;
}

void CMainDlg::Exit()
{
	DestroyWindow();
	::PostQuitMessage(0);
}


void CMainDlg::UpdateUI()
{
	CButton BtnSwitch = GetDlgItem(IDC_BTN_SWITCH);
	BtnSwitch.SetWindowText(m_bAntiScreenLockSwitch ? _T("ª÷∏¥") : _T("∑¿À¯∆¡"));
	CStatic StaticStatus = GetDlgItem(IDC_STATIC_STATUS);
	StaticStatus.SetWindowText(m_bAntiScreenLockSwitch ? _T("∑¿À¯∆¡“—ø™∆Ù") : _T("∑¿À¯∆¡“—πÿ±’"));
	m_Tray.UpdateTip(m_bAntiScreenLockSwitch ? _T("∑¿À¯∆¡ - “—ø™∆Ù") : _T("∑¿À¯∆¡ - Œ¥ø™∆Ù"));
}

LRESULT CMainDlg::OnBnClickedBtnSwitch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_bAntiScreenLockSwitch = !m_bAntiScreenLockSwitch;
	const UINT nTimerID = 0x1984;
	if (m_bAntiScreenLockSwitch)
	{
		SetTimer(nTimerID, 3000);
	}
	else
	{
		KillTimer(nTimerID); 
		SetThreadExecutionState(ES_CONTINUOUS);
	}

	UpdateUI();
	return 0;
}

LRESULT CMainDlg::OnBnClickedBtnPay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}
