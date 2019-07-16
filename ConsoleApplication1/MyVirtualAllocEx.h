#pragma once
#include <Windows.h>
LPVOID yjxVirtualAllocEx(
	DWORD dwPid,//HANDLE hProcess,  // process within which to allocate memory
	LPVOID lpAddress, // desired starting address of allocation
	DWORD dwSize,     // size, in bytes, of region to allocate
	DWORD flAllocationType,
	// type of allocation
	DWORD flProtect   // type of access protection
); 
