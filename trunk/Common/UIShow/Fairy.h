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
	void CreateTransparentWnd(CWnd *parent, int id, std::wstring picName, CPoint pt, int imageWidth = 0 , int imageHeight = 0);

	void DestroyWnd(int id);
	void ShowWnd(int id);
	void HideWnd(int id);
	void ShowAllWnd();
	void HideAllWnd();
	void DestroyAllWnd();
	void CreateCoinInsert(int id, std::wstring picName, int life, int coins, int cointsOneLife ,CPoint pt, int imageWidth = 0 , int imageHeight = 0);
	void ResetCoinInsert(DWORD id,int life, int coins, int coinsOneLife);

private:
	void CreateWnd( CWnd * parent, CWnd* wnd, CPoint pt, int id );
	CShowCutRectWnd::CutRectList GetCutRectList( int life, int coins, int cointsOneLife );
	void AddCutRects( int number, RectF rect, CShowCutRectWnd::CutRectList &cutRectList );

	std::vector<int> GetNumList( int number );
private:
	GdiplusStartupInput		m_gdiplusStartupInput;
	ULONG_PTR				m_gdiplusToken;
	CShowWnd				*m_showWnd;
	CShowCutRectWnd			*mInsertCointSingleWnd;
	std::map<int, CWnd*>	m_showWndList;
};
