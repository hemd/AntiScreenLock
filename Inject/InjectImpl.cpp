#include "stdafx.h"
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include "InjectImpl.h"

#define SVN_NAME L"Svc"
#define SVC_EXE L"Svc.exe"


CInjectImpl::CInjectImpl()
	: m_hTargetProcess(NULL)
	, m_hRemoteThread(NULL)
	, m_pTargetMemoryWithTargetDllFullPath(NULL)
	, m_pTargetMemoryWithTargetDllHModule(NULL)
	, m_hTargetProcessModule(NULL)
{
	EnableSpecificPrivilege(_T("SeDebugPrivilege"));
}


CInjectImpl::~CInjectImpl()
{
	if (m_hTargetProcess)
	{
		CloseHandle(m_hTargetProcess);
		m_hTargetProcess = NULL;
	}

	if (m_hRemoteThread)
	{
		CloseHandle(m_hRemoteThread);
		m_hRemoteThread = NULL;
	}
}

BOOL CInjectImpl::Inject(DWORD dwPid, WCHAR * wszDllFullPath)
{
	if (!IsUnderSvc())
	{
		return InjectByServiceCreateRemoteThread(dwPid, wszDllFullPath);
	}
	else
	{
		return InjectByCreateRemoteThread(dwPid, wszDllFullPath);
	}
}

BOOL CInjectImpl::UnInject(DWORD dwPid, WCHAR * wszDllFullPath)
{
	if (!IsUnderSvc())
	{
		return UnInjectByServiceCreateRomoteThread(dwPid, wszDllFullPath);
	}
	else
	{
		return UnInjectByCreateRomoteThread(dwPid, wszDllFullPath);
	}
}

