#pragma once
#include <vector>
#define IDK_A		0x1e
#define IDX_UP		0xC8
#define IDK_DOWN	0xd0
#define IDK_RIGHT    0xCD
#define IDK_LEFT           0xCB
#define IDK_BACKSAPCE  0x0e

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
	void Excute();
private:
	std::vector<KEYCMD> m_vCmds;
	CString m_Name;
};
