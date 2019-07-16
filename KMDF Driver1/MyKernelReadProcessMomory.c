#include <ntifs.h>
#include "MyKernelReadProcessMemory.h"

//OK 测试通过 遇到2个坑 
//第1个坑 sizeof(PKAPC_STATE)是指针 得改结构大小 sizeof(KAPC_STATE)
//第2个坑 KeStackAttachProcess后 进程空间变化了 得用内核内存 中转 BUF缓冲区
//Address为目标进程的内存地址
//Buffer //当前进程的地址
BOOLEAN KReadProcessMemory(IN PEPROCESS Process, IN PVOID Address, IN UINT32 Length, IN PVOID Buffer)
{

	KAPC_STATE apc_state;
	RtlZeroMemory(&apc_state, sizeof(KAPC_STATE));
	//创建一个在所有进程通用的Pool地址
	PVOID tmpBuf_Kernel = ExAllocatePool(NonPagedPool, Length);

	//可以改MDL把　进程地址　映射到　内核驱动也可以
	//中转内存地址 因为 KeStackAttachProcess后 原来的进程R3 Buffer 在目标地址里不存在
	DbgPrint("yjx:附加到目标进程 Address=%p  Buffer=%p", Address, Buffer);

	//进入目标进程内存空间
	KeStackAttachProcess((PVOID)Process, &apc_state);
	DbgPrint("KeStackAttachProcess完毕");
	//判断目标地址是否可以访问
	BOOLEAN dwRet = MmIsAddressValid(Address);
	if (dwRet)
	{
		DbgPrint("yjx[sys64] RtlCopyMemory(%x, %x, %x);\r\n", Address, Buffer, Length);
		//RtlCopyMemory(Address, tmpBuf_Kernel, Length); //一定要用内核内存 中转
		RtlCopyMemory(tmpBuf_Kernel, Address, Length); //memcpy
	}
	else
	{
		DbgPrint("yjx:Address InValid 传入内存地址不可读");
	}
	//分离目标进程空间 恢复环境
	KeUnstackDetachProcess(&apc_state);
	DbgPrint("yjx: sys分离目标进程");
	// 再把在目标进程空间读到的内存从Pool拷贝到返回区
	RtlCopyMemory(Buffer, tmpBuf_Kernel, Length);
	//DbgPrint("yjx:sys: Buffer[0]=%llx", *(UINT64*)Buffer);;
	ExFreePool(tmpBuf_Kernel);
	return dwRet;
}
//dwPid为目标进程id
//lpBaseAddress 目标进程地址

//lpBuffer 当前进程地址 1
//内核内存地址 当前进程地址 2
int ReadProcessMemoryForPid(UINT32 dwPid, PVOID pBase, PVOID lpBuffer, UINT32 nSize)
{

	PEPROCESS Seleted_pEPROCESS = NULL;
	DbgPrint("yjx:sys64 ReadMemory pid=%d pBase=%p", dwPid, pBase);
	//根据pid获取PEPROCESS
	if (PsLookupProcessByProcessId((PVOID)(UINT_PTR)(dwPid), &Seleted_pEPROCESS) == STATUS_SUCCESS)
	{
		// 读取内存功能函数 （PEPROCESS,Address,Size,(PVOID)Buffer）
		BOOLEAN br = KReadProcessMemory(Seleted_pEPROCESS, (PVOID)pBase, nSize, lpBuffer);
		// 减少一个引用计数，相当于释放进程句柄
		ObDereferenceObject(Seleted_pEPROCESS);
		if (br)
		{
			return nSize;
		}
	}
	else
	{
		DbgPrint(("yjx sys64 PsLookupProcessByProcessId Fail..."));
	}

	return 0;// STATUS_UNSUCCESSFUL;

}
