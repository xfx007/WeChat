
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
//ʵʱ��ʾͼ��
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
	//����ͼƬ
	HRESULT result = img.Load(_T("qrcode.png"));
	img.Draw(GetDC(obj), rect);
}

//��ת�����ĺ��� �����Լ���
VOID __declspec(naked) HookF()
{
	//pushad: �����е�32λͨ�üĴ���ѹ���ջ
	//pushfd:Ȼ��32λ��־�Ĵ���EFLAGSѹ���ջ
	//popad:�����е�32λͨ�üĴ���ȡ����ջ
	//popfd:��32λ��־�Ĵ���EFLAGSȡ����ջ
	//�ȱ���Ĵ���
	// ʹ��pushad��Щ���㻹�ǲ�̫�ȶ�  �����ñ����ѼĴ�����ֵ�������� �����ɿ���
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
	//Ȼ����ת�������Լ��Ĵ����� ��������
	ShowPic(cEcx);
	retCallAdd = WinAdd + 0x5717B0;
	retAdd = WinAdd + 0x25A72B;
	//Ȼ���ڻ�ԭ������֮ǰ����������
	/*popad
		popfd  ��̫�ɿ��ָ���ȫ ���Բ��б����ķ�ʽ���������ٸ�ֵ��ȥ*/
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