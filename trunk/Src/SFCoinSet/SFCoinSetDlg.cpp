// SFCoinSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SFCoinSet.h"
#include "SFCoinSetDlg.h"
#include <io.h>
#include <assert.h>
#include <shlwapi.h>
#include "Input.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD ITEMWITH = 400;
const DWORD ITEMHEIGHT = 30;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSFCoinSetDlg dialog




CSFCoinSetDlg::CSFCoinSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSFCoinSetDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSFCoinSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSFCoinSetDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSFCoinSetDlg message handlers

BOOL CSFCoinSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	if (!m_Input.Initialize(GetSafeHwnd()))
	{
		MessageBox(TEXT("Joystick initialize failed"));
	}

#ifndef _DEBUG
	MoveWindow(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#else
	MoveWindow(0, 0, 800, 600);
#endif

	ReadConfig();

	SetTimer(1, 200, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSFCoinSetDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSFCoinSetDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
 		CRect rect;
 		GetClientRect(&rect);
 		dc.FillSolidRect(&rect, RGB(0, 0, 0));

		rect = m_SettingRect;
		CBrush brush;
		brush.CreateSolidBrush(RGB(100, 0, 0));
		dc.SelectObject(brush);
		dc.RoundRect(rect, CPoint(40, 40));

		dc.SetTextColor(RGB(255, 255, 255));
		dc.SetBkColor(RGB(100, 0,  0));
		CFont font;
		font.CreateFont(30, 12, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, NULL);
		dc.SelectObject(font);
		rect.top += 8;
		rect.bottom = rect.top + ITEMHEIGHT;
		CString setting = TEXT("设    置");
		dc.DrawText(setting, rect, DT_CENTER);

		CBitmap bmp;
		bmp.LoadBitmap(IDB_BITMAP_LOGO);
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		memDC.SelectObject(&bmp);
		GetClientRect(&rect);
		dc.BitBlt((rect.Width() - 640) / 2, 0, 600, 300, &memDC, 0, 50, SRCCOPY);

		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSFCoinSetDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSFCoinSetDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (m_Input.IsKeyDown(VK_UP))
	{
		m_SetItems[m_CurItem]->SetActive(FALSE);
		m_CurItem --;
		if (m_CurItem < 0)
		{
			m_CurItem = m_SetItems.size() - 1;
		}
		m_SetItems[m_CurItem]->SetActive(TRUE);
	}
	else if (m_Input.IsKeyDown(VK_DOWN))
	{
		m_SetItems[m_CurItem]->SetActive(FALSE);
		m_CurItem ++;
		if (m_CurItem == m_SetItems.size())
		{
			m_CurItem = 0;
		}
		m_SetItems[m_CurItem]->SetActive(TRUE);
	}
	if (m_Input.IsKeyDown(VK_LEFT))
	{
		m_SetItems[m_CurItem]->ToPrev();
	}
	if (m_Input.IsKeyDown(VK_RIGHT))
	{
		m_SetItems[m_CurItem]->ToNext();
	}
	// 增加f2退出设置
	if (m_Input.IsKeyDown(VK_F2))
	{
		OnOK();
	}

	CDialog::OnTimer(nIDEvent);
}

void CSFCoinSetDlg::ReadConfig()
{
	CRect rect;
	GetClientRect(&rect);

	m_SettingRect.left = (rect.Width() - ITEMWITH - 20) / 2;
	m_SettingRect.top = 300;
	m_SettingRect.right = m_SettingRect.left + ITEMWITH + 20;
	m_SettingRect.bottom = m_SettingRect.top + ITEMHEIGHT * 6;

	CRect itemRect(0, 0, ITEMWITH, ITEMHEIGHT);
	itemRect.OffsetRect(m_SettingRect.left + 10, m_SettingRect.top + ITEMHEIGHT * 1.5);

	CString app = TEXT("Config"), fileName;
	CString id, key, contents, name;
	TCHAR temp[MAX_PATH] = {0};
	GetModuleFileName(GetModuleHandle(NULL), temp, MAX_PATH);
	PathRemoveFileSpec(temp);
	fileName = temp;
	fileName += "\\Config.ini";

	int count = GetPrivateProfileInt(TEXT("Config"), TEXT("Count"), -1, fileName);
	for (int i = 0; i < count; i ++)
	{
		key.Format(TEXT("Item%d"), i);
		GetPrivateProfileString(app, key, TEXT(""), temp, MAX_PATH, fileName);
		id = temp;
		GetPrivateProfileString(id, TEXT("Name"), TEXT(""), temp, MAX_PATH, fileName);
		name = temp;
		GetPrivateProfileString(id, TEXT("Contents"), TEXT(""), temp, MAX_PATH, fileName);
		contents = temp;

		CStringArray sa;
		while (1)
		{
			int pos = contents.Find(TEXT(';'));
			if (pos == -1)
			{
				sa.Add(contents);
				break;
			}
			sa.Add(contents.Left(pos));
			contents = contents.Right(contents.GetLength() - pos - 1);
		}
		CSetItemWnd *wnd = new CSetItemWnd(id, name, sa, GetPrivateProfileInt(id, TEXT("Sel"), 0, fileName));
		wnd->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, itemRect, this, 0);
		m_SetItems.push_back(wnd);
		itemRect.OffsetRect(0, ITEMHEIGHT);
	}

	if (m_SetItems.size() > 0)
	{
		m_SetItems[0]->SetActive(TRUE);
	}
	m_CurItem = 0;

	m_SettingRect.bottom = m_SettingRect.top + ITEMHEIGHT * (m_SetItems.size() + 2);
}
//回车，退出设置，启动游戏
//esc，退出设置，
void CSFCoinSetDlg::OnOK()
{
	STARTUPINFO si = {0};
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = {0};
	CreateProcessW(TEXT("SFCoinLuancher.exe"), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	CDialog::OnOK();
}
