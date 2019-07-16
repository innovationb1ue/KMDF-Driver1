#include <ntifs.h>
#include "��ؽ���.h"
#include "��ؽ���Ex.h"
#include "�����ڴ汣��.h"
#include "MyKernelReadProcessMemory.h"
#include "DeviceIoControl.h"
#include "HideDriver.h"
//#include "VirtualAllocEx.h"


int ReadProcessMemoryForPid(UINT32 dwPid, PVOID pBase, PVOID lpBuffer, UINT32 nSize);
PDRIVER_OBJECT pDriverObject = NULL;
NTSTATUS CreateDevice(PDRIVER_OBJECT driver);
void DriverUnload(PDRIVER_OBJECT pDriver);
ULONG64 GetSystemModuleBase(char* lpModuleName);
VOID HideDriver(char* pDrvName);
#pragma code_seg("PAGE")

// �������
NTSTATUS DriverEntry(PDRIVER_OBJECT Driver, PUNICODE_STRING szReg)
{
	DbgPrint(("yjx:DbgPrint �ҵ�����"));
	ULONGLONG NtOpenProcess_Cur_Addr;

	//����ж�س���DriverUnload
	Driver->DriverUnload = DriverUnload;
	// ���������豸����
	NTSTATUS status = CreateDevice(Driver);
	// ע��IRP����
	Driver->MajorFunction[IRP_MJ_CREATE] = DeviceIrpCtr;// CreateFile
	Driver->MajorFunction[IRP_MJ_CLOSE] = DeviceIrpCtr; // Unload
	// ע�������ע��IRP������ǰ���IRP����ͬһ������
	Driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIoControl_Irp; // DeviceIO Control
	if (status == STATUS_SUCCESS) { DbgPrint(("CreateDevice �ɹ�")); }

	//��������������
	/*pDriverObject = Driver;
	HideDriver("BlackBoneDrv10.sys");*/

	
	// ����Ȩ��
	// ������������� /INTEGRITYCHECK ���ɽ��ExȨ�޲���������


	// ���(��ֹ�ض�����ִ��)
	//Hook��ؽ���Ex();
	
	//��װ�����ڴ汣��();
	return STATUS_SUCCESS;
};
// ж���¼�
void DriverUnload(PDRIVER_OBJECT pDriver)
{
	if (pDriver->DeviceObject) {
		// ɾ���豸����
		IoDeleteDevice(pDriver->DeviceObject);
		// ɾ����������
		UNICODE_STRING uzSymbolName;
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver");
		IoDeleteSymbolicLink(&uzSymbolName);
		DbgPrint("Unloadɾ���˶���"); 
	}
	
	// ���
	//unHook��ؽ���Ex();
	
	//ж�ؽ����ڴ汣��();
};

// �����豸����
NTSTATUS CreateDevice (PDRIVER_OBJECT driver)
{
	NTSTATUS status;
	UNICODE_STRING MyDriver;
	PDEVICE_OBJECT device;
	RtlInitUnicodeString(&MyDriver, L"\\DEVICE\\MyDriver");// MyDriver ��ʼ��
	// IoCreateDevice��������
	status = IoCreateDevice(driver, sizeof(driver->DriverExtension), &MyDriver, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device);
	
	if (status == STATUS_SUCCESS) 
	{ 
		DbgPrint(("yjx:�豸���󴴽��ɹ�\n"));
		// ������������
		UNICODE_STRING uzSymbolName;
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver"); // CreateFile
		status = IoCreateSymbolicLink(&uzSymbolName, &MyDriver); // DeviceName to SymbolicLinkName
		if (status == STATUS_SUCCESS)
		{
			DbgPrint(("yjx:�����������ӳɹ�"));
		}
		else
		{
			DbgPrint(("yjx:������������ʧ��"));
		}
	}
	else 
	{
		DbgPrint(("yjx:�豸���󴴽�ʧ��"));
		IoDeleteDevice(device);

	}
	return status;
}




