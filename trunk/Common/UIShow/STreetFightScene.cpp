#include "StdAfx.h"
#include "STreetFightScene.h"
#include <shlwapi.h>


CSTreetFightScene::CSTreetFightScene(HMODULE handle)
{
	wchar_t res[512] = {0};
	GetModuleFileName(handle, res, sizeof(res) * sizeof(wchar_t));
	PathRemoveFileSpec(res);
	std::wstring path = res;
	std::wstring backgroundPath = path + L"\\StandbyBG.jpg";
	std::wstring loadingbgPath = path + L"\\LoadingBG.jpg";
	std::wstring creditPath = path + L"\\CREDIT.png";
	std::wstring ipstartPath = path +L"\\IPStart.png";
	std::wstring insertCoinPath = path + L"\\InserCoin.png";
	std::wstring loadingPath = path + L"\\Loading.png";

	m_Fairy.CreateTransparentWnd(0, backgroundPath, CPoint(0,0));
	//m_Fairy.CreateTransparentWnd(1, loadingbgPath, CPoint(0,0));
	//m_Fairy.CreateTransparentWnd(2, creditPath, CPoint(0,0));
	//m_Fairy.CreateTransparentWnd(3, ipstartPath, CPoint(0,0));
	//m_Fairy.CreateTransparentWnd(4, insertCoinPath, CPoint(0,0));
	//m_Fairy.CreateTransparentWnd(5, loadingPath, CPoint(0,0));
}

CSTreetFightScene::~CSTreetFightScene(void)
{
	//m_Fairy.DestroyWnd(0);
	//m_Fairy.DestroyWnd(1);
	//m_Fairy.DestroyWnd(2);
	//m_Fairy.DestroyWnd(3);
	//m_Fairy.DestroyWnd(4);
	//m_Fairy.DestroyWnd(5);
}

void CSTreetFightScene::TitileScene()
{
	m_Fairy.DestroyAllWnd();
	//m_Fairy->CreateCoinInsert();
}