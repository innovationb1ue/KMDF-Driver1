#include "��ؽ���.h"
#include <ntddk.h>

VOID CallBack_CreateProcess(
	_Inout_ PEPROCESS Process,
	_In_ HANDLE ProcessId,
	_Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
	)
{
	DbgPrint("pid = %x,CreateInfo = %p", ProcessId, CreateInfo);
};


void Hook��ؽ���Ex() {
	NTSTATUS st = PsSetCreateProcessNotifyRoutineEx(CallBack_CreateProcess,FALSE); // ��װ���
	if (st == STATUS_SUCCESS) {
		DbgPrint("��װ���Ex�ɹ�");
	}
	else
	{
		DbgPrint("��װ���Exʧ�ܣ�������� = %X", st);
	}
}

void unHook��ؽ���Ex() {
	PsSetCreateProcessNotifyRoutineEx(CallBack_CreateProcess, TRUE);// �Ƴ����
}