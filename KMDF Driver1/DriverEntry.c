#include <ntddk.h>
NTSTATUS CreateDevice(PDRIVER_OBJECT driver);
// 卸载事件
void DriverUnload(PDRIVER_OBJECT pDriver) 
{
	

	if (pDriver->DeviceObject) {

		IoDeleteDevice(pDriver->DeviceObject);
		DbgPrint("Unload删除了对象");
	}
	

};
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
	return STATUS_SUCCESS;
};

// 创建设备对象
NTSTATUS CreateDevice (PDRIVER_OBJECT driver)
{
	NTSTATUS status;
	UNICODE_STRING MyDriver;
	PDEVICE_OBJECT device;
	RtlInitUnicodeString(&MyDriver, L"\\DEVICE\\MyDriver111");// MyDriver 初始化
	// IoCreateDevice创建对象
	status = IoCreateDevice(driver, sizeof(driver->DriverExtension), &MyDriver, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device);
	
	if (status == STATUS_SUCCESS) 
	{ 
		DbgPrint(("yjx:设备对象创建成功\n"));
	}
	else 
	{
		DbgPrint(("yjx:设备对象创建失败"));
		IoDeleteDevice(device);

	}
	return status;
}
// IRP处理函数
NTSTATUS DeviceIrpCtr(PDRIVER_OBJECT driver, PIRP pirp) {
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
		break;
	}
	case IRP_MJ_CREATE:
	{
		DbgPrint(("yjx: 用户层IRP_MJ_CREATE"));
		break;
	}case IRP_MJ_CLOSE:
	{
		DbgPrint(("yjx: 用户层IRP_MJ_CLOSE"));
		break;
	}
	default:
		DbgPrint(("IRP Default 错误"));
		break;
	}

	
	return 0;

}
