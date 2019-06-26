#include <ntddk.h>

// �ص�����
VOID CallBack_NotifyProcess(
	_In_ HANDLE ParentId,//������
	_In_ HANDLE ProcessId,//�������Ľ���ID
	_In_ BOOLEAN Create) //TRUE��ʾ ���̴����� FALSE��ʾ�˳�����
{
	HANDLE ProcessHandle = NULL;
	OBJECT_ATTRIBUTES obj = { 0 };
	CLIENT_ID ClientId = { 0 };

	obj.Length = sizeof(OBJECT_ATTRIBUTES);
	ClientId.UniqueProcess = (HANDLE)ProcessId;

	if (Create) {
		DbgPrint("����%d, ������%d ����", ProcessId, ParentId);
		// ���� �����ص����½��̴�������ֹ����
		// �򿪽���
		NTSTATUS st = ZwOpenProcess(&ProcessHandle,PROCESS_ALL_ACCESS,&obj, &ClientId);
		if (st == STATUS_SUCCESS) {
			DbgPrint("�򿪽��̳ɹ�, ProcessHandle = %X", ProcessHandle);
			if (ProcessHandle) {
				st = ZwTerminateProcess(ProcessHandle, 123);
				if (st == STATUS_SUCCESS) {
					DbgPrint("��ֹ %d ���̴����ɹ�", ProcessId);
				}
				else {
					DbgPrint("��ֹ %d ���̴���ʧ�� ������� = %X", ProcessId,st);
				}
			}
		}
	}
	else {
		DbgPrint("�˳�����,ID = %d, ����ID=%d",ProcessId, ParentId);
	}
	
};


void Hook����() {
	// Hook
	PsSetCreateProcessNotifyRoutine(CallBack_NotifyProcess, FALSE);	//��װ���

}
void UnHook����() {
	// Hook
	PsSetCreateProcessNotifyRoutine(CallBack_NotifyProcess, TRUE);	//�Ƴ����

}
