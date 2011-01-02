// SFCoin.h : main header file for the SFCoin DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSFCoinApp
// See SFCoin.cpp for the implementation of this class
//

class CSFCoinApp : public CWinApp
{
public:
	CSFCoinApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
