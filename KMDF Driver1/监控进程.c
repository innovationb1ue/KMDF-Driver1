#include <ntddk.h>

// 回调函数
VOID CallBack_NotifyProcess(
	_In_ HANDLE ParentId,//父进程
	_In_ HANDLE ProcessId,//被创建的进程ID
	_In_ BOOLEAN Create) //TRUE表示 进程创建， FALSE表示退出进程
{
	HANDLE ProcessHandle = NULL;
	OBJECT_ATTRIBUTES obj = { 0 };
	CLIENT_ID ClientId = { 0 };

	obj.Length = sizeof(OBJECT_ATTRIBUTES);
	ClientId.UniqueProcess = (HANDLE)ProcessId;

	if (Create) {
		DbgPrint("进程%d, 被进程%d 创建", ProcessId, ParentId);
		// 过滤 如果监控到有新进程创建，阻止创建
		// 打开进程
		NTSTATUS st = ZwOpenProcess(&ProcessHandle,PROCESS_ALL_ACCESS,&obj, &ClientId);
		if (st == STATUS_SUCCESS) {
			DbgPrint("打开进程成功, ProcessHandle = %X", ProcessHandle);
			if (ProcessHandle) {
				st = ZwTerminateProcess(ProcessHandle, 123);
				if (st == STATUS_SUCCESS) {
					DbgPrint("阻止 %d 进程创建成功", ProcessId);
				}
				else {
					DbgPrint("阻止 %d 进程创建失败 错误代码 = %X", ProcessId,st);
				}
			}
		}
	}
	else {
		DbgPrint("退出进程,ID = %d, 父进ID=%d",ProcessId, ParentId);
	}
	
};


void Hook进程() {
	// Hook
	PsSetCreateProcessNotifyRoutine(CallBack_NotifyProcess, FALSE);	//安装监控

}
void UnHook进程() {
	// Hook
	PsSetCreateProcessNotifyRoutine(CallBack_NotifyProcess, TRUE);	//移出监控

}
