// wintest.cpp : �������̨Ӧ�ó������ڵ㡣
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
	LOGERROR_N(L"���Ǹ����� %d", 102);
	LOGN(L"Ҫ�и�����:::::%s", L"�Ҿ��Ǹ�����");
	system("pause");
	return 0;
}

