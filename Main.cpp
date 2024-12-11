// Copyright (c) 2024 David A. Frischknecht
//
// SPDX-License-Identifier: Apache-2.0

#include <Windows.h>
#include "MainWindow.h"
#include "resources.h"
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int WINAPI wWinMain(const HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, [[maybe_unused]] LPWSTR lpCmdLine, const int nShowCmd)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

	const auto hwnd = MainWindow_Create(hInstance);

	if (hwnd == nullptr)
	{
		return -1;
	}

	const auto hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

	ShowWindow(hwnd, nShowCmd);

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0) > 0)
	{
		if (!TranslateAccelerator(hwnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);

	return 0;
}
