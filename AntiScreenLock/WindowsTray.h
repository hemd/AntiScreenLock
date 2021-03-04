#pragma once
#include "resource.h"

class CNotifyTray : public CWindowImpl<CNotifyTray>
{
#define  WM_NOTIFYCALLBACK (WM_APP + 1)
public:
	CNotifyTray();
	virtual ~CNotifyTray();

public:
	BOOL Create(HWND hParentWnd, UINT nResIconID, UINT nResMenuID, LPCTSTR lpszTip);
	BOOL Delete();

	BOOL UpdateIcon(UINT nResIconID);
	BOOL UpdateTip(LPCTSTR lpszTip);
	BOOL ShowBalloon(LPCTSTR lpszTitle, LPCTSTR lpszText);

public:
	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_NOTIFYCALLBACK, OnTrayNotifyCallback)
		COMMAND_ID_HANDLER(ID_TRAY_MENU_OPEN, OnTrayMenuOpen)
		COMMAND_ID_HANDLER(ID_TRAY_MENU_EXIT, OnTrayMenuExit)
	END_MSG_MAP()

	LRESULT OnTrayNotifyCallback(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTrayMenuOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTrayMenuExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	void ShowContextMenu();

private:
	UINT m_nMenuID;
};