BOOL CInjectImpl::InjectByCreateRemoteThread(DWORD dwPid, WCHAR * wszDllFullPath)
{
	if (OpenTargetProcess(dwPid) 
		&& AllocTargetProcessLoadLibraryWMemory(m_hTargetProcess, wszDllFullPath) 
		&& CreateTargetThreadLoadLibraryW(m_hTargetProcess, m_pTargetMemoryWithTargetDllFullPath))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CInjectImpl::UnInjectByCreateRomoteThread(DWORD dwPid, WCHAR* wszDllFullPath)
{
	if (GetTargetProcessDllModule(dwPid, wszDllFullPath)
		&& AllocTargetProcessFreeLibraryMemory(m_hTargetProcess, m_hTargetProcessModule)
		&& CreateTargetThreadFreeLibrary(m_hTargetProcess, m_pTargetMemoryWithTargetDllHModule))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CInjectImpl::InjectByServiceCreateRemoteThread(DWORD dwPid, WCHAR* wszDllFullPath)
{
	WCHAR wszSvc[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, wszSvc, MAX_PATH);
	PathRemoveFileSpec(wszSvc);
	PathAppend(wszSvc, SVC_EXE);

	BOOL bSuc = m_Svc.Install(SVN_NAME, wszSvc);
	if (bSuc)
	{
		WCHAR* argv[1] = { L"Disable" };
		bSuc = m_Svc.Start(1, (LPCWSTR*)argv);
	}
	return bSuc;
}

BOOL CInjectImpl::UnInjectByServiceCreateRomoteThread(DWORD dwPid, WCHAR* wszDllFullPath)
{
	WCHAR wszSvc[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, wszSvc, MAX_PATH);
	PathRemoveFileSpec(wszSvc);
	PathAppend(wszSvc, SVC_EXE);

	BOOL bSuc = m_Svc.Install(SVN_NAME, wszSvc);
	if (bSuc)
	{
		WCHAR* argv[1] = { L"Enable" };
		bSuc = m_Svc.Start(1, (LPCWSTR*)argv);
	}
	return bSuc;
}

BOOL CInjectImpl::IsUnderSvc()
{
	WCHAR wszExe[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, wszExe, MAX_PATH);
	WCHAR* pszExe = wcsrchr(wszExe, L'\\') + 1;
	return 0 == wcsnicmp(pszExe, L"svc.exe", MAX_PATH);
}

BOOL CInjectImpl::OpenTargetProcess(DWORD dwPid)
{
	m_hTargetProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwPid);
	return m_hTargetProcess != NULL;
}

BOOL CInjectImpl::AllocTargetProcessLoadLibraryWMemory(HANDLE hTargetProc, WCHAR* wszTargetDllFullPath)
{
	if (hTargetProc)
	{
		m_pTargetMemoryWithTargetDllFullPath = VirtualAllocEx(hTargetProc, 
			NULL, 
			sizeof(WCHAR) * (wcslen(wszTargetDllFullPath) + 1), 
			MEM_COMMIT | MEM_RESERVE, 
			PAGE_READWRITE);
		if (m_pTargetMemoryWithTargetDllFullPath)
		{
			return WriteProcessMemory(hTargetProc, 
				m_pTargetMemoryWithTargetDllFullPath, 
				(LPCVOID)wszTargetDllFullPath, 
				sizeof(WCHAR) * wcslen(wszTargetDllFullPath), 
				NULL);
		}
	}
	return FALSE;
}

BOOL CInjectImpl::AllocTargetProcessFreeLibraryMemory(HANDLE hTargetProc, HMODULE hDllModule)
{
	if (hTargetProc)
	{
		m_pTargetMemoryWithTargetDllHModule = VirtualAllocEx(hTargetProc,
			NULL,
			sizeof(hDllModule),
			MEM_COMMIT | MEM_RESERVE,
			PAGE_READWRITE);
		if (m_pTargetMemoryWithTargetDllHModule)
		{
			return WriteProcessMemory(hTargetProc,
				m_pTargetMemoryWithTargetDllHModule,
				(LPCVOID)&hDllModule,
				sizeof(hDllModule),
				NULL);
		}
	}
	return FALSE;
}

BOOL CInjectImpl::CreateTargetThreadLoadLibraryW(HANDLE hTargetProc, LPVOID pParam)
{
	PVOID pLoadLibraryW = (PVOID)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "LoadLibraryW");
	if (pLoadLibraryW)
	{
		m_hRemoteThread = CreateRemoteThread(hTargetProc,
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)pLoadLibraryW,
			pParam,
			0,
			NULL);

		return m_hRemoteThread != NULL;
	}
	return FALSE;
}

BOOL CInjectImpl::CreateTargetThreadFreeLibrary(HANDLE hTargetProc, LPVOID pParam)
{
	PVOID pFreeLibrary = (PVOID)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "FreeLibrary");
	if (pFreeLibrary)
	{
		m_hRemoteThread = CreateRemoteThread(hTargetProc,
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)pFreeLibrary,
			pParam,
			0,
			NULL);

		return m_hRemoteThread != NULL;
	}
	return FALSE;
}

