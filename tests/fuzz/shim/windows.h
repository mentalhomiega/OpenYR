/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The few Win32 names the parser sources use, so that the fuzz harness builds with clang on
// Linux. Nothing here talks to an operating system; the sources the harness compiles reach
// no window, file or sound API on these paths.

#pragma once

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <strings.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef std::uint32_t DWORD;
typedef std::uint32_t UINT;
typedef std::uint32_t COLORREF;
typedef long LONG;
typedef void * HANDLE;
typedef void * HWND;
typedef void * HINSTANCE;
typedef void * HDC;
typedef void * HBITMAP;
typedef void * HGDIOBJ;
typedef void * HPALETTE;
typedef std::intptr_t LRESULT;
typedef std::uintptr_t WPARAM;
typedef std::intptr_t LPARAM;
typedef void * HMODULE;
typedef void * LPVOID;
typedef char * LPSTR;
typedef char const * LPCSTR;

#define INVALID_HANDLE_VALUE ((HANDLE)(std::intptr_t)-1)

#define RGB(r, g, b) ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)))

#define CP_ACP 0
#define CP_UTF8 65001

// Code pages other than UTF-8 have no table here, so a glyph is reported as unmappable.
inline int WideCharToMultiByte(UINT, DWORD, wchar_t const *, int, char *, int, char const *, BOOL * defaulted)
{
	if (defaulted != NULL) {
		*defaulted = TRUE;
	}
	return(0);
}
