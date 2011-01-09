#pragma once
#include <vector>

class CKeycmd
{
public:
	CKeycmd(CString name=L"No name");
	~CKeycmd(void);
	typedef struct _Key 
	{
		DWORD interval;
		DWORD key_id;
	}KEYCMD;
	bool ReadCmdFile(const char* fileName);
	bool SaveCmdFile(const char* fileName);
	DWORD InsertCmd(DWORD key,DWORD interval,DWORD pos=-1);
	void Excute(BYTE deviceID);// 对指定的设备模拟
private:
	std::vector<KEYCMD> m_vCmds;
	CString m_Name;
};
