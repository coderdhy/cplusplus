#include "stdafx.h"
#include "SimpleSocket.h"


CSimpleSocketClient::CSimpleSocketClient(void)
{
}


CSimpleSocketClient::~CSimpleSocketClient(void)
{
}

CSimpleSocketServer::CSimpleSocketServer()
	: m_socket(NULL)
{

}

CSimpleSocketServer::~CSimpleSocketServer()
{

}

int CSimpleSocketServer::StartServer( int port, char* host, int concurrent )
{
	if (NULL != m_socket)
	{
		printf_s("already started!");
		return -1;
	}

}

int CSimpleSocketServer::Run()
{

}
