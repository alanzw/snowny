#include "StdAfx.h"
#include "ContrlCenter.h"
#include "GlobalVar.h"


CContrlCenter g_ContrlCenter;


CContrlCenter::CContrlCenter(void):m_cmdTitle2SelectChar(L"m_cmdTitle2SelectChar"),m_cmdDemo2SelectChar(L"m_cmdDemo2SelectChar"),
m_cmdMainmenu2Title(L"m_cmdMainmenu2Title"),m_cmdSetting(L"m_cmdSetting")
{
	TRACE(L"StreetFighter CContrlCenter::CContrlCenter\n");
}

CContrlCenter::~CContrlCenter(void)
{
}

bool CContrlCenter::Init()
{
	TRACE(L"StreetFighter CContrlCenter::Init()\n");
	m_Config.ReadConfig(L"config.ini");
	DIHSetKDProc(KeyProc);
	DWORD interval=1000;
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdTitle2SelectChar.InsertCmd(IDK_DOWN,interval);
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval);

	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdDemo2SelectChar.InsertCmd(IDK_DOWN,interval);
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);

	m_cmdMainmenu2Title.InsertCmd(IDK_BACKSAPCE,interval);
	m_cmdMainmenu2Title.InsertCmd(IDK_DOWN,interval);
	m_cmdMainmenu2Title.InsertCmd(IDK_A,interval);

	//SETTING
	m_cmdSetting.InsertCmd(IDK_A,interval);
	m_cmdSetting.InsertCmd(IDK_DOWN,interval);
	m_cmdSetting.InsertCmd(IDK_A,interval);
	m_cmdSetting.InsertCmd(IDK_A,interval+1000);
	int j=0;
	for( j=0;j<abs(m_Config.Difficulty);j++)
	{
		if(m_Config.Difficulty>0)
			m_cmdSetting.InsertCmd(IDK_RIGHT,interval);
		else
			m_cmdSetting.InsertCmd(IDK_LEFT,interval);
	}
	m_cmdSetting.InsertCmd(IDK_DOWN,interval);
	for(j=0;j<abs(m_Config.Rounds);j++)
	{
		if(m_Config.Rounds>0)
		{
			TRACE("StreetFighter IDK_RIGHT\n");
			m_cmdSetting.InsertCmd(IDK_RIGHT,interval);
		}
		else
		{
			TRACE("StreetFighter IDK_LEFT\n");
			m_cmdSetting.InsertCmd(IDK_LEFT,interval);
		}
	}
	m_cmdSetting.InsertCmd(IDK_DOWN,interval);
	for(j=0;j<abs(m_Config.TimeLimit);j++)
	{
		if(m_Config.TimeLimit>0)
			m_cmdSetting.InsertCmd(IDK_RIGHT,interval);
		else
			m_cmdSetting.InsertCmd(IDK_LEFT,interval);
	}
	m_cmdSetting.InsertCmd(IDK_BACKSAPCE,interval);
	TRACE(L"StreetFighter CContrlCenter::Init() compelet\n");
	return true;
}
void CContrlCenter::Run()
{
	GAMEFLOW oldGameFlow=flow_start;
	BOOL bSetting=FALSE;
	Sleep(2000);
	while(1)
	{
		if(g_GameFlow!=oldGameFlow)
		{
			TRACE(L"StreetFighter GameFlow:%d\n",g_GameFlow);
			switch(g_GameFlow)
			{
			case flow_start:
				break;
			case flow_titlemenu:
				if(!bSetting)
				{
					Sleep(1000);
					m_cmdSetting.Excute();
					m_cmdMainmenu2Title.Excute();
					bSetting=TRUE;
				}
				break;
			case flow_demo:
				break;
			case flow_mainmenu:
				if(oldGameFlow==flow_continue)
				{
					Sleep(1500);
					DIHKeyDown(0,IDK_A);      //画廊里有很多画
					m_cmdMainmenu2Title.Excute();
				}
				break;
			case flow_selectchar:
				break;
			default:
				break;
			}
			oldGameFlow = g_GameFlow;
		}
		if(!bSetting)
		{
			DIHKeyDown(0,IDK_A);
			Sleep(1000);
		}
		if(m_bStart)
		{
			if(g_ContrlCenter.m_Players[0].GetCoinNumber()>g_ContrlCenter.m_Config.UnitCoin)
			{
				if(flow_titlemenu==g_GameFlow)
					g_ContrlCenter.m_cmdTitle2SelectChar.Excute();
				else if(flow_demo==g_GameFlow)
					g_ContrlCenter.m_cmdDemo2SelectChar.Excute();
			}
			m_bStart=FALSE;
		}
		Sleep(1);
		GameFlowUpdate();
	}
}
VOID KeyProc(BYTE id, KeyState& state)
{
	//TRACE(L"StreetFighter KeyProc\n");
	if(state.coin==1)//投币
	{
		g_ContrlCenter.m_Players[id].SetCoinNumber(g_ContrlCenter.m_Players[0].GetCoinNumber()+1);
		TRACE(L"StreetFighter insert coin:%d\n",g_ContrlCenter.m_Players[id].GetCoinNumber());
	}
	else if(state.start==1)    //开始
	{
		TRACE(L"StreetFighter start\n");
		g_ContrlCenter.m_bStart=TRUE;
		switch(g_GameFlow)
		{
		case flow_titlemenu:
			break;
		case flow_demo:
			break;
		case flow_continue:
			break;
		default:
			break;
		}
	}//else
	state.coin=0;
	state.start=0;
}