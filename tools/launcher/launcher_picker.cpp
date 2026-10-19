/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "launcher_system.h"

#include "resource.h"

#include <commctrl.h>

namespace launcher {
namespace {

wchar_t const * const PickerClass = L"OpenYRLauncherModPicker";
wchar_t const * const PickerTitle = L"OpenYR";

enum ControlIdType {
	IdList = 100,
	IdDescription,
	IdAsk,
	IdPlay = IDOK,
	IdCancel = IDCANCEL,
};

struct PickerStateType
{
	ModPickerRequestType * Request = nullptr;
	HWND List = nullptr;
	HWND Description = nullptr;
	HWND Ask = nullptr;
	HFONT Font = nullptr;
	bool Done = false;
	bool Play = false;
	int Dpi = 96;
};


int Scale(PickerStateType const & state, int value)
{
	return MulDiv(value, state.Dpi, 96);
}


int Selected_Row(HWND list)
{
	return static_cast<int>(SendMessageW(list, LVM_GETNEXTITEM, static_cast<WPARAM>(-1), LVNI_SELECTED));
}


void Show_Description(PickerStateType & state)
{
	int const row = Selected_Row(state.List);
	if (row >= 0 && row < static_cast<int>(state.Request->Descriptions.size())) {
		SetWindowTextW(state.Description, state.Request->Descriptions[row].c_str());
	} else {
		SetWindowTextW(state.Description, L"");
	}
}


void Finish(PickerStateType & state, bool play)
{
	int const row = Selected_Row(state.List);
	if (play && row >= 0) {
		state.Request->Selected = row;
	}
	state.Request->DontAskAgain = SendMessageW(state.Ask, BM_GETCHECK, 0, 0) == BST_CHECKED;
	state.Play = play && row >= 0;
	state.Done = true;
}


HWND Add_Control(PickerStateType & state, HWND parent, wchar_t const * type, wchar_t const * text, DWORD style, int x, int y, int width, int height, int id)
{
	HWND const control = CreateWindowExW(0, type, text, WS_CHILD | WS_VISIBLE | style,
		Scale(state, x), Scale(state, y), Scale(state, width), Scale(state, height),
		parent, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), GetModuleHandleW(nullptr), nullptr);
	if (control != nullptr) {
		SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(state.Font), TRUE);
	}
	return control;
}


bool Create_Controls(PickerStateType & state, HWND window)
{
	ModPickerRequestType const & request = *state.Request;

	Add_Control(state, window, L"STATIC", L"Choose what to play. OpenYR remembers your choice.", SS_LEFT, 16, 14, 448, 20, 0);

	state.List = Add_Control(state, window, WC_LISTVIEWW, L"", WS_TABSTOP | WS_BORDER | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER, 16, 42, 448, 180, IdList);
	Add_Control(state, window, L"STATIC", L"Description", SS_LEFT, 16, 232, 448, 18, 0);
	state.Description = Add_Control(state, window, L"STATIC", L"", SS_LEFT, 16, 252, 448, 56, IdDescription);
	state.Ask = Add_Control(state, window, L"BUTTON", L"Do not ask again (turn it back on in the game under Options, Mods)", WS_TABSTOP | BS_AUTOCHECKBOX, 16, 318, 448, 22, IdAsk);
	Add_Control(state, window, L"BUTTON", L"Play", WS_TABSTOP | WS_GROUP | BS_DEFPUSHBUTTON, 276, 352, 90, 28, IdPlay);
	Add_Control(state, window, L"BUTTON", L"Cancel", WS_TABSTOP | BS_PUSHBUTTON, 374, 352, 90, 28, IdCancel);

	if (state.List == nullptr || state.Description == nullptr || state.Ask == nullptr) {
		return false;
	}

	SendMessageW(state.List, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	RECT client{};
	GetClientRect(state.List, &client);
	LVCOLUMNW column{};
	column.mask = LVCF_WIDTH;
	column.cx = client.right - GetSystemMetrics(SM_CXVSCROLL);
	SendMessageW(state.List, LVM_INSERTCOLUMNW, 0, reinterpret_cast<LPARAM>(&column));

	for (int index = 0; index < static_cast<int>(request.Names.size()); index++) {
		LVITEMW item{};
		item.mask = LVIF_TEXT;
		item.iItem = index;
		item.pszText = const_cast<wchar_t *>(request.Names[index].c_str());
		SendMessageW(state.List, LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&item));
	}

	int const selected = (request.Selected >= 0 && request.Selected < static_cast<int>(request.Names.size())) ? request.Selected : 0;
	LVITEMW chosen{};
	chosen.mask = LVIF_STATE;
	chosen.state = LVIS_SELECTED | LVIS_FOCUSED;
	chosen.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
	SendMessageW(state.List, LVM_SETITEMSTATE, selected, reinterpret_cast<LPARAM>(&chosen));
	SendMessageW(state.List, LVM_ENSUREVISIBLE, selected, FALSE);
	SendMessageW(state.Ask, BM_SETCHECK, request.DontAskAgain ? BST_CHECKED : BST_UNCHECKED, 0);

	Show_Description(state);
	return true;
}


