// Copyright (c) 2024 David A. Frischknecht
//
// SPDX-License-Identifier: Apache-2.0

#include "SevenSegmentDisplay.h"
#include <unordered_map>
#include "ColorUtil.h"
#include <string>
#include <vector>
#include <gdiplus.h>

#define SEGMENT_LIST TOP,TOP_LEFT,TOP_RIGHT,MIDDLE,BOTTOM_LEFT,BOTTOM_RIGHT,BOTTOM
enum Segment : uint8_t { SEGMENT_LIST };

struct SevenSegmentDisplayProperties
{
	uint8_t value{ 0 };
	SIZE digitSize{ 20, 40 };
	uint8_t segmentThickness{ 3 };
	uint8_t digitSpacing{ 5 };
	uint8_t digitCount{ 3 };
	bool leadingZerosVisible{ false };
	COLORREF background{ RGB(0, 0, 0) };
	COLORREF foreground{ RGB(255, 255, 0) };
};

namespace
{
	WNDCLASS g_sevenSegmentDisplayClass;
	constexpr auto g_sevenSegmentDisplayClassName = L"SevenSegmentDisplay";
	bool g_classRegistered;
	std::unordered_map<HWND, SevenSegmentDisplayProperties> g_propertiesMap;
	std::unordered_map<wchar_t, std::vector<Segment>> g_digitSegmentsMap = {
		{' ', {}},
		{'0', {TOP, TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, BOTTOM}},
		{'1', {TOP_RIGHT, BOTTOM_RIGHT}},
		{'2', {TOP, TOP_RIGHT, MIDDLE, BOTTOM_LEFT, BOTTOM}},
		{'3', {TOP, TOP_RIGHT, MIDDLE, BOTTOM_RIGHT, BOTTOM}},
		{'4', {TOP_LEFT, TOP_RIGHT, MIDDLE, BOTTOM_RIGHT}},
		{'5', {TOP, TOP_LEFT, MIDDLE, BOTTOM_RIGHT, BOTTOM}},
		{'6', {TOP, TOP_LEFT, MIDDLE, BOTTOM_LEFT, BOTTOM_RIGHT, BOTTOM}},
		{'7', {TOP, TOP_RIGHT, BOTTOM_RIGHT}},
		{'8', {SEGMENT_LIST}},
		{'9', {TOP, TOP_LEFT, TOP_RIGHT, MIDDLE, BOTTOM_RIGHT, BOTTOM}},
	};

