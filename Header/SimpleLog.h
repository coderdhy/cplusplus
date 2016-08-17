#ifndef __SIMPLE_LOG_H__
#define __SIMPLE_LOG_H__

#include <atlbase.h>
#include <wtypes.h>
#include <time.h>
#include <queue>
#include <process.h>
#include "wtl\atlapp.h"
#include "wtl\atlmisc.h"
#include <tchar.h>

#define MAX_FUNCNAME 100
#define Log_Perid_DAY
#define LOG_MAX_FORMAT_SIZE 1024*5 //每条日志的最大字符数
#define MSG_FORMAT_FULL _T("[%02d:%02d:%02d:%03d %s]:%s:%d:%s [msg]:%s\r\n") //[hour-minute-second-millisecond level]:filename:line:function name message 
#define MSG_FORMAT_NOFUNC _T("[%02d:%02d:%02d:%03d %s]:%s:%d [msg]:%s\r\n") //without function name


enum Log_Level
{
	LOG_MSG,
	LOG_WARNING,
	LOG_EXCEPTION,
	LOG_ERROR
};

#define MINLEVEL Log_Level::LOG_MSG

struct LogItem 
{
	int m_line;
	Log_Level m_type;
	SYSTEMTIME m_tm;
	TCHAR m_func[MAX_FUNCNAME];
	TCHAR m_filepath[MAX_PATH];
	TCHAR* m_pMsg;
};


UINT WINAPI StartProc(void* pvoid);

class CSimpleLog
{
	CSimpleLog() 
	{
		InitLog();
	};

	void InitLog()
	{
		::InitializeCriticalSection(&m_cs);
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, StartProc, this, NULL, &m_nThreadId);
		m_hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	void ExitLog()
	{
		::SetEvent(m_hEvent);
		::WaitForSingleObject(m_hThread, 2000);
		::CloseHandle(m_hThread);
		::CloseHandle(m_hEvent);
		::DeleteCriticalSection(&m_cs);
	}
	
	void Lock()
	{
		::EnterCriticalSection(&m_cs);
	}

	void UnLock() 
	{
		::LeaveCriticalSection(&m_cs);
	}

	WTL::CString GetLogFileName()
	{
		TCHAR szFileFullPath[MAX_PATH];
		::GetModuleFileName(NULL, szFileFullPath, MAX_PATH);
		WTL::CString szProcessName(szFileFullPath), sFileName;
		int nPos = szProcessName.ReverseFind('\\');
		szProcessName = szProcessName.Mid(nPos+1);
#ifdef Log_Perid_DAY
		SYSTEMTIME tm;
		GetLocalTime(&tm);
		sFileName.Format(_T("%s_%02d-%02d-%02d.log"), szProcessName, tm.wYear, tm.wMonth, tm.wDay);
#else
		sFileName.Format(_T("%s.log"), szProcessName);
#endif // Log_Perid_DAY
		return sFileName;
	}

	LPTSTR Format(LogItem* p, LPTSTR szBuffer, int nbufsize, int& nSize, bool bFuncName = true)
	{
		if (p == NULL)
		{
			nSize = 0;
			return szBuffer;
		}

		static LPCTSTR szType[] = {_T("Message"), _T("Warning"), _T("Exception"), _T("Error")};

		if(!szBuffer)
		{
			nSize = 0;
			return szBuffer;
		}

		int iLen;
		if (bFuncName)
		{
			iLen = _sntprintf_s(szBuffer, nbufsize, _TRUNCATE, MSG_FORMAT_FULL,
				p->m_tm.wHour, p->m_tm.wMinute, p->m_tm.wSecond, p->m_tm.wMilliseconds,
				szType[p->m_type],
				p->m_filepath,
				p->m_line,
				p->m_func,
				p->m_pMsg);
		}
		else
		{
			iLen = _sntprintf_s(szBuffer, nbufsize, _TRUNCATE, MSG_FORMAT_NOFUNC,
				p->m_tm.wHour, p->m_tm.wMinute, p->m_tm.wSecond, p->m_tm.wMilliseconds,
				szType[p->m_type],
				p->m_filepath,
				p->m_line,
				p->m_pMsg);
		}

		if ( iLen >= 0 && iLen < nbufsize )
		{
			nSize = iLen;
			return szBuffer;
		}
		else if (iLen == -1) //截断
		{
			nSize = nbufsize;
			szBuffer[nbufsize-1] = '\n';
			return szBuffer;
		}
		else
		{
			nSize = 0;
			return szBuffer;
		}
	}

public:
	static CSimpleLog& Instance()
	{
		static CSimpleLog _log;
		return _log;
	}
	
	~CSimpleLog() 
	{
		ExitLog();
	}

	void WriteLog(Log_Level level, int line, LPTSTR funcName, LPTSTR filePath, LPTSTR sMsg)
	{
		LogItem* p = new LogItem();
		p->m_type = level;
		GetLocalTime(&(p->m_tm));
		p->m_line = line;
		int funl(0), filel(0), sMsgl(0);
		funl = lstrlen(funcName) + 1;
		lstrcpyn(p->m_func, funcName, funl > MAX_FUNCNAME ? MAX_FUNCNAME : funl);
		filel = lstrlen(filePath) + 1;
		lstrcpyn(p->m_filepath, filePath, filel > MAX_PATH ? MAX_PATH : filel );
		p->m_pMsg = new TCHAR[LOG_MAX_FORMAT_SIZE];
		memset(p->m_pMsg, NULL, LOG_MAX_FORMAT_SIZE);
		sMsgl = lstrlen(sMsg) + 1;
		lstrcpyn(p->m_pMsg, sMsg, sMsgl > LOG_MAX_FORMAT_SIZE ? LOG_MAX_FORMAT_SIZE : sMsgl);
		Lock();
		m_qMsg.push(p);
		UnLock();
	}

