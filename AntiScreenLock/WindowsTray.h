#pragma once
class CWindowsTray
{
public:
	CWindowsTray();
	virtual ~CWindowsTray();

public:
	BOOL Create(UINT nResIconID, UINT nResMenuID, LPCTSTR pszTip);
	BOOL Delete();
	BOOL Show();
	BOOL Hide();
	BOOL IsVisible() const;
	BOOL Enable();
	BOOL Disable();
	BOOL IsEnable() const;

public:
	virtual void OnLButtonUp();
	virtual void OnRButtonUp();
	virtual void OnLButtonDoubleClick();
	virtual void OnMouseMove();

public:
	virtual HWND GetNotifyWnd();

private:
	BOOL m_bEnabled;
	BOOL m_bHidden;

	NOTIFYICONDATA m_nid;
};

