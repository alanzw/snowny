// GDIPlusHelper.cpp: implementation of the CGDIPlusHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageEx.h"
#include <process.h>

//
//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif


ImageExTest::ImageExTest( const WCHAR* filename, BOOL useEmbeddedColorManagement /*= FALSE*/ ) : Image(filename, useEmbeddedColorManagement)
{
	m_imageRect = RectF(0,0,0,0);
	m_show = false;
	Initialize();
	TestForAnimatedGIF();
}

RectF ImageExTest::ImageRect() const
{
	return m_imageRect;
}

void ImageExTest::ImageRect( RectF val)
{
	m_imageRect = val;
	m_state = WholeRectType;
}

void ImageExTest::Initialize()
{
	m_pStream = NULL;
	m_nFramePosition = 0;
	m_nFrameCount = 0;
	m_pStream = NULL;
	lastResult = InvalidParameter;
	m_pPropertyItem = NULL;

#ifdef INDIGO_CTRL_PROJECT
	m_hInst = _Module.GetResourceInstance();
#else
	m_hInst = AfxGetResourceHandle();
#endif
}

bool ImageExTest::TestForAnimatedGIF()
{
	UINT count = 0;
	count = GetFrameDimensionsCount();
	GUID* pDimensionIDs = new GUID[count];
	GetFrameDimensionsList(pDimensionIDs, count);
	m_nFrameCount = GetFrameCount(&pDimensionIDs[0]);
	int nSize = GetPropertyItemSize(PropertyTagFrameDelay);
	m_pPropertyItem = (PropertyItem*) malloc(nSize);
	GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);
	delete  pDimensionIDs;
	return m_nFrameCount > 1;
}

void ImageExTest::SetGifActiveFrame( int nFramePosition )
{
	GUID pageGuid = FrameDimensionTime;
	SelectActiveFrame(&pageGuid, nFramePosition % m_nFrameCount);
}

void ImageExTest::AddCutRect( RectF rect )
{
	m_imageCutRect.cutRectList.push_back(rect);
}

void ImageExTest::ClearRect()
{
	m_imageCutRect.cutRectList.clear();
}

CutRectF ImageExTest::GetCutRect()
{
	return m_imageCutRect;
}

RectType ImageExTest::GetShowRectType()
{
	return m_state;
}

CutRectF ImageExTest::ImageCutRect() const
{
	return m_imageCutRect;
}

void ImageExTest::ImageCutRect( CutRectF val )
{
	m_imageCutRect = val;
	m_state = CutRectType;
}

ImageExManager::ImageExManager( HWND hwnd )
{
	m_hwnd = hwnd;
	m_MaxFrameCount = 0;
	m_hThread = NULL;

	m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hPause = CreateEvent(NULL,TRUE,TRUE,NULL);
	m_curFramePosition = 0;
}

void ImageExManager::CreateImageEx( int id, const WCHAR* filename, CPoint pt, int width /*= 0*/, int height /*= 0*/, BOOL useEmbeddedColorManagement /*= FALSE*/ )
{
	TRACE("aaaa ImageExTest sucess");
	ImageExTest *pImage = new ImageExTest(filename);
	if (width == 0 || height == 0)
	{
		width = pImage->GetWidth();
		height = pImage->GetHeight();
	}
	TRACE("aaaa CreateImageEx sucess b");
	pImage->ImageRect(RectF(pt.x, pt.y, width, height));
	m_ImageExList[id] = pImage;
	TRACE("aaaa CreateImageEx sucess a");
}

bool ImageExManager::ShowImageExFlag( int id )
{
	std::map<int, ImageExTest*>::iterator iter;
	if (m_ImageExList.find(id)  != m_ImageExList.end())
	{
		m_ImageExList[id]->Show();
		return true;
	}
	return false;
}

bool ImageExManager::HideImageExFlag( int id )
{
	std::map<int, ImageExTest*>::iterator iter;
	if (m_ImageExList.find(id)  != m_ImageExList.end())
	{
		m_ImageExList[id]->Hide();
		return true;
	}
	return false;
}

