#ifndef __OBSERVER_MODEL_H__
#define __OBSERVER_MODEL_H__

#include <memory>
#include <vector>

#define THREAD_SAFE_MODEL
namespace dhymodel
{
	class CObServer
	{
	public:
		virtual ~CObServer() {};
		//need quick response, otherwise this is a terrible way
		//nMsg is identity of pData
		void OnNotify(int nMsg, void *pData){};

	protected:
		CObServer() {};
		CObServer(const CObServer& ob) {};
	};
	typedef std::shared_ptr<CObServer> CObServerPtr;
	typedef std::weak_ptr<CObServer> CObServerPtr_W;
	typedef std::vector<CObServerPtr_W> ObServers_W;

	class CNotifyerModel
	{
	public:
		CNotifyerModel() {};
		~CNotifyerModel() 
		{
			m_vObServer.clear();
		};

		void Attach(CObServerPtr& spObServer)
		{
#ifdef THREAD_SAFE_MODEL
			std::lock_guard<std::mutex> guard(m_mutex);
#endif
			CObServerPtr_W spObServer_w(spObServer);
			m_vObServer.push_back(spObServer_w);
		}

		void Notify(int nMsg, void* pData)
		{
#ifdef THREAD_SAFE_MODEL
			std::lock_guard<std::mutex> guard(m_mutex);
#endif // THREAD_SAFE_MODEL
			for (ObServers_W::iterator iter = m_vObServer.begin(); iter != m_vObServer.end(); )
			{
				CObServerPtr sp = (*iter).lock();
				if (!sp)
				{
					m_vObServer.erase(iter);
					continue;
				}
				sp->OnNotify(nMsg, pData);
				iter++;
			}
		}

	private:
		ObServers_W m_vObServer;
#ifdef THREAD_SAFE_MODEL
		std::mutex m_mutex;
#endif
	};
}

#endif