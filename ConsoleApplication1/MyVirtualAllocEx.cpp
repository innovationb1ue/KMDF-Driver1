#include "DeviceIo.h"
#include "MyVirtualAllocEx.h"
#include <Windows.h>
#include <stdio.h>
LPVOID yjxVirtualAllocEx(
	DWORD dwPid,			//HANDLE hProcess,  // process within which to allocate memory
	LPVOID lpAddress,		// desired starting address of allocation
	DWORD dwSize,			// size, in bytes, of region to allocate
	DWORD flAllocationType,	// type of allocation
	DWORD flProtect			// type of access protection
)
{
	// TODO: 
	HANDLE  hDeviceHandle = GetDriverHandle();
	DWORD dwRetSize = 0;//返回字节数
#pragma pack(push)
#pragma pack(1)
	typedef struct TBUF_INPUT
	{
		UINT64 ProcessID;
		UINT64 BaseAddress;
		UINT64 Size;
		UINT64 AllocationType;
		UINT64 Protect;
	} TBUF_INPUT;

#pragma pack(pop)
	//输入参数结构
	TBUF_INPUT inBuf = { dwPid,(UINT64)lpAddress, dwSize ,flAllocationType,flProtect };
	//数组示例
	//DWORD OutBuf[8] = { 0 };//输出缓冲区
	PVOID lpAddress_OUT = NULL;
	DeviceIoControl(
		hDeviceHandle,//CreateFile打开驱动设备 返回的句柄
		IO_VIRTUAL_ALLOC_MEM_EX,//控制码 CTL_CODE
		&inBuf,//输入缓冲区指针
		sizeof(inBuf),//输入缓冲区大小
		&lpAddress_OUT,//返回缓冲区
		sizeof(lpAddress_OUT),//返回缓冲区大小 8
		&dwRetSize, //返回字节数
		NULL);

	CloseHandle(hDeviceHandle);
	//打印返回参数
	printf("yjx:EXE: yjxVirtualAllocEx 返回 dwRetSize = % d, 返回 lpAddress = % p", dwRetSize, lpAddress);
	return lpAddress_OUT;

}