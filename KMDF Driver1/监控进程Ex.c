#include "监控进程.h"
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
		DbgPrint("创建进程pid = %x,CreateInfo = %p %wZ", ProcessId, CreateInfo, CreateInfo->ImageFileName->Buffer);
		// 直接修改创建的状态为拒绝，则无法创建进程

		// 把文件名复制出来，为了在结尾添加\0 结束符
		WCHAR szPathFile[255] = { 0 };
		memcpy_s(szPathFile, 253, CreateInfo->CommandLine->Buffer, CreateInfo->CommandLine->Length);

		if (wcsstr(szPathFile, L"calc.exe")) // 存在问题
		{
			DbgPrint("阻止进程创建pid = %x,CreateInfo = %p  名称=%wZ", ProcessId, CreateInfo,CreateInfo->ImageFileName);
			CreateInfo->CreationStatus = STATUS_ACCESS_DISABLED_NO_SAFER_UI_BY_POLICY ;// 不弹出访问拒绝窗口
		}
	}
	else
	{
		DbgPrint("结束进程pid = %x,CreateInfo = %p", ProcessId, CreateInfo);
	}
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