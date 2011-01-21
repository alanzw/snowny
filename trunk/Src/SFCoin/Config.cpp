#include "StdAfx.h"
#include "Config.h"

CConfig::CConfig(void)
{
	CoinMode = 0;
	UnitCoin = 2;

	//配置数据值是相对游戏默认配置的位置，-1表示默认配置的左一个配置，1表示默认配置的右一个配置
	Difficulty = 3;
	Rounds = -1;
	TimeLimit = -2;
}

CConfig::~CConfig(void)
{
}
BOOL CConfig::ReadConfig(LPCWSTR fileName)
{
	CoinMode=GetPrivateProfileInt(L"CoinMode",L"Sel",0,fileName);
	//m_UnitCoin=索引数+1
	UnitCoin=GetPrivateProfileInt(L"UnitCoin",L"Sel",0,fileName);
	UnitCoin++;
	//Difficuty的默认值是“中等”，索引为3
	Difficulty=GetPrivateProfileInt(L"Difficulty",L"Sel",-1,fileName);
	if(-1==Difficulty)
		return FALSE;
	TRACE(L"StreetFighter Difficulty:%d\n",Difficulty);
	//Difficulty=Difficulty-3;
	Rounds=GetPrivateProfileInt(L"Rounds",L"Sel",0,fileName);
	//Rounds=Rounds-1;
	TimeLimit=GetPrivateProfileInt(L"TimeLimit",L"Sel",0,fileName);
	//TimeLimit=TimeLimit-2;
	IsFree=GetPrivateProfileInt(L"IsFree",L"Sel",0,fileName);
	if (IsFree == 1)	// 免费模式默认单式
	{
		CoinMode = 0;
	}
	TRACE(L"StreetFighter CoinMode:%d\n",CoinMode);
	TRACE(L"StreetFighter m_UnitCoin:%d\n",UnitCoin);
	TRACE(L"StreetFighter Difficulty:%d\n",Difficulty);
	TRACE(L"StreetFighter Rounds:%d\n",Rounds);
	TRACE(L"StreetFighter TimeLimit:%d\n",TimeLimit);
	return TRUE;
} 