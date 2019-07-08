#include <ntddk.h>
#include "��ؽ���.h"
#include "��ؽ���Ex.h"
#include "�����ڴ汣��.h"
#include "SSDT.cpp"
PDRIVER_OBJECT pDriverObject = NULL;
#define kprintf		DbgPrint
#define kmalloc(_s)	ExAllocatePoolWithTag(NonPagedPool, _s, 'SYSQ')
#define kfree(_p)	ExFreePool(_p)

NTSTATUS CreateDevice(PDRIVER_OBJECT driver);
NTSTATUS DeviceIrpCtr(PDEVICE_OBJECT driver, PIRP pirp);
void DriverUnload(PDRIVER_OBJECT pDriver);
ULONG64 GetSystemModuleBase(char* lpModuleName);
VOID HideDriver(char* pDrvName);
#pragma code_seg("PAGE")

// �������
NTSTATUS DriverEntry(PDRIVER_OBJECT Driver, PUNICODE_STRING szReg)
{
	DbgPrint(("yjx:DbgPrint �ҵĵ�һ������"));
	KdPrint(("yjx:KdPrint �ҵĵ�һ������")); 
	ULONGLONG NtOpenProcess_Cur_Addr;
	// ��ȡSSDT��
	//NtOpenProcess_Cur_Addr = GetSSDTFuncCurAddr((ULONG)38);
	//DbgPrint("OpenProcess = %llu", NtOpenProcess_Cur_Addr);
	//����ж�س���DriverUnload
	Driver->DriverUnload = DriverUnload;
	// ���������豸����
	NTSTATUS status = CreateDevice(Driver);
	
	// ע��IRP����
	Driver->MajorFunction[IRP_MJ_CREATE] = DeviceIrpCtr;// CreateFile
	Driver->MajorFunction[IRP_MJ_CLOSE] = DeviceIrpCtr; // Unload
	Driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIrpCtr; // DeviceIO Control
	if (status == STATUS_SUCCESS) { DbgPrint(("CreateDevice �ɹ�")); }

	// ��������������
	pDriverObject = Driver;
	HideDriver("BlackBoneDrv10.sys");

	
	// ����Ȩ��
	// ������������� /INTEGRITYCHECK ���ɽ��ExȨ�޲���������


	// ���(��ֹ�ض�����ִ��)
	//Hook��ؽ���Ex();
	
	//��װ�����ڴ汣��();
	return STATUS_SUCCESS;
};
MmUserProbeAddress;
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
	
	//ж�ؽ����ڴ汣��();
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

}//Ȼ����R0�����ں�ģ�飬ժ�����⡣Ҳ����Ҫע��ṹ����ϸ�ھ����ˡ�
#include <ntddk.h>

NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation
(
	IN ULONG	SystemInformationClass,
	OUT PVOID	SystemInformation,
	IN ULONG	Length,
	OUT PULONG	ReturnLength
);

typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY
{
	ULONG Unknow1;
	ULONG Unknow2;
	ULONG Unknow3;
	ULONG Unknow4;
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT Index;
	USHORT NameLength;
	USHORT LoadCount;
	USHORT ModuleNameOffset;
	char ImageName[256];
} SYSTEM_MODULE_INFORMATION_ENTRY, * PSYSTEM_MODULE_INFORMATION_ENTRY;

typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG Count;//�ں����Լ��ص�ģ��ĸ���
	SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

typedef struct _KLDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY64 InLoadOrderLinks;
	ULONG64 __Undefined1;
	ULONG64 __Undefined2;
	ULONG64 __Undefined3;
	ULONG64 NonPagedDebugInfo;
	ULONG64 DllBase;
	ULONG64 EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG   Flags;
	USHORT  LoadCount;
	USHORT  __Undefined5;
	ULONG64 __Undefined6;
	ULONG   CheckSum;
	ULONG   __padding1;
	ULONG   TimeDateStamp;
	ULONG   __padding2;
}KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;


ULONG64 GetSystemModuleBase(char* lpModuleName)
{
	ULONG NeedSize, i, ModuleCount, BufferSize = 0x5000;
	PVOID pBuffer = NULL;
	PCHAR pDrvName = NULL;
	NTSTATUS Result;
	PSYSTEM_MODULE_INFORMATION pSystemModuleInformation;
	do
	{
		//�����ڴ�
		pBuffer = kmalloc(BufferSize);
		if (pBuffer == NULL)
			return 0;
		//��ѯģ����Ϣ
		Result = ZwQuerySystemInformation(11, pBuffer, BufferSize, &NeedSize);
		if (Result == STATUS_INFO_LENGTH_MISMATCH)
		{
			kfree(pBuffer);
			BufferSize *= 2;
		}
		else if (!NT_SUCCESS(Result))
		{
			//��ѯʧ�����˳�
			kfree(pBuffer);
			return 0;
		}
	} while (Result == STATUS_INFO_LENGTH_MISMATCH);
	pSystemModuleInformation = (PSYSTEM_MODULE_INFORMATION)pBuffer;
	//���ģ���������
	ModuleCount = pSystemModuleInformation->Count;
	//�������е�ģ��
	for (i = 0; i < ModuleCount; i++)
	{
		if ((ULONG64)(pSystemModuleInformation->Module[i].Base) > (ULONG64)0x8000000000000000)
		{
			pDrvName = pSystemModuleInformation->Module[i].ImageName + pSystemModuleInformation->Module[i].ModuleNameOffset;
			if (strstr(pDrvName, lpModuleName) != FALSE) {
				DbgPrint("ƥ��ɹ�");
				DbgPrint("������pDrvName = %s, Addr = %p", pSystemModuleInformation->Module[i].ImageName, pSystemModuleInformation->Module[i].Base);
				return (ULONG64)pSystemModuleInformation->Module[i].Base;
			}

		}
	}
	kfree(pBuffer);
	return 0;
}

VOID HideDriver(char* pDrvName)
{
	PKLDR_DATA_TABLE_ENTRY entry = (PKLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
	PKLDR_DATA_TABLE_ENTRY firstentry;
	ULONG64 pDrvBase = 0;
	KIRQL OldIrql;
	firstentry = entry;
	DbgPrint("Ҫ���ص������ļ���= %s", pDrvName);
	pDrvBase = GetSystemModuleBase(pDrvName);
	//DbgPrint("pDrvBase = %llu",pDrvBase);
	while ((PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink != firstentry)
	{

		if (entry->DllBase == pDrvBase)
		{
			//typedef struct LIST_ENTRY64 {
			//	ULONGLONG Flink;
			//	ULONGLONG Blink;
			//} LIST_ENTRY64;
			//typedef LIST_ENTRY64 *PLIST_ENTRY64;
			//le->Flink->Blink=le->Blink;
			//le->Blink->Flink=le->Flink;
			OldIrql = KeRaiseIrqlToDpcLevel();
			((LIST_ENTRY64*)(entry->InLoadOrderLinks.Flink))->Blink = entry->InLoadOrderLinks.Blink;
			((LIST_ENTRY64*)(entry->InLoadOrderLinks.Blink))->Flink = entry->InLoadOrderLinks.Flink;
			entry->InLoadOrderLinks.Flink = 0;
			entry->InLoadOrderLinks.Blink = 0;
			KeLowerIrql(OldIrql);
			DbgPrint("Remove LIST_ENTRY64 OK!");
			break;
		}
		//kprintf("%llx\t%wZ\t%wZ",entry->DllBase,entry->BaseDllName,entry->FullDllName);
		entry = (PKLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink;
	}
}