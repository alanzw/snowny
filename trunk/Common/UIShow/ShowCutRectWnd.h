#pragma once
#include <vector>
#include <string>

// CShowCutRectWnd
class ImageEx;
class CShowCutRectWnd : public CWnd
{
	DECLARE_DYNAMIC(CShowCutRectWnd)

public:
	typedef std::vector<RectF> CutRectList;
	typedef CutRectList::iterator CutRectListIter;
	CShowCutRectWnd(std::wstring picName, CutRectList cutRectList);
	virtual ~CShowCutRectWnd();

	void ResetCutRect(const CutRectList &cutRectList);
private:
	ImageEx *m_pImage;
	std::wstring m_picName;
	CutRectList m_cutRectList;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


