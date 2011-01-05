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

	CutRectListIter iter = m_cutRectList.begin();
	for (; iter != m_cutRectList.end(); ++iter)
	{
		TRACE(L"bbbb ImageEx x: %f, y %f, GetWidth: %f   GetHeight: %f", iter->X, iter->Y, iter->Width, iter->Height);
		m_pImage->AddCutRect(*iter);
	}

	m_pImage->InitCutRectAnimation(GetSafeHwnd());

	return 0;
}

void CShowCutRectWnd::ResetCutRect( const CutRectList &cutRectList )
{
	m_pImage->ClearRect();
	m_cutRectList = cutRectList;
	CutRectListIter iter = m_cutRectList.begin();
	for (; iter != m_cutRectList.end(); ++iter)
	{

		TRACE(L"bbbb ImageEx x: %f, y %f, GetWidth: %f   GetHeight: %f", iter->X, iter->Y, iter->Width, iter->Height);
		m_pImage->AddCutRect(*iter);
	}

	m_pImage->InitCutRectAnimation(GetSafeHwnd());
}
