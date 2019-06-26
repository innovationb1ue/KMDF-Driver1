#include "DebugPrint.h"
#include <Windows.h>
#include <winsvc.h>
#include <stdio.h>


BOOL  LoadDriver(const char* lpszDriverName, const  char* lpszDriverPath)
{
	char szDriverImagePath[256] = { 0 };//���ڱ��� .sys��ȫ·����
	//�õ�����������·��
	GetFullPathNameA(lpszDriverPath, 256, szDriverImagePath, NULL);
	OutputDebugStringA("yjx:LoadDriver()");
	printf("yjx:lpszDriverPath=%s ȫ·����:szDriverImagePath=%s \n", lpszDriverPath, szDriverImagePath);
	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL;//SCM�������ľ��
	SC_HANDLE hServiceDDK = NULL;//NT��������ķ�����

	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); //SC_MANAGER_CREATE_SERVICE

	printf("yjx:�� SCM������ OpenSCManager() hServiceMgr=%p GetLastError=%d", hServiceMgr, GetLastError());

	//������������Ӧ�ķ���
	hServiceDDK = CreateServiceA(hServiceMgr,
		lpszDriverName, //�����������ע����е�����  
		lpszDriverName, // ע������������ DisplayName ֵ  
		SERVICE_START, // ������������ķ���Ȩ��  SERVICE_START ���� SERVICE_ALL_ACCESS
		SERVICE_KERNEL_DRIVER,// ��ʾ���صķ�������������  
		SERVICE_DEMAND_START, // ע������������ Start ֵ   //ָ�������̵���StartService����ʱ�ɷ�����ƹ����������ķ���
		SERVICE_ERROR_NORMAL,//SERVICE_ERROR_IGNORE, // ע������������ ErrorControl ֵ  
		szDriverImagePath, // ע������������ ImagePath ֵ  
		NULL,  //GroupOrder HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\GroupOrderList
		NULL,
		NULL,
		NULL,
		NULL);

	printf("yjx:������������ hServiceDDK=%p  GetLastError=%d\n", hServiceDDK, GetLastError());
	if (GetLastError() == ERROR_SERVICE_EXISTS) //ERROR_SERVICE_EXISTS 1073 //�����Ѿ�����
	{
		hServiceDDK = OpenServiceA(hServiceMgr, lpszDriverName, SERVICE_START);////���� SERVICE_ALL_ACCESS //

		printf("OpenServiceA hServiceDDK=%08X", hServiceDDK);
		//SERVICE_CONTROL_CONTINUE
		hServiceDDK = OpenServiceA(hServiceMgr, lpszDriverName, SERVICE_PAUSE_CONTINUE | SERVICE_QUERY_STATUS);

	}
	Sleep(1200);//�ȴ�ǰ����� ������Ϣ 
	bRet = StartService(hServiceDDK, NULL, NULL);
	printf("yjx:������������ StartService()=%d  GetLastError=%d", bRet, GetLastError());
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
	// ��SCM������
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	// ����������Ӧ�ĵķ���
	hServiceDDK = OpenServiceA(hServiceMgr, szSvrName, SERVICE_ALL_ACCESS);
	
	if (hServiceDDK == NULL) {
		OutputDebugStringA("hServiceDDK = NULL \n");
	}
	else { OutputDebugString(L"OpenServiceA ok! \n"); }
	// ֹͣ��������
	ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta);
	// ж����������,��SCM��ɾ��ע����Ϣ
	DeleteService(hServiceDDK);
	// �رմ򿪵ľ��
	if (hServiceDDK)CloseHandle(hServiceDDK);
	if (hServiceMgr)CloseHandle(hServiceMgr);
	bRet = 1;
	return bRet;
}
