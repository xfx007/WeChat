#pragma once
#include "pch.h"
DWORD getWechatWin();
VOID StartHook(DWORD hookAdd, LPVOID jmpAdd);
VOID UnHook(DWORD HookAdd);
char* UnicodeToANSI(const wchar_t* str);
CHAR* UnicodeToUTF8(const WCHAR* wideStr);