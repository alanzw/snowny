// DemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DInputProxyDemo.h"
#include "DemoDlg.h"


// CDemoDlg dialog

IMPLEMENT_DYNAMIC(CDemoDlg, CDialog)

CDemoDlg::CDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDemoDlg::IDD, pParent)
{

}

CDemoDlg::~CDemoDlg()
{
}

void CDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDemoDlg, CDialog)
END_MESSAGE_MAP()


// CDemoDlg message handlers
