#pragma once
#include <vector>
using namespace std;

class CTargetProcess
{
public:
	CTargetProcess();
	~CTargetProcess();

public:
	typedef enum _TARGETPROCESSTYPE
	{
		en_explorer = 0,	// explorer.exe 普通进程
		en_self,			// 自己（用于测试）
		en_csrss,			// 用于LockWorkStation的，在session 1里的进程
	}TARGETPROCESSTYPE, *PTARGETPROCESSTYPE;

	BOOL GetTargetPid(IN TARGETPROCESSTYPE type, OUT vector<DWORD>& vecPid);

private:
	static DWORD GetExplorerPid();

	static DWORD GetCsrssPid();
private:
	vector<DWORD> m_vecTargetPid;
};