BOOL CInjectImpl::NtCreateThreadExLoadLibraryW(HANDLE hTargetProc, LPVOID pParam)
{
	PVOID pLoadLibraryW = (PVOID)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "LoadLibraryW");
	if (pLoadLibraryW)
	{
		//m_hRemoteThread = CreateRemoteThread(hTargetProc,
		//	NULL,
		//	0,
		//	(LPTHREAD_START_ROUTINE)pLoadLibraryW,
		//	pParam,
		//	0,
		//	NULL);

		//return m_hRemoteThread != NULL;

		typedef LONG(__stdcall *LPFUN_NtCreateThreadEx)
			(
				OUT HANDLE* hThread,
				IN ACCESS_MASK DesiredAccess,
				IN LPVOID ObjectAttributes,
				IN HANDLE ProcessHandle,
				IN LPTHREAD_START_ROUTINE lpStartAddress,
				IN LPVOID lpParameter,
				IN BOOL CreateSuspended,
				IN ULONG StackZeroBits,
				IN ULONG SizeOfStackCommit,
				IN ULONG SizeOfStackReserve,
				OUT LPVOID lpBytesBuffer
				);

		//Buffer argument passed to NtCreateThreadEx function

		struct NtCreateThreadExBuffer
		{
			ULONG Size;
			ULONG Unknown1;
			ULONG Unknown2;
			PULONG Unknown3;
			ULONG Unknown4;
			ULONG Unknown5;
			ULONG Unknown6;
			PULONG Unknown7;
			ULONG Unknown8;
		};

		HMODULE modNtDll = GetModuleHandle(L"ntdll.dll");
		if (modNtDll)
		{
			LPFUN_NtCreateThreadEx funNtCreateThreadEx =
				(LPFUN_NtCreateThreadEx)GetProcAddress(modNtDll, "NtCreateThreadEx");
			if (funNtCreateThreadEx)
			{
				//setup and initialize the buffer
				NtCreateThreadExBuffer ntbuffer;

				memset(&ntbuffer, 0, sizeof(NtCreateThreadExBuffer));
				DWORD temp1 = 0;
				DWORD temp2 = 0;

				ntbuffer.Size = sizeof(NtCreateThreadExBuffer);
				ntbuffer.Unknown1 = 0x10003;
				ntbuffer.Unknown2 = 0x8;
				ntbuffer.Unknown3 = &temp2;
				ntbuffer.Unknown4 = 0;
				ntbuffer.Unknown5 = 0x10004;
				ntbuffer.Unknown6 = 4;
				ntbuffer.Unknown7 = &temp1;
				ntbuffer.Unknown8 = 0;

				HANDLE hThread = NULL;
				LONG status = funNtCreateThreadEx(
					&hThread,
					0x1FFFFF,
					NULL,
					hTargetProc,
					(LPTHREAD_START_ROUTINE)pLoadLibraryW,
					pParam,
					FALSE, //start instantly
					NULL,
					NULL,
					NULL,
					&ntbuffer
				);
				if (hThread)
				{
					WaitForSingleObject(hThread, INFINITE);
					CloseHandle(hThread);
				}
			}
		}
	}
	return FALSE;
}

BOOL CInjectImpl::EnableSpecificPrivilege(TCHAR* szPrivilegeName)
{
	HANDLE hCurProc = GetCurrentProcess(); // 伪句柄不需要关闭
	HANDLE hCurProcToken = NULL;
	BOOL bSuc = OpenProcessToken(hCurProc, TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hCurProcToken);
	if (!bSuc)
	{
		return FALSE;
	}

	LUID luid = { 0 };
	bSuc = LookupPrivilegeValue(NULL, szPrivilegeName, &luid);
	if (!bSuc)
	{
		CloseHandle(hCurProcToken);
		return FALSE;
	}

	TOKEN_PRIVILEGES tp = { 0 };
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	bSuc = AdjustTokenPrivileges(hCurProcToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, 0);
	if (!bSuc)
	{
		CloseHandle(hCurProcToken);
		return FALSE;
	}
	return TRUE;

}

BOOL CInjectImpl::GetTargetProcessDllModule(DWORD dwPid, WCHAR* wszDllFullPath)
{
	m_hTargetProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
		FALSE,
		dwPid);
	if (m_hTargetProcess)
	{
		HMODULE hMods[1024] = { 0 };
		DWORD cbNeeded = 0;
		if (EnumProcessModules(m_hTargetProcess, hMods, sizeof(hMods), &cbNeeded))
		{
			for (int i =0; i < (cbNeeded / sizeof(HMODULE)); ++i)
			{
				WCHAR wszModFullPath[MAX_PATH] = { 0 };
				if (GetModuleFileNameEx(m_hTargetProcess, hMods[i], wszModFullPath, MAX_PATH))
				{
					if (wcsnicmp(wszDllFullPath, wszModFullPath, MAX_PATH) == 0)
					{
						m_hTargetProcessModule = hMods[i];
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}
