#include <ntifs.h>
#ifdef __cplusplus
extern"C" char* PsGetProcessImageFileName(PEPROCESS Process);;
#else 
char* PsGetProcessImageFileName(PEPROCESS Process);;
#endif

//ͨ��pid ���ؽ����� 15���ֽڵ� �ض�
char* GetProcessImageNameByProcessID(HANDLE ulProcessID)
{
	NTSTATUS Status; PEPROCESS EProcess = NULL;
	Status = PsLookupProcessByProcessId(ulProcessID, &EProcess);
	if (!NT_SUCCESS(Status))
		return NULL;

	ObDereferenceObject(EProcess);
	return (char*)PsGetProcessImageFileName(EProcess);
}