#include "StdAfx.h"
#include "ContrlCenter.h"
#include "GlobalVar.h"
#include "..\..\Common\UIShow\FlashFairy.h"

#pragma data_seg (".HookSection")
// Shared instance for all processes.
HHOOK g_hMouse = NULL;
#pragma data_seg ()

BOOL g_TheWndsCreated = FALSE;

CContrlCenter g_ContrlCenter;


CContrlCenter::CContrlCenter(void)
: m_cmdDemo2Titile(flow_demo, flow_titlemenu, L"m_cmdDemo2Titile")
, m_cmdtitle2Mainmenu(flow_titlemenu, flow_mainmenu, L"m_cmdtitle2Mainmenu")
, m_mainMenu2Title(flow_mainmenu, flow_titlemenu, L"m_mainMenu2Title")
, m_FirstStartedPlayer(0)
, m_GameWnd(NULL)
, m_IsArcadeMode(TRUE)
{
	m_Fairy = new CFairy;
	TRACE(L"StreetFighter CContrlCenter::CContrlCenter\n");
}

CContrlCenter::~CContrlCenter(void)
{
	delete m_Fairy;
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
	CWnd *parentWnd;

	if (g_TheWndsCreated == FALSE)
	{
		g_TheWndsCreated = TRUE;
		TRACE("aaaa NewWindowProc WM_LBUTTONDBLCLK after");
		ShowCursor(FALSE);
		parentWnd = new CWnd;
		parentWnd->Attach(hwnd);
		int screenWidth =GetSystemMetrics(SM_CXSCREEN);
		int screentHeight =GetSystemMetrics(SM_CYSCREEN);
		g_ContrlCenter.m_Fairy->CreateBKWnd();
		TRACE("aaaa NewWindowProc CreateBKWnd %d, %d", screenWidth, screentHeight);
		//g_ContrlCenter.m_Fairy->SetParent(parentWnd);
		// 这里要用正确的width和height计算坐标，不然窗口可能不显示，cxb
		g_ContrlCenter.m_Fairy->CreateTransparentPic(STANDBYBG,L"SF4Con\\StandbyBG.jpg", CPoint(0,0), screenWidth, screentHeight);
		g_ContrlCenter.m_InsertCoinFairy = new CFlashFairy(L"SF4Con\\InsertCoin.png",CPoint(screenWidth/2-80, screentHeight - 140));
		g_ContrlCenter.m_PressStartFairy = new CFlashFairy(L"SF4Con\\1PStart.png",CPoint(screenWidth/2-128, screentHeight - 140));
		g_ContrlCenter.m_Fairy->CreateTransparentPic(TIMECOUNTER,L"SF4Con\\TimeCounter.gif",CPoint(screenWidth/2-35, 200));
		g_ContrlCenter.m_Fairy->CreateTransparentPic(LOADING,L"SF4Con\\Loading.png",CPoint(screenWidth/2-50, screentHeight - 120));
		g_ContrlCenter.m_FreeingFairy = new CFlashFairy(L"SF4Con\\freePlay.png", CPoint(screenWidth/2-128, screentHeight - 180));

		int posX0 = g_Config.CoinMode == 0 ? (screenWidth / 2) : (screenWidth / 4),
			posX1 = g_Config.CoinMode == 0 ? (screenWidth / 2) : (screenWidth / 4 * 3);
		g_ContrlCenter.m_Fairy->CreateTransparentPic(CREDITTEXT0,L"SF4Con\\CREDIT.png",CPoint(posX0-130, screentHeight - 60));
		g_ContrlCenter.m_Fairy->CreateCoinInsert(CREDIT0, L"SF4Con\\Num.png", 0, 0, g_Config.UnitCoin,CPoint(posX0 - 30, screentHeight - 60));
		if (g_Config.CoinMode != 0)	// 双式
		{
			g_ContrlCenter.m_Fairy->CreateTransparentPic(CREDITTEXT1,L"SF4Con\\CREDIT.png",CPoint(posX1 - 130, screentHeight - 60));
			g_ContrlCenter.m_Fairy->CreateCoinInsert(CREDIT1, L"SF4Con\\Num.png", 0, 0, g_Config.UnitCoin,CPoint(posX1 - 30, screentHeight - 60));
		}
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
	m_Players[0] = CPlayer(0);
	m_Players[1] = CPlayer(1);

	TRACE("StreetFighter GAME WND:%d\n",0);
	while(0 == m_GameWnd)
	{
		m_GameWnd = ::FindWindow(L"STREET FIGHTER IV",L"STREET FIGHTER IV");
		Sleep(100);
	}
	::SetWindowPos(m_GameWnd, NULL,-(GetSystemMetrics(SM_CXBORDER) + 2),-(GetSystemMetrics(SM_CYCAPTION) + 3),0,0,SWP_NOSIZE);
	OldWindowProc = (WNDPROC)SetWindowLong(m_GameWnd, GWL_WNDPROC, (LONG)NewWindowProc);
	while (g_TheWndsCreated == FALSE) Sleep(1);

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
		m_cmdtitle2Mainmenu.InsertCmd(IDK_OK, 1000);
		m_cmdtitle2Mainmenu.InsertCmd(IDK_BACKSPACE, 1000);
	}
	{
		m_mainMenu2Title.InsertCmd(IDK_BACKSPACE,1500);
		m_mainMenu2Title.InsertCmd(IDK_DOWN,1000);
		m_mainMenu2Title.InsertCmd(IDK_OK,1000);
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
	m_Fairy->ShowPic(STANDBYBG);
	m_Fairy->ShowPic(LOADING);
	while(flow_titlemenu!=g_GameFlow)
	{
		DIHKeyDown(0,IDK_OK);
		SetForegroundWindow(m_GameWnd);
		Sleep(500);
	}
	 //锁定1p 2p 游戏手柄
	DIHLockPlayerInput(0);
	DIHLockPlayerInput(1);
	Loginning();
	m_Fairy->HidePic(LOADING);
	m_Fairy->ShowPic(CREDITTEXT0);
	m_Fairy->ShowPic(CREDITTEXT1);
	m_Fairy->ShowPic(CREDIT0);
	m_Fairy->ShowPic(CREDIT1);
	UpdateCoinShow();

	if (g_Config.IsFree == 1)
	{
		m_FreeingFairy->Show();
	}

	while(1)
	{
		SetForegroundWindow(m_GameWnd);
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
				m_Fairy->HidePic(LOADING);
				m_Fairy->ShowPic(STANDBYBG);
				{
					int life = m_Players[0].GetCoinNumber() / g_Config.UnitCoin;
					if (life > 0)
					{
						m_InsertCoinFairy->Hide(); // 投币达到一定条件不再显示请投币，应该显示请开始，add here
						m_PressStartFairy->Show(); 
					}
					else
					{
						m_InsertCoinFairy->Show(); 
						m_PressStartFairy->Hide(); 
					}
				}
				oldGameFlow = g_GameFlow;
				break;
			case flow_demo:
				m_Fairy->HidePic(STANDBYBG);
				oldGameFlow = g_GameFlow;
				break;
			case flow_mainmenu:
				m_Fairy->ShowPic(STANDBYBG);
				m_Fairy->ShowPic(LOADING);
				//游戏结束后回到mainmenu
				oldGameFlow = g_GameFlow;
				// 用内存补丁解锁人物后不会有“画廊里有很多画”了，cxb
				//Sleep(1500);
				// DIHKeyDown(0,IDK_OK);导致游戏结束卡住的bug，要对正确的设备模拟cxb
				//DIHKeyDown(m_ActivePlayer,IDK_OK);      //画廊里有很多画
				m_mainMenu2Title.Excute(m_FirstStartedPlayer);

				break;
			case flow_selectchar:
				{
					DIHLockPlayerInput(0);
					DIHLockPlayerInput(1);

					m_Fairy->HidePic(STANDBYBG);
					m_Fairy->HidePic(LOADING);
					m_Fairy->ShowPic(TIMECOUNTER);

					DWORD time = GetTickCount();
					while (GetTickCount() - time < 22000)
					{
						Sleep(1);
						//游戏开始
						if(g_GameFlow==flow_game) break;
					}
					m_Fairy->HidePic(TIMECOUNTER);
					//默认角色
					if(g_GameFlow!=flow_game)
					{
						TRACE(TEXT("SF4 select default char"));
						while (g_GameFlow != flow_game)
						{
							DIHKeyDown(0, IDK_OK);
							DIHKeyDown(1, IDK_OK);
							Sleep(500);
						}
					}
					oldGameFlow = g_GameFlow;
				}
				break;
			default:
				oldGameFlow = g_GameFlow;
			}
		}//IF

		//f2 呼出设置程序
		if( GetAsyncKeyState(VK_F7) )
		{
			TRACE(L"StreetFighter esc DOWN\n");
			STARTUPINFO si = {0};
			si.cb = sizeof(si);
			PROCESS_INFORMATION pi = {0};
			CreateProcessW(TEXT("SFCoinSet.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			ExitProcess(0);
		}

		if (g_IsStartDemo)	// 进入游戏有个过场动画，过之，cxb
		{
			Sleep(2000);
			DIHKeyDown(m_Players[0].IsInGame() ? 0 : 1, IDK_START);
			g_IsStartDemo = FALSE;
		}
		Sleep(200);
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
			TRACE("SF4 player[%d] CoinsChanged, update interface[%d, %d, %d]",
				i, totalCoins, life, rem);
			m_Fairy->ResetCoinInsert(CREDIT0 + i * 2,life, rem, g_Config.UnitCoin);
		
			if ((g_GameFlow == flow_titlemenu || g_GameFlow == flow_demo) 
				&& m_Players[0].IsIdle() && m_Players[1].IsIdle())
			{
				int life = m_Players[0].GetCoinNumber() / g_Config.UnitCoin;
				if (life > 0)
				{
					m_InsertCoinFairy->Hide(); 	// 投币达到一定条件不再显示请投币，应该显示请开始，add here
					m_PressStartFairy->Show(); 
				}
				else
				{
					m_InsertCoinFairy->Show();
					m_PressStartFairy->Hide();
				}
			}
			else
			{
				m_InsertCoinFairy->Hide();
				m_PressStartFairy->Hide();
			}
		}
	}
}