LRESULT CALLBACK Picker_Procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	PickerStateType * state = reinterpret_cast<PickerStateType *>(GetWindowLongPtrW(window, GWLP_USERDATA));

	switch (message) {
		case WM_NOTIFY: {
			NMHDR const * header = reinterpret_cast<NMHDR const *>(lparam);
			if (state != nullptr && header->idFrom == IdList) {
				if (header->code == LVN_ITEMCHANGED) {
					Show_Description(*state);
				} else if (header->code == NM_DBLCLK && Selected_Row(state->List) >= 0) {
					Finish(*state, true);
				}
			}
			return 0;
		}

		case WM_COMMAND:
			if (state != nullptr && HIWORD(wparam) == BN_CLICKED) {
				if (LOWORD(wparam) == IdPlay) {
					Finish(*state, true);
				} else if (LOWORD(wparam) == IdCancel) {
					Finish(*state, false);
				}
			}
			return 0;

		case WM_CLOSE:
			if (state != nullptr) {
				Finish(*state, false);
			}
			return 0;

		default:
			break;
	}

	return DefWindowProcW(window, message, wparam, lparam);
}

}


bool Show_Mod_Picker(ModPickerRequestType & request)
{
	if (request.Names.empty() || request.Names.size() != request.Descriptions.size()) {
		return false;
	}

	static bool registered = false;
	if (!registered) {
		WNDCLASSEXW cls{};
		cls.cbSize = sizeof(cls);
		cls.lpfnWndProc = Picker_Procedure;
		cls.hInstance = GetModuleHandleW(nullptr);
		cls.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		cls.hIcon = LoadIconW(cls.hInstance, MAKEINTRESOURCEW(IDI_OPENYR));
		cls.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
		cls.lpszClassName = PickerClass;
		registered = RegisterClassExW(&cls) != 0;
		if (!registered) {
			return false;
		}
	}

	PickerStateType state;
	state.Request = &request;

	HDC const screen = GetDC(nullptr);
	state.Dpi = GetDeviceCaps(screen, LOGPIXELSY);
	ReleaseDC(nullptr, screen);

	NONCLIENTMETRICSW metrics{};
	metrics.cbSize = sizeof(metrics);
	bool font_owned = false;
	if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0)) {
		state.Font = CreateFontIndirectW(&metrics.lfMessageFont);
		font_owned = state.Font != nullptr;
	}
	if (state.Font == nullptr) {
		state.Font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
	}

	DWORD const style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	RECT frame{0, 0, Scale(state, 480), Scale(state, 396)};
	AdjustWindowRectEx(&frame, style, FALSE, WS_EX_CONTROLPARENT);
	int const width = frame.right - frame.left;
	int const height = frame.bottom - frame.top;
	int const left = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	int const top = (GetSystemMetrics(SM_CYSCREEN) - height) / 3;

	HWND const window = CreateWindowExW(WS_EX_CONTROLPARENT, PickerClass, PickerTitle, style, left, top, width, height, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
	if (window == nullptr) {
		if (font_owned) {
			DeleteObject(state.Font);
		}
		return false;
	}

	SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&state));
	if (!Create_Controls(state, window)) {
		DestroyWindow(window);
		if (font_owned) {
			DeleteObject(state.Font);
		}
		return false;
	}

	ShowWindow(window, SW_SHOW);
	SetForegroundWindow(window);
	SetFocus(state.List);

	MSG message;
	while (!state.Done && GetMessageW(&message, nullptr, 0, 0) > 0) {
		if (!IsDialogMessageW(window, &message)) {
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
	}

	DestroyWindow(window);
	if (font_owned) {
		DeleteObject(state.Font);
	}
	return state.Play;
}

}
