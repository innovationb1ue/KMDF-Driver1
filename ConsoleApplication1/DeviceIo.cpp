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

int ReadProcessMemoryForPid(DWORD dwPid, PVOID pBase/*Ŀ���ַ*/, PVOID lpBuf/*������*/, DWORD nSize)
{
	// TODO: 
	// ��ȡ����Handle
	HANDLE  hDeviceHandle = GetDriverHandle();
	DWORD dwRetSize = 0;//�����ֽ���
// �޸Ķ��뷽ʽ
#pragma pack(push)
#pragma pack(1)
	// ���崫�ݵ�R0��Ĳ����ṹ��
	typedef struct TINPUT_BUF
	{
		DWORD dwPid;
		PVOID pBase; //Ŀ����̵�ַ
		DWORD nSize;//��������С

	}TINPUT_BUF;
#pragma pack(pop)
	//�ṹ
	TINPUT_BUF inBuf = { dwPid,pBase,nSize };
	//����ʾ��
	//DWORD OutBuf[8] = { 0 };//���������
	OutputDebugStringA("��������DeviceIoControl");
	DeviceIoControl(
		hDeviceHandle,	//CreateFile�������豸 ���صľ��
		IO_READ_MEMORY,	//������ CTL_CODE
		&inBuf,			//���뻺����ָ��
		sizeof(inBuf),	//���뻺������С
		lpBuf,			//���ػ�����(����)
		nSize,			//���ػ�������С(����)
		&dwRetSize,		//�����ֽ��� ��0
		NULL);			
	// CLose����handle
	CloseHandle(hDeviceHandle);
	//��ӡ���ز���
	printf("yjx:EXE: dwRetSize = % d, lpBuf = % x", dwRetSize, lpBuf);
	OutputDebugStringW(L"ReadProcessMemoryFromPid ����");
	return dwRetSize;

}



int ReadProcessMemoryForPid2(DWORD dwPid, PVOID pBase/*Ŀ���ַ*/, PVOID lpBuf/*������*/, DWORD nSize)
{
	// TODO: 
	HANDLE  hDeviceHandle = GetDriverHandle();
	DWORD dwRetSize = 0;//�����ֽ���
#pragma pack(push)
#pragma pack(1)
	typedef struct TINPUT_BUF
	{
		DWORD dwPid;
		PVOID pBase; //Ŀ����̵�ַ
		DWORD nSize;//��������С

	}TINPUT_BUF;
#pragma pack(pop)
	//�ṹ
	TINPUT_BUF inBuf = { dwPid,pBase,nSize };
	//����ʾ��
	//DWORD OutBuf[8] = { 0 };//���������
	DeviceIoControl(
		hDeviceHandle,//CreateFile�������豸 ���صľ��
		IO_READ_MEMORY2,//������ CTL_CODE
		&inBuf,//���뻺����ָ��
		sizeof(inBuf),//���뻺������С
		lpBuf,//���ػ�����
		nSize,//���ػ�������С
		&dwRetSize, //�����ֽ���
		NULL);

	CloseHandle(hDeviceHandle);
	//��ӡ���ز���
	printf("yjx:EXE: dwRetSize = % d, lpBuf = % p", dwRetSize, lpBuf);
	OutputDebugStringW(L"ReadProcessMemoryFromPid2 ����");
	return dwRetSize;

}