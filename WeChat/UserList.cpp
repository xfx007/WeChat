
#include "pch.h"
#include "resource.h"
#include <Windows.h>
#include "commctrl.h"
#include <TlHelp32.h>
#include <stdlib.h>
#include <conio.h>
#include <locale.h>
#include <map>
#include <iostream>
#include "Common.h"

HWND hDlgList = 0;
DWORD hookData = 0;
DWORD WinAddList = 0;
DWORD retCallAddList = 0;
DWORD retAddList = 0;
HWND gHwndList = 0;
wchar_t oldWxid[0x100] = { 0 };
wchar_t newWxid[0x100] = { 0 };
//显示好友列表
VOID insertUserLists(DWORD userData)
{
	/*
	eax + 0x10 wxid 群
	eax + 0x30 wxid 群
	eax + 0x44 微信号
	eax + 0x58 V1数据
	eax + 0x8C 昵称
	eax + 0x11C 小头像
	eax + 0x130 大头像
	eax + 0x144 未知md5数据
	eax + 0x1C8 国籍
	eax + 0x1DC 省份
	eax + 0x1F0 城市
	eax + 0x204 添加来源
	eax + 0x294 朋友圈壁纸
	*/

	DWORD wxidAdd = userData + 0x8;
	DWORD wxuserIDAdd = userData + 0x1C;
	DWORD wxidV1Add = userData + 0x30;
	DWORD wxNickAdd = userData + 0x64;
	DWORD headPicAdd = userData + 0xF4;
	//DWORD wxNickAdd = userData + 0x8C;

	wchar_t wxid[0x100] = { 0 };
	swprintf_s(wxid, L"%s", *((LPVOID*)wxidAdd));

	wchar_t nick[0x100] = { 0 };
	swprintf_s(nick, L"%s", *((LPVOID*)wxNickAdd));

	wchar_t wxuserID[0x100] = { 0 };
	swprintf_s(wxuserID, L"%s", *((LPVOID*)wxuserIDAdd));

	wchar_t headPic[0x100] = { 0 };
	if ((LPVOID*)headPicAdd != 0x0) {
		swprintf_s(headPic, L"%s", *((LPVOID*)headPicAdd));
	}
	if (oldWxid[0] == 0 && newWxid[0] == 0) {
		swprintf_s(newWxid, L"%s", *((LPVOID*)wxidAdd));
	}

	if (oldWxid[0] == 0 && newWxid[0] != 0) {
		swprintf_s(oldWxid, L"%s", newWxid);
		swprintf_s(newWxid, L"%s", *((LPVOID*)wxidAdd));
	}

	if (oldWxid[0] != 0 && newWxid[0] != 0) {
		swprintf_s(oldWxid, L"%s", newWxid);
		swprintf_s(newWxid, L"%s", *((LPVOID*)wxidAdd));
	}

	if (wcscmp(oldWxid, newWxid) != 0) {

		LVITEM item = { 0 };
		item.mask = LVIF_TEXT;

		item.iSubItem = 0;
		item.pszText = UnicodeToANSI(wxid);
		ListView_InsertItem(gHwndList, &item);

		item.iSubItem = 1;
		item.pszText = UnicodeToANSI(wxuserID);
		ListView_SetItem(gHwndList, &item);

		item.iSubItem = 2;
		item.pszText = UnicodeToANSI(nick);
		ListView_SetItem(gHwndList, &item);
	}
}


DWORD cEaxList = 0;
DWORD cEcxList = 0;
DWORD cEdxList = 0;
DWORD cEbxList = 0;
DWORD cEspList = 0;
DWORD cEbpList = 0;
DWORD cEsiList = 0;
DWORD cEdiList = 0;
DWORD userData = 0;
//跳转过来的函数 我们自己的
/*
5BBB989D  |.  E8 5EE8FFFF   call WeChatWi.5BBB8100   418100                                 ;  这里也可以获取到数
5BBB98A2  |.  84C0          test al,al                                                ;  edi是数据
51280
*/
VOID __declspec(naked) HookList()
{
	//pushad: 将所有的32位通用寄存器压入堆栈
	//pushfd:然后将32位标志寄存器EFLAGS压入堆栈
	//popad:将所有的32位通用寄存器取出堆栈
	//popfd:将32位标志寄存器EFLAGS取出堆栈
	//先保存寄存器
	// 使用pushad这些来搞还是不太稳定  还是用变量把寄存器的值保存下来 这样可靠点

	/*
	mov cEax, eax
		mov cEcx, ecx
		mov cEdx, edx
		mov cEbx, ebx
		mov cEsp, esp
		mov cEbp, ebp
		mov cEsi, esi
		mov cEdi, edi
	*/
	__asm {
		call retCallAddList
		mov cEaxList, eax
		mov cEcxList, ecx
		mov cEdxList, edx
		mov cEbxList, ebx
		mov cEspList, esp
		mov cEbpList, ebp
		mov cEsiList, esi
		mov cEdiList, edi
	}
	//然后跳转到我们自己的处理函数 想干嘛干嘛
	insertUserLists(cEaxList);
	//然后在还原他进来之前的所有数据
	/*popad
		popfd  不太可靠恢复不全 所以才有变量的方式保存下来再赋值过去*/
		/*
		mov eax, cEax
			mov ecx, cEcx
			mov edx, cEdx
			mov ebx, cEbx
			mov esp, cEsp
			mov ebp, cEbp
			mov esi, cEsi
			mov edi, cEdi
		*/
	__asm {
		mov eax, cEaxList
		mov ecx, cEcxList
		mov edx, cEdxList
		mov ebx, cEbxList
		mov esp, cEspList
		mov ebp, cEbpList
		mov esi, cEsiList
		mov edi, cEdiList
		jmp retAddList
	}
}

//41989D
//418154
VOID HookWechatUserList(HWND hwndDlg, HWND hwndList, DWORD HookAdd)
{
	gHwndList = hwndList;
	WinAddList = getWechatWin();
	hDlgList = hwndDlg;
	retCallAddList = WinAddList + 0x62520;
	retAddList = WinAddList + 0x50B53D;
	StartHook(WinAddList + HookAdd, &HookList);
}