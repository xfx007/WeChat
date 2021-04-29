
#include <Windows.h>
#include "pch.h"
#include "Common.h"
#include <atlimage.h>
#include <stdio.h>
#include "resource.h"
#include <stdlib.h>



DWORD retCallAdd = 0;
DWORD retAdd = 0;
DWORD WinAdd = 0;
HWND hDlg = 0;
DWORD cEax = 0;
DWORD cEcx = 0;
DWORD cEdx = 0;
DWORD cEbx = 0;
DWORD cEsp = 0;
DWORD cEbp = 0;
DWORD cEsi = 0;
DWORD cEdi = 0;
//实时显示图、
VOID ShowPic(DWORD picAdd)
{

	DWORD picLen = picAdd + 0x4;

	char PicData[0xFFF] = { 0 };
	size_t cpyLen = (size_t) * ((LPVOID*)picLen);

	memcpy(PicData, *((LPVOID*)picAdd), cpyLen);
	FILE* pFile;
	fopen_s(&pFile, "qrcode.png", "wb");
	fwrite(PicData, sizeof(char), sizeof(PicData), pFile);
	fclose(pFile);

	CImage img;
	CRect rect;
	HWND obj = GetDlgItem(hDlg, IMG_LOGIN);
	GetClientRect(obj, &rect);
	//载入图片
	HRESULT result = img.Load(_T("qrcode.png"));
	img.Draw(GetDC(obj), rect);
}

//跳转过来的函数 我们自己的
VOID __declspec(naked) HookF()
{
	//pushad: 将所有的32位通用寄存器压入堆栈
	//pushfd:然后将32位标志寄存器EFLAGS压入堆栈
	//popad:将所有的32位通用寄存器取出堆栈
	//popfd:将32位标志寄存器EFLAGS取出堆栈
	//先保存寄存器
	// 使用pushad这些来搞还是不太稳定  还是用变量把寄存器的值保存下来 这样可靠点
	__asm {
		mov cEax, eax
		mov cEcx, ecx
		mov cEdx, edx
		mov cEbx, ebx
		mov cEsp, esp
		mov cEbp, ebp
		mov cEsi, esi
		mov cEdi, edi
	}
	//然后跳转到我们自己的处理函数 想干嘛干嘛
	ShowPic(cEcx);
	retCallAdd = WinAdd + 0x5717B0;
	retAdd = WinAdd + 0x25A72B;
	//然后在还原他进来之前的所有数据
	/*popad
		popfd  不太可靠恢复不全 所以才有变量的方式保存下来再赋值过去*/
	__asm {
		mov eax, cEax
		mov ecx, cEcx
		mov edx, cEdx
		mov ebx, cEbx
		mov esp, cEsp
		mov ebp, cEbp
		mov esi, cEsi
		mov edi, cEdi
		call retCallAdd
		jmp retAdd
	}
}
VOID HookWechatQrcode(HWND hwndDlg, DWORD HookAdd)
{
	WinAdd = getWechatWin();
	hDlg = hwndDlg;
	StartHook(HookAdd, &HookF);
}