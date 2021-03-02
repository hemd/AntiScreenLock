#include "stdafx.h"
#include <strsafe.h>
#include "WindowsTray.h"

class __declspec(uuid("9D0B8B92-4E1C-488e-A1E1-2331AFCE2CB5")) IconUUID;
#define  WM_APP_NOTIFYCALLBACK (WM_APP + 1)

CWindowsTray::CWindowsTray() : m_bEnabled(TRUE), m_bHidden(FALSE), m_nid{ sizeof(m_nid) }
{
}

CWindowsTray::~CWindowsTray()
{
	Delete();   // 防止外面退出时不调Delete
}

BOOL CWindowsTray::Create(UINT nResIconID, UINT nResMenuID, LPCTSTR pszTip)
{
	m_nid.hWnd = GetNotifyWnd();
	m_nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_GUID;
	m_nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(nResIconID));
	StringCchCopy(m_nid.szTip, _countof(m_nid.szTip), pszTip);
	m_nid.uCallbackMessage = WM_APP_NOTIFYCALLBACK;
	m_nid.guidItem = __uuidof(IconUUID);
	return Shell_NotifyIcon(NIM_ADD, &m_nid);
}

BOOL CWindowsTray::Delete()
{
	return Shell_NotifyIcon(NIM_DELETE, &m_nid);
}

BOOL CWindowsTray::Show()
{
	return 0;
}

BOOL CWindowsTray::Hide()
{
	return 0;
}

BOOL CWindowsTray::IsVisible() const
{
	return !m_bHidden;
}

BOOL CWindowsTray::Enable()
{
	return 0;
}

BOOL CWindowsTray::Disable()
{
	return 0;
}

BOOL CWindowsTray::IsEnable() const
{
	return m_bEnabled;
}

void CWindowsTray::OnLButtonUp()
{
}

void CWindowsTray::OnRButtonUp()
{
}

void CWindowsTray::OnLButtonDoubleClick()
{
}

void CWindowsTray::OnMouseMove()
{
}

HWND CWindowsTray::GetNotifyWnd()
{
	class CTrayNotifyWnd : public CWindowImpl<CTrayNotifyWnd>
	{

	public:
		BOOL ProcessWindowMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam, _Inout_ LRESULT& lResult, _In_ DWORD dwMsgMapID) override
		{
			return TRUE;
		}

	};
	static CTrayNotifyWnd NotifyWnd;
	NotifyWnd.Create(HWND_MESSAGE);
	return NotifyWnd.m_hWnd;
}
