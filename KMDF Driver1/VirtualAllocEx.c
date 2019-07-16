//#include <ntifs.h>
//
////自写VirtualAllocEx 实现跨进程 分配内存空间
////NTSYSAPI
////NTSTATUS
////NTAPI
////ZwAllocateVirtualMemory(
////	_In_ HANDLE ProcessHandle,
////	_Inout_ PVOID *BaseAddress,
////	_In_ ULONG_PTR ZeroBits,
////	_Inout_ PSIZE_T RegionSize,
////	_In_ ULONG AllocationType,
////	_In_ ULONG Protect
////);
////__kernel_entry NTSYSCALLAPI
////NTSTATUS
////NTAPI
////NtAllocateVirtualMemory(
////	_In_ HANDLE ProcessHandle,
////	_Inout_ _At_(*BaseAddress, _Readable_bytes_(*RegionSize) _Writable_bytes_(*RegionSize) _Post_readable_byte_size_(*RegionSize)) PVOID *BaseAddress,
////	_In_ ULONG_PTR ZeroBits,
////	_Inout_ PSIZE_T RegionSize,
////	_In_ ULONG AllocationType,
////	_In_ ULONG Protect
////);
//
////LPVOID VirtualAllocEx(
////	DWORD npid,//HANDLE hProcess,  // process within which to allocate memory
////	LPVOID lpAddress, // desired starting address of allocation
////	DWORD dwSize,     // size, in bytes, of region to allocate
////	DWORD flAllocationType,
////	// type of allocation
////	DWORD flProtect   // type of access protection
////);
//
//
//// 代码 case IO_CTRL_YJX_ALLOCATEMEM:
//NTSTATUS  IRP_YJX_VirtualAllocEx(PDEVICE_OBJECT device_Object, PIRP pirp)
//{
//
//	UNREFERENCED_PARAMETER(device_Object); //未使用的参数 禁止警告
//	DbgPrint("yjx:sysNtAllocateVirtualMemory 进入 IRP_YJX_VirtualAllocEx \n");
//	NTSTATUS ntStatus = STATUS_SUCCESS;
//	PIO_STACK_LOCATION     irpStack = NULL;
//	irpStack = IoGetCurrentIrpStackLocation(pirp);
//
//	struct TBUF_INPUT
//	{
//		UINT64 ProcessID;
//		UINT64 BaseAddress;
//		UINT64 Size;
//		UINT64 AllocationType;
//		UINT64 Protect;
//	} *bufInput;
//
//	PVOID BaseAddress = NULL; //返回地址
//
//	PEPROCESS selectedprocess = NULL;//目标进程	
//	SIZE_T RegionSize = 0;//分配大小
//
//	//bufInput = pirp->AssociatedIrp.SystemBuffer; //输入 输出 缓冲区
//	//BaseAddress = (PVOID)(UINT_PTR)bufInput->BaseAddress;
//	BaseAddress = pirp->AssociatedIrp.SystemBuffer;
//	RegionSize = (SIZE_T)(bufInput->Size);
//	DbgPrint("BaseAddress = %X", BaseAddress);
//
//	if (PsLookupProcessByProcessId((PVOID)(UINT_PTR)(bufInput->ProcessID), &selectedprocess) == STATUS_SUCCESS)
//	{
//		__try
//		{
//			KAPC_STATE apc_state;
//			RtlZeroMemory(&apc_state, sizeof(apc_state));
//			//切换进程 附加到指定进程
//			PRKPROCESS
//			KeAttachProcess(selectedprocess); //
//
//			__try
//			{
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->Calling ZwAllocateVirtualMemory\n");
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->想要分配的地址: BaseAddress=%p\n", BaseAddress);
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->想要分配的大小: RegionSize=%x\n", RegionSize);
//				//__debugbreak();0xFFFFFFFFFFFFF
//				//VirtualAllocEx 对应
//				ntStatus = ZwAllocateVirtualMemory((HANDLE)-1, //ProcessID所指向的 目标进程
//					&BaseAddress, //输入 输出 优先按BaseAddress
//					0, //
//					&RegionSize, //想要分配的大小 dwSize
//					(ULONG)bufInput->AllocationType, //flAllocationType 
//					(ULONG)bufInput->Protect); //flProtect 
//				//__debugbreak();
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->ntStatus=%x\n", ntStatus);
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->返回地址=%p\n", BaseAddress);
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->成功分配 RegionSize=%x\n", RegionSize);
//				//*(PUINT64)Irp->AssociatedIrp.SystemBuffer=0;
//
//				*(PUINT_PTR)pirp->AssociatedIrp.SystemBuffer = (UINT_PTR)BaseAddress; //返回地址
//
//			}
//			__finally
//			{  //切换原进程
//				KeDetachProcess();
//			}
//
//		}
//		__except (1)
//		{
//			ntStatus = STATUS_UNSUCCESSFUL;
//			//return ntStatus;;
//		}
//		//删除对象
//		ObDereferenceObject(selectedprocess);
//	}
//
//	//ENDCODE:
//	//pirp->IoStatus.Status = STATUS_SUCCESS;//
//	////pirp->IoStatus.Information = 4;//返回给DeviceIoControl中的 倒数第二个参数lpBytesReturned
//	//IoCompleteRequest(pirp, IO_NO_INCREMENT);//调用方已完成所有I/O请求处理操作 并且不增加优先级 
//	if (irpStack) //
//	{
//		if (ntStatus == STATUS_SUCCESS)
//		{ //成功则返回 缓冲区大小
//			pirp->IoStatus.Information = irpStack->Parameters.DeviceIoControl.OutputBufferLength;//DeviceIoControl
//		}
//		else
//		{ //失败则不返回
//			pirp->IoStatus.Information = 0;
//		}
//		//完成请求
//		IoCompleteRequest(pirp, IO_NO_INCREMENT);
//	}
//
//	pirp->IoStatus.Status = ntStatus;
//	return ntStatus;
//}
//
//
