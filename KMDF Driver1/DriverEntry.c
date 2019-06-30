#include <ntddk.h>
#include "监控进程.h"
#include "监控进程Ex.h"
#include "进程内存保护.h"


NTSTATUS CreateDevice(PDRIVER_OBJECT driver);
NTSTATUS DeviceIrpCtr(PDEVICE_OBJECT driver, PIRP pirp);
void DriverUnload(PDRIVER_OBJECT pDriver);
#pragma code_seg("PAGE")


// 驱动入口
NTSTATUS DriverEntry(PDRIVER_OBJECT Driver, PUNICODE_STRING szReg)
{
	DbgPrint(("yjx:DbgPrint 我的第一个驱动"));
	KdPrint(("yjx:KdPrint 我的第一个驱动")); 
	//设置卸载程序DriverUnload
	Driver->DriverUnload = DriverUnload;
	// 创建驱动设备对象
	NTSTATUS status = CreateDevice(Driver);

	// 注册IRP函数
	Driver->MajorFunction[IRP_MJ_CREATE] = DeviceIrpCtr;// CreateFile
	Driver->MajorFunction[IRP_MJ_CLOSE] = DeviceIrpCtr; // Unload
	Driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIrpCtr; // DeviceIO Control
	if (status == STATUS_SUCCESS) { DbgPrint(("CreateDevice 成功")); }

	// 处理权限
	// 在命令行中添加 /INTEGRITYCHECK 即可解决Ex权限不够的问题

	// 监控
	//Hook监控进程Ex();
	
	安装进程内存保护();
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
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver1");
		IoDeleteSymbolicLink(&uzSymbolName);
		DbgPrint("Unload删除了对象");
	}
	// 监控
	//unHook监控进程Ex();
	
	卸载进程内存保护();
};

// 创建设备对象
NTSTATUS CreateDevice (PDRIVER_OBJECT driver)
{
	NTSTATUS status;
	UNICODE_STRING MyDriver;
	PDEVICE_OBJECT device;
	RtlInitUnicodeString(&MyDriver, L"\\DEVICE\\MyDriver1");// MyDriver 初始化
	// IoCreateDevice创建对象
	status = IoCreateDevice(driver, sizeof(driver->DriverExtension), &MyDriver, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device);
	
	if (status == STATUS_SUCCESS) 
	{ 
		DbgPrint(("yjx:设备对象创建成功\n"));
		// 创建符号链接
		UNICODE_STRING uzSymbolName;
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver1"); // CreateFile
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

// IRP处理函数
NTSTATUS DeviceIrpCtr(PDEVICE_OBJECT device, PIRP pirp) {
	DbgPrint(("进入派遣函数"));
	PIO_STACK_LOCATION irpStackL;
	ULONG CtlCode;
	ULONG INputBuffLength;
	// 获取R3层传递的参数
	irpStackL = IoGetCurrentIrpStackLocation(pirp);

	switch (irpStackL->MajorFunction)
	{
	case IRP_MJ_DEVICE_CONTROL:
	{
		DbgPrint(("yjx: 用户层DeviceIoControl"));
		// 获取传入参数
		unsigned int* pInBuf = pirp->AssociatedIrp.SystemBuffer;
		DbgPrint("IoControl 传入参数: %x,%x,%x,%x,%x,%x", pInBuf[0], pInBuf[1], pInBuf[2], pInBuf[3], pInBuf[4], pInBuf[5]);
		// 返回参数到应用层 
		pInBuf[0] = 0x111abc;
		pInBuf[1] = 0x222abc;
		pInBuf[2] = 0x333abc;
		pInBuf[3] = 0x444abc;
		pInBuf[4] = 0x555abc;
		pInBuf[5] = 0x666abc;
		pirp->IoStatus.Information = 4*6;
		
		break;
	}
	case IRP_MJ_CREATE:
	{
		DbgPrint(("yjx: 用户层CreateFIle"));
		break;
	}case IRP_MJ_CLOSE:
	{
		DbgPrint(("yjx: 用户层CloseHandle"));
		break;
	}
	default:
		DbgPrint(("IRP Default 错误"));
		break;
	}
	pirp->IoStatus.Status = STATUS_SUCCESS; 
	//pirp->IoStatus.Information = 4;
	IoCompleteRequest(pirp, IO_NO_INCREMENT);
	DbgPrint(("离开派遣函数"));
	return STATUS_SUCCESS;

}
