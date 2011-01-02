#include "StdAfx.h"
#include "Keycmd.h"
#pragma comment(lib,"DirectInput-Hook.lib")
#include "../../Common/DirectInput-Hook/DirectInput-Hook.h"
CKeycmd::CKeycmd(CString name):m_Name(name)
{
	
}

CKeycmd::~CKeycmd(void)
{
}

bool CKeycmd::ReadCmdFile(const char* fileName)
{
	return true;
}
bool CKeycmd::SaveCmdFile(const char* fileName)
{
	return true;
}
DWORD CKeycmd::InsertCmd(DWORD key,DWORD interval,DWORD pos/* =-1 */)
{
	KEYCMD cmd;
	cmd.key_id=key;
	cmd.interval=interval;
	if(-1==pos)
	{
		m_vCmds.push_back(cmd);
	}
	else
		m_vCmds.insert(m_vCmds.begin()+pos,cmd);
	return 0;
}
void CKeycmd::Excute()
{
	TRACE(L"StreetFighter Excute:%s\n",m_Name);
	std::vector<KEYCMD>::iterator it;
	for(it=m_vCmds.begin();it!=m_vCmds.end();it++)
	{
		Sleep( (*it).interval );
		DIHKeyDown(0,(BYTE)(*it).key_id);
		TRACE(L"StreetFighter DIHKeyDown:%x\n",(*it).key_id);
	}
}