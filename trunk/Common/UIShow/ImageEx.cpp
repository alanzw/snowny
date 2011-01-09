// GDIPlusHelper.cpp: implementation of the CGDIPlusHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageEx.h"
#include <process.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
ImageEx::ImageEx(LPCTSTR sResourceType, LPCTSTR sResource)
{
	Initialize();

	if (Load(sResourceType, sResource))
	{

	   nativeImage = NULL;
	   
	   lastResult = DllExports::GdipLoadImageFromStreamICM(m_pStream, &nativeImage);
	  
	   TestForAnimatedGIF();
    }
}


////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
ImageEx::ImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement) : Image(filename, useEmbeddedColorManagement)
{
	Initialize();

	m_bIsInitialized = true;

	TestForAnimatedGIF();
}

////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
ImageEx::~ImageEx()
{
	Destroy();
}
////////////////////////////////////////////////////////////////////////////////
// Name						Date		Version		Comments
// N T ALMOND and zyc      29012002		1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::InitAnimation(HWND hWnd, CPoint pt /*= CPoint(0,0)*/, int width/* = 0*/, int height/* = 0*/)
{

	m_hWnd = hWnd;
	m_pt = pt;
	m_imageWidth = width;
	m_imageHeght = height;

	TRACE("bbbb ImageEx::InitAnimation()1");

	if (width ==0 || height == 0)
	{
		m_imageWidth = GetWidth();
		m_imageHeght = GetHeight();
	}

	if (!m_bIsInitialized)
	{
		TRACE(_T("GIF not initialized\n"));
		return false;
	};

	if (IsAnimatedGIF())
	{
		TRACE(_T("GIF is initialized\n"));
		if (m_hThread == NULL)
		{
		
			unsigned int nTID = 0;

			m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadAnimationProc, this, CREATE_SUSPENDED,&nTID);
			
			if (!m_hThread)
			{
				TRACE(_T("Couldn't start a GIF animation thread\n"));
				return true;
			} 
			else 
				ResumeThread(m_hThread);
		}
		else
		{
			return true;
		}
	}
	else
	{
		if (m_transparent)
		{
			DrawTransprentWindow();
		}
		else
		{ 
			DrawNormalWindow();
		}
	}

	return false;	

}
////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::LoadFromBuffer(BYTE* pBuff, int nSize)
{
	bool bResult = false;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	if (hGlobal)
	{
		void* pData = GlobalLock(hGlobal);
		if (pData)
			memcpy(pData, pBuff, nSize);
		
		GlobalUnlock(hGlobal);

		if (CreateStreamOnHGlobal(hGlobal, TRUE, &m_pStream) == S_OK)
			bResult = true;

	}


	return bResult;
}
////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::GetResource(LPCTSTR lpName, LPCTSTR lpType, void* pResource, int& nBufSize)
{ 
	HRSRC		hResInfo;
	HANDLE		hRes;
	LPSTR		lpRes	= NULL; 
	int			nLen	= 0;
	bool		bResult	= FALSE;

	// Find the resource

	hResInfo = FindResource(m_hInst , lpName, lpType);
	if (hResInfo == NULL) 
	{
		DWORD dwErr = GetLastError();
		return false;
	}

	// Load the resource
	hRes = LoadResource(m_hInst , hResInfo);

	if (hRes == NULL) 
		return false;

	// Lock the resource
	lpRes = (char*)LockResource(hRes);

	if (lpRes != NULL)
	{ 
		if (pResource == NULL)
		{
			nBufSize = SizeofResource(m_hInst , hResInfo);
			bResult = true;
		}
		else
		{
			if (nBufSize >= (int)SizeofResource(m_hInst , hResInfo))
			{
				memcpy(pResource, lpRes, nBufSize);
				bResult = true;
			}
		} 

		UnlockResource(hRes);  
	}

	// Free the resource
	FreeResource(hRes);

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::Load(CString sResourceType, CString sResource)
{
	bool bResult = false;


	BYTE*	pBuff = NULL;
	int		nSize = 0;
	if (GetResource(sResource.GetBuffer(0), sResourceType.GetBuffer(0), pBuff, nSize))
	{
		if (nSize > 0)
		{
			pBuff = new BYTE[nSize];

			if (GetResource(sResource, sResourceType.GetBuffer(0), pBuff, nSize))
			{
				if (LoadFromBuffer(pBuff, nSize))
				{

					bResult = true;
				}
			}

			delete [] pBuff;
		}
	}


	m_bIsInitialized = bResult;

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
CSize ImageEx::GetSize()
{
	return CSize(GetWidth(), GetHeight());
}

////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::TestForAnimatedGIF()
{
   UINT count = 0;
   count = GetFrameDimensionsCount();
   GUID* pDimensionIDs = new GUID[count];

   // Get the list of frame dimensions from the Image object.
   GetFrameDimensionsList(pDimensionIDs, count);

   // Get the number of frames in the first dimension.
   m_nFrameCount = GetFrameCount(&pDimensionIDs[0]);

	// Assume that the image has a property item of type PropertyItemEquipMake.
	// Get the size of that property item.
   int nSize = GetPropertyItemSize(PropertyTagFrameDelay);

   // Allocate a buffer to receive the property item.
   m_pPropertyItem = (PropertyItem*) malloc(nSize);

   GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);

   
   delete  pDimensionIDs;

   return m_nFrameCount > 1;
}

////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void ImageEx::Initialize()
{
	m_pStream = NULL;
	m_nFramePosition = 0;
	m_nFrameCount = 0;
	m_pStream = NULL;
	lastResult = InvalidParameter;
	m_hThread = NULL;
	m_bIsInitialized = false;
	m_pPropertyItem = NULL;
	m_transparent = false;
	
#ifdef INDIGO_CTRL_PROJECT
	m_hInst = _Module.GetResourceInstance();
#else
	m_hInst = AfxGetResourceHandle();
#endif


	m_bPause = false;

	m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	m_hPause = CreateEvent(NULL,TRUE,TRUE,NULL);

}

////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
UINT WINAPI ImageEx::_ThreadAnimationProc(LPVOID pParam)
{
	ASSERT(pParam);
	ImageEx *pImage = reinterpret_cast<ImageEx *> (pParam);
	pImage->ThreadAnimation();

	return 0;
}
////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void ImageEx::ThreadAnimation()
{
	m_nFramePosition = 0;

	bool bExit = false;
	while (bExit == false)
	{
		bExit = DrawFrameGIF();
		Sleep(1);
	}
}
////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::DrawFrameGIF()
{
	if (!::IsWindowVisible(m_hWnd))
	{
		m_nFramePosition = 0;
		return false;
	}
	::WaitForSingleObject(m_hPause, INFINITE);
	long hmHeight = GetHeight();

	GUID   pageGuid = FrameDimensionTime;

	long hmWidth = GetWidth();

	if (m_transparent)
	{
		DrawTransprentWindow();
	}
	else
	{
		DrawNormalWindow();
	}

	SelectActiveFrame(&pageGuid, m_nFramePosition++);		
	
	if (m_nFramePosition == m_nFrameCount)
		m_nFramePosition = 0;


	long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;

	DWORD dwErr = WaitForSingleObject(m_hExitEvent, lPause);

	return dwErr == WAIT_OBJECT_0;


}


////////////////////////////////////////////////////////////////////////////////
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void ImageEx::SetPause(bool bPause)
{
	if (!IsAnimatedGIF())
		return;

	if (bPause && !m_bPause)
	{
		::ResetEvent(m_hPause);
	}
	else
	{

		if (m_bPause && !bPause)
		{
			::SetEvent(m_hPause);
		}
	}

	m_bPause = bPause;
}


void ImageEx::Destroy()
{
	
	if (m_hThread)
	{
		// If pause un pause
		SetPause(false);

		SetEvent(m_hExitEvent);
		WaitForSingleObject(m_hThread, INFINITE);
	}

	CloseHandle(m_hThread);
	CloseHandle(m_hExitEvent);
	CloseHandle(m_hPause);

	free(m_pPropertyItem);

	m_pPropertyItem = NULL;
	m_hThread = NULL;
	m_hExitEvent = NULL;
	m_hPause = NULL;

	if (m_pStream)
		m_pStream->Release();

}

void ImageEx::SetTransprentWindow( bool transprent )
{
	m_transparent = transprent;
}

void ImageEx::DrawTransprentWindow()
{
	HDC hDC = GetDC(m_hWnd);	
	HDC hdcMemory=CreateCompatibleDC(hDC);
	HBITMAP hBitMap=CreateCompatibleBitmap(hDC, m_imageWidth, m_imageHeght);
	SelectObject(hdcMemory,hBitMap);
	if (hDC)
	{
		m_Blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
		m_Blend.BlendFlags=0; //nothingelseisspecial...
		m_Blend.AlphaFormat=1; //...
		m_Blend.SourceConstantAlpha= 255;//AC_SRC_ALPHA]

		Graphics graphics(hdcMemory);

		TRACE("bbbb ImageEx::DrawTransprentWindow()");


		RectF rect;
		rect.X = m_pt.x;
		rect.Y = m_pt.y;
		rect.Width = m_imageWidth;
		rect.Height = m_imageHeght;
		graphics.DrawImage(this, rect, 0, 0, GetWidth(),GetHeight(), UnitPixel );

		TRACE("bbbb gif DrawImage width %d, height %d", GetWidth(), GetHeight());


		RECT rct;
		::GetWindowRect(m_hWnd, &rct);
		POINT ptWinPos={rct.left, rct.top};// 这里要用窗口本身的位置，不能用{0,0}，倒计时跑到窗口左上角就是因为它，cxb
		SIZE sizeWindow={m_imageWidth , m_imageHeght};
		BOOL bRet=FALSE;
		POINT ptSrc={0, 0};
		DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
		if((dwExStyle&0x80000)!=0x80000)
			SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^0x80000);

		::UpdateLayeredWindow(m_hWnd, hDC, &ptWinPos,
			&sizeWindow, hdcMemory, &ptSrc,0, &m_Blend, 2);

		DeleteObject(hBitMap);
		ReleaseDC(m_hWnd, hdcMemory);
		ReleaseDC(m_hWnd, hDC);
	}
}

