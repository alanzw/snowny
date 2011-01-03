// GDIPlusHelper.h: interface for the CGDIPlusHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_)
#define AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>

typedef struct CutRect
{
	RectF srcRect;
}CutRect;

class ImageEx : public Image
{
public:
	ImageEx(IN LPCTSTR  sResourceType, IN LPCTSTR  sResource);
	ImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement = FALSE);

	
	~ImageEx();
public:

	void	Draw(CDC* pDC);
	CSize	GetSize();

	bool	IsAnimatedGIF() { return m_nFrameCount > 1; }
	void	SetPause(bool bPause);
	bool	IsPaused() { return m_bPause; }
	bool	InitAnimation(HWND hWnd, CPoint pt = CPoint(0,0), int width = 0, int height = 0);
	void	DrawNormalWindow();

	void	SetTransprentWindow(bool transprent);
	void	Destroy();

	void    SetCutRectTLPt(CPoint pt);
	//只能添加Rect的一个元素
	void	AddCutRect(RectF rect);
	void	ClearRect();
	bool	InitCutRectAnimation(HWND hwnd);

protected:
	void				DrawTransprentWindow();
	void				DrawTransprentWindowCutRect();

	bool				TestForAnimatedGIF();
	void				Initialize();
	bool				DrawFrameGIF();

	IStream*			m_pStream;
	
	bool LoadFromBuffer(BYTE* pBuff, int nSize);
	bool GetResource(LPCTSTR lpName, LPCTSTR lpType, void* pResource, int& nBufSize);
	bool Load(CString sResourceType, CString sResource);

	void ThreadAnimation();

	static UINT WINAPI _ThreadAnimationProc(LPVOID pParam);

	HANDLE			m_hThread;
	HANDLE			m_hPause;
	HANDLE			m_hExitEvent;
	HINSTANCE		m_hInst;
	HWND			m_hWnd;
	UINT			m_nFrameCount;
	UINT			m_nFramePosition;
	bool			m_bIsInitialized;
	bool			m_bPause;
	PropertyItem*	m_pPropertyItem;
	CPoint			m_pt;
	bool			m_transparent;

	int				m_imageWidth;
	int				m_imageHeght;
	CPoint			m_cutRectPt;
	std::vector<RectF> m_cutRectList;



	BLENDFUNCTION	m_Blend;
};

/////////////////////////////////////////////////////////////////////
// Name		Date		Version	   Comments
// zyc      29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
class NULLImageEx:public ImageEx
{
public:
	NULLImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement = FALSE):ImageEx(filename, useEmbeddedColorManagement) {;}

	void	Draw(CDC* pDC) {;}
	CSize	GetSize() {return true;}

	bool	IsAnimatedGIF() { return true; }
	void	SetPause(bool bPause) {;}
	bool	IsPaused() { return true; }
	bool	InitAnimation(HWND hWnd, CPoint pt) {return true;}
	void	Destroy() {;}
};


#endif // !defined(AFX_GDIPLUSHELPER_H__BD5F6266_5686_43E2_B146_5EA1217A56FE__INCLUDED_)
