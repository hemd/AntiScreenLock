#pragma once
class CService
{
public:
	CService();
	~CService();

public:
	BOOL Install(WCHAR* wszName, WCHAR* wszBinaryPath, WCHAR* wszDisplayName = NULL, DWORD dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, DWORD dwStartType = SERVICE_DEMAND_START);
	BOOL UnInstall();
	BOOL Start(int argc, LPCWSTR* argv);
	BOOL Stop();

protected:
	BOOL StopDepend();
private:
	WCHAR m_wszSvcName[MAX_PATH];
	SC_HANDLE m_hsvc;
};

