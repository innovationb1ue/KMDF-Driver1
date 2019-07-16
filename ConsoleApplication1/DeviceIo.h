#pragma once
#include <devioctl.h>
#include <Windows.h>
#define IO_READ_MEMORY   CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x804, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测
#define IO_VIRTUAL_ALLOC_MEM_EX  CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x805, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试
#define IO_READ_MEMORY2   CTL_CODE(FILE_DEVICE_UNKNOWN, 	0x806, 	METHOD_BUFFERED,FILE_ANY_ACCESS) //控制码测试#pragma once
HANDLE GetDriverHandle();
int ReadProcessMemoryForPid(DWORD dwPid, PVOID pBase/*目标地址*/, PVOID lpBuf/*缓冲区*/, DWORD nSize);
int ReadProcessMemoryForPid2(DWORD dwPid, PVOID pBase/*目标地址*/, PVOID lpBuf/*缓冲区*/, DWORD nSize);