void ImageExManager::ProcessShow()
{
	CRect winRect;
	::GetWindowRect(m_hwnd, &winRect);
	HDC hDC = GetDC(m_hwnd);	
	HDC hdcMemory=CreateCompatibleDC(hDC);

	HBITMAP hBitMap=CreateCompatibleBitmap(hDC, winRect.Width(), winRect.Height());//创建需要大小的dc
	SelectObject(hdcMemory,hBitMap);
	if (hDC)
	{
		m_Blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
		m_Blend.BlendFlags=0; //nothingelseisspecial...
		m_Blend.AlphaFormat=1; //...
		m_Blend.SourceConstantAlpha= 255;//AC_SRC_ALPHA
		Graphics graphics(hdcMemory);

		DrawImageList(graphics);

		POINT ptdes={winRect.left,  winRect.top};									//代表指定dc的位置
		SIZE sizeWindow={winRect.Width() , winRect.Height()};						//指定dc的大小
		BOOL bRet=FALSE;
		POINT ptSrc={0, 0};
		DWORD dwExStyle=GetWindowLong(m_hwnd,GWL_EXSTYLE);
		if((dwExStyle&0x80000)!=0x80000)
			SetWindowLong(m_hwnd,GWL_EXSTYLE,dwExStyle^0x80000);

		::UpdateLayeredWindow(m_hwnd, hDC, &ptdes,
			&sizeWindow, hdcMemory, &ptSrc,0, &m_Blend, 2);

		DeleteObject(hBitMap);
		DeleteObject(hdcMemory); // 资源泄露，在测试机上表现的很卡，cxb
		ReleaseDC(m_hwnd, hDC);
	}
}

void ImageExManager::Hide( int id )
{
	if (!HideImageExFlag(id))
	{
		TRACE(_T("aaaa not hide\n"));
		return;
	}
	// 退出gif绘制线程，不然会无谓的刷新界面，cxb
	if (!IsGifInShow())
	{
		DestroyThreadToShow();
	}
	ProcessShow();
}

void ImageExManager::Show( int id )
{
	if (!ShowImageExFlag(id))
	{
		TRACE(_T("aaaa not show\n"));
		return;
	}
	if (IsGifInShow())
	{
		CreateThreadToShow();
	}
	else
	{
		ProcessShow();
	}

}

bool ImageExManager::IsGifInShow()
{
	std::map<int, ImageExTest*>::iterator iter = m_ImageExList.begin();
	for (; iter != m_ImageExList.end(); ++iter)
	{
		if ((*iter).second->IsAnimatedGIF() && iter->second->IsShow())	// 有需要显示的gif，cxb
		{
			return true;
		}
	}
	return false;
}

bool ImageExManager::CreateThreadToShow()
{
	if (m_hThread == NULL)
	{
		TRACE(_T("Create thread for gif\n"));
		unsigned int nTID = 0;
		// 清空信号状态，cxb
		ResetEvent(m_hExitEvent);
		SetEvent(m_hPause);
		m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadAnimationProc, this, CREATE_SUSPENDED,&nTID);
		if (!m_hThread)
		{
			TRACE(_T("Couldn't start a GIF animation thread\n"));
			return true;
		} 
		else 
		{
			ResumeThread(m_hThread);
		}
	}
	return false;
}

UINT WINAPI ImageExManager::_ThreadAnimationProc( LPVOID pParam )
{
	ASSERT(pParam);
	ImageExManager *pImageManager = reinterpret_cast<ImageExManager *> (pParam);
	pImageManager->m_curFramePosition = 0;
	bool bExit = false;
	while (bExit == false)
	{
		bExit = pImageManager->DrawFrameGIF();
		Sleep(1);
	}

	return 0;
}

bool ImageExManager::DrawFrameGIF()
{
	if (!::IsWindowVisible(m_hwnd))
	{
		m_curFramePosition = 0;
		return false;
	}
	::WaitForSingleObject(m_hPause, INFINITE);
	//my code
	ProcessShow();
	ImageExTest* pImageMaxTest = GetMaxFrameGif();
	ProcessGifsActiveFrame(m_curFramePosition++);
	if (m_curFramePosition == m_MaxFrameCount)
	{
		m_curFramePosition = 0;
	}
	long lPause = ((long*) pImageMaxTest->m_pPropertyItem->value)[pImageMaxTest->m_nFramePosition] * 10;
	DWORD dwErr = WaitForSingleObject(m_hExitEvent, lPause);
	return dwErr == WAIT_OBJECT_0;
}

void ImageExManager::ProcessGifsActiveFrame(int framePosition)
{
	std::map<int, ImageExTest*>::iterator iter = m_ImageExList.begin();
	for (; iter != m_ImageExList.end(); ++iter)
	{
		if ((*iter).second->IsAnimatedGIF())
		{
			(*iter).second->SetGifActiveFrame(framePosition);
		}
	}
}

