#include "StdAfx.h"
#include "FlashFairy.h"

ULONG_PTR           gdiplusToken;

CFlashFairy::CFlashFairy( LPCTSTR picFileName, const RECT &rect, DWORD flashInterval /*= 1000*/ )
: m_Image(picFileName)
, m_Rect(rect)
, m_FlashInterval(flashInterval)
{
	Initialize();
}

CFlashFairy::CFlashFairy( LPCTSTR picFileName, const POINT &pt, DWORD flashInterval /*= 1000*/ )
: m_Image(picFileName)
, m_FlashInterval(flashInterval)
{
	m_Rect.left = pt.x;
	m_Rect.top  = pt.y;
	m_Rect.right  = m_Rect.left + m_Image.GetWidth();
	m_Rect.bottom = m_Rect.top + m_Image.GetHeight();

	Initialize();
}
CFlashFairy::~CFlashFairy(void)
{
	GdiplusShutdown(gdiplusToken);
}

void CALLBACK TimerProc(
						HWND hwnd, 
						UINT uMsg, 
						UINT idEvent, 
						DWORD dwTime 
						)
{
	if (IsWindowVisible(hwnd))
	{
		ShowWindow(hwnd, SW_HIDE);
	}
	else
	{
		ShowWindow(hwnd, SW_SHOWNOACTIVATE);
	}
}

void CFlashFairy::Initialize()
{
	TRACE("CFlashFairy::Initialize() Rect(%d, %d, %d, %d)", m_Rect.left, m_Rect.top, m_Rect.right, m_Rect.right);
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	m_Wnd = new CWnd;
	m_Wnd->CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL), 
		_T(""), WS_POPUP,
		m_Rect, NULL ,NULL ,NULL); 

	m_Wnd->ShowWindow(SW_HIDE);
	m_Wnd->EnableWindow(FALSE);// 还是需要灰掉窗口，这样窗口不再被激活，因为游戏窗口要在激活的情况下才响应输入，cxb

	CSize sizeWindow(m_Rect.right - m_Rect.left, m_Rect.bottom - m_Rect.top);
	CDC* pDC = m_Wnd->GetDC();
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, sizeWindow.cx, sizeWindow.cy);
	CBitmap *pOldBmp = memDC.SelectObject(&bmp);

	Graphics graph(memDC.m_hDC);
	Rect rect(0, 0, sizeWindow.cx, sizeWindow.cy);
	graph.DrawImage(&m_Image, rect,
		0,
		0,
		m_Image.GetWidth(),
		m_Image.GetHeight(), UnitPixel);

 	BLENDFUNCTION blend;
 	blend.BlendOp = 0; 
 	blend.BlendFlags = 0; 
 	blend.AlphaFormat = 1;
 	blend.SourceConstantAlpha = 255;
  	POINT ptWinPos = {m_Rect.left, m_Rect.top};
  	POINT ptSrc = {0,0};
	DWORD dwExStyle = GetWindowLong(m_Wnd->GetSafeHwnd(), GWL_EXSTYLE);
  	if((dwExStyle&0x80000) != 0x80000)
	{
		SetWindowLong(m_Wnd->GetSafeHwnd(), GWL_EXSTYLE, dwExStyle^0x80000);
	}
  
  	BOOL ret = ::UpdateLayeredWindow(m_Wnd->GetSafeHwnd(),pDC->m_hDC,&ptWinPos,
  		&sizeWindow,memDC.m_hDC,&ptSrc,0,&blend, 2);
  	
  	TRACE("UpdateLayeredWindow %d %d rect(%d, %d, %d, %d)",
  		ret, GetLastError(), m_Rect.left, m_Rect.top, sizeWindow.cx, sizeWindow.cy);

	graph.ReleaseHDC(memDC.m_hDC);
	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();
	bmp.DeleteObject();    
	m_Wnd->ReleaseDC(pDC);
}

void CFlashFairy::Show()
{
	m_Wnd->ShowWindow(SW_SHOW);
	m_Wnd->SetTimer(1, m_FlashInterval, TimerProc);
}

void CFlashFairy::Hide()
{
	m_Wnd->KillTimer(1);
	m_Wnd->ShowWindow(SW_HIDE);
}