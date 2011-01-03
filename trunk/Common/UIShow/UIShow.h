// UIShow.h : main header file for the UIShow DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CUIShowApp
// See UIShow.cpp for the implementation of this class
//

class CUIShowApp : public CWinApp
{
public:
	CUIShowApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
