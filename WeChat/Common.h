#pragma once
#include "pch.h"
#include <string>
DWORD getWechatWin();
VOID StartHook(DWORD hookAdd, LPVOID jmpAdd);
VOID UnHook(DWORD HookAdd);
char* UnicodeToANSI(const wchar_t* str);
CHAR* UnicodeToUTF8(const WCHAR* wideStr);
std::wstring GetMsgByAddress(DWORD memAddress);	//�ӵ�ַ�л�ȡ��Ϣ����
wchar_t* UTF8ToUnicode(const char* str);
std::string Wchar_tToString(wchar_t* wchar);