#pragma once
#include <string>
#include <map>
#include <vector>

class CShowWnd;
class ImageExManager;
class CFairy
{
public:
	CFairy();
	~CFairy(void);

	//pt: SHowWnd×óÉÏ½ÇµÄÎ»ÖÃ
	void CreateTransparentPic(int id, std::wstring picName, CPoint pt, int imageWidth = 0 , int imageHeight = 0);
	void SetParent(CWnd* parentWnd);
	void DestroyPic(int id);
	void ShowPic(int id);
	void HidePic(int id);
	void ShowAllPic();
	void HideAllPic();
	void DestroyAllPic();
	void CreateCoinInsert(int id, std::wstring picName, int life, int coins, int cointsOneLife ,CPoint pt, int imageWidth = 0 , int imageHeight = 0);
	void ResetCoinInsert(DWORD id,int life, int coins, int coinsOneLife);
	void CreateBKWnd();
private:
	std::vector<RectF> GetCutRectList( int life, int coins, int cointsOneLife );
	void AddCutRects( int number, RectF rect, std::vector<RectF> &cutRectList );

	std::vector<int> GetNumList( int number );
private:
	GdiplusStartupInput		m_gdiplusStartupInput;
	ULONG_PTR				m_gdiplusToken;
	CWnd*					m_showWnd;
	std::map<int, CWnd*>	m_showWndList;

	ImageExManager*			m_imageExManager;
};
