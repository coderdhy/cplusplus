#include "stdafx.h"
#include "InitSocket.h"
#include <WinSock2.h>

#pragma comment(lib, "WS2_32")

CInitSocket::CInitSocket(int minorVer/*=2*/, int majorVer/*=2*/)
{
	//init WS32_32.dll
	WSADATA wsadata;
	WORD socketVer = MAKEWORD(minorVer, majorVer);
	if (::WSAStartup(socketVer, &wsadata)!=0)
	{
		OutputDebugString(L"::WSAStartup failed");
	}
}


CInitSocket::~CInitSocket(void)
{
	::WSACleanup();
}
