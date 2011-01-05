// ShowWnd.cpp : implementation file
//

#include "stdafx.h"
#include "UIShow.h"
#include "ShowWnd.h"
#include "ImageEx.h"


// CShowWnd

IMPLEMENT_DYNAMIC(CShowWnd, CWnd)

CShowWnd::CShowWnd(std::wstring picName)
{
	m_picName = picName;
	m_bakWidth = 0;
	m_bakHeight = 0;
}

CShowWnd::~CShowWnd()
{
	delete m_pImage;
}


BEGIN_MESSAGE_MAP(CShowWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CShowWnd message handlers



int CShowWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	//这里记得改变设置图片大小界面
	TRACE(L"bbbb ImageEx Picname %s", m_picName.c_str());
	m_pImage = new ImageEx(m_picName.c_str());

	DWORD ret = GetLastError();

	m_pImage->SetTransprentWindow(true);
	if (m_bakWidth == 0 || m_bakHeight == 0)
	{
		m_bakWidth  =m_pImage->GetWidth();
		m_bakHeight =m_pImage->GetHeight();
	}

	TRACE(L"bbbb CShowWnd ImageEx GetWidth: %d   GetHeight: %d", m_bakWidth, m_bakHeight);
	m_pImage->InitAnimation(this->GetSafeHwnd(), CPoint(0, 0),m_bakWidth, m_bakHeight);

	//ret = SetTimer(1, 100, NULL);

	TRACE(L"bbbb ImageEx SetTimer %d  GetlastError %d\n", ret, GetLastError());
	//::SetTimer(this->GetSafeHwnd(), 1, 100, NULL);

	return 0;
}

void CShowWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default


	TRACE(L"bbbb ImageEx OnTimer \n");

	//m_pImage->InitAnimation(this->GetSafeHwnd(), CPoint(0, 0));
	CWnd::OnTimer(nIDEvent);
}

void CShowWnd::SetImageSize( int width, int height )
{
	m_bakWidth = width;
	m_bakHeight = height;
}
