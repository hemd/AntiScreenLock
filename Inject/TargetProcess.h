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
		en_explorer = 0,	// explorer.exe ��ͨ����
		en_self,			// �Լ������ڲ��ԣ�
		en_csrss,			// ����LockWorkStation�ģ���session 1��Ľ���
	}TARGETPROCESSTYPE, *PTARGETPROCESSTYPE;

	BOOL GetTargetPid(IN TARGETPROCESSTYPE type, OUT vector<DWORD>& vecPid);

private:
	static DWORD GetExplorerPid();

	static DWORD GetCsrssPid();
private:
	vector<DWORD> m_vecTargetPid;
};

