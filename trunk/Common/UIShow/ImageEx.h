// GDIPlusHelper.h: interface for the CGDIPlusHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_)
#define AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include <map>


/////////////////////////////////////////////////////////////////////
// Name		Date		Version
// zyc      2011		2.0			
// 
////////////////////////////////////////////////////////////////////////////////

typedef struct CutRect
{
	RectF srcRect;
}CutRect;
typedef enum{WholeRectType, CutRectType} RectType; 


typedef struct 
{
	CPoint pt;
	std::vector<RectF> cutRectList;
}CutRectF;

class ImageExTest:public Image
{
public:
	ImageExTest(const WCHAR* filename, BOOL useEmbeddedColorManagement = FALSE);
	RectF	ImageRect() const;
	void	ImageRect(RectF val);
	CutRectF	ImageCutRect() const;
	void		ImageCutRect(CutRectF val);
	void	Show() {m_show = true;}
	void	Hide() {m_show = false;}
	bool	IsShow() {return m_show;}
	bool	IsAnimatedGIF() {return m_nFrameCount > 1;}
	bool	TestForAnimatedGIF();
	void	SetGifActiveFrame(int nFramePosition);

	void	AddCutRect( RectF rect );
	void	ClearRect();
	
	CutRectF GetCutRect();
	RectType GetShowRectType();
	

private:
	void	Initialize();
public:
	UINT			m_nFrameCount;
	UINT			m_nFramePosition;
	PropertyItem*	m_pPropertyItem;


private:
	CutRectF		m_imageCutRect;
	RectF			m_imageRect;
	bool			m_show;
	RectType		m_state;

	IStream*		m_pStream;
	HINSTANCE		m_hInst;
	HWND			m_hWnd;

	CPoint			m_cutRectPt;
	std::vector<RectF> m_cutRectList;

	BLENDFUNCTION	m_Blend;

};

class NULLImageEx:public ImageExTest
{
public:
	NULLImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement = FALSE):ImageExTest(filename, useEmbeddedColorManagement) {;}

	void	Draw(CDC* pDC) {;}
	CSize	GetSize() {return true;}

	bool	IsAnimatedGIF() { return true; }
	void	SetPause(bool bPause) {;}
	bool	IsPaused() { return true; }
	bool	InitAnimation(HWND hWnd, CPoint pt) {return true;}
	void	Destroy() {;}
};

class ImageExManager
{
public:
	ImageExManager(HWND hwnd);
	void CreateImageEx(int id, const WCHAR* filename,CPoint pt, int width = 0, int height = 0, BOOL useEmbeddedColorManagement = FALSE);
	void CreateCutRectImageEx(int id, const WCHAR* filename,  CutRectF cutRectF ,BOOL useEmbeddedColorManagement = FALSE);
	void Hide(int id);
	void HideAll();
	void Show(int id);
	void ShowAll();
	void Reset(int id, CutRectF cutRectF);
	void Destroy(int id);
	void DestroyAll();

private:
	bool IsGifInShow();
	bool CreateThreadToShow();
	void DestroyThreadToShow();
	bool ShowImageExFlag( int id );
	bool HideImageExFlag(int id);
	void ProcessShow();
	void GenerateRect(RECT &rect)
	{
		rect.left = 0x0fffffff;
		rect.top = 0x0fffffff;
		rect.bottom = 0;
		rect.right = 0;
		std::map<int, ImageExTest*>::iterator iter = m_ImageExList.begin();
		for (; iter != m_ImageExList.end(); ++iter)
		{
			if (iter->second->IsShow())	// 有需要显示的gif，cxb
			{
				if (iter->second->GetShowRectType() == WholeRectType)
				{
					RectF r = iter->second->ImageRect();
					rect.left = rect.left > r.X ? r.X : rect.left;
					rect.top = rect.top > r.Y ? r.Y : rect.top;
					rect.right = rect.right < r.X + r.Width ? r.X + r.Width : rect.right;
					rect.bottom = rect.bottom < r.Y + r.Height ? r.Y + r.Height : rect.bottom;
				}
				else
				{
					CutRectF cr = iter->second->ImageCutRect();
					rect.left = rect.left > cr.pt.x ? cr.pt.x : rect.left;
					rect.top = rect.top > cr.pt.y ? cr.pt.y : rect.top;
					int width = 0, height = 0;
					for (size_t i = 0;  i < cr.cutRectList.size(); i ++)
					{
						width += cr.cutRectList[i].Width;
						height = height < cr.cutRectList[i].Height ? cr.cutRectList[i].Height : height;
					}
					rect.right = rect.right < cr.pt.x + width ? cr.pt.x + width : rect.right;
					rect.bottom = rect.bottom < cr.pt.y + height ? cr.pt.y + height : rect.bottom;
				}
			}
		}
	}

	void DrawImageList( Graphics &graphics , int offsetX, int offsetY);
	static UINT WINAPI _ThreadAnimationProc(LPVOID pParam);
	bool DrawFrameGIF(); 
	void ProcessGifsActiveFrame(int framePosition);
	ImageExTest *GetMaxFrameGif();
private:
	std::map<int, ImageExTest*>	m_ImageExList;
	HWND					m_hwnd;
	BLENDFUNCTION			m_Blend;
	HANDLE					m_hThread;
	HANDLE					m_hExitEvent;

public:
	HANDLE					m_hPause;
	int						m_MaxFrameCount;
	int						m_curFramePosition;
};

#endif // !defined(AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_)
