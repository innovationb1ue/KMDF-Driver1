#include <ntifs.h>
#include "DeviceIoControl.h"
#include "MyKernelReadProcessMemory.h"


// 新DeviceIoCOntrol IRP函数
NTSTATUS DeviceIoControl_Irp(PDEVICE_OBJECT device, PIRP pirp) {
	DbgPrint(("yjx:进入DeviceIo新派遣函数"));
	PIO_STACK_LOCATION irpStackL;
	ULONG CtlCode;
	ULONG InputBuffLength;
	irpStackL = IoGetCurrentIrpStackLocation(pirp); //获取应用层传来的参数
	ULONG nCode = irpStackL->Parameters.DeviceIoControl.IoControlCode;
	DbgPrint("yjx:用户层调用了 DeviceIoControl nCode=%d  IO_READ_MEMORY=%d", nCode, IO_READ_MEMORY);
	switch (nCode)
	{
	case IO_VIRTUAL_ALLOC_MEM_EX:
	{
		break;
		//return IRP_YJX_VirtualAllocEx(device, pirp);
	}
	case IO_READ_MEMORY2:
	{
		break;
		//return IRP_ReadProcessMemory2(device, pirp);
	}
	case IO_READ_MEMORY: //DeviceIoControl
	{
		//接收 传入的参数 
#pragma pack(push)
#pragma pack(1)
		// 自定义一个结构体来获取传入的参数（传入时使用相同结构体）
		typedef struct TINPUT_BUF
		{
			DWORD32 dwPid;
			PVOID pBase;
			//PVOID lpBuf;
			DWORD32 nSize;

		}TINPUT_BUF;
#pragma pack(pop)
		// 参数指针
		TINPUT_BUF* pInBuf = (TINPUT_BUF*)pirp->AssociatedIrp.SystemBuffer;
		// 要读取的字节数
		ULONG nSize = pInBuf->nSize;

		DbgPrint("yjx:sys64 Entry IO_READ_MEMORY pBase=%p", pInBuf->pBase);
		// 功能函数
		ReadProcessMemoryForPid(pInBuf->dwPid, pInBuf->pBase, pInBuf, pInBuf->nSize);
		//返回数据给应用层R3
		pirp->IoStatus.Information = nSize;//返回给DeviceIoControl中的 倒数第二个参数lpBytesReturned
		break;
	}
	}

	pirp->IoStatus.Status = STATUS_SUCCESS;
	//pirp->IoStatus.Information = 4;//返回给DeviceIoControl中的 倒数第二个参数lpBytesReturned
	IoCompleteRequest(pirp, IO_NO_INCREMENT);//调用方已完成所有I/O请求处理操作 并且不增加优先级 
	DbgPrint(("yjx:离开派遣函数"));
	return STATUS_SUCCESS;

}

// IRP处理函数
NTSTATUS DeviceIrpCtr(PDEVICE_OBJECT device, PIRP pirp) {
	DbgPrint("进入派遣函数");
	PIO_STACK_LOCATION irpStackL;
	ULONG CtlCode;
	ULONG INputBuffLength;
	// 获取R3层传递的参数
	irpStackL = IoGetCurrentIrpStackLocation(pirp);

	switch (irpStackL->MajorFunction)
	{
	case IRP_MJ_DEVICE_CONTROL:
	{
		DbgPrint("不应该调用老IRP函数里面的Control!ERROR!!!");
		// 跳转到新IRP函数
		//DeviceIoControl_Irp(device, pirp);
		//break;

		// 老IRP函数
		DbgPrint("yjx: 用户层DeviceIoControl 进入老IRP处理函数");
		// 获取传入参数
		unsigned int* pInBuf = (unsigned int*)pirp->AssociatedIrp.SystemBuffer;
		DbgPrint("IoControl 传入参数: %x,%x,%x,%x,%x,%x", pInBuf[0], pInBuf[1], pInBuf[2], pInBuf[3], pInBuf[4], pInBuf[5]);
		// 返回参数到应用层 
		pInBuf[0] = 0x111abc;
		pInBuf[1] = 0x222abc;
		pInBuf[2] = 0x333abc;
		pInBuf[3] = 0x444abc;
		pInBuf[4] = 0x555abc;
		pInBuf[5] = 0x666abc;
		pirp->IoStatus.Information = 4 * 6;
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
