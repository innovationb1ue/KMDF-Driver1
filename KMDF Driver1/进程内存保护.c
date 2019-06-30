#include <ntddk.h>

#include "进程内存保护.h"
#include "进程信息.h"
//回调函数
OB_PREOP_CALLBACK_STATUS
my_obPreProc(
	_In_ PVOID RegistrationContext,
	_Inout_ POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
	//判断是用户层 动作 还是内核动作
	if (OperationInformation->KernelHandle)
	{
		return OB_PREOP_SUCCESS;
	}
	char* szProcessName = NULL;
	// 判断进程名是否包含下面的子串，包含则会被保护
	char* szExeName = "MFC_FORREAD";
	PEPROCESS pEp = (PEPROCESS)OperationInformation->Object;
	HANDLE pid = PsGetProcessId(pEp);//获取目标进程的pid
	szProcessName = GetProcessImageNameByProcessID(pid);

	if (strstr(szProcessName, szExeName))
	//else
	{

		//用户层
		ACCESS_MASK mask = OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess & ~PROCESS_VM_READ;
		//取消PROCESS_VM_READ;
		ACCESS_MASK mask2 = mask & ~PROCESS_TERMINATE;//拒绝结束进程
		mask2 = mask2 & ~PROCESS_VM_READ;//禁止读进程内存
		//禁止写入
		mask2 = mask2 & ~PROCESS_VM_WRITE;//禁止写进程内存 WriteProcessMemory
		//禁止更新内存页面属性
		mask2 = mask2 & ~PROCESS_VM_OPERATION;
		//禁止查询进程信息 PROCESS_QUERY_INFORMATION
		mask2 = mask2 & ~PROCESS_QUERY_INFORMATION;
		//PROCESS_CREATE_PROCESS //CreateProcess
		mask2 = mask2 & ~PROCESS_CREATE_PROCESS;
		//PROCESS_CREATE_THREAD 禁止了 CreateRemoteThread
		mask2 = mask2 & ~PROCESS_CREATE_THREAD; //
		//PROCESS_DUP_HANDLE
		mask2 = mask2 & ~PROCESS_DUP_HANDLE;
		OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = mask2;



	}
	return 1;
};


static HANDLE gs_Handleback = NULL;
void 安装进程内存保护()
{
	OB_CALLBACK_REGISTRATION ob1_backreg;
	OB_OPERATION_REGISTRATION ob2_Opertion;
	ob1_backreg.Version = OB_FLT_REGISTRATION_VERSION;
	ob1_backreg.OperationRegistrationCount = 1;
	RtlInitUnicodeString(&ob1_backreg.Altitude, L"321000");
	ob1_backreg.OperationRegistration = &ob2_Opertion;
	ob1_backreg.RegistrationContext = NULL;
	//初始化这个 子结构
	ob2_Opertion.ObjectType = PsProcessType;
	ob2_Opertion.Operations = OB_OPERATION_HANDLE_CREATE;
	ob2_Opertion.PostOperation = NULL;//不注这个回调
	ob2_Opertion.PreOperation = my_obPreProc; //注册pre回调
	ObRegisterCallbacks(&ob1_backreg, &gs_Handleback);
}

void 卸载进程内存保护()
{
	ObUnRegisterCallbacks(gs_Handleback);
}