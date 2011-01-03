#pragma once
#include "Keycmd.h"
#include "Player.h"
#include "Config.h"
#include "../../Common/DirectInput-Hook/DirectInput-Hook.h"
#include "../../Common/UIShow/Fairy.h"

#define MOST_SIMPLE (-3)
#define VERY_SIMPLE (-2)
#define SIMPLE (-1)
#define MIDDLE 0
#define LITTLE_HARD 1
#define HARD 2
#define VERY_HARD 3
#define MOST_HARD 4
//最简单 非常简单  简单 中等 偏难 困难 非常困难 最难


#define STANDBYBG 0
#define LOADINGBG 1
#define CREDIT	2
#define IPSTART	3
#define INSERTCOIN 4
#define LOADING 5


VOID KeyProc(BYTE, KeyState&);
static class CContrlCenter
{
public:
	CContrlCenter(void);
	~CContrlCenter(void);
	bool Init();
	void Run();
	 CConfig m_Config;
	 CKeycmd m_cmdDemo2SelectChar;  //游戏开始，从demo画面到角色选择
	 CKeycmd m_cmdTitle2SelectChar; //游戏开始，从Title画面到角色选择
	 CKeycmd m_cmdSetting;          //游戏初始设置
	 CKeycmd m_cmdMainmenu2Title;   //游戏结束后回到title画面
	 CPlayer m_Players[2];
	 BOOL m_bStart;                 //判定玩家是否点击开始
	 BOOL m_bInsertCoin;
	 BOOL m_bIsBusy;                //判定游戏是否处于设置，状态切换等忙碌状态

	 //CScreen m_CoverScreen;
	 //CSTreetFightScene m_Scene;
	 CFairy m_Fairy;
	 HWND m_GameWnd;
};
extern CContrlCenter g_ContrlCenter;