	void WriteLogN(Log_Level level, int line, LPTSTR funcName, LPTSTR filePath, LPTSTR sFormat, ...)
	{
		if (level < MINLEVEL)
		{
			return;
		}

		TCHAR szBuffer[LOG_MAX_FORMAT_SIZE] = {0};
		va_list va;
		va_start(va, sFormat);
		_vsntprintf_s(szBuffer, LOG_MAX_FORMAT_SIZE, LOG_MAX_FORMAT_SIZE-1, sFormat, va);
		va_end(va);
		WriteLog(level, line, funcName, filePath, szBuffer);
	}

	void Run()
	{
		HANDLE hHandle = ::CreateFile(
			GetLogFileName(),
			GENERIC_READ | GENERIC_WRITE ,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, 
			NULL
			);
		if (hHandle == NULL)
		{
			return;
		}
		SetFilePointer(hHandle, 0, 0, FILE_END);

		for (;;)
		{
			DWORD ret = ::WaitForSingleObject(m_hEvent, IGNORE);
			if (ret == WAIT_OBJECT_0)
			{
				break;
			}

			for (;;)
			{
				LogItem* p(NULL);
				Lock();
				if (!m_qMsg.empty())
				{
					p = m_qMsg.front();
					m_qMsg.pop();
				}
				UnLock();
				if(p == NULL)
				{
					break;
				}

				int uSize;
				DWORD dwNumberOfBytesWritten;
				TCHAR szBuffer[LOG_MAX_FORMAT_SIZE];
				memset(szBuffer, NULL, LOG_MAX_FORMAT_SIZE);
				Format(p, szBuffer, LOG_MAX_FORMAT_SIZE, uSize, true);

				try
				{
#ifdef _UNICODE
					DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, szBuffer, -1, NULL, 0, NULL, FALSE);
					char *pbuff = new char[dwNum];
					WideCharToMultiByte (CP_ACP, NULL, szBuffer, -1, pbuff, dwNum, NULL, FALSE);
					WriteFile(hHandle, pbuff, dwNum-1, &dwNumberOfBytesWritten, NULL);
#else
					WriteFile(hHandle, szBuffer, uSize*sizeof(TCHAR), &dwNumberOfBytesWritten, NULL);
#endif
					OutputDebugString(szBuffer);
				}
				catch (...)
				{
					OutputDebugString(_T("log failed\n"));
				}
			}

			Sleep(200);
		}
		
	}

private:
	CRITICAL_SECTION m_cs;
	HANDLE m_hThread;
	HANDLE m_hEvent;
	UINT m_nThreadId;
	std::queue<LogItem*> m_qMsg; 
};

UINT WINAPI StartProc(void* pvoid)
{
	if (pvoid == NULL)
	{
		return -1;
	}
	CSimpleLog* pLog = (CSimpleLog* )pvoid;
	pLog->Run();
	return 0;
}

#define WIDEN2(str) L ## str
#define WIDEN(str)  WIDEN2(str)

#ifdef _UNICODE
#define __TFILE__ WIDEN(__FILE__)
#else
#define __TFILE__ __FILE__
#endif

#ifdef _UNICODE
#define __TFUNCTION__ WIDEN(__FUNCTION__)
#else
#define __TFUNCTION__ __FUNCTION__
#endif

#define LOGBASE(level, sMsg)						CSimpleLog::Instance().WriteLog(level, __LINE__, __TFUNCTION__, __TFILE__, sMsg)
#define LOGMSG(sMsg)								LOGBASE(Log_Level::LOG_MSG, sMsg)
#define LOGWARNING(sMsg)						LOGBASE(Log_Level::LOG_WARNING, sMsg)
#define LOGEXCEPTION(sMsg)						LOGBASE(Log_Level::LOG_EXCEPTION, sMsg)
#define LOGERROR(sMsg)							LOGBASE(Log_Level::LOG_ERROR, sMsg)
#define LOG(sMsg)										LOGMSG(sMsg)

#define LOGBASE_N(level, sFormat, ...)			CSimpleLog::Instance().WriteLogN(level, __LINE__, __TFUNCTION__, __TFILE__, sFormat, ##__VA_ARGS__)
#define LOGMSG_N(sFormat, ...)					LOGBASE_N(Log_Level::LOG_MSG, (sFormat), ##__VA_ARGS__)
#define LOGWARNING_N(sFormat, ...)			LOGBASE_N(Log_Level::LOG_WARNING, (sFormat), ##__VA_ARGS__)
#define LOGEXCEPTION_N(sFormat, ...)		LOGBASE_N(Log_Level::LOG_EXCEPTION, (sFormat), ##__VA_ARGS__)
#define LOGERROR_N(sFormat, ...)				LOGBASE_N(Log_Level::LOG_ERROR, (sFormat), ##__VA_ARGS__)
#define LOGN												LOGMSG_N
#endif