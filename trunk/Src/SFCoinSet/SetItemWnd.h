#pragma once


// CSetItemWnd

class CSetItemWnd : public CWnd
{
	DECLARE_DYNAMIC(CSetItemWnd)

public:
	CSetItemWnd(CString id, CString name, CStringArray &contents, int curSetIndex);
	virtual ~CSetItemWnd();

	void SetActive(BOOL isActive)
	{
		m_IsActive = isActive;
		this->Invalidate(TRUE);
	}
 	void ToNext()
 	{
 		if (m_CurSetIndex < m_Contents.GetSize() - 1)
		{
			m_CurSetIndex ++;
			this->Invalidate(TRUE);
			SaveConfig();
 		}
 	}

	void ToPrev()
	{
		if (m_CurSetIndex > 0)
		{
			m_CurSetIndex --;
			this->Invalidate(TRUE);
			SaveConfig();
		}
	}

protected:
	void SaveConfig();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

private:
	BOOL			m_IsActive;
	CString			m_ID;
	CString			m_Name;
	CStringArray	m_Contents;
	int				m_CurSetIndex;
};


