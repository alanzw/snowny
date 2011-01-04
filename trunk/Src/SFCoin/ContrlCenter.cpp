#include "StdAfx.h"
#include "ContrlCenter.h"
#include "GlobalVar.h"



CContrlCenter g_ContrlCenter;


CContrlCenter::CContrlCenter(void):m_cmdTitle2SelectChar(L"m_cmdTitle2SelectChar"),m_cmdDemo2SelectChar(L"m_cmdDemo2SelectChar"),
m_cmdMainmenu2Title(L"m_cmdMainmenu2Title"),m_cmdSetting(L"m_cmdSetting"),m_bIsBusy(FALSE)/*,m_Scene(NULL)*/,m_bCoinsChanged(FALSE)
{
	TRACE(L"StreetFighter CContrlCenter::CContrlCenter\n");
}

CContrlCenter::~CContrlCenter(void)
{
}
WNDPROC OldWindowProc = NULL;
LRESULT CALLBACK NewWindowProc(
							HWND hwnd, 
							UINT uMsg, 
							WPARAM wParam, 
							LPARAM lParam 
							)
{
	static BOOL inited = FALSE;
	static CWnd parentWnd;
	if (inited == FALSE)
	{
		parentWnd.Attach(hwnd);
		g_ContrlCenter.m_Fairy.CreateTransparentWnd(&parentWnd,STANDBYBG,L"SF4Con\\StandbyBG.jpg",CPoint(0,0));
		g_ContrlCenter.m_Fairy.CreateTransparentWnd(&parentWnd,INSERTCOIN,L"SF4Con\\InsertCoin.png",CPoint(g_ContrlCenter.m_ScreenX/2-80,g_ContrlCenter.m_ScreenY-200));
		g_ContrlCenter.m_Fairy.CreateTransparentWnd(&parentWnd,TIMECOUNTER,L"SF4Con\\TimeCounter.gif",CPoint(g_ContrlCenter.m_ScreenX/2-20,200));
		g_ContrlCenter.m_Fairy.CreateTransparentWnd(&parentWnd,CREDITTEXT,L"SF4Con\\CREDIT.png",CPoint(g_ContrlCenter.m_ScreenX/2-150,g_ContrlCenter.m_ScreenY-100));
		g_ContrlCenter.m_Fairy.CreateTransparentWnd(&parentWnd,LOADING,L"SF4Con\\Loading.png",CPoint(g_ContrlCenter.m_ScreenX/2-20,g_ContrlCenter.m_ScreenY-200));
		g_ContrlCenter.m_Fairy.CreateCoinInsert(CREDIT, L"SF4Con\\Num.png", 0, 0, g_ContrlCenter.m_Config.UnitCoin,CPoint(g_ContrlCenter.m_ScreenX/2-20,g_ContrlCenter.m_ScreenY-100));
		inited = TRUE;
	}

	return CallWindowProc(OldWindowProc, hwnd, uMsg, wParam, lParam);
}


