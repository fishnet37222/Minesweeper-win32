# Copyright (c) 2024 David A. Frischknecht
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.30)
project(Minesweeper)
set(CMAKE_CXX_STANDARD 20)

add_executable(Minesweeper WIN32 Minesweeper.rc resources.h Main.cpp MainWindow.h MainWindow.cpp "SevenSegmentDisplay.h" "SevenSegmentDisplay.cpp" "ColorUtil.h" "ColorUtil.cpp")
target_compile_definitions(Minesweeper PRIVATE UNICODE)
target_precompile_headers(Minesweeper PRIVATE <Windows.h>)
