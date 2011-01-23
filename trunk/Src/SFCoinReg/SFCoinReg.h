// SFCoinReg.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSFCoinRegApp:
// See SFCoinReg.cpp for the implementation of this class
//

class CSFCoinRegApp : public CWinApp
{
public:
	CSFCoinRegApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSFCoinRegApp theApp;