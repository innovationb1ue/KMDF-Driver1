#include <ntddk.h>


NTSTATUS DriverEntry(PDRIVER_OBJECT Driver, PUNICODE_STRING szReg)
{
	DbgPrint(("DbgPrint:Entry"));
	KdPrint(("111"));
	return 0;
}