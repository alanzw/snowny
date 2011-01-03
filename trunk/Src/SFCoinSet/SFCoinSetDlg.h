// SFCoinSetDlg.h : header file
//

#pragma once

#include "SetItemWnd.h"
#include <vector>

// CSFCoinSetDlg dialog
class CSFCoinSetDlg : public CDialog
{
// Construction
public:
	CSFCoinSetDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SFCOINSET_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();

	void ReadConfig();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	std::vector<CSetItemWnd *> m_SetItems;
	int m_CurItem;
	RECT m_SettingRect;
	
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
