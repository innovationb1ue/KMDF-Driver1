#include<ntddk.h>
#include<intrin.h>
#include "SSDT.h"
typedef struct _SYSTEM_SERVICE_TABLE {
	PVOID ServiceTableBase;
	PVOID ServiceCounterTableBase;
	ULONGLONG NumberOfServices;
	PVOID ParamTableBase;
} SYSTEM_SERVICE_TABLE, * PSYSTEM_SERVICE_TABLE;
PSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;

ULONGLONG MyGetKeServiceDescriptorTable()
{
	PUCHAR StartSearchAddress = (PUCHAR)__readmsr(0xC0000082);
	PUCHAR EndSearchAddress = StartSearchAddress + 0x500;
	PUCHAR i = NULL;
	UCHAR b1 = 0, b2 = 0, b3 = 0;
	ULONG templong = 0;
	ULONGLONG addr = 0;

	for (i = StartSearchAddress; i < EndSearchAddress; i++)
	{
		if (MmIsAddressValid(i) && MmIsAddressValid(i + 1) && MmIsAddressValid(i + 1))
		{
			b1 = *i;
			b2 = *(i + 1);
			b3 = *(i + 2);
			if (b1 == 0x4c && b2 == 0x8d && b3 == 0x15) //4c8d15
			{
				memcpy(&templong, i + 3, 4);
				addr = (ULONGLONG)templong + (ULONGLONG)i + 7;
				return addr;
			}
		}
	}
	return 0;
}

ULONGLONG GetSSDTFuncCurAddr(ULONG id)
{
	LONG dwtmp = 0;
	PULONG ServiceTableBase = NULL;
	ServiceTableBase = (PULONG)KeServiceDescriptorTable->ServiceTableBase;
	dwtmp = ServiceTableBase[id];
	dwtmp = dwtmp >> 4;
	return (LONGLONG)dwtmp + (ULONGLONG)ServiceTableBase;
}