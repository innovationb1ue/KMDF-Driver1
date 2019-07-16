//#include <ntifs.h>
//
////��дVirtualAllocEx ʵ�ֿ���� �����ڴ�ռ�
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
//// ���� case IO_CTRL_YJX_ALLOCATEMEM:
//NTSTATUS  IRP_YJX_VirtualAllocEx(PDEVICE_OBJECT device_Object, PIRP pirp)
//{
//
//	UNREFERENCED_PARAMETER(device_Object); //δʹ�õĲ��� ��ֹ����
//	DbgPrint("yjx:sysNtAllocateVirtualMemory ���� IRP_YJX_VirtualAllocEx \n");
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
//	PVOID BaseAddress = NULL; //���ص�ַ
//
//	PEPROCESS selectedprocess = NULL;//Ŀ�����	
//	SIZE_T RegionSize = 0;//�����С
//
//	//bufInput = pirp->AssociatedIrp.SystemBuffer; //���� ��� ������
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
//			//�л����� ���ӵ�ָ������
//			PRKPROCESS
//			KeAttachProcess(selectedprocess); //
//
//			__try
//			{
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->Calling ZwAllocateVirtualMemory\n");
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->��Ҫ����ĵ�ַ: BaseAddress=%p\n", BaseAddress);
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->��Ҫ����Ĵ�С: RegionSize=%x\n", RegionSize);
//				//__debugbreak();0xFFFFFFFFFFFFF
//				//VirtualAllocEx ��Ӧ
//				ntStatus = ZwAllocateVirtualMemory((HANDLE)-1, //ProcessID��ָ��� Ŀ�����
//					&BaseAddress, //���� ��� ���Ȱ�BaseAddress
//					0, //
//					&RegionSize, //��Ҫ����Ĵ�С dwSize
//					(ULONG)bufInput->AllocationType, //flAllocationType 
//					(ULONG)bufInput->Protect); //flProtect 
//				//__debugbreak();
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->ntStatus=%x\n", ntStatus);
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->���ص�ַ=%p\n", BaseAddress);
//				DbgPrint("yjx:sysNtAllocateVirtualMemory ->�ɹ����� RegionSize=%x\n", RegionSize);
//				//*(PUINT64)Irp->AssociatedIrp.SystemBuffer=0;
//
//				*(PUINT_PTR)pirp->AssociatedIrp.SystemBuffer = (UINT_PTR)BaseAddress; //���ص�ַ
//
//			}
//			__finally
//			{  //�л�ԭ����
//				KeDetachProcess();
//			}
//
//		}
//		__except (1)
//		{
//			ntStatus = STATUS_UNSUCCESSFUL;
//			//return ntStatus;;
//		}
//		//ɾ������
//		ObDereferenceObject(selectedprocess);
//	}
//
//	//ENDCODE:
//	//pirp->IoStatus.Status = STATUS_SUCCESS;//
//	////pirp->IoStatus.Information = 4;//���ظ�DeviceIoControl�е� �����ڶ�������lpBytesReturned
//	//IoCompleteRequest(pirp, IO_NO_INCREMENT);//���÷����������I/O��������� ���Ҳ��������ȼ� 
//	if (irpStack) //
//	{
//		if (ntStatus == STATUS_SUCCESS)
//		{ //�ɹ��򷵻� ��������С
//			pirp->IoStatus.Information = irpStack->Parameters.DeviceIoControl.OutputBufferLength;//DeviceIoControl
//		}
//		else
//		{ //ʧ���򲻷���
//			pirp->IoStatus.Information = 0;
//		}
//		//�������
//		IoCompleteRequest(pirp, IO_NO_INCREMENT);
//	}
//
//	pirp->IoStatus.Status = ntStatus;
//	return ntStatus;
//}
//
//
