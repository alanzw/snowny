#pragma once
#include <string>
#include <map>
#include "showcutrectwnd.h"

class CShowWnd;
class CShowCutRectWnd;
class CFairy
{
public:
	CFairy(void);
	~CFairy(void);

	//pt: SHowWnd×óÉÏ½ÇµÄÎ»ÖÃ
	void CreateTransparentWnd(int id, std::wstring picName, CPoint pt, int width = 0 , int height = 0);
	void DestroyWnd(int id);
	void ShowWnd(int id);
	void HideWnd(int id);
	void ShowAllWnd();
	void HideAllWnd();
	void DestroyAllWnd();
	void CreateCoinInsert(int id, int life, int coins, int cointsOneLife ,CPoint pt, int width = 0 , int height = 0);

private:
	CShowCutRectWnd::CutRectList GetCutRectList( int life, int coins, int cointsOneLife );
	std::vector<int> GetNumList( int number );
private:
	GdiplusStartupInput		m_gdiplusStartupInput;
	ULONG_PTR				m_gdiplusToken;
	CShowWnd				*m_showWnd;
	CShowCutRectWnd			*mInsertCointSingleWnd;
	std::map<int, CWnd*>	m_showWndList;
};
