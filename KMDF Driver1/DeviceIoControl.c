#include <ntifs.h>
#include "DeviceIoControl.h"
#include "MyKernelReadProcessMemory.h"


// ��DeviceIoCOntrol IRP����
NTSTATUS DeviceIoControl_Irp(PDEVICE_OBJECT device, PIRP pirp) {
	DbgPrint(("yjx:����DeviceIo����ǲ����"));
	PIO_STACK_LOCATION irpStackL;
	ULONG CtlCode;
	ULONG InputBuffLength;
	irpStackL = IoGetCurrentIrpStackLocation(pirp); //��ȡӦ�ò㴫���Ĳ���
	ULONG nCode = irpStackL->Parameters.DeviceIoControl.IoControlCode;
	DbgPrint("yjx:�û�������� DeviceIoControl nCode=%d  IO_READ_MEMORY=%d", nCode, IO_READ_MEMORY);
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
		//���� ����Ĳ��� 
#pragma pack(push)
#pragma pack(1)
		// �Զ���һ���ṹ������ȡ����Ĳ���������ʱʹ����ͬ�ṹ�壩
		typedef struct TINPUT_BUF
		{
			DWORD32 dwPid;
			PVOID pBase;
			//PVOID lpBuf;
			DWORD32 nSize;

		}TINPUT_BUF;
#pragma pack(pop)
		// ����ָ��
		TINPUT_BUF* pInBuf = (TINPUT_BUF*)pirp->AssociatedIrp.SystemBuffer;
		// Ҫ��ȡ���ֽ���
		ULONG nSize = pInBuf->nSize;

		DbgPrint("yjx:sys64 Entry IO_READ_MEMORY pBase=%p", pInBuf->pBase);
		// ���ܺ���
		ReadProcessMemoryForPid(pInBuf->dwPid, pInBuf->pBase, pInBuf, pInBuf->nSize);
		//�������ݸ�Ӧ�ò�R3
		pirp->IoStatus.Information = nSize;//���ظ�DeviceIoControl�е� �����ڶ�������lpBytesReturned
		break;
	}
	}

	pirp->IoStatus.Status = STATUS_SUCCESS;
	//pirp->IoStatus.Information = 4;//���ظ�DeviceIoControl�е� �����ڶ�������lpBytesReturned
	IoCompleteRequest(pirp, IO_NO_INCREMENT);//���÷����������I/O��������� ���Ҳ��������ȼ� 
	DbgPrint(("yjx:�뿪��ǲ����"));
	return STATUS_SUCCESS;

}

// IRP������
NTSTATUS DeviceIrpCtr(PDEVICE_OBJECT device, PIRP pirp) {
	DbgPrint("������ǲ����");
	PIO_STACK_LOCATION irpStackL;
	ULONG CtlCode;
	ULONG INputBuffLength;
	// ��ȡR3�㴫�ݵĲ���
	irpStackL = IoGetCurrentIrpStackLocation(pirp);

	switch (irpStackL->MajorFunction)
	{
	case IRP_MJ_DEVICE_CONTROL:
	{
		DbgPrint("��Ӧ�õ�����IRP���������Control!ERROR!!!");
		// ��ת����IRP����
		//DeviceIoControl_Irp(device, pirp);
		//break;

		// ��IRP����
		DbgPrint("yjx: �û���DeviceIoControl ������IRP������");
		// ��ȡ�������
		unsigned int* pInBuf = (unsigned int*)pirp->AssociatedIrp.SystemBuffer;
		DbgPrint("IoControl �������: %x,%x,%x,%x,%x,%x", pInBuf[0], pInBuf[1], pInBuf[2], pInBuf[3], pInBuf[4], pInBuf[5]);
		// ���ز�����Ӧ�ò� 
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
