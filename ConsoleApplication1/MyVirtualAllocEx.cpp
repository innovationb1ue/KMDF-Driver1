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
	DWORD dwRetSize = 0;//�����ֽ���
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
	//��������ṹ
	TBUF_INPUT inBuf = { dwPid,(UINT64)lpAddress, dwSize ,flAllocationType,flProtect };
	//����ʾ��
	//DWORD OutBuf[8] = { 0 };//���������
	PVOID lpAddress_OUT = NULL;
	DeviceIoControl(
		hDeviceHandle,//CreateFile�������豸 ���صľ��
		IO_VIRTUAL_ALLOC_MEM_EX,//������ CTL_CODE
		&inBuf,//���뻺����ָ��
		sizeof(inBuf),//���뻺������С
		&lpAddress_OUT,//���ػ�����
		sizeof(lpAddress_OUT),//���ػ�������С 8
		&dwRetSize, //�����ֽ���
		NULL);

	CloseHandle(hDeviceHandle);
	//��ӡ���ز���
	printf("yjx:EXE: yjxVirtualAllocEx ���� dwRetSize = % d, ���� lpAddress = % p", dwRetSize, lpAddress);
	return lpAddress_OUT;

}