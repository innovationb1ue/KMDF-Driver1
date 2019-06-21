#include <ntddk.h>
NTSTATUS CreateDevice(PDRIVER_OBJECT driver);
// ж���¼�
void DriverUnload(PDRIVER_OBJECT pDriver) 
{
	

	if (pDriver->DeviceObject) {

		IoDeleteDevice(pDriver->DeviceObject);
		DbgPrint("Unloadɾ���˶���");
	}
	

};
// �������
NTSTATUS DriverEntry(PDRIVER_OBJECT Driver, PUNICODE_STRING szReg)
{
	DbgPrint(("yjx:DbgPrint �ҵĵ�һ������"));
	KdPrint(("yjx:KdPrint �ҵĵ�һ������"));
	//����ж�س���DriverUnload
	Driver->DriverUnload = DriverUnload;
	// ���������豸����
	NTSTATUS status = CreateDevice(Driver);

	// ע��IRP����
	Driver->MajorFunction[IRP_MJ_CREATE] = DeviceIrpCtr;// CreateFile
	Driver->MajorFunction[IRP_MJ_CLOSE] = DeviceIrpCtr; // Unload
	Driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIrpCtr; // DeviceIO Control

	if (status == STATUS_SUCCESS) { DbgPrint(("CreateDevice �ɹ�")); }
	return STATUS_SUCCESS;
};

// �����豸����
NTSTATUS CreateDevice (PDRIVER_OBJECT driver)
{
	NTSTATUS status;
	UNICODE_STRING MyDriver;
	PDEVICE_OBJECT device;
	RtlInitUnicodeString(&MyDriver, L"\\DEVICE\\MyDriver111");// MyDriver ��ʼ��
	// IoCreateDevice��������
	status = IoCreateDevice(driver, sizeof(driver->DriverExtension), &MyDriver, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device);
	
	if (status == STATUS_SUCCESS) 
	{ 
		DbgPrint(("yjx:�豸���󴴽��ɹ�\n"));
	}
	else 
	{
		DbgPrint(("yjx:�豸���󴴽�ʧ��"));
		IoDeleteDevice(device);

	}
	return status;
}
// IRP������
NTSTATUS DeviceIrpCtr(PDRIVER_OBJECT driver, PIRP pirp) {
	DbgPrint(("������ǲ����"));
	PIO_STACK_LOCATION irpStackL;
	ULONG CtlCode;
	ULONG INputBuffLength;
	// ��ȡR3�㴫�ݵĲ���
	irpStackL = IoGetCurrentIrpStackLocation(pirp);

	switch (irpStackL->MajorFunction)
	{
	case IRP_MJ_DEVICE_CONTROL:
	{
		DbgPrint(("yjx: �û���DeviceIoControl"));
		break;
	}
	case IRP_MJ_CREATE:
	{
		DbgPrint(("yjx: �û���IRP_MJ_CREATE"));
		break;
	}case IRP_MJ_CLOSE:
	{
		DbgPrint(("yjx: �û���IRP_MJ_CLOSE"));
		break;
	}
	default:
		DbgPrint(("IRP Default ����"));
		break;
	}

	
	return 0;

}
