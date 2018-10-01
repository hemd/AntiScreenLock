#include "stdafx.h"
#include "TargetProcess.h"


CTargetProcess::CTargetProcess()
{
}


CTargetProcess::~CTargetProcess()
{
	m_vecTargetPid.clear();
}

BOOL CTargetProcess::GetTargetPid(IN TARGETPROCESSTYPE type, OUT vector<DWORD>& vecPid)
{
	switch (type)
	{
	case en_explorer:
	{
		m_vecTargetPid.clear();
		DWORD dwExplorerPid = GetExplorerPid();
		m_vecTargetPid.push_back(dwExplorerPid);
		vecPid = m_vecTargetPid;
		return TRUE;
	}
	break;
	case en_self:
	{
		m_vecTargetPid.clear();
		m_vecTargetPid.push_back(GetCurrentProcessId());
		vecPid = m_vecTargetPid;
		return TRUE;
	}
	break;
	case en_csrss:
	{
		m_vecTargetPid.clear();
		m_vecTargetPid.push_back(GetCsrssPid());
		vecPid = m_vecTargetPid;
		return TRUE;
	}
	break;
	default:
		break;
	}
	return FALSE;
}

DWORD CTargetProcess::GetExplorerPid()
{
	DWORD dwExplorerPid = 0;
	GetWindowThreadProcessId(FindWindow(_T("Shell_TrayWnd"), NULL), &dwExplorerPid);
	return dwExplorerPid;
}

DWORD CTargetProcess::GetCsrssPid()
{
	return 524;
}