bool CContrlCenter::Init()
{
	TRACE(L"StreetFighter CContrlCenter::Init()\n");
	m_Config.ReadConfig(L"config.ini");
	TRACE("StreetFighter GAME WND:%d\n",0);
	HWND wnd=0;
	while(0==wnd)
	{
		wnd=::FindWindow(L"STREET FIGHTER IV",L"STREET FIGHTER IV");
		Sleep(100);
	}
	m_GameWnd.Attach(wnd);
	m_GameWnd.SetWindowPos(NULL,-2,-30,0,0,SWP_NOSIZE);
	OldWindowProc = (WNDPROC)SetWindowLong(wnd, GWL_WNDPROC, (LONG)NewWindowProc);

	TRACE("StreetFighter GAME WND:%d\n",wnd);
	m_ScreenX=GetSystemMetrics(SM_CXSCREEN);
	m_ScreenY=GetSystemMetrics(SM_CYSCREEN);
	DIHSetKDProc(KeyProc);

	//m_Fairy.CreateTransparentWnd(&m_GameWnd,STANDBYBG,L"SF4Con\\StandbyBG.jpg",CPoint(0,0));
	//m_Fairy.CreateTransparentWnd(&m_GameWnd,INSERTCOIN,L"SF4Con\\InsertCoin.png",CPoint(m_ScreenX/2-80,m_ScreenY-200));
	//m_Fairy.CreateTransparentWnd(&m_GameWnd,TIMECOUNTER,L"SF4Con\\TimeCounter.gif",CPoint(m_ScreenX/2-20,200));
	//m_Fairy.CreateTransparentWnd(&m_GameWnd,CREDITTEXT,L"SF4Con\\CREDIT.png",CPoint(m_ScreenX/2-150,m_ScreenY-100));
	//m_Fairy.CreateTransparentWnd(&m_GameWnd,LOADING,L"SF4Con\\Loading.png",CPoint(m_ScreenX/2-20,m_ScreenY-200));
	//m_Fairy.CreateCoinInsert(CREDIT, L"SF4Con\\Num.png", 0, 0, m_Config.UnitCoin,CPoint(m_ScreenX/2-20,m_ScreenY-100));

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
	interval=1000;
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
		SetActiveWindow(m_GameWnd);
		
		//检测状态切换
		if(g_GameFlow!=oldGameFlow)
		{
			TRACE(L"StreetFighter GameFlow change to :%d\n",g_GameFlow);
			switch(g_GameFlow)
			{
			case flow_start:
				break;
			case flow_titlemenu:
				//初始设置
				if(!bSetting)
				{
					m_bIsBusy=TRUE;
					m_cmdSetting.Excute();
					Sleep(2000);
					m_cmdMainmenu2Title.Excute();
					bSetting=TRUE;
					m_bIsBusy=FALSE;
				}
				else
				{
					m_Fairy.ShowWnd(STANDBYBG);
				}
				if(bSetting)
				{
					m_Fairy.ShowWnd(INSERTCOIN);
					m_Fairy.ShowWnd(CREDITTEXT);
					m_Fairy.ShowWnd(CREDIT);
				}
				break;
			case flow_demo:
				m_Fairy.HideWnd(STANDBYBG);
				if(bSetting)
				{
					m_Fairy.ShowWnd(INSERTCOIN);
					m_Fairy.ShowWnd(CREDITTEXT);
					m_Fairy.ShowWnd(CREDIT);
				}
				break;
			case flow_mainmenu:
				m_Fairy.ShowWnd(STANDBYBG);
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
					m_Fairy.HideWnd(INSERTCOIN);
					m_Fairy.HideWnd(STANDBYBG);
					m_Fairy.HideWnd(LOADING);
					//m_Fairy.CreateTransparentWnd(&m_GameWnd,TIMECOUNTER,L"SF4Con\\TimeCounter.gif",CPoint(m_ScreenX/2-20,200));
					//m_Fairy.ShowWnd(TIMECOUNTER);
					DWORD cunt=20;
					while(cunt-->0)
					{
						Sleep(1000);
						TRACE(L"StreetFighter count:%d\n",cunt);
						//游戏开始
						if(g_GameFlow==flow_game)
							break;
					}
					m_Fairy.HideWnd(TIMECOUNTER);
					m_Fairy.DestroyWnd(TIMECOUNTER);
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
		}//IF
		
		if(m_bCoinsChanged)
		{
			TRACE(L"StreetFighter m_bCoinsChanged\n");
			//m_Fairy.DestroyWnd(CREDIT);
			TRACE(L"StreetFighter m_bCoinsChanged\n");
			int life=m_Players[0].GetCoinNumber()/m_Config.UnitCoin;
			int rem=m_Players[0].GetCoinNumber()%m_Config.UnitCoin;
			//m_Fairy.CreateCoinInsert(CREDIT, L"SF4Con\\Num.png", life, rem, m_Config.UnitCoin,CPoint(m_ScreenX/2-20,m_ScreenY-100));
			//m_Fairy.ShowWnd(CREDIT);
			m_bCoinsChanged=FALSE;
		}
		
		if(m_bStart)
		{
			TRACE(L"SF4 Start game, %d coins and unit coins is %d", m_Players[0].GetCoinNumber(), m_Config.UnitCoin);
			if(m_Players[0].GetCoinNumber() >= m_Config.UnitCoin)
			{
				m_Fairy.ShowWnd(STANDBYBG);
				m_Fairy.ShowWnd(LOADING);
				//进入角色选择
				if(flow_titlemenu==g_GameFlow)
				{
					TRACE(L"SF4 game_flow is flow_titlemenu");
					// 先减币, cxb
					m_Players[0].SetCoinNumber(m_Players[0].GetCoinNumber()-m_Config.UnitCoin);
					m_cmdTitle2SelectChar.Excute();
				}
				else if(flow_demo==g_GameFlow)
				{
					TRACE(L"SF4 game_flow is flow_titlemenu");
					// 先减币, cxb
					m_Players[0].SetCoinNumber(m_Players[0].GetCoinNumber()-m_Config.UnitCoin);
					m_cmdDemo2SelectChar.Excute();
				}
				m_Fairy.HideWnd(INSERTCOIN);
				m_bCoinsChanged=TRUE;
			}
			m_bStart=FALSE;
		}
		Sleep(1);
		//检测是否gameover状态
		if(flow_game==g_GameFlow)
		{
			GameFlowUpdate();
		}
	}
}
VOID KeyProc(BYTE id, KeyState& state)
{
 	if(state.coin==1)//投币,任何时候都有效
 	{
 		g_ContrlCenter.m_Players[id].SetCoinNumber(g_ContrlCenter.m_Players[id].GetCoinNumber()+1);
 		g_ContrlCenter.m_bCoinsChanged=TRUE;
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
 		if(g_ContrlCenter.m_Players[id].GetCoinNumber()>=g_ContrlCenter.m_Config.UnitCoin)
 		{
 			DIHKeyDown(0,IDK_ESC);
 			//减币
 			g_ContrlCenter.m_Players[0].SetCoinNumber(g_ContrlCenter.m_Players[0].GetCoinNumber()-g_ContrlCenter.m_Config.UnitCoin);
 			g_ContrlCenter.m_bCoinsChanged=TRUE;
 			TRACE(L"StreetFighter continue coin %d\n",g_ContrlCenter.m_Players[id].GetCoinNumber());
 		}
 		else
 			TRACE(L"StreetFighter continue failed coin %d\n",g_ContrlCenter.m_Players[id].GetCoinNumber());
 		state.start=0;
 	}
 	if(g_ContrlCenter.m_bIsBusy)
 	{
 		state.start=0;
 		//TRACE(L"StreetFighter start shielded\n");
 	}
 	if(g_GameFlow!=flow_game)
 		state.esc=0;
 #ifndef DEBUG
 	state.backspace=0;
 	state.enter=0;
 #endif
}