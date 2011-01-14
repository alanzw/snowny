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

#define IPSTART	3
#define INSERTCOIN 4
#define LOADING 5
#define TIMECOUNTER  6
#define CREDITTEXT0 7
#define CREDIT0	8
#define CREDITTEXT1 9
#define CREDIT1	10


VOID KeyProc(BYTE, KeyState&);
class CContrlCenter
{
public:
	CContrlCenter(void);
	~CContrlCenter(void);
	bool Init();
	void Run();

	static void KeyProc(BYTE id, KeyState& state);

	void RefreshPlayerStatus();
	void UpdateCoinShow();

	// 保持街机模式和对战模式上回合、时间限制一致
	void Setting();	// 街机模式、对战模式设置

	CKeycmd m_cmdDemo2Titile;		//游戏开始，从demo画面到title画面
	CKeycmd m_cmdtitle2Mainmenu;	//游戏开始，从title画面到Mainmenu画面
	CKeycmd m_mainmenu2SelectChar;	//游戏开始，从Mainmenu到角色选择
	CKeycmd m_mainMenu2Title;		//游戏从Mainmenu到title画面
	CKeycmd m_mainMenuSetting2SelectChar;		//游戏初始设置
	CKeycmd m_selectChar2mainMenu;				//游戏选角色到主菜单

	CKeycmd m_VersusSetting;				// 对战模式设置
	CKeycmd m_VersusMainmenu2SelectChar;	// 对战模式进入选人
	CKeycmd m_Game2MainMenu;				// 街机模式切换至对战模式，如已在游戏中，退出游戏
	
	//CKeycmd m_cmdDemo2SelectChar;  //游戏开始，从demo画面到角色选择
	//CKeycmd m_cmdTitle2SelectChar; //游戏开始，从Title画面到角色选择
	//CKeycmd m_cmdSetting;          //游戏初始设置
	//CKeycmd m_cmdMainmenu2Title;   //游戏结束后回到title画面
	CPlayer m_Players[2];
	BYTE	m_ActivePlayer;
	BOOL m_Setted;

	CFairy* m_Fairy;
	HWND   m_GameWnd;

	BOOL	m_IsArcadeMode;		// 街机模式
};
extern CContrlCenter g_ContrlCenter;