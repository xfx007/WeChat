// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "framework.h"
#include "resource.h"
#include <Windows.h>
#include <atlimage.h>
#include "Login.h"
#include "Common.h"
#include "UserList.h"
#include "commctrl.h"
#include "RecvMessage.h"
#include <TlHelp32.h>

INT_PTR CALLBACK DialogProc(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);
void InitListContrl(HWND List);
HWND gUserListView = NULL;
VOID ShowUI(HMODULE hModule);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ShowUI, hModule,NULL,0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

VOID ShowUI(HMODULE hModule)
{
	DialogBox(hModule, MAKEINTRESOURCE(MAIN), NULL, &DialogProc);
}


INT_PTR CALLBACK DialogProc(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	HWND hListView = NULL;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		hListView = GetDlgItem(hwndDlg, USER_LISTS);
		gUserListView = hListView;
		InitListContrl(hListView);
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;
	case WM_COMMAND:
		if (wParam==BUT_LOGIN) {
			//在获取二维码
			HookWechatQrcode(hwndDlg, getWechatWin() + 0x25A726);
			HookWechatUserList(hwndDlg, gUserListView, 0x50B538);
			HookWechatMsg(hwndDlg, getWechatWin() + 0x3CCB65);
			//开始登陆HOOK
		}
		break;
	default:
		break;
	}
	return FALSE;
}


void InitListContrl(HWND List)
{
	LVCOLUMN pcol = { 0 };
	 //wchar_t titleBuffer[] = { L"wxid",L"微信账号",L"微信昵称" };
	pcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	pcol.fmt = LVCFMT_LEFT;
	pcol.cx = 120;
	for (int i = 0; i < 3; i++) {
		//创建list成员信息
		if (i == 0) {
			pcol.pszText = (wchar_t*)L"wxid";
		}
		if (i == 1) {
			pcol.pszText = (wchar_t*)L"微信账号";
		}
		if (i == 2) {
			pcol.pszText = (wchar_t*)L"微信昵称";
		}
		ListView_InsertColumn(List, i, &pcol);
	}
}