ImageExTest * ImageExManager::GetMaxFrameGif()
{
	ImageExTest *pMaxImage = NULL;
	m_MaxFrameCount = 0;
	std::map<int, ImageExTest*>::iterator iter = m_ImageExList.begin();
	for (; iter != m_ImageExList.end(); ++iter)
	{
		if ((*iter).second->IsAnimatedGIF())
		{
			int frameCount = (*iter).second->m_nFrameCount;
			if (frameCount >= m_MaxFrameCount)
			{
				pMaxImage = (*iter).second;
				m_MaxFrameCount = pMaxImage->m_nFrameCount;
			}
		}
	}
	return pMaxImage;
}

void ImageExManager::DrawImageList( Graphics &graphics )
{
	std::map<int, ImageExTest*>::iterator iter = m_ImageExList.begin();
	for (; iter != m_ImageExList.end(); ++iter)
	{
		if ((*iter).second->IsShow())
		{
			RectF rect;
			ImageExTest* pImage = (*iter).second;
			RectType type = pImage->GetShowRectType();
			switch (type)
			{
			case WholeRectType:
				rect = pImage->ImageRect();
				graphics.DrawImage(pImage, rect, 0, 0, pImage->GetWidth(),pImage->GetHeight(), UnitPixel );
				break;
			case CutRectType:
				CutRectF cutRect = pImage->GetCutRect();
				rect.X = cutRect.pt.x;
				rect.Y = cutRect.pt.y;	// 这个bug，我找了好久，cxb
				int hmWidth = 0;
				int hmHeight = 0;
				std::vector<RectF>::iterator cutRectIter = cutRect.cutRectList.begin();
				TRACE("bbbb cutrect count:%d", cutRect.cutRectList.size());
				for (; cutRectIter !=  cutRect.cutRectList.end(); ++cutRectIter)
				{
					//TRACE("bbbb destRect x: %f, y: %f, width: %f, height: %f", rect.X, rect.Y, cutRectIter->Width, cutRectIter->Height);
					//TRACE("bbbb RectF x: %f, y: %f, width:%f, height: %f",cutRectIter->X, cutRectIter->Y, cutRectIter->Width, cutRectIter->Height);
					hmWidth += cutRectIter->Width;
					hmHeight = cutRectIter->Height;
					rect.Width = cutRectIter->Width;
					rect.Height = cutRectIter->Height;
					graphics.DrawImage(pImage, rect,cutRectIter->X, cutRectIter->Y,cutRectIter->Width,cutRectIter->Height, UnitPixel );
					rect.X += cutRectIter->Width;
				}
				break;
			}

		}
	}
}

void ImageExManager::CreateCutRectImageEx( int id, const WCHAR* filename, CutRectF cutRectF, BOOL useEmbeddedColorManagement /*= FALSE*/ )
{
	ImageExTest *pImage = new ImageExTest(filename);
	pImage->ImageCutRect(cutRectF);
	m_ImageExList[id] = pImage;
}

void ImageExManager::Reset( int id, CutRectF cutRectF )
{
	std::map<int, ImageExTest*>::iterator iter;
	if (m_ImageExList.find(id)  != m_ImageExList.end())
	{
		cutRectF.pt = m_ImageExList[id]->ImageCutRect().pt;
		m_ImageExList[id]->ImageCutRect(cutRectF);
	}
}

void ImageExManager::Destroy( int id )
{
	std::map<int, ImageExTest*>::iterator iter = m_ImageExList.find(id);
	if (iter  != m_ImageExList.end())
	{
		delete m_ImageExList[id];
		m_ImageExList.erase(iter);
	}
}

void ImageExManager::DestroyAll()
{
	std::map<int, ImageExTest*>::iterator iter = m_ImageExList.begin();
	for (; iter != m_ImageExList.end(); ++iter)
	{
		delete (*iter).second;
	}
	m_ImageExList.clear();
}

void ImageExManager::HideAll()
{
	std::map<int, ImageExTest*>::iterator iter = m_ImageExList.begin();
	for (; iter != m_ImageExList.end(); ++iter)
	{
		Hide((*iter).first);
	}
}

void ImageExManager::ShowAll()
{
	std::map<int, ImageExTest*>::iterator iter = m_ImageExList.begin();
	for (; iter != m_ImageExList.end(); ++iter)
	{
		Show((*iter).first);
	}
}

void ImageExManager::DestroyThreadToShow()
{
	if (m_hThread != NULL)
	{
		TRACE(_T("Destroy thread for gif\n"));
		SetEvent(m_hExitEvent);
		m_hThread = NULL;
	}
}
