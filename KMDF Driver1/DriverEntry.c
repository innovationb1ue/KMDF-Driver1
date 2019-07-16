#include <ntifs.h>
#include "监控进程.h"
#include "监控进程Ex.h"
#include "进程内存保护.h"
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

// 驱动入口
NTSTATUS DriverEntry(PDRIVER_OBJECT Driver, PUNICODE_STRING szReg)
{
	DbgPrint(("yjx:DbgPrint 我的驱动"));
	ULONGLONG NtOpenProcess_Cur_Addr;

	//设置卸载程序DriverUnload
	Driver->DriverUnload = DriverUnload;
	// 创建驱动设备对象
	NTSTATUS status = CreateDevice(Driver);
	// 注册IRP函数
	Driver->MajorFunction[IRP_MJ_CREATE] = DeviceIrpCtr;// CreateFile
	Driver->MajorFunction[IRP_MJ_CLOSE] = DeviceIrpCtr; // Unload
	// 注意这里的注册IRP函数和前面的IRP不是同一个函数
	Driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIoControl_Irp; // DeviceIO Control
	if (status == STATUS_SUCCESS) { DbgPrint(("CreateDevice 成功")); }

	//隐藏驱动：断链
	/*pDriverObject = Driver;
	HideDriver("BlackBoneDrv10.sys");*/

	
	// 处理权限
	// 在命令行中添加 /INTEGRITYCHECK 即可解决Ex权限不够的问题


	// 监控(阻止特定进程执行)
	//Hook监控进程Ex();
	
	//安装进程内存保护();
	return STATUS_SUCCESS;
};
// 卸载事件
void DriverUnload(PDRIVER_OBJECT pDriver)
{
	if (pDriver->DeviceObject) {
		// 删除设备对象
		IoDeleteDevice(pDriver->DeviceObject);
		// 删除符号链接
		UNICODE_STRING uzSymbolName;
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver");
		IoDeleteSymbolicLink(&uzSymbolName);
		DbgPrint("Unload删除了对象"); 
	}
	
	// 监控
	//unHook监控进程Ex();
	
	//卸载进程内存保护();
};

// 创建设备对象
NTSTATUS CreateDevice (PDRIVER_OBJECT driver)
{
	NTSTATUS status;
	UNICODE_STRING MyDriver;
	PDEVICE_OBJECT device;
	RtlInitUnicodeString(&MyDriver, L"\\DEVICE\\MyDriver");// MyDriver 初始化
	// IoCreateDevice创建对象
	status = IoCreateDevice(driver, sizeof(driver->DriverExtension), &MyDriver, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device);
	
	if (status == STATUS_SUCCESS) 
	{ 
		DbgPrint(("yjx:设备对象创建成功\n"));
		// 创建符号链接
		UNICODE_STRING uzSymbolName;
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver"); // CreateFile
		status = IoCreateSymbolicLink(&uzSymbolName, &MyDriver); // DeviceName to SymbolicLinkName
		if (status == STATUS_SUCCESS)
		{
			DbgPrint(("yjx:创建符号链接成功"));
		}
		else
		{
			DbgPrint(("yjx:创建符号连接失败"));
		}
	}
	else 
	{
		DbgPrint(("yjx:设备对象创建失败"));
		IoDeleteDevice(device);

	}
	return status;
}




