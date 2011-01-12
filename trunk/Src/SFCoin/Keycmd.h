#pragma once
#include <vector>
#include "GlobalVar.h"

class CKeycmd
{
public:
	// 模拟序列关心从哪个状态fromFlow跳变到哪个状态toFlow，
	// 因为序列是建立在状态fromFlow上的，且达到了toFlow才结束它的使命
	// 离开状态谈模拟不够完整和独立
	// 所以职责划分给它
	CKeycmd(GAMEFLOW fromFlow, GAMEFLOW toFlow,	
		CString name=L"No name");
	~CKeycmd(void);
	typedef struct _Key 
	{
		DWORD interval;
		DWORD key_id;
	}KEYCMD;
	bool ReadCmdFile(const char* fileName);
	bool SaveCmdFile(const char* fileName);
	DWORD InsertCmd(DWORD key,DWORD interval,DWORD pos=-1);
	void Excute(BYTE deviceID);// 对指定的设备模拟，阻塞的，直到跳变到toFlow
private:
	std::vector<KEYCMD> m_vCmds;
	CString m_Name;

	GAMEFLOW m_FromFlow;
	GAMEFLOW m_ToFlow;
};
