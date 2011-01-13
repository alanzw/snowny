#include "StdAfx.h"
#include "ContrlCenter.h"
#include "GlobalVar.h"

#pragma data_seg (".HookSection")
// Shared instance for all processes.
HHOOK g_hMouse = NULL;
#pragma data_seg ()

CContrlCenter g_ContrlCenter;


CContrlCenter::CContrlCenter(void)
: m_cmdDemo2Titile(flow_demo, flow_titlemenu, L"m_cmdDemo2Titile")
, m_cmdtitle2Mainmenu(flow_titlemenu, flow_mainmenu, L"m_cmdtitle2Mainmenu")
, m_mainmenu2SelectChar(flow_mainmenu, flow_selectchar, L"m_mainmenu2SelectChar")
, m_mainMenu2Title(flow_mainmenu, flow_titlemenu, L"m_mainMenu2Title")
, m_mainMenuSetting2SelectChar(flow_mainmenu, flow_selectchar, L"m_mainMenuSetting2SelectChar")
, m_selectChar2mainMenu(flow_selectchar, flow_mainmenu, L"m_selectChar2mainMenu")
, m_VersusSetting(flow_mainmenu, flow_selectchar, L"m_VersusSetting")
, m_VersusMainmenu2SelectChar(flow_mainmenu, flow_selectchar, L"m_VersusMainmenu2SelectChar")
, m_Game2MainMenu(flow_game, flow_mainmenu, L"m_Game2MainMenu")
, m_ActivePlayer(0)
, m_GameWnd(NULL)
, m_IsArcadeMode(TRUE)
{
	m_Players[0] = CPlayer(0);
	m_Players[1] = CPlayer(1);
	m_Fairy = new CFairy;
	TRACE(L"StreetFighter CContrlCenter::CContrlCenter\n");
}

