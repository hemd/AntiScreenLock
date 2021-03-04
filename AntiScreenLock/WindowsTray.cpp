#include "stdafx.h"
#include <strsafe.h>
#include "WindowsTray.h"
#include "MainDlg.h"

class __declspec(uuid("952C9030-4189-42E7-B3DC-8AECCD7290FF")) IconUUID;

CNotifyTray::CNotifyTray() : m_nMenuID(0)
{
}

CNotifyTray::~CNotifyTray()
{
	Delete();   // 防止外面退出时不调Delete
}

BOOL CNotifyTray::Create(HWND hParentWnd, UINT nResIconID, UINT nResMenuID, LPCTSTR lpszTip)
{
	// 先保存一下MenuID
	m_nMenuID = nResMenuID;

	// 创建托盘
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.hWnd = CWindowImpl::Create(hParentWnd); // 在父窗口上创建一个隐藏的子窗口
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.uID = 0;
	nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(nResIconID));
	StringCchCopy(nid.szTip, _countof(nid.szTip), lpszTip);
	nid.uCallbackMessage = WM_NOTIFYCALLBACK;

	return Shell_NotifyIcon(NIM_ADD, &nid);
}

BOOL CNotifyTray::Delete()
{
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.hWnd = m_hWnd;
	nid.uID = 0;
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

BOOL CNotifyTray::UpdateIcon(UINT nResIconID)
{
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.uFlags = NIF_ICON;
	nid.hWnd = m_hWnd;
	nid.uID = 0;
	nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(nResIconID));
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL CNotifyTray::UpdateTip(LPCTSTR lpszTip)
{
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.uFlags = NIF_TIP;
	nid.hWnd = m_hWnd;
	nid.uID = 0;
	StringCchCopy(nid.szTip, _countof(nid.szTip), lpszTip);
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL CNotifyTray::ShowBalloon(LPCTSTR lpszTitle, LPCTSTR lpszText)
{
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.uFlags = NIF_INFO; 
	nid.hWnd = m_hWnd;
	nid.uID = 0;
	StringCchCopy(nid.szInfoTitle, _countof(nid.szInfoTitle), lpszTitle);
	StringCchCopy(nid.szInfo, _countof(nid.szInfo), lpszText);
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

LRESULT CNotifyTray::OnTrayNotifyCallback(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WORD wNotifyEvent = LOWORD(lParam);
	switch (wNotifyEvent)
	{
	case WM_LBUTTONDBLCLK:
	{
		CMainDlg::GetInstance().Show();	
	}
	break;

	case WM_RBUTTONUP:
	{
		ShowContextMenu();
	}
	break;
	default:
		break;
	}
	return 0L;
}

LRESULT CNotifyTray::OnTrayMenuOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CMainDlg::GetInstance().Show();
	return 0;
}

LRESULT CNotifyTray::OnTrayMenuExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CMainDlg::GetInstance().Exit();
	return 0;
}

void CNotifyTray::ShowContextMenu()
{
	HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(m_nMenuID));
	if (hMenu)
	{
		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (hSubMenu)
		{
			// our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
			SetForegroundWindow(m_hWnd);

			// respect menu drop alignment
			UINT uFlags = TPM_RIGHTBUTTON;
			if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
			{
				uFlags |= TPM_RIGHTALIGN;
			}
			else
			{
				uFlags |= TPM_LEFTALIGN;
			}

			POINT pt;
			GetCursorPos(&pt);

			TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, m_hWnd, NULL);
		}
		DestroyMenu(hMenu);
	}
}
