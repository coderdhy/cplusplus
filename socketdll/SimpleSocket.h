#pragma once
#include <thread>
#include <mutex>
#include <winsock2.h>
#include "..\ObServerModel.h"
using namespace dhymodel;

class CSimpleSocketClient : public CNotifyerModel
{
public:
	CSimpleSocketClient(void);
	~CSimpleSocketClient(void);

private:
	std::thread m_hTreadClient;
};

class CSimpleSocketServer : public CNotifyerModel
{
public:
	CSimpleSocketServer();
	~CSimpleSocketServer();

	int StartServer(int port, char* host, int concurrent);

protected:
	int Run();

private:
	SOCKET m_socket;
	std::mutex m_mutex;
	std::thread m_hTreadServer;
};