CContrlCenter::~CContrlCenter(void)
{
	//UnhookWindowsHookEx(g_hMouse);
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
		g_ContrlCenter.m_Fairy->CreateBKWnd();
		TRACE("aaaa NewWindowProc CreateBKWnd");
		//g_ContrlCenter.m_Fairy->SetParent(parentWnd);
		// 这里要用正确的width和height计算坐标，不然窗口可能不显示，cxb
		g_ContrlCenter.m_Fairy->CreateTransparentPic(parentWnd,STANDBYBG,L"SF4Con\\StandbyBG.jpg", CPoint(0,0), screenWidth, screentHeight);
		g_ContrlCenter.m_Fairy->CreateTransparentPic(parentWnd,INSERTCOIN,L"SF4Con\\InsertCoin.png",CPoint(screenWidth/2-80, screentHeight - 160));
		g_ContrlCenter.m_Fairy->CreateTransparentPic(parentWnd,TIMECOUNTER,L"SF4Con\\TimeCounter.gif",CPoint(screenWidth/2-35, 200));
		g_ContrlCenter.m_Fairy->CreateTransparentPic(parentWnd,LOADING,L"SF4Con\\Loading.png",CPoint(screenWidth/2-50, screentHeight - 120));
		
		int posX0 = g_Config.CoinMode == 0 ? (screenWidth / 2) : (screenWidth / 4),
			posX1 = g_Config.CoinMode == 0 ? (screenWidth / 2) : (screenWidth / 4 * 3);
		g_ContrlCenter.m_Fairy->CreateTransparentPic(parentWnd,CREDITTEXT0,L"SF4Con\\CREDIT.png",CPoint(posX0-100, screentHeight - 70));
		g_ContrlCenter.m_Fairy->CreateTransparentPic(parentWnd,CREDITTEXT1,L"SF4Con\\CREDIT.png",CPoint(posX1-100, screentHeight - 70));
		g_ContrlCenter.m_Fairy->CreateCoinInsert(CREDIT0, L"SF4Con\\Num.png", 0, 0, g_Config.UnitCoin,CPoint(posX0, screentHeight - 70));
		g_ContrlCenter.m_Fairy->CreateCoinInsert(CREDIT1, L"SF4Con\\Num.png", 0, 0, g_Config.UnitCoin,CPoint(posX1, screentHeight - 70));
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
	//g_hMouse = SetWindowsHookEx(WH_MOUSE, LowLevelMouseProc, handle, 0);
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
		interval = 200;
	}
	interval= 500;
	m_mainMenuSetting2SelectChar.InsertCmd(IDK_DOWN,interval);
	for(j=0;j<abs(g_Config.TimeLimit);j++)
	{
		if(g_Config.TimeLimit>0)
			m_mainMenuSetting2SelectChar.InsertCmd(IDK_RIGHT,interval);
		else
			m_mainMenuSetting2SelectChar.InsertCmd(IDK_LEFT,interval);

		interval = 100;
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
		interval = 500;
		m_VersusSetting.InsertCmd(IDK_DOWN, 1000);	// 选择对战模式
		m_VersusMainmenu2SelectChar.InsertCmd(IDK_DOWN, 1000);	// 选择对战模式
		m_VersusSetting.InsertCmd(IDK_OK, 500);	// 确定
		m_VersusMainmenu2SelectChar.InsertCmd(IDK_OK, 500);	// 确定
		m_VersusSetting.InsertCmd(IDK_OK, 1000);	// 确定
		m_VersusMainmenu2SelectChar.InsertCmd(IDK_OK, 1000);	// 确定
		BYTE direct = g_Config.Rounds > 0 ? IDK_RIGHT : IDK_LEFT;
		for(j=0;j<abs(g_Config.Rounds);j++)
		{
			TRACE("StreetFighter %s\n", direct == IDK_RIGHT ? "IDK_RIGHT" : "IDK_LEFT");
			m_VersusSetting.InsertCmd(direct,interval);
			interval = 100;
		}
		m_VersusSetting.InsertCmd(IDK_DOWN, 500);
		direct = g_Config.TimeLimit > 0 ? IDK_RIGHT : IDK_LEFT;
		interval = 500;
		for(j=0;j<abs(g_Config.TimeLimit);j++)
		{
			m_VersusSetting.InsertCmd(direct,500);
			interval = 100;
		}
		m_VersusSetting.InsertCmd(IDK_OK, 1000);	// 确定
		m_VersusMainmenu2SelectChar.InsertCmd(IDK_OK, 1000);	// 确定
	}

	{
		m_Game2MainMenu.InsertCmd(IDK_UP, 1500);	// 退出窗口选择退出
		m_Game2MainMenu.InsertCmd(IDK_OK, 500);		// 确定
		m_Game2MainMenu.InsertCmd(IDK_UP, 1500);	// 退回到主菜单
		m_Game2MainMenu.InsertCmd(IDK_OK, 500);		// 确定;
		m_Game2MainMenu.InsertCmd(IDK_OK, 2000);	// 画廊很多画
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
	m_Fairy->ShowPic(LOADING);
	Setting();
	m_Fairy->HidePic(LOADING);
	m_Setted=TRUE;
	while(1)
	{
		SetActiveWindow(m_GameWnd);
		//检测是否gameover状态
		GameFlowUpdate();
		UpdateCoinShow();
		RefreshPlayerStatus();
		
		//检测状态切换
		if(g_GameFlow!=oldGameFlow)
		{
			TRACE(L"StreetFighter GameFlow change to :%d\n",g_GameFlow);
			switch(g_GameFlow)
			{
			case flow_start:
				break;
			case flow_titlemenu:
				m_Fairy->HideAllPic();
				m_Fairy->ShowPic(STANDBYBG);
				m_Fairy->ShowPic(INSERTCOIN);
				m_Fairy->ShowPic(CREDITTEXT0);
				m_Fairy->ShowPic(CREDITTEXT1);
				m_Fairy->ShowPic(CREDIT0);
				m_Fairy->ShowPic(CREDIT1);
				oldGameFlow = g_GameFlow;
				break;
			case flow_demo:
				m_Fairy->HideAllPic();
				m_Fairy->ShowPic(INSERTCOIN);
				m_Fairy->ShowPic(CREDITTEXT0);
				m_Fairy->ShowPic(CREDITTEXT1);
				m_Fairy->ShowPic(CREDIT0);
				m_Fairy->ShowPic(CREDIT1);
				oldGameFlow = g_GameFlow;
				break;
			case flow_mainmenu:
				m_Fairy->ShowPic(STANDBYBG);
				m_Fairy->ShowPic(LOADING);
				m_Fairy->ShowPic(CREDITTEXT0);
				m_Fairy->ShowPic(CREDITTEXT1);
				m_Fairy->ShowPic(CREDIT0);
				m_Fairy->ShowPic(CREDIT1);
				//游戏结束后回到mainmenu
				if(oldGameFlow==flow_continue || oldGameFlow == flow_game)
				{
					oldGameFlow = g_GameFlow;
					Sleep(1500);
					DIHKeyDown(0,IDK_OK);      //画廊里有很多画
					m_mainMenu2Title.Excute(m_ActivePlayer);
				}
				break;
			case flow_selectchar:
				{
					m_Fairy->HideAllPic();
					m_Fairy->ShowPic(CREDITTEXT0);
					m_Fairy->ShowPic(CREDITTEXT1);
					m_Fairy->ShowPic(CREDIT0);
					m_Fairy->ShowPic(CREDIT1);
					m_Fairy->ShowPic(TIMECOUNTER);

					if (m_IsArcadeMode == FALSE)
					{
						// 对战模式需要模拟2P的开始
						DIHKeyDown(1, IDK_START);
					}

					DWORD time = GetTickCount();
					while (GetTickCount() - time < 22000)
					{
						Sleep(1);
						UpdateCoinShow();// 循环里头无法及时响应投币动作，故作此重构，cxb

						//游戏开始
						if(g_GameFlow==flow_game) break;
					}
					m_Fairy->HidePic(TIMECOUNTER);
					//默认角色
					if(g_GameFlow!=flow_game)
					{
						TRACE(TEXT("SF4 select default char"));
						if (m_IsArcadeMode)	// 街机模式选择默认角色
						{
							DIHKeyDown(m_ActivePlayer, IDK_OK);
							Sleep(1000);
							DIHKeyDown(m_ActivePlayer, IDK_OK);
						}
						else // 对战模式选择默认角色和关卡
						{
							while (g_GameFlow != flow_game)
							{
								DIHKeyDown(0, IDK_OK);
								DIHKeyDown(1, IDK_OK);
								Sleep(500);
							}
						}
					}
					oldGameFlow = g_GameFlow;
				}
				break;
			default:
				oldGameFlow = g_GameFlow;
			}
		}//IF
		
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

			if (life > 0)
			{
				m_Fairy->HidePic(INSERTCOIN);	// 投币达到一定条件不再显示请投币，应该显示请开始，add here
			}
			else
			{
				//m_Fairy->ShowPic(INSERTCOIN);
			}

			m_Fairy->ResetCoinInsert(CREDIT0 + i * 2,life, rem, g_Config.UnitCoin);
			m_Fairy->ShowPic(CREDITTEXT0);
			m_Fairy->ShowPic(CREDITTEXT1);
			m_Fairy->ShowPic(CREDIT0);
			m_Fairy->ShowPic(CREDIT1);
		}
	}
}

