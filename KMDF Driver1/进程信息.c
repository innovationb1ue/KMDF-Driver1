#include <ntifs.h>
#ifdef __cplusplus
extern"C" char* PsGetProcessImageFileName(PEPROCESS Process);;
#else 
char* PsGetProcessImageFileName(PEPROCESS Process);;
#endif

//通过pid 返回进程名 15个字节的 截断
char* GetProcessImageNameByProcessID(HANDLE ulProcessID)
{
	NTSTATUS Status; PEPROCESS EProcess = NULL;
	Status = PsLookupProcessByProcessId(ulProcessID, &EProcess);
	if (!NT_SUCCESS(Status))
		return NULL;

	ObDereferenceObject(EProcess);
	return (char*)PsGetProcessImageFileName(EProcess);
}