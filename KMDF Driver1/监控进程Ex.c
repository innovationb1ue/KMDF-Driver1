#include "监控进程.h"
#include <ntddk.h>

VOID CallBack_CreateProcess(
	_Inout_ PEPROCESS Process,
	_In_ HANDLE ProcessId,
	_Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
	)
{
	DbgPrint("pid = %x,CreateInfo = %p", ProcessId, CreateInfo);
};


void Hook监控进程Ex() {
	NTSTATUS st = PsSetCreateProcessNotifyRoutineEx(CallBack_CreateProcess,FALSE); // 安装监控
	if (st == STATUS_SUCCESS) {
		DbgPrint("安装监控Ex成功");
	}
	else
	{
		DbgPrint("安装监控Ex失败，错误代码 = %X", st);
	}
}

void unHook监控进程Ex() {
	PsSetCreateProcessNotifyRoutineEx(CallBack_CreateProcess, TRUE);// 移除监控
}