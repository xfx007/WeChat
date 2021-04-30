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
#include "Function.h"

#pragma comment(lib,"comctl32.lib")
INT_PTR CALLBACK DialogProcMin(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);
INT_PTR CALLBACK DialogProc(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);
void CheckIsLogin(HWND hwndDlg);
void InitTabContrl(HWND Tab);
void InitListContrl(HWND List);
void InitLogDataContrl(HWND LogData);
BOOL isDll = false;
BOOL isMain = false;
HWND gUserListView = NULL;
HWND gLogListView = NULL;
HMODULE hModuleDou = NULL;
VOID ShowUI(HMODULE hModule);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		hModuleDou = hModule;
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
	
	DialogBox(hModule, MAKEINTRESOURCE(IDD_DIALOG1), NULL, &DialogProc);
}


INT_PTR CALLBACK DialogProc(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	HWND hListView = NULL;
	HWND hTabView = NULL;
	HWND hLogDataView = NULL;
	switch (uMsg)
	{
		
	case WM_INITDIALOG:
		if (isMain) {
			hListView = GetDlgItem(hwndDlg, USER_LISTS);
			hTabView = GetDlgItem(hwndDlg, TAB);
			hLogDataView = GetDlgItem(hwndDlg, LOGDATA);
			gUserListView = hListView;//通讯录列表
			gLogListView = hLogDataView;//聊天记录列表
			InitTabContrl(hTabView);
			InitListContrl(hListView);
			InitLogDataContrl(hLogDataView);
			//在获取二维码
			HookWechatUserList(hwndDlg, gUserListView, 0x50B538);//通讯录
			HookWechatMsg(hwndDlg, gLogListView, getWechatWin() + 0x3CCB65);//微信消息
		}
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;
	case WM_NOTIFY:
	{
		int i = SendMessage(GetDlgItem(hwndDlg, TAB), TCM_GETCURSEL, 0, 0);
		//第一个标签被点击时
		if (i == 0)
		{
			ShowWindow(GetDlgItem(hwndDlg, USER_LISTS), true);
			ShowWindow(GetDlgItem(hwndDlg, LOGDATA), false);
		}
		else if (i == 1) {
			ShowWindow(GetDlgItem(hwndDlg, USER_LISTS), false);
			ShowWindow(GetDlgItem(hwndDlg, LOGDATA), true);
		}
		else if (i == 2) {
		}
	}
	break;
	case WM_COMMAND:
		if (wParam == BUT_LOGIN && !isDll) {
			//开始登陆HOOK
			isDll = true;
			HookWechatQrcode(hwndDlg, getWechatWin() + 0x25A726);
			MessageBox(NULL, L"注入成功", L"提示", MB_OK);
			/*DWORD dwIsLogin = dwIsLogin = getWechatWin() + 0x18A2990;
			if (*(DWORD*)dwIsLogin != 0) {
			}*/

			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CheckIsLogin,hwndDlg, NULL, 0);
		}
		if(wParam == SEND_BUT){//发送信息
			SendMsg(hwndDlg, gUserListView);
		}
	break;
	case LVN_KEYDOWN:
	MessageBox(NULL, L"LVN_KEYDOWN", L"提示", MB_OK);
	break;
	default:
		break;
	}
	return FALSE;
}

INT_PTR CALLBACK DialogProcMin(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
)
{
	HWND hListView = NULL;
	HWND hTabView = NULL;
	HWND hLogDataView = NULL;
	switch (uMsg)
	{
	case WM_INITDIALOG:
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;
	case WM_NOTIFY:
	{
	}
	break;
	case WM_COMMAND:
		if (wParam == BUT_LOGIN && !isDll) {
			isDll = true;
			//在获取二维码
			HookWechatQrcode(hwndDlg, getWechatWin() + 0x25A726);
			HookWechatUserList(hwndDlg, gUserListView, 0x50B538);//通讯录
			HookWechatMsg(hwndDlg, gLogListView, getWechatWin() + 0x3CCB65);//微信消息
			EndDialog(hwndDlg, 0);
			DialogBox(hModuleDou, MAKEINTRESOURCE(MAIN), NULL, &DialogProc);
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

	// TODO:  在此添加额外的初始化
	TCHAR titleBuffer[3][5] = { L"wxid",L"微信账号" ,L"微信昵称" };
	pcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	pcol.fmt = LVCFMT_LEFT;
	pcol.cx = 120;
	for (int i = 0; i < 3; i++) {
		//创建list成员信息
		pcol.pszText = titleBuffer[i];
		ListView_InsertColumn(List, i, &pcol);
	}
}

void InitTabContrl(HWND Tab)
{
	TCHAR titleBuffer[3][5] = { L"好友列表",L"聊天记录" , L"功能大全" };
	TCITEM tci = { 0 };
	tci.mask = TCIF_TEXT;
	for (int i = 0; i < 3; i++) {
		tci.pszText = titleBuffer[i];
		TabCtrl_InsertItem(Tab, i, &tci);;
	}
}
void InitLogDataContrl(HWND LogData)
{
	TCHAR titleBuffer[6][5] = { L"消息类型", L"wxid",L"微信账号" , L"微信昵称",  L"内容", L"时间"};
	LVCOLUMN pcol = { 0 };
	pcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	pcol.fmt = LVCFMT_LEFT;
	pcol.cx = 120;
	for (int i = 0; i < 6; i++) {
		pcol.pszText = titleBuffer[i];
		ListView_InsertColumn(LogData, i, &pcol);
	}
}
//************************************************************
// 函数名称: CheckIsLogin
// 函数说明: 检查微信是否登陆
// 作    者: GuiShou
// 时    间: 2019/6/30
// 参    数: void
// 返 回 值: void
//************************************************************
void CheckIsLogin(HWND hwndDlg)
{
	//获取WeChatWin的基址
	DWORD  dwWeChatWinAddr = getWechatWin();
	while (true)
	{
		if (isMain) {
			break;
		}
		DWORD dwIsLogin = dwWeChatWinAddr + 0x18A2990;
		if (*(DWORD*)dwIsLogin != 0 && !isMain) {
			isMain = true;
			EndDialog(hwndDlg, 0);
			DialogBox(hModuleDou, MAKEINTRESOURCE(MAIN), NULL, &DialogProc);
		}
	}
}