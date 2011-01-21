#pragma once

class CFlashFairy
{
public:
	CFlashFairy(LPCTSTR picFileName, const RECT &rect, DWORD flashInterval = 1000);
	CFlashFairy(LPCTSTR picFileName, const POINT &rect, DWORD flashInterval = 1000);

	~CFlashFairy(void);

	void Show();
	void Hide();

private:
	void Initialize();

	CWnd *m_Wnd;
	RECT  m_Rect;
	Image m_Image;
	DWORD m_FlashInterval;
};
