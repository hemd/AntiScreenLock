#pragma once
#include "IInject.h"
#include "Service.h"

class CInjectImpl :
	public IInject
{
public:
	CInjectImpl();
	virtual ~CInjectImpl();

public:
	virtual BOOL __stdcall Inject(DWORD dwPid, WCHAR* wszDllFullPath);
	virtual BOOL __stdcall UnInject(DWORD dwPid, WCHAR* wszDllFullPath);

protected:
	BOOL InjectByCreateRemoteThread(DWORD dwPid, WCHAR* wszDllFullPath);
	BOOL UnInjectByCreateRomoteThread(DWORD dwPid, WCHAR* wszDllFullPath);
	BOOL InjectByServiceCreateRemoteThread(DWORD dwPid, WCHAR* wszDllFullPath);
	BOOL UnInjectByServiceCreateRomoteThread(DWORD dwPid, WCHAR* wszDllFullPath);

	BOOL IsUnderSvc();

private:
	BOOL OpenTargetProcess(DWORD dwPid);
	BOOL GetTargetProcessDllModule(DWORD dwPid, WCHAR* wszDllFullPath);
	BOOL AllocTargetProcessLoadLibraryWMemory(HANDLE hTargetProc, WCHAR* wszTargetDllFullPath);
	BOOL AllocTargetProcessFreeLibraryMemory(HANDLE hTargetProc, HMODULE hDllModule);
	BOOL CreateTargetThreadLoadLibraryW(HANDLE hTargetProc, LPVOID pParam);
	BOOL CreateTargetThreadFreeLibrary(HANDLE hTargetProc, LPVOID pParam);
	BOOL NtCreateThreadExLoadLibraryW(HANDLE hTargetProc, LPVOID pParam);
	
	static BOOL EnableSpecificPrivilege(TCHAR* szPrivilegeName);
	

private:
	HANDLE m_hTargetProcess;
	LPVOID m_pTargetMemoryWithTargetDllFullPath;
	LPVOID m_pTargetMemoryWithTargetDllHModule;
	HANDLE m_hRemoteThread;
	HMODULE m_hTargetProcessModule;
	CService m_Svc;

};

