#include "stdafx.h"
#include "Commom.h"
#include <atltrace.h>

int MD5(const BYTE* data,int len,char strMD5[33]);
void GetDiskPhysicalSN(char pchDiskPhysicalSN[14]);

std::string GenerateKey()
{
	ATLTRACE("GenerateKey");
	//ªÒ»°”≤≈Ã–Ú¡–∫≈
	char cCode[17] = {0};
	char md5[33] = {0};
	GetDiskPhysicalSN(cCode);
	ATLTRACE("%s", cCode);
	MD5((BYTE *)cCode, (int)strlen(cCode), md5);
	ATLTRACE("%s", md5);
	MD5((BYTE *)md5, 32, md5);
	ATLTRACE("%s", md5);
	MD5((BYTE *)md5, 32, md5);
	ATLTRACE("%s", md5);

	return md5;
}

BOOL HasRegistered(const std::string &appName)
{
	char key[512] = {0};
	GetPrivateProfileStringA("key", appName.c_str(), "", key, 512, "fy.ini");
	return GenerateKey() == key;
}

BOOL Register( const std::string &appName )
{
	WritePrivateProfileStringA("key", appName.c_str(), GenerateKey().c_str(), "fy.ini");
	return TRUE;
}