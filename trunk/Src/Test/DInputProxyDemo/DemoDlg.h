#pragma once


// CDemoDlg dialog

class CDemoDlg : public CDialog
{
	DECLARE_DYNAMIC(CDemoDlg)

public:
	CDemoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDemoDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_DEMO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
