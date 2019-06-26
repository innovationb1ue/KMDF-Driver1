// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <iostream>
#include <winioctl.h>
#include "LoadDriver.h"
void CreateFileFunc();
void CloseHandleFunc();
void DeviceControlFunc();
HANDLE DeviceHandle = NULL;
int main()
{
	using namespace std;
	int a;
	while(cin>>a) {
		cout <<"a="<<a << endl;
		if (a == 1) {
			cout << "加载驱动LoadDriver()" << endl;
			LoadDriver("MyDriver","MyDriver.sys");
		}
		if (a == 2) {
			cout << "调用CreateFuleFunc()" << endl;
			CreateFileFunc();
		}
		if (a == 3) {
			cout << "调用DeviceControlFunc()" << endl;
			DeviceControlFunc();
		}
		if (a == 4) {
			cout << "调用CloseHandleFunc()" << endl;
			CloseHandleFunc();
		}	
		if (a == 5) {
			cout << "调用UnloadDriver()" << endl;
			UnloadDriver("MyDriver");
		}
		if (a == 0) {
			break;
		}
	}

    std::cout << "Hello World!\n";
}

void CreateFileFunc() {
	DeviceHandle = CreateFileW(L"\\??\\MyDriver1",
		GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
}
void CloseHandleFunc() {
	CloseHandle(DeviceHandle); // IRP_MJ_CLOSE
}

#define IO_IS_OK_TEST CTL_CODE(FILE_DEVICE_UNKNOWN,0x803,METHOD_BUFFERED,FILE_ANY_ACCESS)
void DeviceControlFunc() {
	DWORD dwRetSize = 0; // 返回字节数

	typedef struct TINPUT_BUF {
		DWORD m_arg1;
		DWORD m_arg2;
		DWORD m_arg3;
		DWORD m_arg4;
		DWORD m_arg5;
		DWORD m_arg6;
	}TINPUT_BUF;

	TINPUT_BUF inBuf = { 1,2,3,4,5,0x6ABC666 };	//输入参数
	DWORD OutBuf[6] = { 0 };					//输出
	DeviceIoControl(DeviceHandle,//驱动句柄
		IO_IS_OK_TEST,			// CTL_CODE
		&inBuf,			//缓冲区指针
		sizeof(inBuf),	//缓冲区大小
		&OutBuf,		//返回缓冲区
		sizeof(OutBuf),	//返回缓冲区大小
		&dwRetSize,		// 返回字节数
		NULL);

	// 接受返回参数
	printf("yjx:EXE: 接受驱动层返回参数(%x,%x,%x,%x,%x,%x)", OutBuf[0], OutBuf[1], OutBuf[2], OutBuf[3], OutBuf[4], OutBuf[5]);
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