void CContrlCenter::Loginning()
{
	// 不使用模拟操作来初始设置，直接修改内存，cxb
	BYTE *difficulty = (BYTE *)0x00a4a6d0,
		*rounds = (BYTE *)0x00a4a6e4;
	WORD *timelimit = (WORD *)0x00a4a6d6;
	*difficulty = g_Config.Difficulty;
	*rounds = g_Config.Rounds * 2 + 1;
	*timelimit  = g_Config.TimeLimit == 0 ? 30 :
		g_Config.TimeLimit == 1 ? 60 :
		g_Config.TimeLimit == 2 ? 99 : 0xFFFE;
}

void CContrlCenter::RefreshPlayerStatus()
{
	// 遍历玩家输入和状态
	// 街机模式
	if ((m_Players[0].IsClickStart())|| m_Players[1].IsClickStart())
	{
		m_PressStartFairy->Hide();
		m_FirstStartedPlayer = m_Players[0].IsClickStart() ? 0 : 1;
		m_IsArcadeMode = TRUE;
		TRACE(L"SF4 Player[%d] Arcade mode Starting", m_FirstStartedPlayer);
		m_Fairy->ShowPic(STANDBYBG);
		m_Fairy->ShowPic(LOADING);

		if (g_GameFlow == flow_demo)
		{
			if (!m_cmdDemo2Titile.Excute(m_FirstStartedPlayer)) return;
		}
		m_Players[m_FirstStartedPlayer].RefreshStatus(g_GameFlow);
		if (g_GameFlow == flow_titlemenu)
		{
			if (!m_cmdtitle2Mainmenu.Excute(m_FirstStartedPlayer)) return;
		}
		Sleep(1000);
		for (DWORD addr = 0x00A4BB38; addr <= 0x00A4CAD4; addr += 4)
		{
			*(DWORD *)addr = 1;
		}
		while (g_GameFlow != flow_selectchar)
		{
			DIHKeyDown(m_FirstStartedPlayer, IDK_OK);
			m_Players[m_FirstStartedPlayer].RefreshStatus(g_GameFlow);
			Sleep(100);
		}
		m_Players[m_FirstStartedPlayer].RefreshStatus(g_GameFlow);
		m_Fairy->HidePic(STANDBYBG);
		m_Fairy->HidePic(LOADING);
	}

	m_Players[0].RefreshStatus(g_GameFlow);
	m_Players[1].RefreshStatus(g_GameFlow);

	// 根据是否在游戏中来锁定和解锁输入
	// 选人界面中通过截获并模拟操作即不会受锁定的影响，表现良好
	// 把选人纳入锁定范围的诱因：游戏逻辑中选人界面可以回退
	for (int i = 0; i < 2; i ++)
	{
		//if (m_Players->IsInGame())// 导致2P输入失效的bug，cxb
		if (m_Players[i].IsInGame())
		{
			DIHLockPlayerInput(i, IDK_START);
		}
		else
		{
			DIHLockPlayerInput(i);
		}
	}
	// 不用切换了：进入街机模式的玩家要负责从mainmenu退出到title，
	// 如：1P率先点击开始，然后2P加入对战，1P战败，2P再被pc打败，那么游戏退回到title界面时需要模拟1P的退出操作（m_mainmenu2title）
	// 否则在这种情况下将卡死，by cxb
// 	if (m_Players[m_ActivePlayer].IsDead() && m_Players[m_ActivePlayer ^ 0x01].IsInGame())
// 	{
// 		m_ActivePlayer ^= 0x01;	// 切换活动玩家，退出游戏才会正常和过开始动画才不会导致重新开始的bug，cxb
// 		TRACE(TEXT("SF4 Exchange active player %d"), m_ActivePlayer);
// 	}
}

