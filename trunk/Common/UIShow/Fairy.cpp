#include "StdAfx.h"
#include "Fairy.h"
#include "showwnd.h"
#include "showcutrectwnd.h"
#include <shlwapi.h>

CFairy::CFairy(void)
{
	GdiplusStartup(&m_gdiplusToken,&m_gdiplusStartupInput, NULL);
}

CFairy::~CFairy(void)
{
	GdiplusShutdown(m_gdiplusToken);
}

void CFairy::CreateTransparentWnd(CWnd *parent,int id, std::wstring picName, CPoint pt, int imageWidth /*= 0*/, int imageHeight/* = 0*/)
{
	m_showWnd = new CShowWnd(picName);
	m_showWnd->SetImageSize(imageWidth, imageHeight);
	CreateWnd(parent, m_showWnd, pt, id);
}


void CFairy::CreateCoinInsert(int id, std::wstring picName, int life, int coins, int coinsOneLife ,CPoint pt, int imageWidth/* = 0*/ , int imageHeight/* = 0*/)
{
	CShowCutRectWnd::CutRectList cutRectList = GetCutRectList(life, coins, coinsOneLife);
	std::wstring backgroundPath =picName;
	mInsertCointSingleWnd = new CShowCutRectWnd(backgroundPath, cutRectList);
	CreateWnd(NULL, mInsertCointSingleWnd, pt, id);
}

void CFairy::ResetCoinInsert( int life, int coins, int coinsOneLife )
{
	CShowCutRectWnd::CutRectList cutRectList = GetCutRectList(life, coins, coinsOneLife);
	mInsertCointSingleWnd->ResetCutRect(cutRectList);
}

void CFairy::DestroyWnd( int id )
{
	std::map<int, CWnd*>::iterator iter = m_showWndList.find(id);
	if (iter == m_showWndList.end())
	{
		return;
	}
	CWnd* wnd = m_showWndList[id];
	wnd->DestroyWindow();
	delete wnd;
	m_showWndList.erase(iter);
}

void CFairy::DestroyAllWnd()
{
	std::map<int, CWnd*>::iterator iter  = m_showWndList.begin();
	for (; iter != m_showWndList.end(); ++iter)
	{
		(*iter).second->DestroyWindow();
		delete (*iter).second;
	}
	m_showWndList.clear();
}
void CFairy::ShowWnd( int id )
{
	m_showWndList[id]->ShowWindow(SW_SHOWNA);
}

void CFairy::HideWnd( int id )
{
	m_showWndList[id]->ShowWindow(SW_HIDE);
}

void CFairy::ShowAllWnd()
{
	std::map<int, CWnd*>::iterator iter  = m_showWndList.begin();
	for (; iter != m_showWndList.end(); ++iter)
	{
		(*iter).second->ShowWindow(SW_NORMAL);
	}
}

void CFairy::HideAllWnd()
{
	std::map<int, CWnd*>::iterator iter  = m_showWndList.begin();
	for (; iter != m_showWndList.end(); ++iter)
	{
		(*iter).second->ShowWindow(SW_HIDE);
	}
}

std::vector<int> CFairy::GetNumList( int number )
{
	std::vector<int> numList;
	while (true)
	{
		numList.push_back(number % 10);
		number = number / 10;
		if (number == 0)
		{
			break;
		}
	}
	return numList;
}

CShowCutRectWnd::CutRectList CFairy::GetCutRectList( int life, int coins, int cointsOneLife )
{
	CShowCutRectWnd::CutRectList cutRectList;

	std::vector<int> lifeList;
	std::vector<int> coinList;
	std::vector<int> coinsOneLifeList;
	lifeList = GetNumList(life);
	coinList = GetNumList(coins);
	coinsOneLifeList = GetNumList(cointsOneLife);
	RectF rect;
	rect.Y = 0;
	rect.Width = 20;
	rect.Height = 40;
	//life
	std::vector<int>::reverse_iterator iter = lifeList.rbegin();
	for (; iter != lifeList.rend(); ++iter)
	{
		rect.X = ( *iter ) * 20;
		cutRectList.push_back(rect);
	}
	//:
	rect.X = 10 * 20;
	cutRectList.push_back(rect);

	//[
	rect.X = 11 * 20;
	cutRectList.push_back(rect);

	//coins
	iter = coinList.rbegin();
	for (; iter != coinList.rend(); ++iter)
	{
		rect.X = ( *iter ) * 20;
		cutRectList.push_back(rect);
	}

	//  /
	rect.X = 12 * 20;
	cutRectList.push_back(rect);

	//cointsOneLife
	iter = coinsOneLifeList.rbegin();
	for (; iter != coinsOneLifeList.rend(); ++iter)
	{
		rect.X = ( *iter ) * 20;
		cutRectList.push_back(rect);
	}

	//  ]
	rect.X = 13 * 20;
	cutRectList.push_back(rect);

	return cutRectList;
}

void CFairy::CreateWnd( CWnd * parent, CWnd* wnd, CPoint pt, int id )
{
	wnd->CreateEx(WS_EX_TOPMOST   |   WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,::LoadCursor(NULL,   IDC_ARROW),  
		HBRUSH(COLOR_WINDOW+1),   NULL),
		_T("MyPopupWindow "), WS_POPUP,
		CRect(CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT), parent ,NULL ,NULL); 
	wnd->ShowWindow(SW_HIDE);
	wnd->MoveWindow(pt.x, pt.y, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	m_showWndList.insert(std::make_pair(id, wnd));
}