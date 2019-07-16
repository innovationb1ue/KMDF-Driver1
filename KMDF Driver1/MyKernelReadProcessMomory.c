#include <ntifs.h>
#include "MyKernelReadProcessMemory.h"

//OK ����ͨ�� ����2���� 
//��1���� sizeof(PKAPC_STATE)��ָ�� �øĽṹ��С sizeof(KAPC_STATE)
//��2���� KeStackAttachProcess�� ���̿ռ�仯�� �����ں��ڴ� ��ת BUF������
//AddressΪĿ����̵��ڴ��ַ
//Buffer //��ǰ���̵ĵ�ַ
BOOLEAN KReadProcessMemory(IN PEPROCESS Process, IN PVOID Address, IN UINT32 Length, IN PVOID Buffer)
{

	KAPC_STATE apc_state;
	RtlZeroMemory(&apc_state, sizeof(KAPC_STATE));
	//����һ�������н���ͨ�õ�Pool��ַ
	PVOID tmpBuf_Kernel = ExAllocatePool(NonPagedPool, Length);

	//���Ը�MDL�ѡ����̵�ַ��ӳ�䵽���ں�����Ҳ����
	//��ת�ڴ��ַ ��Ϊ KeStackAttachProcess�� ԭ���Ľ���R3 Buffer ��Ŀ���ַ�ﲻ����
	DbgPrint("yjx:���ӵ�Ŀ����� Address=%p  Buffer=%p", Address, Buffer);

	//����Ŀ������ڴ�ռ�
	KeStackAttachProcess((PVOID)Process, &apc_state);
	DbgPrint("KeStackAttachProcess���");
	//�ж�Ŀ���ַ�Ƿ���Է���
	BOOLEAN dwRet = MmIsAddressValid(Address);
	if (dwRet)
	{
		DbgPrint("yjx[sys64] RtlCopyMemory(%x, %x, %x);\r\n", Address, Buffer, Length);
		//RtlCopyMemory(Address, tmpBuf_Kernel, Length); //һ��Ҫ���ں��ڴ� ��ת
		RtlCopyMemory(tmpBuf_Kernel, Address, Length); //memcpy
	}
	else
	{
		DbgPrint("yjx:Address InValid �����ڴ��ַ���ɶ�");
	}
	//����Ŀ����̿ռ� �ָ�����
	KeUnstackDetachProcess(&apc_state);
	DbgPrint("yjx: sys����Ŀ�����");
	// �ٰ���Ŀ����̿ռ�������ڴ��Pool������������
	RtlCopyMemory(Buffer, tmpBuf_Kernel, Length);
	//DbgPrint("yjx:sys: Buffer[0]=%llx", *(UINT64*)Buffer);;
	ExFreePool(tmpBuf_Kernel);
	return dwRet;
}
//dwPidΪĿ�����id
//lpBaseAddress Ŀ����̵�ַ

//lpBuffer ��ǰ���̵�ַ 1
//�ں��ڴ��ַ ��ǰ���̵�ַ 2
int ReadProcessMemoryForPid(UINT32 dwPid, PVOID pBase, PVOID lpBuffer, UINT32 nSize)
{

	PEPROCESS Seleted_pEPROCESS = NULL;
	DbgPrint("yjx:sys64 ReadMemory pid=%d pBase=%p", dwPid, pBase);
	//����pid��ȡPEPROCESS
	if (PsLookupProcessByProcessId((PVOID)(UINT_PTR)(dwPid), &Seleted_pEPROCESS) == STATUS_SUCCESS)
	{
		// ��ȡ�ڴ湦�ܺ��� ��PEPROCESS,Address,Size,(PVOID)Buffer��
		BOOLEAN br = KReadProcessMemory(Seleted_pEPROCESS, (PVOID)pBase, nSize, lpBuffer);
		// ����һ�����ü������൱���ͷŽ��̾��
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
