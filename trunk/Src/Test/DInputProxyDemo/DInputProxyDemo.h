// DInputProxyDemo.h : main header file for the DInputProxyDemo DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDInputProxyDemoApp
// See DInputProxyDemo.cpp for the implementation of this class
//

class CDInputProxyDemoApp : public CWinApp
{
public:
	CDInputProxyDemoApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
