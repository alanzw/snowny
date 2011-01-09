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
#define TIMECOUNTER  6
#define CREDITTEXT 7



VOID KeyProc(BYTE, KeyState&);
class CContrlCenter
{
public:
	CContrlCenter(void);
	~CContrlCenter(void);
	bool Init();
	void Run();

	void UpdateCoinShow();
	CConfig m_Config;

	CKeycmd m_cmdDemo2Titile;		//游戏开始，从demo画面到title画面
	CKeycmd m_cmdtitle2Mainmenu;	//游戏开始，从title画面到Mainmenu画面
	CKeycmd m_mainmenu2SelectChar;	//游戏开始，从Mainmenu到角色选择
	CKeycmd m_mainMenu2Title;		//游戏从Mainmenu到title画面
	CKeycmd m_mainMenuSetting2SelectChar;		//游戏初始设置
	CKeycmd m_selectChar2mainMenu;				//游戏选角色到主菜单
	
	//CKeycmd m_cmdDemo2SelectChar;  //游戏开始，从demo画面到角色选择
	//CKeycmd m_cmdTitle2SelectChar; //游戏开始，从Title画面到角色选择
	//CKeycmd m_cmdSetting;          //游戏初始设置
	//CKeycmd m_cmdMainmenu2Title;   //游戏结束后回到title画面
	CPlayer m_Players[2];
	BYTE	m_ActivePlayer;
	BOOL m_bCoinsChanged;
	BOOL m_Setted;

	CFairy m_Fairy;
	CWnd m_GameWnd;
};
extern CContrlCenter g_ContrlCenter;