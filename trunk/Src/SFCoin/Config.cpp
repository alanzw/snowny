#include "StdAfx.h"
#include "Config.h"

CConfig::CConfig(void)
{
	CoinMode=1;
	UnitCoin=2;

	Difficulty=3;
	Rounds=-1;
	TimeLimit=-2;
}

CConfig::~CConfig(void)
{
}
BOOL CConfig::ReadConfig(LPCWSTR fileName)
{
	/*GetPrivateProfileString(L"ARCADE",L"CoinMode",L"",CoinMode.GetBuffer(),MAX_PATH,fileName);
	TRACE("StreetFighter CoinMode:%s\n",CoinMode);*/
	return TRUE;
}