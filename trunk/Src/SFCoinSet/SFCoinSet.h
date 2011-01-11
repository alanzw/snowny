// SFCoinSet.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSFCoinSetApp:
// See SFCoinSet.cpp for the implementation of this class
//

class CSFCoinSetApp : public CWinApp
{
public:
	CSFCoinSetApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
public:
	virtual int ExitInstance();
};

extern CSFCoinSetApp theApp;