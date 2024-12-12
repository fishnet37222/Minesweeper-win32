// Copyright (c) 2024 David A. Frischknecht
//
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include <Windows.h>

HWND SevenSegmentDisplay_Create(HINSTANCE appInstance, HWND parent);
void SevenSegmentDisplay_SetValue(HWND hwnd, UINT8 value);
UINT8 SevenSegmentDisplay_GetValue(HWND hwnd);
