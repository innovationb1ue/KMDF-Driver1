#include "��ؽ���.h"
#include <ntddk.h>

VOID CallBack_CreateProcess(
	_Inout_ PEPROCESS Process,
	_In_ HANDLE ProcessId,
	_Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
	)
{
	DbgPrint("pid = %x,CreateInfo = %p", ProcessId, CreateInfo);
	if (CreateInfo)
	{
		DbgPrint("��������pid = %x,CreateInfo = %p %wZ", ProcessId, CreateInfo, CreateInfo->ImageFileName->Buffer);
		// ֱ���޸Ĵ�����״̬Ϊ�ܾ������޷���������

		// ���ļ������Ƴ�����Ϊ���ڽ�β���\0 ������
		WCHAR szPathFile[255] = { 0 };
		memcpy_s(szPathFile, 253, CreateInfo->CommandLine->Buffer, CreateInfo->CommandLine->Length);

		if (wcsstr(szPathFile, L"calc.exe")) // ��������
		{
			DbgPrint("��ֹ���̴���pid = %x,CreateInfo = %p  ����=%wZ", ProcessId, CreateInfo,CreateInfo->ImageFileName);
			CreateInfo->CreationStatus = STATUS_ACCESS_DISABLED_NO_SAFER_UI_BY_POLICY ;// ���������ʾܾ�����
		}
	}
	else
	{
		DbgPrint("��������pid = %x,CreateInfo = %p", ProcessId, CreateInfo);
	}
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