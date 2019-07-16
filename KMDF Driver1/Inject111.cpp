
#include <ntddk.h>

PVOID pfn_BaseDispatchApc = NULL;
PVOID pfn_LoadLibraryA = NULL;
PVOID pszText_Kernel32 = NULL;
//PVOID pfn_NtQueueApcThread = NULL;

NTSTATUS(NTAPI* pfn_NtQueueApcThread)(
	__in HANDLE     ThreadHandle,
	__in PVOID      ApcRoutine,
	__in_opt PVOID  ApcArgument1,
	__in_opt PVOID  ApcArgument2,
	__in_opt PVOID  ApcArgument3
	);

VOID LoadImageNotifyRoutine(
	IN PUNICODE_STRING  FullImageName,
	IN HANDLE  ProcessId, // where image is mapped
	IN PIMAGE_INFO  ImageInfo
)
{
	PWSTR ptr = FullImageName->Buffer + FullImageName->Length / sizeof(WCHAR) - 4;

	if (ptr > FullImageName->Buffer && _wcsicmp(ptr, L".exe") == 0)
	{
		ptr -= 8;
		if (ptr > FullImageName->Buffer && _wcsicmp(ptr, L"\\notepad.exe") == 0)
		{
			pfn_NtQueueApcThread(ZwCurrentThread(),
				pfn_BaseDispatchApc,
				pfn_LoadLibraryA,
				pszText_Kernel32,
				NULL);
		}
	}
}

VOID DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	PsRemoveLoadImageNotifyRoutine(LoadImageNotifyRoutine);
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	//just for winxp
	KdBreakPoint();

	//填写kernel32!BaseDispatchApc//win7上是ntdll!RtlDispatchAPC
	(PVOID)pfn_BaseDispatchApc = (PVOID)0x77BFB7A0;

	//填写kernel32!LoadLibraryA
	(PVOID)pfn_LoadLibraryA = (PVOID)0x7c801d7b;

	//保证待Load的DLL的名称为text.dll，可以放到system目录下
	//填写kernel32的头部text字符的位置
	(PVOID)pszText_Kernel32 = (PVOID)0x7c8001e9;

	//填写nt!NtQueueApcThread   
	PVOID pfn_NtQueueApcThread= (PVOID)0x805d3756;

	DriverObject->DriverUnload = DriverUnload;

	return PsSetLoadImageNotifyRoutine(LoadImageNotifyRoutine);
}
