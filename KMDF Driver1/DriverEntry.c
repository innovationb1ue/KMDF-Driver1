#include <ntddk.h>
#include "监控进程.h"
#include "监控进程Ex.h"
#include "进程内存保护.h"
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

// 驱动入口
NTSTATUS DriverEntry(PDRIVER_OBJECT Driver, PUNICODE_STRING szReg)
{
	DbgPrint(("yjx:DbgPrint 我的第一个驱动"));
	KdPrint(("yjx:KdPrint 我的第一个驱动")); 
	ULONGLONG NtOpenProcess_Cur_Addr;
	// 读取SSDT表
	//NtOpenProcess_Cur_Addr = GetSSDTFuncCurAddr((ULONG)38);
	//DbgPrint("OpenProcess = %llu", NtOpenProcess_Cur_Addr);
	//设置卸载程序DriverUnload
	Driver->DriverUnload = DriverUnload;
	// 创建驱动设备对象
	NTSTATUS status = CreateDevice(Driver);
	
	// 注册IRP函数
	Driver->MajorFunction[IRP_MJ_CREATE] = DeviceIrpCtr;// CreateFile
	Driver->MajorFunction[IRP_MJ_CLOSE] = DeviceIrpCtr; // Unload
	Driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIrpCtr; // DeviceIO Control
	if (status == STATUS_SUCCESS) { DbgPrint(("CreateDevice 成功")); }

	// 隐藏驱动：断链
	pDriverObject = Driver;
	HideDriver("BlackBoneDrv10.sys");

	
	// 处理权限
	// 在命令行中添加 /INTEGRITYCHECK 即可解决Ex权限不够的问题


	// 监控(阻止特定进程执行)
	//Hook监控进程Ex();
	
	//安装进程内存保护();
	return STATUS_SUCCESS;
};
MmUserProbeAddress;
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
	
	//卸载进程内存保护();
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

}//然后是R0隐藏内核模块，摘链问题。也就是要注意结构定义细节就行了。
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
	ULONG Count;//内核中以加载的模块的个数
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
		//分配内存
		pBuffer = kmalloc(BufferSize);
		if (pBuffer == NULL)
			return 0;
		//查询模块信息
		Result = ZwQuerySystemInformation(11, pBuffer, BufferSize, &NeedSize);
		if (Result == STATUS_INFO_LENGTH_MISMATCH)
		{
			kfree(pBuffer);
			BufferSize *= 2;
		}
		else if (!NT_SUCCESS(Result))
		{
			//查询失败则退出
			kfree(pBuffer);
			return 0;
		}
	} while (Result == STATUS_INFO_LENGTH_MISMATCH);
	pSystemModuleInformation = (PSYSTEM_MODULE_INFORMATION)pBuffer;
	//获得模块的总数量
	ModuleCount = pSystemModuleInformation->Count;
	//遍历所有的模块
	for (i = 0; i < ModuleCount; i++)
	{
		if ((ULONG64)(pSystemModuleInformation->Module[i].Base) > (ULONG64)0x8000000000000000)
		{
			pDrvName = pSystemModuleInformation->Module[i].ImageName + pSystemModuleInformation->Module[i].ModuleNameOffset;
			if (strstr(pDrvName, lpModuleName) != FALSE) {
				DbgPrint("匹配成功");
				DbgPrint("返回了pDrvName = %s, Addr = %p", pSystemModuleInformation->Module[i].ImageName, pSystemModuleInformation->Module[i].Base);
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
	DbgPrint("要隐藏的驱动文件名= %s", pDrvName);
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