// SetItemWnd.cpp : implementation file
//

#include "stdafx.h"
#include "SFCoinSet.h"
#include "SetItemWnd.h"
#include <assert.h>
#include <shlwapi.h>


// CSetItemWnd

IMPLEMENT_DYNAMIC(CSetItemWnd, CWnd)

CSetItemWnd::CSetItemWnd(CString id, CString name, CStringArray &contents, int curSetIndex)
: m_IsActive(FALSE)
, m_ID(id)
, m_Name(name)
, m_CurSetIndex(curSetIndex)
{
	m_Contents.Copy(contents);
	assert(m_Contents.GetSize() > 0);
}

CSetItemWnd::~CSetItemWnd()
{
}


BEGIN_MESSAGE_MAP(CSetItemWnd, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CSetItemWnd message handlers



void CSetItemWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	CRect rect;
	GetClientRect(&rect);
	CFont font;
	font.CreateFont(25, 10, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
	dc.SelectObject(font);
	dc.SetTextColor(RGB(255, 255, 255));
	CPoint textPt;
	if (m_IsActive)
	{
		dc.FillSolidRect(&rect, RGB(255, 0, 0));
		textPt = CPoint(50, 0);
	}
	else
	{
		dc.FillSolidRect(&rect, RGB(0, 0, 0));
		textPt = CPoint(50, 5);
	}
	dc.TextOut(textPt.x, textPt.y, m_Name);
	if (m_CurSetIndex > 0)
	{
		dc.SetTextColor(RGB(0, 0, 0));
		dc.TextOut(textPt.x + 140, textPt.y, TEXT("<"));
	}
	dc.SetTextColor(RGB(255, 255, 255));
	CRect contectRect(textPt.x + 110, textPt.y, textPt.x + 290, textPt.y + 25);
	dc.DrawText(m_Contents[m_CurSetIndex], &contectRect, DT_CENTER);
	if (m_CurSetIndex < m_Contents.GetSize() - 1)
	{
		dc.SetTextColor(RGB(0, 0, 0));
		dc.TextOut(textPt.x + 250, textPt.y, TEXT(">"));
	}
}

void CSetItemWnd::SaveConfig()
{
	CString sel;
	sel.Format(TEXT("%d"), m_CurSetIndex);

	TCHAR temp[MAX_PATH] = {0};
	GetModuleFileName(GetModuleHandle(NULL), temp, MAX_PATH);
	PathRemoveFileSpec(temp);
	CString fileName = temp;
	fileName += "\\Config.ini";
	WritePrivateProfileString(m_ID, TEXT("Sel"), sel, fileName);
}