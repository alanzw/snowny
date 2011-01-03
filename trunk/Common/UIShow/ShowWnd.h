#pragma once
#include <string>

// CShowWnd
class ImageEx;
class CShowWnd : public CWnd
{
	DECLARE_DYNAMIC(CShowWnd)

public:
	CShowWnd(std::wstring picName,int width = 0, int Heght = 0);
	virtual ~CShowWnd();

protected:
	DECLARE_MESSAGE_MAP()
	ImageEx *m_pImage;
	std::wstring m_picName;

	int m_bakWidth;
	int m_bakHeight;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


