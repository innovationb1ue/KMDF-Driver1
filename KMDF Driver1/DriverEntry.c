#include <ntddk.h>
#include "��ؽ���.h"
#include "��ؽ���Ex.h"
#include "�����ڴ汣��.h"


NTSTATUS CreateDevice(PDRIVER_OBJECT driver);
NTSTATUS DeviceIrpCtr(PDEVICE_OBJECT driver, PIRP pirp);
void DriverUnload(PDRIVER_OBJECT pDriver);
#pragma code_seg("PAGE")


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

	// ����Ȩ��
	// ������������� /INTEGRITYCHECK ���ɽ��ExȨ�޲���������

	// ���
	//Hook��ؽ���Ex();
	
	��װ�����ڴ汣��();
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
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver1");
		IoDeleteSymbolicLink(&uzSymbolName);
		DbgPrint("Unloadɾ���˶���");
	}
	// ���
	//unHook��ؽ���Ex();
	
	ж�ؽ����ڴ汣��();
};

// �����豸����
NTSTATUS CreateDevice (PDRIVER_OBJECT driver)
{
	NTSTATUS status;
	UNICODE_STRING MyDriver;
	PDEVICE_OBJECT device;
	RtlInitUnicodeString(&MyDriver, L"\\DEVICE\\MyDriver1");// MyDriver ��ʼ��
	// IoCreateDevice��������
	status = IoCreateDevice(driver, sizeof(driver->DriverExtension), &MyDriver, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device);
	
	if (status == STATUS_SUCCESS) 
	{ 
		DbgPrint(("yjx:�豸���󴴽��ɹ�\n"));
		// ������������
		UNICODE_STRING uzSymbolName;
		RtlInitUnicodeString(&uzSymbolName, L"\\??\\MyDriver1"); // CreateFile
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

// IRP������
NTSTATUS DeviceIrpCtr(PDEVICE_OBJECT device, PIRP pirp) {
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
		// ��ȡ�������
		unsigned int* pInBuf = pirp->AssociatedIrp.SystemBuffer;
		DbgPrint("IoControl �������: %x,%x,%x,%x,%x,%x", pInBuf[0], pInBuf[1], pInBuf[2], pInBuf[3], pInBuf[4], pInBuf[5]);
		// ���ز�����Ӧ�ò� 
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
		DbgPrint(("yjx: �û���CreateFIle"));
		break;
	}case IRP_MJ_CLOSE:
	{
		DbgPrint(("yjx: �û���CloseHandle"));
		break;
	}
	default:
		DbgPrint(("IRP Default ����"));
		break;
	}
	pirp->IoStatus.Status = STATUS_SUCCESS; 
	//pirp->IoStatus.Information = 4;
	IoCompleteRequest(pirp, IO_NO_INCREMENT);
	DbgPrint(("�뿪��ǲ����"));
	return STATUS_SUCCESS;

}
