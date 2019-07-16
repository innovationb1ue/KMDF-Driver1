#include <Windows.h>
#include <stdio.h>
#include "DeviceIo.h"

HANDLE GetDriverHandle()
{
	HANDLE DeviceHandle = CreateFileW(
		L"\\??\\MyDriver",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	return DeviceHandle;
}

int ReadProcessMemoryForPid(DWORD dwPid, PVOID pBase/*目标地址*/, PVOID lpBuf/*缓冲区*/, DWORD nSize)
{
	// TODO: 
	// 获取自身Handle
	HANDLE  hDeviceHandle = GetDriverHandle();
	DWORD dwRetSize = 0;//返回字节数
// 修改对齐方式
#pragma pack(push)
#pragma pack(1)
	// 定义传递到R0层的参数结构体
	typedef struct TINPUT_BUF
	{
		DWORD dwPid;
		PVOID pBase; //目标进程地址
		DWORD nSize;//缓冲区大小

	}TINPUT_BUF;
#pragma pack(pop)
	//结构
	TINPUT_BUF inBuf = { dwPid,pBase,nSize };
	//数组示例
	//DWORD OutBuf[8] = { 0 };//输出缓冲区
	OutputDebugStringA("即将调用DeviceIoControl");
	DeviceIoControl(
		hDeviceHandle,	//CreateFile打开驱动设备 返回的句柄
		IO_READ_MEMORY,	//控制码 CTL_CODE
		&inBuf,			//输入缓冲区指针
		sizeof(inBuf),	//输入缓冲区大小
		lpBuf,			//返回缓冲区(传入)
		nSize,			//返回缓冲区大小(传入)
		&dwRetSize,		//返回字节数 传0
		NULL);			
	// CLose自身handle
	CloseHandle(hDeviceHandle);
	//打印返回参数
	printf("yjx:EXE: dwRetSize = % d, lpBuf = % x", dwRetSize, lpBuf);
	OutputDebugStringW(L"ReadProcessMemoryFromPid 返回");
	return dwRetSize;

}



int ReadProcessMemoryForPid2(DWORD dwPid, PVOID pBase/*目标地址*/, PVOID lpBuf/*缓冲区*/, DWORD nSize)
{
	// TODO: 
	HANDLE  hDeviceHandle = GetDriverHandle();
	DWORD dwRetSize = 0;//返回字节数
#pragma pack(push)
#pragma pack(1)
	typedef struct TINPUT_BUF
	{
		DWORD dwPid;
		PVOID pBase; //目标进程地址
		DWORD nSize;//缓冲区大小

	}TINPUT_BUF;
#pragma pack(pop)
	//结构
	TINPUT_BUF inBuf = { dwPid,pBase,nSize };
	//数组示例
	//DWORD OutBuf[8] = { 0 };//输出缓冲区
	DeviceIoControl(
		hDeviceHandle,//CreateFile打开驱动设备 返回的句柄
		IO_READ_MEMORY2,//控制码 CTL_CODE
		&inBuf,//输入缓冲区指针
		sizeof(inBuf),//输入缓冲区大小
		lpBuf,//返回缓冲区
		nSize,//返回缓冲区大小
		&dwRetSize, //返回字节数
		NULL);

	CloseHandle(hDeviceHandle);
	//打印返回参数
	printf("yjx:EXE: dwRetSize = % d, lpBuf = % p", dwRetSize, lpBuf);
	OutputDebugStringW(L"ReadProcessMemoryFromPid2 返回");
	return dwRetSize;

}