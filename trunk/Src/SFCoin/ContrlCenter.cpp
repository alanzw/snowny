#include "StdAfx.h"
#include "ContrlCenter.h"
#include "GlobalVar.h"

#pragma data_seg (".HookSection")
// Shared instance for all processes.
HHOOK g_hMouse = NULL;
#pragma data_seg ()

CContrlCenter g_ContrlCenter;


CContrlCenter::CContrlCenter(void):m_cmdDemo2Titile(L"m_cmdDemo2Titile")
,m_cmdtitle2Mainmenu(L"m_cmdtitle2Mainmenu")
,m_mainmenu2SelectChar(L"m_mainmenu2SelectChar")
,m_mainMenu2Title(L"m_mainMenu2Title")
,m_mainMenuSetting2SelectChar(L"m_mainMenuSetting2SelectChar")
,m_selectChar2mainMenu(L"m_selectChar2mainMenu")
,m_ActivePlayer(0)
, m_GameWnd(NULL)
{
	m_Players[0] = CPlayer(0);
	m_Players[1] = CPlayer(1);
	TRACE(L"StreetFighter CContrlCenter::CContrlCenter\n");
}

CContrlCenter::~CContrlCenter(void)
{
	UnhookWindowsHookEx(g_hMouse);
}
WNDPROC OldWindowProc = NULL;
LRESULT CALLBACK NewWindowProc(
							HWND hwnd, 
							UINT uMsg, 
							WPARAM wParam, 
							LPARAM lParam 
							)
{
	//TRACE("aaaa NewWindowProc WM_LBUTTONDBLCLK before");
	static BOOL inited = FALSE;
	CWnd *parentWnd;

	if (inited == FALSE)
	{
		TRACE("aaaa NewWindowProc WM_LBUTTONDBLCLK after");
		ShowCursor(FALSE);
		parentWnd = new CWnd;
		parentWnd->Attach(hwnd);
		int screenWidth =GetSystemMetrics(SM_CXSCREEN);
		int screentHeight =GetSystemMetrics(SM_CYSCREEN);
		// 这里要用正确的width和height计算坐标，不然窗口可能不显示，cxb
		g_ContrlCenter.m_Fairy.CreateTransparentWnd(parentWnd,STANDBYBG,L"SF4Con\\StandbyBG.jpg", CPoint(0,0), screenWidth, screentHeight);
		g_ContrlCenter.m_Fairy.CreateTransparentWnd(parentWnd,INSERTCOIN,L"SF4Con\\InsertCoin.png",CPoint(screenWidth/2-80, screentHeight - 200));
		g_ContrlCenter.m_Fairy.CreateTransparentWnd(parentWnd,TIMECOUNTER,L"SF4Con\\TimeCounter.gif",CPoint(screenWidth/2-20, 200));
		g_ContrlCenter.m_Fairy.CreateTransparentWnd(parentWnd,CREDITTEXT,L"SF4Con\\CREDIT.png",CPoint(screenWidth/2-150, screentHeight - 100));
		g_ContrlCenter.m_Fairy.CreateTransparentWnd(parentWnd,LOADING,L"SF4Con\\Loading.png",CPoint(screenWidth/2-20, screentHeight - 200));
		g_ContrlCenter.m_Fairy.CreateCoinInsert(CREDIT, L"SF4Con\\Num.png", 0, 0, g_Config.UnitCoin,CPoint(screenWidth / 2 - 20, screentHeight - 100));
		inited = TRUE;
	}

	return CallWindowProc(OldWindowProc, hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc( int nCode,
								    WPARAM wParam,
								    LPARAM lParam
								   )
{
	return 1;
}


bool CContrlCenter::Init()
{
	TRACE(L"StreetFighter CContrlCenter::Init()\n");
	g_Config.ReadConfig(L"./config.ini");
	TRACE("StreetFighter GAME WND:%d\n",0);
	while(0 == m_GameWnd)
	{
		m_GameWnd = ::FindWindow(L"STREET FIGHTER IV",L"STREET FIGHTER IV");
		Sleep(100);
	}
	::SetWindowPos(m_GameWnd, NULL,-(GetSystemMetrics(SM_CXBORDER) + 2),-(GetSystemMetrics(SM_CYCAPTION) + 3),0,0,SWP_NOSIZE);
	OldWindowProc = (WNDPROC)SetWindowLong(m_GameWnd, GWL_WNDPROC, (LONG)NewWindowProc);

	HMODULE handle = GetModuleHandleA("SFCoin.dll");
	g_hMouse = SetWindowsHookEx(WH_MOUSE, LowLevelMouseProc, handle, 0);
	TRACE("StreetFighter GAME WND:%d\n",m_GameWnd);
	DIHSetKDProc(KeyProc);

	int interval;

	{
		interval = 400;
		m_cmdDemo2Titile.InsertCmd(IDK_OK, interval);
	}
	{
		interval = 400;
		m_cmdtitle2Mainmenu.InsertCmd(IDK_OK, interval);
		m_cmdtitle2Mainmenu.InsertCmd(IDK_DOWN, interval);
		m_cmdtitle2Mainmenu.InsertCmd(IDK_OK, interval);
	}
	{
		m_mainmenu2SelectChar.InsertCmd(IDK_OK, 1000);
		m_mainmenu2SelectChar.InsertCmd(IDK_OK, 1000);
		m_mainmenu2SelectChar.InsertCmd(IDK_OK, 1000);
		m_mainmenu2SelectChar.InsertCmd(IDK_OK, 1000);
	}
	{
		interval = 600;
		m_mainMenu2Title.InsertCmd(IDK_BACKSPACE,interval);
		m_mainMenu2Title.InsertCmd(IDK_DOWN,interval);
		m_mainMenu2Title.InsertCmd(IDK_OK,interval);
	}

	m_mainMenuSetting2SelectChar.InsertCmd(IDK_OK,interval+1000);//进入setting
	interval= 500;

	int j=0;
	for( j=0;j<abs(g_Config.Difficulty);j++)
	{
		if(g_Config.Difficulty>0)
			m_mainMenuSetting2SelectChar.InsertCmd(IDK_RIGHT,interval);
		else
			m_mainMenuSetting2SelectChar.InsertCmd(IDK_LEFT,interval);
	}
	m_mainMenuSetting2SelectChar.InsertCmd(IDK_DOWN,interval);
	for(j=0;j<abs(g_Config.Rounds);j++)
	{
		if(g_Config.Rounds>0)
		{
			TRACE("StreetFighter IDK_RIGHT\n");
			m_mainMenuSetting2SelectChar.InsertCmd(IDK_RIGHT,interval);
		}
		else
		{
			TRACE("StreetFighter IDK_LEFT\n");
			m_mainMenuSetting2SelectChar.InsertCmd(IDK_LEFT,interval);
		}
	}
	m_mainMenuSetting2SelectChar.InsertCmd(IDK_DOWN,interval);
	for(j=0;j<abs(g_Config.TimeLimit);j++)
	{
		if(g_Config.TimeLimit>0)
			m_mainMenuSetting2SelectChar.InsertCmd(IDK_RIGHT,interval);
		else
			m_mainMenuSetting2SelectChar.InsertCmd(IDK_LEFT,interval);
	}
	//进入select
	interval=1000;
	m_mainMenuSetting2SelectChar.InsertCmd(IDK_OK,interval);
	m_mainMenuSetting2SelectChar.InsertCmd(IDK_OK,interval);
	m_mainMenuSetting2SelectChar.InsertCmd(IDK_OK,interval);

	//返回到mainmenu
	//interval=1000;
	m_selectChar2mainMenu.InsertCmd(IDK_BACKSPACE,2000);
	m_selectChar2mainMenu.InsertCmd(IDK_UP,interval);
	m_selectChar2mainMenu.InsertCmd(IDK_OK,interval);

	// 对战模式设置
	{
		m_VersusSetting.InsertCmd(IDK_DOWN, 1000);	// 选择对战模式
		m_VersusSetting.InsertCmd(IDK_OK, 1000);	// 确定
		m_VersusSetting.InsertCmd(IDK_OK, 1000);	// 确定
		BYTE direct = g_Config.Rounds > 0 ? IDK_RIGHT : IDK_LEFT;
		for(j=0;j<abs(g_Config.Rounds);j++)
		{
			TRACE("StreetFighter %s\n", direct == IDK_RIGHT ? "IDK_RIGHT" : "IDK_LEFT");
			m_VersusSetting.InsertCmd(direct,500);
		}
		m_VersusSetting.InsertCmd(IDK_DOWN, 500);
		direct = g_Config.TimeLimit > 0 ? IDK_RIGHT : IDK_LEFT;
		for(j=0;j<abs(g_Config.TimeLimit);j++)
		{
			m_VersusSetting.InsertCmd(direct,500);
		}
		m_VersusSetting.InsertCmd(IDK_OK, 1000);	// 确定
	}

	TRACE(L"StreetFighter CContrlCenter::Init() complete\n");
	return true;
}


void CContrlCenter::Run()
{
	GAMEFLOW oldGameFlow=flow_start;
	GAMEFLOW oldStartGameFlow=flow_mainmenu;
	Sleep(2000);
	//快速跨越开机动画
	while(flow_titlemenu!=g_GameFlow)
	{
		DIHKeyDown(0,IDK_OK);
		Sleep(500);
	}
	// 锁定1p 2p 游戏手柄
	DIHLockInput(0);
	DIHLockInput(1);

	m_Setted=FALSE;
	m_Fairy.ShowWnd(LOADING);
	Setting();
	m_Fairy.HideWnd(LOADING);
	m_Setted=TRUE;
	while(1)
	{
		SetActiveWindow(m_GameWnd);
		//检测是否gameover状态
		GameFlowUpdate();
		
		//检测状态切换
		if(g_GameFlow!=oldGameFlow)
		{
			TRACE(L"StreetFighter GameFlow change to :%d\n",g_GameFlow);
			switch(g_GameFlow)
			{
			case flow_start:
				m_Fairy.ShowWnd(LOADING);
				break;
			case flow_titlemenu:
				m_Fairy.HideWnd(LOADING);
				m_Fairy.ShowWnd(STANDBYBG);
				m_Fairy.ShowWnd(INSERTCOIN);
				m_Fairy.ShowWnd(CREDITTEXT);
				m_Fairy.ShowWnd(CREDIT);
				break;
			case flow_demo:
				m_Fairy.HideWnd(STANDBYBG);
				m_Fairy.ShowWnd(INSERTCOIN);
				m_Fairy.ShowWnd(CREDITTEXT);
				m_Fairy.ShowWnd(CREDIT);
				break;
			case flow_mainmenu:
				//游戏结束后回到mainmenu
				if(oldGameFlow==flow_continue)
				{
					m_Fairy.ShowWnd(STANDBYBG);
					m_Fairy.ShowWnd(CREDITTEXT);
					m_Fairy.ShowWnd(CREDIT);
					Sleep(1500);
					DIHKeyDown(0,IDK_OK);      //画廊里有很多画
					m_mainMenu2Title.Excute(m_ActivePlayer);
				}
				break;
			case flow_selectchar:
				{
					m_Fairy.HideWnd(INSERTCOIN);
					m_Fairy.HideWnd(STANDBYBG);
					m_Fairy.HideWnd(LOADING);
					m_Fairy.ShowWnd(CREDITTEXT);
					m_Fairy.ShowWnd(CREDIT);
					m_Fairy.ShowWnd(TIMECOUNTER);

					DWORD time = GetTickCount();
					while (GetTickCount() - time < 22000)
					{
						Sleep(1);
						UpdateCoinShow();// 循环里头无法及时响应投币动作，故作此重构，cxb

						//游戏开始
						if(g_GameFlow==flow_game) break;
					}
					m_Fairy.HideWnd(TIMECOUNTER);
					//默认角色
					if(g_GameFlow!=flow_game)
					{
						TRACE(TEXT("SF4 select default char"));
						DIHKeyDown(m_ActivePlayer, IDK_OK);
						Sleep(1000);
						DIHKeyDown(m_ActivePlayer, IDK_OK);
					}
				}
				break;
			default:
				break;
			}
			oldGameFlow = g_GameFlow;
		}//IF
		
		UpdateCoinShow();
		
		// 遍历玩家输入
		for (int i = 0; i < 2; i ++)
		{
			if (m_Players[i].IsClickStart())
			{
				int coinsOfThePlayer = m_Players[i].GetCoinNumber();
				if (g_Config.CoinMode == 0) // 单式
				{
					coinsOfThePlayer = m_Players[0].GetCoinNumber();
				}
				TRACE(L"SF4 Start game,player[%d] %d coins and unit coins is %d gameflow %d",
				i, coinsOfThePlayer, g_Config.UnitCoin, g_GameFlow);

				m_Fairy.ShowWnd(STANDBYBG);
				m_Fairy.ShowWnd(LOADING);
				m_Fairy.HideWnd(INSERTCOIN);

				if (g_GameFlow == flow_demo)
				{
					TRACE(L"aaaa SF4 m_bStart is flow_demo");
					m_cmdDemo2Titile.Excute(i);
				}
				if (g_GameFlow == flow_titlemenu)
				{
					TRACE(L"aaaa SF4 m_bStart is flow_titlemenu");
					m_cmdtitle2Mainmenu.Excute(i);
				}
				if (g_GameFlow == flow_mainmenu)
				{
					Sleep(400);
					m_mainmenu2SelectChar.Excute(i);
					TRACE(L"aaaa SF4 m_bStart is flow_mainmenu");
				}
			}
		}

		// 根据是否在游戏中来锁定和解锁输入
		// 选人界面中通过截获并模拟操作即不会受锁定的影响，表现良好
		// 把选人纳入锁定范围的诱因：游戏逻辑中选人界面可以回退
		if(flow_game==g_GameFlow)
		{
			DIHUnlockInput(m_ActivePlayer);
		}
		else
		{
			DIHLockInput(m_ActivePlayer);
		}
		//esc 呼出设置程序
		if( GetAsyncKeyState(VK_ESCAPE) )
		{
			TRACE(L"StreetFighter esc DOWN\n");
			STARTUPINFO si = {0};
			si.cb = sizeof(si);
			PROCESS_INFORMATION pi = {0};
			CreateProcessW(TEXT("SFCoinSet.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			ExitProcess(0);
		}
		Sleep(1);
	}
}

void CContrlCenter::UpdateCoinShow()
{
	for (int i = 0; i < 2; i ++)
	{
		if(m_Players[i].CoinsChanged())
		{
			int totalCoins = m_Players[i].GetCoinNumber();
			int life = totalCoins / g_Config.UnitCoin;
			int rem = totalCoins % g_Config.UnitCoin;
			TRACE(L"StreetFighter m_bCoinsChanged %d %d\n", totalCoins, g_Config.CoinMode);

			m_Fairy.ResetCoinInsert(life, rem, g_Config.UnitCoin);
			m_Fairy.ShowWnd(CREDITTEXT);
			m_Fairy.ShowWnd(CREDIT);
		}
	}
	
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

void CContrlCenter::Setting()
{
	TRACE("SF4 Setting <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	for (int i = 0; i < 2; i ++)
	{
		TRACE("SF4 Setting[%d]", i);
		m_cmdtitle2Mainmenu.Excute(0);
		if (!CheckGameFlow(flow_mainmenu, 10000))
		{
			TRACE("SF4 m_cmdtitle2Mainmenu timeout !!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		}

		if (i == 0)
		{
			m_mainMenuSetting2SelectChar.Excute(0);
			if (!CheckGameFlow(flow_selectchar, 10000))
			{
				TRACE("SF4 m_mainMenuSetting2SelectChar timeout !!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			}
		}
		else
		{
			m_VersusSetting.Excute(0);
			if (!CheckGameFlow(flow_selectchar, 10000))
			{
				TRACE("SF4 m_VersusSetting timeout !!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			}
		}

		m_selectChar2mainMenu.Excute(0);
		if (!CheckGameFlow(flow_mainmenu, 10000))
		{
			TRACE("SF4 m_selectChar2mainMenu timeout !!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		}

		m_mainMenu2Title.Excute(0);
		if (!CheckGameFlow(flow_titlemenu, 10000))
		{
			TRACE("SF4 m_mainMenu2Title timeout !!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		}

		Sleep(1000);
	}
	TRACE("SF4 Setting >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}

VOID KeyProc(BYTE id, KeyState& state)
{
 	if(state.coin==1)//投币,任何时候都有效
 	{
		g_ContrlCenter.m_Players[id].IncrementCoin();
 		TRACE(L"StreetFighter player [%d] insert coin:%d\n", id, g_ContrlCenter.m_Players[id].GetCoinNumber());
 	}
 	else if(state.start==1)
 	{
		//开始,暂时游戏中不能开始，等待对战模式的支持
		if (flow_titlemenu == g_GameFlow || flow_demo==g_GameFlow)
		{
			TRACE(L"StreetFighter start\n");
			g_ContrlCenter.m_Players[id].ClickStart();
		}
		else if (flow_continue==g_GameFlow)
		{
			TRACE(L"StreetFighter continue\n");
			if (g_ContrlCenter.m_Players[id].ClickStart())
			{
 				DIHKeyDown(id, IDK_CONTINUE);
 				//减币
 				TRACE(L"StreetFighter continue coin %d\n",g_ContrlCenter.m_Players[id].GetCoinNumber());
 			}
 			else
			{
 				TRACE(L"StreetFighter continue failed coin %d\n",g_ContrlCenter.m_Players[id].GetCoinNumber());
			}
		}
		else if (flow_game == g_GameFlow)	// 对战模式要修改此处
		{
			DIHLockInputOnce(id);// 暂不支持对战模式，所以不允许开始
		}
 	}
	else if (g_GameFlow == flow_selectchar && g_ContrlCenter.m_Setted)
	{
		if (state.up == 1){	DIHKeyDown(id, IDK_UP);	}
		else if (state.down == 1){	DIHKeyDown(id, IDK_DOWN);	}
		else if (state.left == 1){	DIHKeyDown(id, IDK_LEFT);	}
		else if (state.right == 1){	DIHKeyDown(id, IDK_RIGHT);	}
		else if (state.ok == 1){	DIHKeyDown(id, IDK_OK);	}
	}
}