// ShowCutRectWnd.cpp : implementation file
//

#include "stdafx.h"
#include "UIShow.h"
#include "ShowCutRectWnd.h"
#include "imageex.h"


// CShowCutRectWnd

IMPLEMENT_DYNAMIC(CShowCutRectWnd, CWnd)

CShowCutRectWnd::CShowCutRectWnd(std::wstring picName, CutRectList cutRectList)
{
	m_picName = picName;
	m_cutRectList = cutRectList;
}

CShowCutRectWnd::~CShowCutRectWnd()
{
}


BEGIN_MESSAGE_MAP(CShowCutRectWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CShowCutRectWnd message handlers



int CShowCutRectWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_pImage = new ImageEx(m_picName.c_str());
	m_pImage->SetTransprentWindow(true);

	m_pImage->SetCutRectTLPt(CPoint(0,0));
	int m_bakWidth  =m_pImage->GetWidth();
	int m_bakHeight =m_pImage->GetHeight();
	TRACE(L"bbbb ImageEx GetWidth: %d   GetHeight: %d", m_bakWidth, m_bakHeight);

	//RectF rect;
	//rect.X = 20;
	//rect.Y = 0;
	//rect.Width = 20; 
	//rect.Height = 40;

	//TRACE("bbbb -------++++ destRect x: %f, y: %f, width: %f, height: %f", rect.X, rect.Y, rect.Width, rect.Height);
	//m_pImage->AddCutRect(rect);
	//rect.X = 40;
	//m_pImage->AddCutRect(rect);

	CutRectListIter iter = m_cutRectList.begin();
	for (; iter != m_cutRectList.end(); ++iter)
	{

		TRACE(L"bbbb ImageEx x: %f, y %f, GetWidth: %f   GetHeight: %f", iter->X, iter->Y, iter->Width, iter->Height);
		m_pImage->AddCutRect(*iter);
	}

	SetTimer(NULL, 100, NULL);
	return 0;
}

void CShowCutRectWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	m_pImage->InitCutRectAnimation(GetSafeHwnd());
	CWnd::OnTimer(nIDEvent);
}
