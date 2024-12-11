// Copyright (c) 2024 David A. Frischknecht
//
// SPDX-License-Identifier: Apache-2.0

#include "MainWindow.h"
#include "resources.h"
#include "SevenSegmentDisplay.h"

namespace
{
	WNDCLASS g_mainWindowClass;
	constexpr auto g_mainWindowClassName = L"MainWindow";
	bool g_classRegistered;
	HWND g_btnNewGame;
	HWND g_ssdMinesLeft;
	HWND g_ssdElapsedTime;
	HBITMAP g_bmpSmile1;
	HBITMAP g_bmpSmile2;
	HBITMAP g_bmpSmile3;
	HBITMAP g_bmpSmile4;

	LRESULT CALLBACK MainWindowProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
	{
		switch (uMsg)
		{
			case WM_CREATE:
			{
				g_btnNewGame = CreateWindowEx(0, L"BUTTON", nullptr, WS_CHILD | WS_VISIBLE | BS_ICON,
					CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwnd,
					nullptr, g_mainWindowClass.hInstance, nullptr);
				auto hIcon = LoadIcon(g_mainWindowClass.hInstance, MAKEINTRESOURCE(IDI_SMILE1));
				ICONINFOEX iconInfo{};
				iconInfo.cbSize = sizeof(ICONINFOEX);
				GetIconInfoEx(hIcon, &iconInfo);
				g_bmpSmile1 = iconInfo.hbmColor;

				SendMessage(g_btnNewGame, BM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(hIcon));

				hIcon = LoadIcon(g_mainWindowClass.hInstance, MAKEINTRESOURCE(IDI_SMILE2));
				GetIconInfoEx(hIcon, &iconInfo);
				g_bmpSmile2 = iconInfo.hbmColor;

				hIcon = LoadIcon(g_mainWindowClass.hInstance, MAKEINTRESOURCE(IDI_SMILE3));
				GetIconInfoEx(hIcon, &iconInfo);
				g_bmpSmile3 = iconInfo.hbmColor;

				hIcon = LoadIcon(g_mainWindowClass.hInstance, MAKEINTRESOURCE(IDI_SMILE4));
				GetIconInfoEx(hIcon, &iconInfo);
				g_bmpSmile4 = iconInfo.hbmColor;

				BITMAP bmp{};
				GetObject(g_bmpSmile1, sizeof(bmp), &bmp);

				SetWindowPos(g_btnNewGame, nullptr, 0, 0, bmp.bmWidth + 12, bmp.bmHeight + 12, SWP_NOZORDER | SWP_NOMOVE);
				RECT rcBtnNewGame{};
				GetWindowRect(g_btnNewGame, &rcBtnNewGame);
				OffsetRect(&rcBtnNewGame, -rcBtnNewGame.left, -rcBtnNewGame.top);

				g_ssdMinesLeft = SevenSegmentDisplay_Create(g_mainWindowClass.hInstance, hwnd);
				RECT rcSsdMinesLeft{};
				GetWindowRect(g_ssdMinesLeft, &rcSsdMinesLeft);
				OffsetRect(&rcSsdMinesLeft, -rcSsdMinesLeft.left, -rcSsdMinesLeft.top);

				g_ssdElapsedTime = SevenSegmentDisplay_Create(g_mainWindowClass.hInstance, hwnd);
				RECT rcSsdElapsedTime{};
				GetWindowRect(g_ssdElapsedTime, &rcSsdElapsedTime);
				OffsetRect(&rcSsdElapsedTime, -rcSsdElapsedTime.left, -rcSsdElapsedTime.top);

				RECT rcDesiredClientSize{};
				rcDesiredClientSize.right = 12 + rcSsdMinesLeft.right + 5 + rcBtnNewGame.right + 5 + rcSsdElapsedTime.right + 12;
				rcDesiredClientSize.bottom = 5 + max(rcSsdMinesLeft.bottom, rcBtnNewGame.bottom) + 12;
				AdjustWindowRect(&rcDesiredClientSize, WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX), true);
				OffsetRect(&rcDesiredClientSize, -rcDesiredClientSize.left, -rcDesiredClientSize.top);

				RECT rcWorkingArea{};
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkingArea, 0);
				OffsetRect(&rcWorkingArea, -rcWorkingArea.left, -rcWorkingArea.top);

				const auto x = rcWorkingArea.right / 2 - rcDesiredClientSize.right / 2;
				const auto y = rcWorkingArea.bottom / 2 - rcDesiredClientSize.bottom / 2;
				SetWindowPos(hwnd, nullptr, x, y, rcDesiredClientSize.right, rcDesiredClientSize.bottom, SWP_NOZORDER);

				return 0;
			}

			case WM_SIZE:
			{
				const auto clientWidth = LOWORD(lParam);

				RECT rcSsdMinesLeft{};
				RECT rcSsdElapsedTime{};
				GetWindowRect(g_ssdMinesLeft, &rcSsdMinesLeft);
				GetWindowRect(g_ssdElapsedTime, &rcSsdElapsedTime);
				OffsetRect(&rcSsdMinesLeft, -rcSsdMinesLeft.left, -rcSsdMinesLeft.top);
				OffsetRect(&rcSsdElapsedTime, -rcSsdElapsedTime.left, -rcSsdElapsedTime.top);

				SetWindowPos(g_ssdMinesLeft, nullptr, 12, 5, 0, 0, SWP_NOSIZE);
				SetWindowPos(g_ssdElapsedTime, nullptr, clientWidth - rcSsdElapsedTime.right - 12, 5, 0, 0, SWP_NOSIZE);

				RECT rcBtnNewGame{};
				GetWindowRect(g_btnNewGame, &rcBtnNewGame);
				OffsetRect(&rcBtnNewGame, -rcBtnNewGame.left, -rcBtnNewGame.top);
				SetWindowPos(g_btnNewGame, nullptr, clientWidth / 2 - rcBtnNewGame.right / 2, 5 + (rcSsdMinesLeft.bottom / 2 - rcBtnNewGame.bottom / 2),
					0, 0, SWP_NOZORDER | SWP_NOSIZE);

				return 0;
			}

			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}

			case WM_COMMAND:
			{
				if (lParam == 0) // message came from a menu item or accelerator key
				{
					switch (const auto menuId = LOWORD(wParam))
					{
						case IDM_GAME_EXIT:
						{
							DestroyWindow(hwnd);
							break;
						}

						default:
							break;
					}
				}
				else // message came from a control.
				{
				}

				return 0;
			}

			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				const auto dc = BeginPaint(hwnd, &ps);

				FillRect(dc, &ps.rcPaint, reinterpret_cast<HBRUSH>(COLOR_3DFACE + 1));  // NOLINT(performance-no-int-to-ptr)

				EndPaint(hwnd, &ps);

				return 0;
			}

			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

HWND MainWindow_Create(const HINSTANCE appInstance)
{
	if (!g_classRegistered)
	{
		g_mainWindowClass.lpszClassName = g_mainWindowClassName;
		g_mainWindowClass.lpfnWndProc = MainWindowProc;
		g_mainWindowClass.hInstance = appInstance;
		g_mainWindowClass.hIcon = LoadIcon(appInstance, MAKEINTRESOURCE(IDI_APP_ICON));

		RegisterClass(&g_mainWindowClass);
		g_classRegistered = true;
	}

	const auto hwnd = CreateWindowEx(0, g_mainWindowClassName, L"Minesweeper",
		WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX), CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, nullptr, LoadMenu(appInstance, MAKEINTRESOURCE(IDR_MENU)), appInstance, nullptr);

	return hwnd;
}
