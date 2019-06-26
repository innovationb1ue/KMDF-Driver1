#include "DebugPrint.h"
#include <Windows.h>
#include <winsvc.h>
#include <stdio.h>


BOOL  LoadDriver(const char* lpszDriverName, const  char* lpszDriverPath)
{
	char szDriverImagePath[256] = { 0 };//用于保存 .sys的全路径名
	//得到完整的驱动路径
	GetFullPathNameA(lpszDriverPath, 256, szDriverImagePath, NULL);
	OutputDebugStringA("yjx:LoadDriver()");
	printf("yjx:lpszDriverPath=%s 全路径名:szDriverImagePath=%s \n", lpszDriverPath, szDriverImagePath);
	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL;//SCM管理器的句柄
	SC_HANDLE hServiceDDK = NULL;//NT驱动程序的服务句柄

	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); //SC_MANAGER_CREATE_SERVICE

	printf("yjx:打开 SCM管理器 OpenSCManager() hServiceMgr=%p GetLastError=%d", hServiceMgr, GetLastError());

	//创建驱动所对应的服务
	hServiceDDK = CreateServiceA(hServiceMgr,
		lpszDriverName, //驱动程序的在注册表中的名字  
		lpszDriverName, // 注册表驱动程序的 DisplayName 值  
		SERVICE_START, // 加载驱动程序的访问权限  SERVICE_START 或者 SERVICE_ALL_ACCESS
		SERVICE_KERNEL_DRIVER,// 表示加载的服务是驱动程序  
		SERVICE_DEMAND_START, // 注册表驱动程序的 Start 值   //指定当进程调用StartService函数时由服务控制管理器启动的服务。
		SERVICE_ERROR_NORMAL,//SERVICE_ERROR_IGNORE, // 注册表驱动程序的 ErrorControl 值  
		szDriverImagePath, // 注册表驱动程序的 ImagePath 值  
		NULL,  //GroupOrder HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\GroupOrderList
		NULL,
		NULL,
		NULL,
		NULL);

	printf("yjx:创建驱动服务 hServiceDDK=%p  GetLastError=%d\n", hServiceDDK, GetLastError());
	if (GetLastError() == ERROR_SERVICE_EXISTS) //ERROR_SERVICE_EXISTS 1073 //服务已经存在
	{
		hServiceDDK = OpenServiceA(hServiceMgr, lpszDriverName, SERVICE_START);////或者 SERVICE_ALL_ACCESS //

		printf("OpenServiceA hServiceDDK=%08X", hServiceDDK);
		//SERVICE_CONTROL_CONTINUE
		hServiceDDK = OpenServiceA(hServiceMgr, lpszDriverName, SERVICE_PAUSE_CONTINUE | SERVICE_QUERY_STATUS);

	}
	Sleep(1200);//等待前边输出 调试信息 
	bRet = StartService(hServiceDDK, NULL, NULL);
	printf("yjx:加载驱动服务 StartService()=%d  GetLastError=%d", bRet, GetLastError());
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}

	return bRet;
}

BOOL UnloadDriver(const char* szSvrName)
{
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;
	SERVICE_STATUS SvrSta;
	// 打开SCM管理器
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	// 打开驱动所对应的的服务
	hServiceDDK = OpenServiceA(hServiceMgr, szSvrName, SERVICE_ALL_ACCESS);
	
	if (hServiceDDK == NULL) {
		OutputDebugStringA("hServiceDDK = NULL \n");
	}
	else { OutputDebugString(L"OpenServiceA ok! \n"); }
	// 停止驱动程序
	ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta);
	// 卸载驱动程序,从SCM里删除注册信息
	DeleteService(hServiceDDK);
	// 关闭打开的句柄
	if (hServiceDDK)CloseHandle(hServiceDDK);
	if (hServiceMgr)CloseHandle(hServiceMgr);
	bRet = 1;
	return bRet;
}
