#include "StdAfx.h"
#include "Fairy.h"
#include "ImageEx.h"
#include <shlwapi.h>

CFairy::CFairy()
{
	GdiplusStartup(&m_gdiplusToken,&m_gdiplusStartupInput, NULL);
}

CFairy::~CFairy(void)
{
	delete m_imageExManager;
	GdiplusShutdown(m_gdiplusToken);
}

void CFairy::CreateTransparentPic(CWnd *parent,int id, std::wstring picName, CPoint pt, int imageWidth /*= 0*/, int imageHeight/* = 0*/)
{
	TRACE(_T("aaaa CreateTransparentPic %s"), picName.c_str());
	m_imageExManager->CreateImageEx(id, picName.c_str(),pt, imageWidth, imageHeight);
}

void CFairy::CreateCoinInsert(int id, std::wstring picName, int life, int coins, int coinsOneLife ,CPoint pt, int imageWidth/* = 0*/ , int imageHeight/* = 0*/)
{
	CutRectF rect;
	rect.pt = pt;
	rect.cutRectList = GetCutRectList(life, coins, coinsOneLife);
	m_imageExManager->CreateCutRectImageEx(id, picName.c_str(), rect);
}

void CFairy::ResetCoinInsert(DWORD id, int life, int coins, int coinsOneLife )
{
	TRACE(_T("aaaa CreateTransparentPic life:%d, conis:%d, coinsonelife : %d", life, coins, coinsOneLife)
		, life, coins, coinsOneLife);
	CutRectF rect;
	rect.cutRectList = GetCutRectList(life, coins, coinsOneLife);
	m_imageExManager->Reset(id, rect);
}

void CFairy::DestroyPic( int id )
{
	m_imageExManager->Destroy(id);
}

void CFairy::DestroyAllPic()
{
	m_imageExManager->DestroyAll();
}

void CFairy::ShowPic( int id )
{
	m_showWnd->ShowWindow(SW_NORMAL);
	m_imageExManager->Show(id);
}

void CFairy::HidePic( int id )
{
	m_showWnd->ShowWindow(SW_HIDE);
	m_imageExManager->Hide(id);
}

void CFairy::ShowAllPic()
{
	m_showWnd->ShowWindow(SW_NORMAL);
	m_imageExManager->ShowAll();
}

void CFairy::HideAllPic()
{
	m_showWnd->ShowWindow(SW_HIDE);
	m_imageExManager->HideAll();
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

std::vector<RectF> CFairy::GetCutRectList( int life, int coins, int cointsOneLife )
{
	std::vector<RectF> cutRectList;

	RectF rect;
	rect.X = 0;
	rect.Y = 0;
	rect.Width = 20;
	rect.Height = 40;
	//life
	AddCutRects(life, rect, cutRectList);

	//:
	rect.X = 10 * 20;
	cutRectList.push_back(rect);

	//[
	rect.X = 11 * 20;
	cutRectList.push_back(rect);

	//coins
	AddCutRects(coins, rect, cutRectList);

	//  /
	rect.X = 12 * 20;
	cutRectList.push_back(rect);

	//cointsOneLife
	AddCutRects(cointsOneLife, rect, cutRectList);

	//  ]
	rect.X = 13 * 20;
	cutRectList.push_back(rect);

	return cutRectList;
}

void CFairy::AddCutRects( int number, RectF rect, std::vector<RectF> &cutRectList )
{
	std::vector<int> lifeList;
	lifeList = GetNumList(number);
	std::vector<int>::reverse_iterator iter = lifeList.rbegin();
	for (; iter != lifeList.rend(); ++iter)
	{
		rect.X = ( *iter ) * 20;
		cutRectList.push_back(rect);
	}
}

void CFairy::CreateBKWnd()
{
	m_showWnd = new CWnd;
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	m_showWnd->CreateEx(WS_EX_TOPMOST   |   WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,::LoadCursor(NULL,   IDC_ARROW),  
		HBRUSH(COLOR_WINDOW+1),   NULL), 
		_T("MyPopupWindow "), WS_POPUP,
		CRect(0,0,width,height), NULL ,NULL ,NULL); 

	m_showWnd->ShowWindow(SW_HIDE);
	TRACE("aaaa ImageExManager a ");
	m_imageExManager = new ImageExManager(m_showWnd->GetSafeHwnd());
	TRACE("aaaa ImageExManager b ");
}

void CFairy::SetParent( CWnd* parentWnd )
{
	m_showWnd->SetParent(parentWnd);
}