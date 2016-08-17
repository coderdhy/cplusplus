// wintest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\Header\SimpleLog.h"

int _tmain(int argc, _TCHAR* argv[])
{
	LOG(L"wo de ri zhi");
	LOG(L"wo de ri zhi");
	LOG(L"wo de ri zhi");
	LOG(L"wo de ri zhi");
	LOGN(L"wo de ri zhi %d", 10);
	LOGERROR_N(L"这是个错误 %d", 102);
	LOGN(L"要有个汉字:::::%s", L"我就是个汉字");
	system("pause");
	return 0;
}