void CContrlCenter::Setting()
{
	TRACE("SF4 Setting <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	for (int i = 0; i < 2; i ++)
	{
		TRACE("SF4 Setting[%d]", i);
		m_cmdtitle2Mainmenu.Excute(0);
		if (i == 0)
		{
			m_mainMenuSetting2SelectChar.Excute(0);
		}
		else
		{
			m_VersusSetting.Excute(0);
		}
		m_selectChar2mainMenu.Excute(0);
		m_mainMenu2Title.Excute(0);
		Sleep(1000);
	}
	TRACE("SF4 Setting >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
}

void CContrlCenter::RefreshPlayerStatus()
{
	// 遍历玩家输入和状态
	// 街机模式
	if ((m_Players[0].IsClickStart() && m_Players[1].GetStatus() == CPlayer::PS_IDLE)||
		m_Players[0].GetStatus() == CPlayer::PS_IDLE && m_Players[1].IsClickStart())
	{
		m_ActivePlayer = m_Players[0].IsClickStart() ? 0 : 1;
		m_IsArcadeMode = TRUE;
		TRACE(L"SF4 Player[%d] Arcade mode Starting", m_ActivePlayer);
		m_Fairy->HideAllPic();
		m_Fairy->ShowPic(STANDBYBG);
		m_Fairy->ShowPic(LOADING);
		m_Fairy->ShowPic(CREDITTEXT0);
		m_Fairy->ShowPic(CREDITTEXT1);
		m_Fairy->ShowPic(CREDIT0);
		m_Fairy->ShowPic(CREDIT1);

		if (g_GameFlow == flow_demo)
		{
			m_cmdDemo2Titile.Excute(m_ActivePlayer);
		}
		m_Players[m_ActivePlayer].RefreshStatus(g_GameFlow);
		if (g_GameFlow == flow_titlemenu)
		{
			m_cmdtitle2Mainmenu.Excute(m_ActivePlayer);
		}
		m_Players[m_ActivePlayer].RefreshStatus(g_GameFlow);
		if (g_GameFlow == flow_mainmenu)
		{
			Sleep(400);
			m_mainmenu2SelectChar.Excute(m_ActivePlayer);
		}
		m_Players[m_ActivePlayer].RefreshStatus(g_GameFlow);
		m_Fairy->HideAllPic();
		m_Fairy->ShowPic(CREDITTEXT0);
		m_Fairy->ShowPic(CREDITTEXT1);
		m_Fairy->ShowPic(CREDIT0);
		m_Fairy->ShowPic(CREDIT1);
	}
	// 对战模式
	else if (m_Players[0].IsClickStart() || m_Players[1].IsClickStart())
	{
		BYTE id = m_Players[0].IsClickStart() ? 0 : 1;
		TRACE(L"SF4 Versus mode Starting");

// 		m_Fairy->HideAllPic();
// 		m_Fairy->ShowPic(STANDBYBG);
// 		m_Fairy->ShowPic(LOADING);
// 		m_Fairy->ShowPic(CREDITTEXT0);
// 		m_Fairy->ShowPic(CREDITTEXT1);
// 		m_Fairy->ShowPic(CREDIT0);
// 		m_Fairy->ShowPic(CREDIT1);
		m_IsArcadeMode = FALSE;
		if (g_GameFlow == flow_game)
		{
			DIHKeyDown(m_ActivePlayer, IDK_START);// 点击退出，开始即是退出
			m_Game2MainMenu.Excute(m_ActivePlayer);
		}
		if (g_GameFlow == flow_mainmenu)
		{
			m_mainMenu2Title.Excute(m_ActivePlayer);
		}
		Sleep(1000);
		// 以上是回退，现在才开始进游戏，以1P的身份开始，因为后面的设置由开始的人设置，方便统一模拟
		m_cmdtitle2Mainmenu.Excute(0);
		m_Players[0].RefreshStatus(g_GameFlow);
		m_Players[1].RefreshStatus(g_GameFlow);
		m_VersusMainmenu2SelectChar.Excute(0);

// 		m_Fairy->HideAllPic();
// 		m_Fairy->ShowPic(CREDITTEXT0);
// 		m_Fairy->ShowPic(CREDITTEXT1);
// 		m_Fairy->ShowPic(CREDIT0);
// 		m_Fairy->ShowPic(CREDIT1);
	}

	m_Players[0].RefreshStatus(g_GameFlow);
	m_Players[1].RefreshStatus(g_GameFlow);

	// 对战模式死亡处理，赢家可以继续街机模式闯关
	if (m_IsArcadeMode == FALSE && (m_Players[0].IsDead() || m_Players[1].IsDead()))
	{
		m_Game2MainMenu.Excute(0);
		m_mainMenu2Title.Excute(0);
		m_Players[m_Players[0].IsDead() ? 1 : 0].SetStatus(CPlayer::PS_CLICKSTART);
	}
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
		if (flow_continue==g_GameFlow)
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
		}//开始,暂时游戏中不能开始，等待对战模式的支持
		//if (flow_titlemenu == g_GameFlow || flow_demo==g_GameFlow)
		{
			TRACE(L"StreetFighter start\n");
			if (!g_ContrlCenter.m_Players[id].ClickStart())
			{
				DIHLockInputOnce(id);
			}
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