// socketdll.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "socketdll.h"


// ���ǵ���������һ��ʾ��
SOCKETDLL_API int nsocketdll=0;

// ���ǵ���������һ��ʾ����
SOCKETDLL_API int fnsocketdll(void)
{
	return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� socketdll.h
Csocketdll::Csocketdll()
{
	return;
}
