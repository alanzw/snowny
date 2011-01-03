#include "StdAfx.h"
#include "ContrlCenter.h"
#include "GlobalVar.h"



CContrlCenter g_ContrlCenter;


CContrlCenter::CContrlCenter(void):m_cmdTitle2SelectChar(L"m_cmdTitle2SelectChar"),m_cmdDemo2SelectChar(L"m_cmdDemo2SelectChar"),
m_cmdMainmenu2Title(L"m_cmdMainmenu2Title"),m_cmdSetting(L"m_cmdSetting"),m_bIsBusy(FALSE)/*,m_Scene(NULL)*/
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

	m_Fairy.CreateTransparentWnd(STANDBYBG,L"D:\\StandbyBG.jpg",CPoint(0,0));
	m_Fairy.CreateCoinInsert(INSERTCOIN, 4, 2, 3, CPoint(0,0));
	

	////CWnd* pTargeWnd = FindWindow(NULL,"STREET FIGHTER IV");
	//HWND pTargeWnd = GetDesktopWindow();
	//RECT r;
	//r.left=0;
	//r.top=0;
	//r.bottom=GetSystemMetrics(SM_CYSCREEN)/2;
	//r.right=GetSystemMetrics(SM_CXSCREEN)/2;
	//if(NULL==pTargeWnd)
	//	pTargeWnd=GetDesktopWindow();
	//TRACE(L"StreetFighter create window \n");
	//BOOL ret=m_CoverScreen.Create(NULL,NULL,WS_VISIBLE,r,NULL,12345);
	//TRACE(L"StreetFighter create window ret:%d\n",ret);

	//m_CoverScreen.AddScreen(L"./SF4Con/01.jpg");
	//m_CoverScreen.AddScreen(L"./SF4Con/02.jpg");


	DWORD interval=500;
	m_cmdMainmenu2Title.InsertCmd(IDK_BACKSAPCE,interval);
	m_cmdMainmenu2Title.InsertCmd(IDK_DOWN,interval);
	m_cmdMainmenu2Title.InsertCmd(IDK_A,interval);

	interval=400;
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval+1000);
	m_cmdTitle2SelectChar.InsertCmd(IDK_DOWN,interval);
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval);     //进入mainmenu
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval+1000);
	interval=1000;
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdTitle2SelectChar.InsertCmd(IDK_A,interval);

	interval=400;
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval+1000);
	m_cmdDemo2SelectChar.InsertCmd(IDK_DOWN,interval);
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);//进入mainmenu
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval+1000);
	interval=1000;
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);
	m_cmdDemo2SelectChar.InsertCmd(IDK_A,interval);


	//SETTING,原点：title
	interval=400;
	m_cmdSetting.InsertCmd(IDK_A,interval+1000);
	m_cmdSetting.InsertCmd(IDK_DOWN,interval);
	m_cmdSetting.InsertCmd(IDK_A,interval);     //进入mainmenu
	m_cmdSetting.InsertCmd(IDK_A,interval+1000);//进入setting
	interval=200;
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
	//进入select
	interval=1000;
	m_cmdSetting.InsertCmd(IDK_A,interval);
	m_cmdSetting.InsertCmd(IDK_A,interval);
	m_cmdSetting.InsertCmd(IDK_A,interval);
	//返回到mainmenu
	interval=800;
	m_cmdSetting.InsertCmd(IDK_BACKSAPCE,2500);
	m_cmdSetting.InsertCmd(IDK_UP,interval);
	m_cmdSetting.InsertCmd(IDK_A,interval);
	TRACE(L"StreetFighter CContrlCenter::Init() compelet\n");
	return true;
}
void CContrlCenter::Run()
{
	GAMEFLOW oldGameFlow=flow_start;
	BOOL bSetting=FALSE;
	Sleep(2000);
	//快速跨越开机动画
	while(flow_titlemenu!=g_GameFlow)
	{
		DIHKeyDown(0,IDK_A);
		Sleep(1000);
	}
	while(1)
	{
		//检测状态切换
		if(g_GameFlow!=oldGameFlow)
		{
			TRACE(L"StreetFighter GameFlow:%d\n",g_GameFlow);
			switch(g_GameFlow)
			{
			case flow_start:
				break;
			case flow_titlemenu:
				m_Fairy.ShowWnd(STANDBYBG);
				m_Fairy.ShowWnd(INSERTCOIN);
				//初始设置
				if(!bSetting)
				{
					//m_CoverScreen.ShowWindow(SW_SHOW);
					m_bIsBusy=TRUE;
					m_cmdSetting.Excute();
					Sleep(2000);
					m_cmdMainmenu2Title.Excute();
					bSetting=TRUE;
					m_bIsBusy=FALSE;
					//m_CoverScreen.ShowWindow(SW_HIDE);
				}
				m_Fairy.HideWnd(STANDBYBG);
				break;
			case flow_demo:
				break;
			case flow_mainmenu:
				//游戏结束后回到mainmenu
				if(oldGameFlow==flow_continue)
				{
					
					m_bIsBusy=TRUE;
					Sleep(1500);
					DIHKeyDown(0,IDK_A);      //画廊里有很多画
					m_cmdMainmenu2Title.Excute();
					m_bIsBusy=FALSE;
				}
				break;
			case flow_selectchar:
				{
					DWORD cunt=10;
					while(cunt-->0)
					{
						Sleep(1000);
						TRACE(L"StreetFighter count:%d\n",cunt);
						//游戏开始
						if(g_GameFlow==flow_game)
							break;
					}
					//默认角色
					if(g_GameFlow!=flow_game)
					{
						DIHKeyDown(0,IDK_A);
						Sleep(1000);
						DIHKeyDown(0,IDK_A);
					}
				}
				break;
			default:
				break;
			}
			oldGameFlow = g_GameFlow;
		}

		if(m_bStart)
		{
			TRACE(L"StreetFighter m_bStart game_flow %d  coin %d\n",g_GameFlow,g_ContrlCenter.m_Players[0].GetCoinNumber());
			if(g_ContrlCenter.m_Players[0].GetCoinNumber()>g_ContrlCenter.m_Config.UnitCoin)
			{
				//进入角色选择
				if(flow_titlemenu==g_GameFlow)
				{
					g_ContrlCenter.m_cmdTitle2SelectChar.Excute();
					//减币
					g_ContrlCenter.m_Players[0].SetCoinNumber(g_ContrlCenter.m_Players[0].GetCoinNumber()-g_ContrlCenter.m_Config.UnitCoin);
				}
				else if(flow_demo==g_GameFlow)
				{
					g_ContrlCenter.m_cmdDemo2SelectChar.Excute();
					//减币
					g_ContrlCenter.m_Players[0].SetCoinNumber(g_ContrlCenter.m_Players[0].GetCoinNumber()-g_ContrlCenter.m_Config.UnitCoin);
				}
				
			}
			m_bStart=FALSE;
		}
		Sleep(1);
		//检测是否gameover状态
		if(flow_game==g_GameFlow)
			GameFlowUpdate();
	}
}
VOID KeyProc(BYTE id, KeyState& state)
{
	if(state.coin==1)//投币,任何时候都有效
	{
		g_ContrlCenter.m_Players[id].SetCoinNumber(g_ContrlCenter.m_Players[id].GetCoinNumber()+1);
		state.coin=0;
		TRACE(L"StreetFighter insert coin:%d\n",g_ContrlCenter.m_Players[id].GetCoinNumber());
	}
	if(state.start==1&&(flow_titlemenu==g_GameFlow||flow_demo==g_GameFlow))//开始
	{
		TRACE(L"StreetFighter start\n");
		g_ContrlCenter.m_bStart=TRUE;
		state.start=0;
	}
	//start key to continue
	if(state.start==1&&flow_continue==g_GameFlow)
	{
		TRACE(L"StreetFighter continue\n");
		if(g_ContrlCenter.m_Players[id].GetCoinNumber()>g_ContrlCenter.m_Config.UnitCoin)
		{
			DIHKeyDown(0,IDK_ESC);
			//减币
			g_ContrlCenter.m_Players[0].SetCoinNumber(g_ContrlCenter.m_Players[0].GetCoinNumber()-g_ContrlCenter.m_Config.UnitCoin);
			TRACE(L"StreetFighter continue coin %d\n",g_ContrlCenter.m_Players[id].GetCoinNumber());
		}
		else
			TRACE(L"StreetFighter continue failed coin %d\n",g_ContrlCenter.m_Players[id].GetCoinNumber());
		state.start=0;
	}
	if(g_ContrlCenter.m_bIsBusy)
	{
		state.start=0;
		TRACE(L"StreetFighter start shielded\n");
	}
	if(g_GameFlow!=flow_game)
		state.esc=0;
#ifndef DEBUG
	state.backspace=0;
	state.enter=0;
#endif
}