void CContrlCenter::KeyProc( BYTE id, KeyState& state )
{
 	if(state.coin==1)//投币,任何时候都有效
 	{
		g_ContrlCenter.m_Players[id].IncrementCoin();
		TRACE(L"StreetFighter player [%d] insert coin:%d\n", id, g_ContrlCenter.m_Players[id].GetCoinNumber());
		g_ContrlCenter.UpdateCoinShow();
 		}
 	else if(state.start==1)
 	{
		// 可以开始的不同情况
		BOOL isStartFirst =  g_ContrlCenter.m_Players[id].IsIdle() && g_ContrlCenter.m_Players[id ^ 0x1].IsIdle(),	// 普通开始
			isDead = g_ContrlCenter.m_Players[id].IsDead(), // Continue中，街机模式和对战模式一样
			isJoinVersus = g_ContrlCenter.m_Players[id].IsIdle() && g_ContrlCenter.m_Players[id ^ 0x1].IsInGame(),// 加入对战
			isGameOverInVersus = g_ContrlCenter.m_Players[id].GetStatus() == CPlayer::PS_GAMEOVER && g_ContrlCenter.m_Players[id ^ 0x1].IsInGame();// 街机模式gameover，仍可开始
		TRACE("SF4 Click start %d, %d, %d, %d", isStartFirst, isDead, isJoinVersus, isGameOverInVersus);
		if (isStartFirst || isDead || isJoinVersus || isGameOverInVersus) 
		{
			// 根据开始状态来设置地址，暂时。。。
			DWORD *continueValAddr = isStartFirst ? (DWORD *)0x00A44FFC : 
				isJoinVersus ? (DWORD *)0x00A4500C : NULL;

			if (g_ContrlCenter.m_Players[id].ClickStart(continueValAddr))
			{
				if (isDead || isJoinVersus || isGameOverInVersus) // continue开始
				{
					DIHKeyDown(id, IDK_START);
					g_ContrlCenter.m_Players[id].SetStatus(CPlayer::PS_STARTTING);
				}
			}
			else
			{
				DIHLockPlayerInput(id);
			}
		}
 	}
	else if (g_GameFlow == flow_selectchar)
	{
		if (state.up == 1){	DIHKeyDown(id, IDK_UP);	}
		else if (state.down == 1){	DIHKeyDown(id, IDK_DOWN);	}
		else if (state.left == 1){	DIHKeyDown(id, IDK_LEFT);	}
		else if (state.right == 1){	DIHKeyDown(id, IDK_RIGHT);	}
		else if (state.ok == 1){	DIHKeyDown(id, IDK_OK);	}
	}
}