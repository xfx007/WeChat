
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
//��ʾ�����б�
VOID insertUserLists(DWORD userData)
{
	/*
	eax + 0x10 wxid Ⱥ
	eax + 0x30 wxid Ⱥ
	eax + 0x44 ΢�ź�
	eax + 0x58 V1����
	eax + 0x8C �ǳ�
	eax + 0x11C Сͷ��
	eax + 0x130 ��ͷ��
	eax + 0x144 δ֪md5����
	eax + 0x1C8 ����
	eax + 0x1DC ʡ��
	eax + 0x1F0 ����
	eax + 0x204 �����Դ
	eax + 0x294 ����Ȧ��ֽ
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
//��ת�����ĺ��� �����Լ���
/*
5BBB989D  |.  E8 5EE8FFFF   call WeChatWi.5BBB8100   418100                                 ;  ����Ҳ���Ի�ȡ����
5BBB98A2  |.  84C0          test al,al                                                ;  edi������
51280
*/
VOID __declspec(naked) HookList()
{
	//pushad: �����е�32λͨ�üĴ���ѹ���ջ
	//pushfd:Ȼ��32λ��־�Ĵ���EFLAGSѹ���ջ
	//popad:�����е�32λͨ�üĴ���ȡ����ջ
	//popfd:��32λ��־�Ĵ���EFLAGSȡ����ջ
	//�ȱ���Ĵ���
	// ʹ��pushad��Щ���㻹�ǲ�̫�ȶ�  �����ñ����ѼĴ�����ֵ�������� �����ɿ���

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
	//Ȼ����ת�������Լ��Ĵ����� ��������
	insertUserLists(cEaxList);
	//Ȼ���ڻ�ԭ������֮ǰ����������
	/*popad
		popfd  ��̫�ɿ��ָ���ȫ ���Բ��б����ķ�ʽ���������ٸ�ֵ��ȥ*/
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