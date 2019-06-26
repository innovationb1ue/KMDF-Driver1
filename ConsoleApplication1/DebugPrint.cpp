
#include <stdio.h>
#include <Windows.h>

void DbgPrintfA(char*pszFormat,...)
{
	//www.yjxsoft.com

	char *szbufFormat=new char[0x1000];
	char *szBufFormat_Game=new char[0x1008];
	strcpy_s(szBufFormat_Game,0x1008,"yjx:");
	va_list argList;
	va_start(argList,pszFormat);//参数列表初始化

	vsprintf_s(szbufFormat,0x1000,pszFormat,argList);//printf

	strcat_s(szBufFormat_Game,0x1008,szbufFormat);
	OutputDebugStringA(szBufFormat_Game); //关键

	va_end(argList);
	delete szbufFormat;
	delete szBufFormat_Game;

}