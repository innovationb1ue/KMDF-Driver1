#include <ntddk.h>

#include "�����ڴ汣��.h"
#include "������Ϣ.h"
//�ص�����
OB_PREOP_CALLBACK_STATUS
my_obPreProc(
	_In_ PVOID RegistrationContext,
	_Inout_ POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
	//�ж����û��� ���� �����ں˶���
	if (OperationInformation->KernelHandle)
	{
		return OB_PREOP_SUCCESS;
	}
	char* szProcessName = NULL;
	// �жϽ������Ƿ����������Ӵ���������ᱻ����
	char* szExeName = "MFC_FORREAD";
	PEPROCESS pEp = (PEPROCESS)OperationInformation->Object;
	HANDLE pid = PsGetProcessId(pEp);//��ȡĿ����̵�pid
	szProcessName = GetProcessImageNameByProcessID(pid);

	if (strstr(szProcessName, szExeName))
	//else
	{

		//�û���
		ACCESS_MASK mask = OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & ~PROCESS_VM_READ;
		//ȡ��PROCESS_VM_READ;
		ACCESS_MASK mask2 = mask & ~PROCESS_TERMINATE;//�ܾ���������
		mask2 = mask2 & ~PROCESS_VM_READ;//��ֹ�������ڴ�
		//��ֹд��
		mask2 = mask2 & ~PROCESS_VM_WRITE;//��ֹд�����ڴ� WriteProcessMemory
		//��ֹ�����ڴ�ҳ������
		mask2 = mask2 & ~PROCESS_VM_OPERATION;
		//��ֹ��ѯ������Ϣ PROCESS_QUERY_INFORMATION
		mask2 = mask2 & ~PROCESS_QUERY_INFORMATION;
		//PROCESS_CREATE_PROCESS //CreateProcess
		mask2 = mask2 & ~PROCESS_CREATE_PROCESS;
		//PROCESS_CREATE_THREAD ��ֹ�� CreateRemoteThread
		mask2 = mask2 & ~PROCESS_CREATE_THREAD; //
		//PROCESS_DUP_HANDLE
		mask2 = mask2 & ~PROCESS_DUP_HANDLE;
		OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = mask2;



	}
	return 1;
};


static HANDLE gs_Handleback = NULL;
void ��װ�����ڴ汣��()
{
	OB_CALLBACK_REGISTRATION ob1_backreg;
	OB_OPERATION_REGISTRATION ob2_Opertion;
	ob1_backreg.Version = OB_FLT_REGISTRATION_VERSION;
	ob1_backreg.OperationRegistrationCount = 1;
	RtlInitUnicodeString(&ob1_backreg.Altitude, L"321000");
	ob1_backreg.OperationRegistration = &ob2_Opertion;
	ob1_backreg.RegistrationContext = NULL;
	//��ʼ����� �ӽṹ
	ob2_Opertion.ObjectType = PsProcessType;
	ob2_Opertion.Operations = OB_OPERATION_HANDLE_CREATE;
	ob2_Opertion.PostOperation = NULL;//��ע����ص�
	ob2_Opertion.PreOperation = my_obPreProc; //ע��pre�ص�
	ObRegisterCallbacks(&ob1_backreg, &gs_Handleback);
}

void ж�ؽ����ڴ汣��()
{
	ObUnRegisterCallbacks(gs_Handleback);
}