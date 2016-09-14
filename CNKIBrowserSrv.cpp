// CNKIBrowserSrv.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h" 
#include "Windows.h" 
#include "..\..\Common\CxLog.h"
#define SZAPPNAME            _T("CNKIBrowserSrv")         //服务程序名 
#define SZSERVICENAME    _T("CNKIBrowserSrvInstance")        //标识服务的内部名 

SERVICE_STATUS_HANDLE StatusHandler = NULL;
HANDLE endEvent = NULL;

//下面的函数由程序实现 
void WINAPI Service_Main(DWORD dwArgc, LPTSTR *lpszArgv); 
void WINAPI Service_Ctrl(DWORD dwCtrlCode); 

BOOL ReportStatus(DWORD dwCurrentState, DWORD dwError, DWORD dwSpecificError, DWORD dwCheckPoint, DWORD dwWaithint);

int _tmain(int argc, _TCHAR* argv[])
{
	SERVICE_TABLE_ENTRY dispatchTable[]= 
	{ 
		{SZSERVICENAME, (LPSERVICE_MAIN_FUNCTION)Service_Main}, 
		{ NULL,NULL} 
	}; 

	if(!StartServiceCtrlDispatcher(dispatchTable)) 
		LOGN(L"StartServiceCtrlDispatcher failed.");
	else 
		LOGN(L"StartServiceCtrlDispatcher OK."); 
	return 0; 
}

void WINAPI Service_Main( DWORD dwArgc, LPTSTR *lpszArgv )
{
	BOOL success = TRUE;
	StatusHandler = RegisterServiceCtrlHandler(SZSERVICENAME, (LPHANDLER_FUNCTION)Service_Ctrl);
	if (!StatusHandler)
	{
		return;
	}
	success = ReportStatus(SERVICE_START_PENDING,NO_ERROR,0,1,5000);
	if (!success)
	{
		return; 
	}
	endEvent = CreateEvent(0,TRUE,FALSE,0);

	if (!endEvent)
	{
		return;
	}
	success = ReportStatus (SERVICE_START_PENDING,NO_ERROR,0,2,5000);
	if (!success)
	{
		return;
	}
	////init parameter start
	//RecvParam(argc,argv);
	////init parameter end
 	success = ReportStatus (SERVICE_START_PENDING,NO_ERROR,0,3,5000);
 	if (!success)
 	{
 		return;
 	}
	success = InitService();
	if (!success)
	{
		return;
	}
	success = ReportStatus (SERVICE_RUNNING,NO_ERROR,0,0,0);
	if (!success)
	{
		return;
	}
	WaitForSingleObject(endEvent,INFINITE);
}

void WINAPI Service_Ctrl(DWORD controlCode)
{
 	DWORD currentState = 0;
 	BOOL success;
 	switch(controlCode)
 	{
 	case SERVICE_CONTROL_STOP:
 		success=ReportStatus(SERVICE_STOP_PENDING,NO_ERROR,0,1,5000);
 		CloseTask();
 		success=ReportStatus(SERVICE_STOPPED,NO_ERROR,0,0,0);
 		return;
 	case SERVICE_CONTROL_PAUSE:
 		if (runningService&&!pauseService)
 		{
 			success=ReportStatus(SERVICE_PAUSE_PENDING,NO_ERROR,0,1,1000);
 			pauseService=TRUE;
 			ServicePause();
 			currentState=SERVICE_PAUSED;
 		}
 		break;
 	case SERVICE_CONTROL_CONTINUE:
 		if (runningService&&pauseService)
 		{
 			success = ReportStatus(SERVICE_CONTINUE_PENDING,
 				NO_ERROR,0,1,1000);
 			pauseService = FALSE;
 			ServiceContinue();
 			currentState=SERVICE_RUNNING;
 		}
 		break;
 	case SERVICE_CONTROL_INTERROGATE://检索更新状态的时
 		break;
 	case SERVICE_CONTROL_SHUTDOWN://告诉服务即将关机
 		success=ReportStatus(SERVICE_STOP_PENDING,NO_ERROR,0,1,5000);
 		CloseTask();
 		return;
 	default:
 		break;
 	}
 	ReportStatus(currentState,NO_ERROR,0,0,0);
}

BOOL ReportStatus(DWORD dwCurrentState, DWORD dwError, DWORD dwSpecificError, DWORD dwCheckPoint, DWORD dwWaithint)
{
	SERVICE_STATUS servicestatus;
	servicestatus.dwCurrentState = dwCurrentState;
	servicestatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	servicestatus.dwWin32ExitCode = dwError;
	servicestatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN|SERVICE_ACCEPT_STOP;
	servicestatus.dwServiceSpecificExitCode = dwSpecificError;
	servicestatus.dwCheckPoint = dwCheckPoint;
	servicestatus.dwWaitHint = dwWaithint;
	return SetServiceStatus(StatusHandler, &servicestatus);
}
