#pragma once
#include "pch.h"
#include <iostream>
#include <string.h>
using namespace std;
#define HOOK_LEN 5

BYTE backCode[HOOK_LEN] = { 0 };
//获取模块基址
DWORD getWechatWin()
{
	return (DWORD)LoadLibrary(L"WeChatWin.dll");
}


//************************************************************
// 函数名称: GetMsgByAddress
// 函数说明: 从地址中获取消息内容
// 作    者: GuiShou
// 时    间: 2019/7/6
// 参    数: DWORD memAddress  目标地址
// 返 回 值: LPCWSTR 消息内容
//************************************************************
std::wstring GetMsgByAddress(DWORD memAddress)
{
	wstring tmp;
	DWORD msgLength = *(DWORD*)(memAddress + 4);
	if (msgLength > 0) {
		WCHAR* msg = new WCHAR[msgLength + 1]{ 0 };
		wmemcpy_s(msg, msgLength + 1, (WCHAR*)(*(DWORD*)memAddress), msgLength + 1);
		tmp = msg;
		delete[]msg;
	}
	return  tmp;
}
//开始hook
/**
 * 参数一 hookAdd 想要hook的地址
 * 参数二 jmpAdd hook完回去的地址
 * 参数三 oldBye 备份被hook地址的那段二进制数据 用于恢复hook
**/
VOID StartHook(DWORD hookAdd, LPVOID jmpAdd)
{
	BYTE JmpCode[HOOK_LEN] = { 0 };
	//我们需要组成一段这样的数据
	// E9 11051111(这里是跳转的地方这个地方不是一个代码地址 而是根据hook地址和跳转的代码地址的距离计算出来的)
	JmpCode[0] = 0xE9;
	//计算跳转的距离公式是固定的
	//计算公式为 跳转的地址(也就是我们函数的地址) - hook的地址 - hook的字节长度
	*(DWORD*)&JmpCode[1] = (DWORD)jmpAdd - hookAdd - HOOK_LEN;

	//hook第二步 先备份将要被我们覆盖地址的数据 长度为我们hook的长度 HOOK_LEN 5个字节

	//获取进程句柄
	HANDLE hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());

	//备份数据
	if (ReadProcessMemory(hWHND, (LPVOID)hookAdd, backCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook地址的数据读取失败", L"读取失败", MB_OK);
		return;
	}

	//真正的hook开始了 把我们要替换的函数地址写进去 让他直接跳到我们函数里面去然后我们处理完毕后再放行吧！
	if (WriteProcessMemory(hWHND, (LPVOID)hookAdd, JmpCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook写入失败，函数替换失败", L"错误", MB_OK);
		return;
	}

}

//卸载hook
VOID UnHook(DWORD HookAdd)
{
	HANDLE hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
	if (WriteProcessMemory(hWHND, (LPVOID)HookAdd, backCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook卸载失败", L"失败", MB_OK);
		return;
	}
}


CHAR* UnicodeToUTF8(const WCHAR* wideStr)
{
	char* utf8Str = NULL;
	int charLen = -1;
	charLen = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
	utf8Str = (char*)malloc(charLen);
	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, charLen, NULL, NULL);
	return utf8Str;
}

wchar_t* UTF8ToUnicode(const char* str)
{
	int    textlen = 0;
	wchar_t* result;
	textlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	result = (wchar_t*)malloc((textlen + 1) * sizeof(wchar_t));
	memset(result, 0, (textlen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)result, textlen);
	return    result;
}
char* UnicodeToANSI(const wchar_t* str)
{
	char* result;
	int textlen = 0;
	textlen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	result = (char*)malloc((textlen + 1) * sizeof(char));
	memset(result, 0, sizeof(char) * (textlen + 1));
	WideCharToMultiByte(CP_ACP, 0, str, -1, result, textlen, NULL, NULL);
	return result;
}
//wchar_t转string
std::string Wchar_tToString(wchar_t* wchar)
{
	std::string szDst;
	wchar_t* wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
	char* psText; // psText为char*的临时数组，作为赋值给std::string的中间变量
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用
	szDst = psText;// std::string赋值
	delete[]psText;// psText的清除
	return szDst;
}