void ImageEx::DrawNormalWindow()
{
	HDC hDC = GetDC(m_hWnd);	
	if (hDC)
	{
		Graphics graphics(hDC);
		RectF rect;
		rect.X = 0;
		rect.Y = 0;
		rect.Width = m_imageWidth;
		rect.Height = m_imageHeght;
		graphics.DrawImage(this, rect, m_pt.x, m_pt.y, GetWidth(),GetHeight(), UnitPixel );
		ReleaseDC(m_hWnd, hDC);
	}
}

void ImageEx::AddCutRect( RectF rect )
{
	m_cutRectList.push_back(rect);
}

void ImageEx::ClearRect()
{
	m_cutRectList.clear();
}

void ImageEx::DrawTransprentWindowCutRect()
{
	HDC hDC = GetDC(m_hWnd);
	HDC hdcMemory=CreateCompatibleDC(hDC);
	HBITMAP hBitMap=CreateCompatibleBitmap(hDC, GetWidth(),GetHeight());
	SelectObject(hdcMemory,hBitMap);
	if (hDC)
	{
		m_Blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
		m_Blend.BlendFlags=0; //nothingelseisspecial...
		m_Blend.AlphaFormat=1; //...
		m_Blend.SourceConstantAlpha= 255;//AC_SRC_ALPHA]

		Graphics graphics(hdcMemory);

		RectF rect;
		rect.X = m_cutRectPt.x;
		rect.Y = m_cutRectPt.y;
		int hmWidth = 0;
		int hmHeight = 0;

		std::vector<RectF>::iterator iter = m_cutRectList.begin();
		for (; iter != m_cutRectList.end(); ++iter)
		{
			TRACE("bbbb destRect x: %f, y: %f, width: %f, height: %f", rect.X, rect.Y, iter->Width, iter->Height);
			TRACE("bbbb RectF x: %f, y: %f, width:%f, height: %f",iter->X, iter->Y, iter->Width, iter->Height);
			hmWidth += iter->Width;
			hmHeight = iter->Height;
			rect.Width = iter->Width;
			rect.Height = iter->Height;
			graphics.DrawImage(this, rect,iter->X, iter->Y,iter->Width,iter->Height, UnitPixel );
			rect.X += iter->Width;
		}


		RECT rct;
		::GetWindowRect(m_hWnd, &rct);
		POINT ptWinPos={0, 0};
		SIZE sizeWindow={GetWidth() , GetHeight()};
		BOOL bRet=FALSE;
		POINT ptSrc={0, 0};
		DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
		if((dwExStyle&0x80000)!=0x80000)
			SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^0x80000);

		::UpdateLayeredWindow(m_hWnd, hDC, NULL,
			&sizeWindow, hdcMemory, &ptSrc,0, &m_Blend, 2);

		DeleteObject(hBitMap);
		ReleaseDC(m_hWnd, hdcMemory);
		ReleaseDC(m_hWnd, hDC);
	}
}

bool ImageEx::InitCutRectAnimation( HWND hwnd )
{
	if (m_cutRectList.size() == 0)
	{
		return false;
	}
	m_hWnd = hwnd;

	if (!m_bIsInitialized)
	{
		TRACE(_T("GIF not initialized\n"));
		return false;
	};

	{
		if (m_transparent)
		{
			DrawTransprentWindowCutRect();
		}
		else
		{ 
			//DrawNormalWindow();
		}
	}

	return true;
}

void ImageEx::SetCutRectTLPt( CPoint pt )
{
	m_cutRectPt = pt;
}