	LRESULT CALLBACK SevenSegmentDisplayProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
	{
		switch (uMsg)
		{
			case WM_CREATE:
			{
				constexpr SevenSegmentDisplayProperties properties;
				g_propertiesMap[hwnd] = properties;

				constexpr auto clientWidth = properties.digitSpacing + (properties.digitSize.cx + properties.digitSpacing) * properties.digitCount + 1;
				constexpr auto clientHeight = properties.digitSpacing * 2 + properties.digitSize.cy + 1;

				SetWindowPos(hwnd, nullptr, 0, 0, clientWidth, clientHeight, SWP_NOMOVE);

				return 0;
			}

			case WM_DESTROY:
			{
				g_propertiesMap.erase(hwnd);

				return 0;
			}

			case WM_PAINT:
			{
				const auto& [value, digitSize, segmentThickness, digitSpacing, digitCount, leadingZerosVisible, background, foreground] = g_propertiesMap[hwnd];
				PAINTSTRUCT ps;
				const auto dc = BeginPaint(hwnd, &ps);
				const auto memDC = CreateCompatibleDC(dc);
				const auto memBmp = CreateCompatibleBitmap(dc, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
				const auto oldBmp = static_cast<HBITMAP>(SelectObject(memDC, memBmp));

				Gdiplus::Graphics graphics(memDC);

				const Gdiplus::Color backColor(GetRValue(background), GetGValue(background), GetBValue(background));
				const auto litColor = foreground;
				const auto unlitColor = ChangeColorLightness(foreground, 25);

				graphics.Clear(backColor);

				const auto halfHeight = digitSize.cy / 2;
				const auto [digitWidth, digitHeight] = digitSize;

				auto valueString = std::to_wstring(value);
				if (valueString.length() > static_cast<size_t>(digitCount))
				{
					valueString = valueString.substr(valueString.length() - digitCount);
				}
				else if (valueString.length() < static_cast<size_t>(digitCount))
				{
					valueString.insert(0, digitCount - valueString.length(), leadingZerosVisible ? '0' : ' ');
				}

				for (auto i = 0; i < digitCount; i++)
				{
					const auto digit = valueString[i];
					const auto digitOriginX = digitSpacing + (digitWidth + digitSpacing) * i;
					const auto digitOriginY = digitSpacing;
					const auto digitSegments = g_digitSegmentsMap[digit];

					for (const auto segment : { SEGMENT_LIST })
					{
						std::vector<Gdiplus::Point> points;

						switch (segment)
						{
							case TOP:
							{
								points.emplace_back(digitOriginX + 1, digitOriginY);
								points.emplace_back(digitOriginX + digitWidth - 1, digitOriginY);
								points.emplace_back(digitOriginX + digitWidth - segmentThickness - 1, digitOriginY + segmentThickness);
								points.emplace_back(digitOriginX + segmentThickness + 1, digitOriginY + segmentThickness);

								break;
							}

							case TOP_LEFT:
							{
								points.emplace_back(digitOriginX, digitOriginY + 1);
								points.emplace_back(digitOriginX, digitOriginY + halfHeight - 1);
								points.emplace_back(digitOriginX + segmentThickness, digitOriginY + halfHeight - segmentThickness - 1);
								points.emplace_back(digitOriginX + segmentThickness, digitOriginY + segmentThickness + 1);

								break;
							}

							case TOP_RIGHT:
							{
								points.emplace_back(digitOriginX + digitWidth, digitOriginY + 1);
								points.emplace_back(digitOriginX + digitWidth, digitOriginY + halfHeight - 1);
								points.emplace_back(digitOriginX + digitWidth - segmentThickness, digitOriginY + halfHeight - segmentThickness - 1);
								points.emplace_back(digitOriginX + digitWidth - segmentThickness, digitOriginY + segmentThickness + 1);

								break;
							}

							case MIDDLE:
							{
								points.emplace_back(digitOriginX + 1, digitOriginY + halfHeight);
								points.emplace_back(digitOriginX + segmentThickness + 1, digitOriginY + halfHeight - segmentThickness + 1);
								points.emplace_back(digitOriginX + digitWidth - segmentThickness - 1, digitOriginY + halfHeight - segmentThickness + 1);
								points.emplace_back(digitOriginX + digitWidth - 1, digitOriginY + halfHeight);
								points.emplace_back(digitOriginX + digitWidth - segmentThickness - 1, digitOriginY + halfHeight + segmentThickness - 1);
								points.emplace_back(digitOriginX + segmentThickness + 1, digitOriginY + halfHeight + segmentThickness - 1);

								break;
							}

							case BOTTOM_LEFT:
							{
								points.emplace_back(digitOriginX, digitOriginY + halfHeight + 1);
								points.emplace_back(digitOriginX, digitOriginY + digitHeight - 1);
								points.emplace_back(digitOriginX + segmentThickness, digitOriginY + digitHeight - segmentThickness - 1);
								points.emplace_back(digitOriginX + segmentThickness, digitOriginY + halfHeight + segmentThickness + 1);

								break;
							}

							case BOTTOM_RIGHT:
							{
								points.emplace_back(digitOriginX + digitWidth, digitOriginY + halfHeight + 1);
								points.emplace_back(digitOriginX + digitWidth, digitOriginY + digitHeight - 1);
								points.emplace_back(digitOriginX + digitWidth - segmentThickness, digitOriginY + digitHeight - segmentThickness - 1);
								points.emplace_back(digitOriginX + digitWidth - segmentThickness, digitOriginY + halfHeight + segmentThickness + 1);

								break;
							}

							case BOTTOM:
							{
								points.emplace_back(digitOriginX + 1, digitOriginY + digitHeight);
								points.emplace_back(digitOriginX + digitWidth - 1, digitOriginY + digitHeight);
								points.emplace_back(digitOriginX + digitWidth - segmentThickness - 1, digitOriginY + digitHeight - segmentThickness);
								points.emplace_back(digitOriginX + segmentThickness + 1, digitOriginY + digitHeight - segmentThickness);

								break;
							}
						}

						Gdiplus::Color color;

						if (std::ranges::find(digitSegments.begin(), digitSegments.end(), segment) != digitSegments.end())
						{
							color.SetFromCOLORREF(litColor);
						}
						else
						{
							color.SetFromCOLORREF(unlitColor);
						}

						Gdiplus::Pen pen(color);
						Gdiplus::SolidBrush brush(color);
						graphics.DrawPolygon(&pen, points.data(), static_cast<int>(points.size()));
						graphics.FillPolygon(&brush, points.data(), static_cast<int>(points.size()));
					}
				}

				BitBlt(dc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, memDC, 0, 0, SRCCOPY);
				SelectObject(memDC, oldBmp);
				DeleteObject(memBmp);
				DeleteObject(memDC);

				EndPaint(hwnd, &ps);

				return 0;
			}

			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
}

HWND SevenSegmentDisplay_Create(const HINSTANCE appInstance, const HWND parent)
{
	if (!g_classRegistered)
	{
		g_sevenSegmentDisplayClass.lpfnWndProc = SevenSegmentDisplayProc;
		g_sevenSegmentDisplayClass.lpszClassName = g_sevenSegmentDisplayClassName;
		g_sevenSegmentDisplayClass.hInstance = appInstance;
		g_sevenSegmentDisplayClass.hbrBackground = nullptr;

		RegisterClass(&g_sevenSegmentDisplayClass);
		g_classRegistered = true;
	}

	const auto hwnd = CreateWindowEx(0, g_sevenSegmentDisplayClassName, nullptr, WS_CHILD | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parent, nullptr, appInstance, nullptr);

	return hwnd;
}
