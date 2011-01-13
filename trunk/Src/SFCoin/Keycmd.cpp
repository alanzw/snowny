#include "StdAfx.h"
#include "Keycmd.h"
#pragma comment(lib,"DirectInput-Hook.lib")
#include "../../Common/DirectInput-Hook/DirectInput-Hook.h"

CKeycmd::CKeycmd( GAMEFLOW fromFlow, GAMEFLOW toFlow, CString name/*=L"No name"*/ )
: m_FromFlow(fromFlow)
, m_ToFlow(toFlow)
, m_Name(name)
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

BOOL CheckGameFlow(GAMEFLOW wannaFlow, DWORD timeout)
{
	DWORD lastTime = GetTickCount();
	while (GetTickCount() - lastTime < timeout)
	{
		if (g_GameFlow == wannaFlow)
		{
			return TRUE;
		}
		Sleep(1);
	}
	return FALSE;
}


void CKeycmd::Excute( BYTE deviceID )
{
	TRACE(L"SF4 CKeycmd Excute:%s devid:%d\n",m_Name, deviceID);
	if (!CheckGameFlow(m_FromFlow, 10000))
	{
		TRACE(TEXT("SF4 %s fromFlow error!!!!!!!!!!!!!!!!!!!!!!!"), m_Name);
	}

	std::vector<KEYCMD>::iterator it;
	for(it=m_vCmds.begin();it!=m_vCmds.end();it++)
	{
		Sleep(it->interval);
		DIHKeyDown(deviceID, (BYTE)it->key_id);
#ifdef _DEBUG
		CString out;
		out.Format(L"StreetFighter DIHKeyDown:%s Internal:%d\n",
			it->key_id == IDK_START ? L"start" : 
			it->key_id == IDK_OK ? L"ok" :
			it->key_id == IDK_UP ? L"up" :
			it->key_id == IDK_DOWN ? L"down" :
			it->key_id == IDK_LEFT ? L"left" :
			it->key_id == IDK_RIGHT ? L"right" :
			it->key_id == IDK_BACKSPACE ? L"back" :
			it->key_id == IDK_CONTINUE ? L"continue" : L"unknown" , it->interval);
		TRACE((LPCTSTR)out);
#endif
	}

	if (!CheckGameFlow(m_ToFlow, 10000))
	{
		TRACE(TEXT("SF4 %s toFlow timeout!!!!!!!!!!!!!!!!!!!!!!!"), m_Name);
	}
}