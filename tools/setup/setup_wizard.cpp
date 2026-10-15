/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "setup_wizard.h"

#include "resource.h"
#include "setup_names.h"
#include "setup_system.h"

#include <commctrl.h>
#include <objbase.h>
#include <shobjidl.h>
#include <wincodec.h>

#include <cstdio>
#include <string>
#include <vector>

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

namespace setup {
namespace {

wchar_t const * const WindowClass = L"OpenYRSetupWindow";
wchar_t const * const AppTitle = L"OpenYR Setup";

enum ControlIdType
{
	IdNext = 1001,
	IdBack,
	IdCancel,
	IdBrowseFind,
	IdBrowseLocation,
	IdFindList,
	IdLocationEdit,
	IdDesktop,
	IdStart,
	IdDeleteData,
};

enum class PageType
{
	Welcome,
	Find,
	Location,
	Progress,
	Done,
	UninstallConfirm,
	UninstallDone,
	Count,
};

// The file each page is saved as when /SHOTDIR is given, in the order the pages are shown.
wchar_t const * const PageNames[] = {
	L"1-welcome",
	L"2-find",
	L"3-location",
	L"4-progress",
	L"5-done",
	L"u1-uninstall-confirm",
	L"u2-uninstall-done",
};

// Layout in pixels at 96 dpi, scaled to the monitor's dpi when the window is made.
int const ClientWidth = 560;
int const ClientHeight = 392;


// A static control reads '&' as a mnemonic, so each one is doubled to show as written.
void Set_Text(HWND control, std::wstring const & text)
{
	std::wstring escaped;
	escaped.reserve(text.size());
	for (wchar_t character : text) {
		escaped += character;
		if (character == L'&') {
			escaped += L'&';
		}
	}
	SetWindowTextW(control, escaped.c_str());
}


class ComScopeType
{
	public:
		ComScopeType(void) : Initialized(false)
		{
			Initialized = SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));
		}

		~ComScopeType(void)
		{
			if (Initialized) {
				CoUninitialize();
			}
		}

		ComScopeType(ComScopeType const &) = delete;
		ComScopeType & operator = (ComScopeType const &) = delete;

	private:
		bool Initialized;
};


/*
 * A folder picker, or false when the player cancels it or the dialog cannot be shown.
 */
bool Pick_Folder(HWND owner, std::wstring const & title, std::wstring & folder)
{
	ComScopeType com;

	IFileOpenDialog * dialog = nullptr;
	if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dialog)))) {
		return(false);
	}

	bool picked = false;
	FILEOPENDIALOGOPTIONS options = 0;
	if (SUCCEEDED(dialog->GetOptions(&options)) && SUCCEEDED(dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM))) {
		dialog->SetTitle(title.c_str());
		if (SUCCEEDED(dialog->Show(owner))) {
			IShellItem * item = nullptr;
			if (SUCCEEDED(dialog->GetResult(&item))) {
				PWSTR path = nullptr;
				if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) && path != nullptr) {
					folder = Normalize_Path(path);
					CoTaskMemFree(path);
					picked = true;
				}
				item->Release();
			}
		}
	}

	dialog->Release();
	return(picked);
}


void Schedule_Removal(std::wstring const & folder, std::wstring const & self)
{
	// The uninstaller stays on disk when its path would change meaning inside the cmd.exe line below.
	if (!Is_Safe_For_Cmd(folder) || !Is_Safe_For_Cmd(self)) {
		return;
	}

	wchar_t system[MAX_PATH] = {};
	GetSystemDirectoryW(system, MAX_PATH);

	std::wstring const command = std::wstring(L"/d /v:off /s /c \"ping -n 3 127.0.0.1 >nul & del /f /q \"") + self + L"\" & rmdir \"" + folder + L"\"\"";
	std::wstring line = std::wstring(system) + L"\\cmd.exe " + command;
	std::vector<wchar_t> buffer(line.begin(), line.end());
	buffer.push_back(L'\0');

	STARTUPINFOW startup{};
	startup.cb = sizeof(startup);
	PROCESS_INFORMATION process{};
	if (CreateProcessW(nullptr, buffer.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW | DETACHED_PROCESS, nullptr, nullptr, &startup, &process)) {
		CloseHandle(process.hThread);
		CloseHandle(process.hProcess);
	}
}


class WizardType
{
	public:
		WizardType(AppOptionsType & options);
		~WizardType(void);

		int Run(void);

		LRESULT Handle(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

	private:
		bool Create_Window(void);
		void Create_Pages(void);
		HWND Add_Control(PageType page, wchar_t const * kind, wchar_t const * text, DWORD style, int x, int y, int width, int height, int id = 0);
		void Place_Title_And_Body(PageType page, wchar_t const * title);
		int Scale(int value) const;

		void Show_Page(PageType page);
		void Set_Buttons(void);
		void Refresh_Page_Text(void);
		void Refresh_Location_Status(void);

		void On_Next(void);
		void On_Back(void);
		void On_Cancel(void);
		void On_Browse_Find(void);
		void On_Browse_Location(void);
		void On_Install(void);
		void On_Uninstall(void);
		void On_Finish(void);
		void Report(int percent, std::wstring const & line);
		void Pump_Messages(void);
		void Shot_Tick(void);
		void Capture(PageType page);

		std::wstring Selected_Yuri(void) const;
		std::wstring Location_Folder(void) const;
		void Add_Yuri(std::wstring const & folder, std::wstring const & source);

		AppOptionsType & Options;
		std::vector<CandidateType> Candidates;
		std::wstring Yuri;
		InstalledType Before;

		HWND Window;
		HWND Title[static_cast<int>(PageType::Count)];
		HWND Body[static_cast<int>(PageType::Count)];
		std::vector<HWND> Controls[static_cast<int>(PageType::Count)];
		HWND Back;
		HWND Next;
		HWND Cancel;
		HWND FindList;
		HWND FindStatus;
		HWND LocationEdit;
		HWND LocationStatus;
		HWND DesktopCheck;
		HWND StartCheck;
		HWND DeleteDataCheck;
		HWND ProgressBar;
		HFONT BodyFont;
		HFONT TitleFont;
		PageType Page;
		bool Busy;
		bool Shot_Done;
		bool Shot_Progress;
		int ExitCode;
		int Dpi;
		std::wstring Left_Behind;
};


LRESULT CALLBACK Window_Procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_NCCREATE) {
		CREATESTRUCTW const * create = reinterpret_cast<CREATESTRUCTW const *>(lparam);
		SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
	}

	WizardType * wizard = reinterpret_cast<WizardType *>(GetWindowLongPtrW(window, GWLP_USERDATA));
	if (wizard != nullptr && message != WM_NCCREATE && message != WM_CREATE) {
		return(wizard->Handle(window, message, wparam, lparam));
	}
	return(DefWindowProcW(window, message, wparam, lparam));
}


WizardType::WizardType(AppOptionsType & options)
	: Options(options), Window(nullptr), Back(nullptr), Next(nullptr), Cancel(nullptr), FindList(nullptr),
	  FindStatus(nullptr), LocationEdit(nullptr), LocationStatus(nullptr), DesktopCheck(nullptr), StartCheck(nullptr),
	  DeleteDataCheck(nullptr), ProgressBar(nullptr), BodyFont(nullptr), TitleFont(nullptr), Page(PageType::Welcome),
	  Busy(false), Shot_Done(false), Shot_Progress(false), ExitCode(0), Dpi(96)
{
	for (int index = 0; index < static_cast<int>(PageType::Count); index++) {
		Title[index] = nullptr;
		Body[index] = nullptr;
	}
}


WizardType::~WizardType(void)
{
	if (BodyFont != nullptr) {
		DeleteObject(BodyFont);
	}
	if (TitleFont != nullptr) {
		DeleteObject(TitleFont);
	}
}


int WizardType::Scale(int value) const
{
	return(MulDiv(value, Dpi, 96));
}


bool WizardType::Create_Window(void)
{
	HDC const screen = GetDC(nullptr);
	Dpi = GetDeviceCaps(screen, LOGPIXELSY);
	ReleaseDC(nullptr, screen);

	WNDCLASSEXW cls{};
	cls.cbSize = sizeof(cls);
	cls.lpfnWndProc = Window_Procedure;
	cls.hInstance = GetModuleHandleW(nullptr);
	cls.hIcon = LoadIconW(cls.hInstance, MAKEINTRESOURCEW(IDI_OPENYR));
	cls.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	cls.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
	cls.lpszClassName = WindowClass;
	RegisterClassExW(&cls);

	RECT frame{0, 0, Scale(ClientWidth), Scale(ClientHeight)};
	AdjustWindowRectEx(&frame, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE, 0);

	Window = CreateWindowExW(0, WindowClass, AppTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, frame.right - frame.left, frame.bottom - frame.top, nullptr, nullptr, cls.hInstance, this);
	if (Window == nullptr) {
		return(false);
	}

	NONCLIENTMETRICSW metrics{};
	metrics.cbSize = sizeof(metrics);
	SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0);
	LOGFONTW body = metrics.lfMessageFont;
	body.lfHeight = -MulDiv(9, Dpi, 72);
	BodyFont = CreateFontIndirectW(&body);

	LOGFONTW title = body;
	title.lfHeight = -MulDiv(13, Dpi, 72);
	title.lfWeight = FW_BOLD;
	TitleFont = CreateFontIndirectW(&title);

	Create_Pages();
	return(true);
}


HWND WizardType::Add_Control(PageType page, wchar_t const * kind, wchar_t const * text, DWORD style, int x, int y, int width, int height, int id)
{
	HWND const control = CreateWindowExW(0, kind, text, WS_CHILD | style, Scale(x), Scale(y), Scale(width), Scale(height),
		Window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), GetModuleHandleW(nullptr), nullptr);
	SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(BodyFont), TRUE);
	Controls[static_cast<int>(page)].push_back(control);
	return(control);
}


void WizardType::Place_Title_And_Body(PageType page, wchar_t const * title)
{
	int const index = static_cast<int>(page);

	Title[index] = Add_Control(page, L"STATIC", title, SS_LEFT, 112, 26, 424, 26);
	SendMessageW(Title[index], WM_SETFONT, reinterpret_cast<WPARAM>(TitleFont), TRUE);

	Body[index] = Add_Control(page, L"STATIC", L"", SS_LEFT, 112, 60, 424, 180);
}


void WizardType::Create_Pages(void)
{
	// Welcome: the icon, a short statement of what the setup does, and nothing to choose.
	Place_Title_And_Body(PageType::Welcome, L"Set up OpenYR");
	HICON const icon = static_cast<HICON>(LoadImageW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(IDI_OPENYR), IMAGE_ICON, Scale(64), Scale(64), LR_DEFAULTCOLOR));
	HWND const picture = Add_Control(PageType::Welcome, L"STATIC", L"", SS_ICON, 24, 24, 64, 64);
	SendMessageW(picture, STM_SETICON, reinterpret_cast<WPARAM>(icon), 0);

	// Find: the Yuri's Revenge installs the search found, or a folder to browse to.
	Place_Title_And_Body(PageType::Find, L"Find Yuri's Revenge");
	FindList = Add_Control(PageType::Find, L"LISTBOX", L"", WS_VSCROLL | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, 112, 122, 424, 112, IdFindList);
	Add_Control(PageType::Find, L"BUTTON", L"Browse...", BS_PUSHBUTTON, 112, 244, 120, 26, IdBrowseFind);
	FindStatus = Add_Control(PageType::Find, L"STATIC", L"", SS_LEFT, 112, 282, 424, 34);

	// Location: where OpenYR goes, the desktop shortcut, and what an install here would do.
	Place_Title_And_Body(PageType::Location, L"Where to install OpenYR");
	Add_Control(PageType::Location, L"STATIC", L"Install folder:", SS_LEFT, 112, 150, 200, 18);
	LocationEdit = Add_Control(PageType::Location, L"EDIT", L"", WS_BORDER | ES_AUTOHSCROLL, 112, 172, 300, 24, IdLocationEdit);
	Add_Control(PageType::Location, L"BUTTON", L"Browse...", BS_PUSHBUTTON, 420, 171, 116, 26, IdBrowseLocation);
	DesktopCheck = Add_Control(PageType::Location, L"BUTTON", L"Create a desktop shortcut", BS_AUTOCHECKBOX | WS_TABSTOP, 112, 210, 300, 22, IdDesktop);
	SendMessageW(DesktopCheck, BM_SETCHECK, BST_CHECKED, 0);
	LocationStatus = Add_Control(PageType::Location, L"STATIC", L"", SS_LEFT, 112, 244, 424, 60);

	// Progress: the step being done and the bar.
	Place_Title_And_Body(PageType::Progress, L"Installing OpenYR");
	ProgressBar = Add_Control(PageType::Progress, PROGRESS_CLASSW, L"", 0, 112, 140, 424, 20);
	SendMessageW(ProgressBar, PBM_SETRANGE32, 0, 100);

	// Done: what was installed, and whether to start the game.
	Place_Title_And_Body(PageType::Done, L"OpenYR is installed");
	StartCheck = Add_Control(PageType::Done, L"BUTTON", L"Start OpenYR", BS_AUTOCHECKBOX | WS_TABSTOP, 112, 270, 300, 22, IdStart);
	SendMessageW(StartCheck, BM_SETCHECK, BST_CHECKED, 0);
	SendMessageW(DesktopCheck, BM_SETCHECK, Options.Desktop ? BST_CHECKED : BST_UNCHECKED, 0);

	// Uninstall: a confirmation with the one choice about the player's own files.
	Place_Title_And_Body(PageType::UninstallConfirm, L"Uninstall OpenYR");
	DeleteDataCheck = Add_Control(PageType::UninstallConfirm, L"BUTTON", L"Also delete my settings, saves and Debug log (default: keep them)", BS_AUTOCHECKBOX | WS_TABSTOP, 112, 250, 424, 36, IdDeleteData);
	SendMessageW(DeleteDataCheck, BM_SETCHECK, BST_UNCHECKED, 0);

	Place_Title_And_Body(PageType::UninstallDone, L"OpenYR is removed");

	// The divider and the buttons are shared by every page.
	Add_Control(PageType::Welcome, L"STATIC", L"", SS_ETCHEDHORZ, 0, 320, 560, 2);
	for (PageType page : {PageType::Find, PageType::Location, PageType::Progress, PageType::Done, PageType::UninstallConfirm, PageType::UninstallDone}) {
		Controls[static_cast<int>(page)].push_back(Controls[static_cast<int>(PageType::Welcome)].back());
	}

	Back = CreateWindowExW(0, L"BUTTON", L"< Back", WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP, Scale(240), Scale(338), Scale(96), Scale(28), Window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IdBack)), GetModuleHandleW(nullptr), nullptr);
	Next = CreateWindowExW(0, L"BUTTON", L"Next >", WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP, Scale(344), Scale(338), Scale(96), Scale(28), Window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IdNext)), GetModuleHandleW(nullptr), nullptr);
	Cancel = CreateWindowExW(0, L"BUTTON", L"Cancel", WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP, Scale(448), Scale(338), Scale(96), Scale(28), Window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(IdCancel)), GetModuleHandleW(nullptr), nullptr);
	for (HWND button : {Back, Next, Cancel}) {
		SendMessageW(button, WM_SETFONT, reinterpret_cast<WPARAM>(BodyFont), TRUE);
	}
}


void WizardType::Show_Page(PageType page)
{
	Page = page;
	for (int index = 0; index < static_cast<int>(PageType::Count); index++) {
		for (HWND control : Controls[index]) {
			ShowWindow(control, index == static_cast<int>(page) ? SW_SHOW : SW_HIDE);
		}
	}

	// The divider and the buttons stay on every page, so they are shown here.
	ShowWindow(Next, SW_SHOW);
	ShowWindow(Back, SW_SHOW);
	ShowWindow(Cancel, SW_SHOW);

	Refresh_Page_Text();
	Set_Buttons();
	UpdateWindow(Window);
	Pump_Messages();
}


void WizardType::Set_Buttons(void)
{
	bool const working = Busy;
	bool const finished = Page == PageType::Done || Page == PageType::UninstallDone;

	EnableWindow(Back, !working && (Page == PageType::Find || Page == PageType::Location));
	EnableWindow(Cancel, !working);
	EnableWindow(Next, !working);

	wchar_t const * next_text = L"Next >";
	if (Page == PageType::Location) {
		next_text = L"Install";
	} else if (Page == PageType::UninstallConfirm) {
		next_text = L"Uninstall";
	} else if (finished) {
		next_text = L"Finish";
	}
	Set_Text(Next, next_text);
	Set_Text(Cancel, finished ? L"Close" : L"Cancel");
}


void WizardType::Refresh_Page_Text(void)
{
	std::wstring const folder = Location_Folder();
	int const index = static_cast<int>(Page);

	switch (Page) {
		case PageType::Welcome:
			Set_Text(Body[index],
				L"This sets up OpenYR, an open-source rebuild of Red Alert 2: Yuri's Revenge.\r\n\r\n"
				L"OpenYR does not include the game. You need your own copy of Yuri's Revenge. "
				L"This setup does not change that copy: OpenYR reads the game's files from it.\r\n\r\n"
				L"OpenYR is installed into its own folder. The setup needs no administrator rights and writes "
				L"nothing into your Yuri's Revenge folder. Your settings and saves stay in OpenYR's folder, so an "
				L"update keeps them.");
			break;

		case PageType::Find:
			if (Candidates.empty()) {
				Set_Text(Body[index],
					L"No Yuri's Revenge install was found in the registry, the default Steam, Origin or EA App "
					L"folders, or the Steam libraries. Browse to the folder that holds ra2md.mix and ra2.mix.");
			} else {
				Set_Text(Body[index],
					L"Choose the Yuri's Revenge install that OpenYR reads its game files from. OpenYR only reads "
					L"them: your Yuri's Revenge folder is not changed.");
			}
			{
				std::wstring const chosen = Selected_Yuri();
				if (chosen.empty()) {
					Set_Text(FindStatus, L"No folder chosen yet.");
				} else {
					std::wstring source;
					for (CandidateType const & candidate : Candidates) {
						if (Same_Path(candidate.Folder, chosen)) {
							source = candidate.Source;
						}
					}
					std::wstring const how = source.empty() || source == L"given with /YR=" ? std::wstring(L"") : L" (found through " + source + L")";
					Set_Text(FindStatus, (L"Selected: " + chosen + how).c_str());
				}
			}
			break;

		case PageType::Location:
			{
				std::wstring summary = L"Yuri's Revenge found at " + Yuri + L"; OpenYR will be installed to " + folder + L"; your Yuri's Revenge folder is not changed.";
				Set_Text(Body[index], summary.c_str());
			}
			Refresh_Location_Status();
			break;

		case PageType::Progress:
			Set_Text(Title[index], Options.Uninstall ? L"Removing OpenYR" : (Before.Present ? L"Updating OpenYR" : L"Installing OpenYR"));
			Set_Text(Body[index], L"Working. This takes a few seconds.");
			SendMessageW(ProgressBar, PBM_SETPOS, 0, 0);
			break;

		case PageType::Done:
			{
				std::wstring const build = Options.Files.empty() ? std::wstring() : Package_Build(Options.Files).Build;
				std::wstring text;
				if (Before.Present) {
					text = L"OpenYR was updated from build " + (Before.Build.empty() ? std::wstring(L"unknown") : Before.Build) + L" to build " + (build.empty() ? std::wstring(L"unknown") : build) + L" in " + folder + L".";
				} else {
					text = L"OpenYR build " + (build.empty() ? std::wstring(L"unknown") : build) + L" is installed in " + folder + L".";
				}
				text += L"\r\n\r\nYour settings and saves are in " + folder + L"\\user. Your Yuri's Revenge folder " + Yuri + L" was not changed.";
				Set_Text(Title[index], Before.Present ? L"OpenYR is updated" : L"OpenYR is installed");
				Set_Text(Body[index], text.c_str());
			}
			break;

		case PageType::UninstallConfirm:
			Set_Text(Body[index],
				(L"OpenYR will be removed from " + folder + L": its program files, its shortcuts and its entry in "
				 L"Apps & Features. Your Yuri's Revenge folder " + Yuri + L" is not changed.\r\n\r\n"
				 L"Your settings and saves in " + folder + L"\\user are kept unless you tick the box below.").c_str());
			break;

		case PageType::UninstallDone:
			{
				std::wstring text = L"OpenYR was removed from " + folder + L".\r\n\r\n";
				if (Options.DeleteData) {
					text += L"Your settings, saves and Debug log were deleted.";
				} else {
					text += L"Your settings and saves were kept in " + folder + L"\\user. Install OpenYR again to use them.";
				}
				text += L"\r\n\r\nYour Yuri's Revenge folder " + Yuri + L" was not changed.";
				if (!Left_Behind.empty()) {
					text += L"\r\n\r\nSome files could not be removed: " + Left_Behind;
				}
				Set_Text(Body[index], text.c_str());
			}
			break;

		default:
			break;
	}
}


void WizardType::Refresh_Location_Status(void)
{
	std::wstring const folder = Location_Folder();
	InstalledType const here = Read_Installed(folder);

	std::wstring const build = Options.Files.empty() ? std::wstring() : Package_Build(Options.Files).Build;
	std::wstring text;
	if (here.Present) {
		text = L"OpenYR build " + (here.Build.empty() ? std::wstring(L"unknown") : here.Build) + L" is installed here. It will be updated to build " + (build.empty() ? std::wstring(L"unknown") : build) + L". Your settings and saves in " + folder + L"\\user are kept.";
	} else {
		text = L"A new install. Settings and saves will be kept in " + folder + L"\\user.";
	}

	std::wstring const problem = Check_Install_Folder(folder, Yuri);
	if (!problem.empty()) {
		text = problem;
	}
	Set_Text(LocationStatus, text.c_str());
}


std::wstring WizardType::Selected_Yuri(void) const
{
	if (FindList == nullptr) {
		return(std::wstring());
	}

	LRESULT const index = SendMessageW(FindList, LB_GETCURSEL, 0, 0);
	if (index == LB_ERR || index < 0 || static_cast<std::size_t>(index) >= Candidates.size()) {
		return(std::wstring());
	}
	return(Candidates[static_cast<std::size_t>(index)].Folder);
}


std::wstring WizardType::Location_Folder(void) const
{
	if (LocationEdit == nullptr) {
		return(std::wstring());
	}

	int const length = GetWindowTextLengthW(LocationEdit);
	std::wstring text(static_cast<std::size_t>(length), L'\0');
	if (length > 0) {
		GetWindowTextW(LocationEdit, text.data(), length + 1);
	}
	return(Normalize_Path(text));
}


void WizardType::Add_Yuri(std::wstring const & folder, std::wstring const & source)
{
	CandidateType candidate;
	candidate.Folder = folder;
	candidate.Source = source;
	Candidates.push_back(candidate);

	SendMessageW(FindList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(folder.c_str()));
}


int WizardType::Run(void)
{
	if (!Create_Window()) {
		return(1);
	}

	std::vector<CandidateType> found = Find_Yuri_Installs(FindOptionsType());
	Candidates.clear();
	for (CandidateType const & candidate : found) {
		Add_Yuri(candidate.Folder, candidate.Source);
	}

	// A folder given with /YR= is selected, and listed if the search did not find it.
	if (!Options.Yuri.empty()) {
		std::wstring const given = Normalize_Path(Options.Yuri);
		std::size_t index = 0;
		while (index < Candidates.size() && !Same_Path(Candidates[index].Folder, given)) {
			index++;
		}
		if (index == Candidates.size()) {
			Add_Yuri(given, L"given with /YR=");
		}
		for (index = 0; index < Candidates.size(); index++) {
			if (Same_Path(Candidates[index].Folder, given)) {
				SendMessageW(FindList, LB_SETCURSEL, static_cast<WPARAM>(index), 0);
				break;
			}
		}
	} else if (!Candidates.empty()) {
		SendMessageW(FindList, LB_SETCURSEL, 0, 0);
	}

	SetWindowTextW(LocationEdit, (Options.Folder.empty() ? Default_Install_Folder() : Normalize_Path(Options.Folder)).c_str());

	if (Options.Uninstall) {
		Yuri = Read_Installed(Options.Folder).Yuri;
		Show_Page(PageType::UninstallConfirm);
	} else {
		if (!Options.HasPackage) {
			MessageBoxW(nullptr, L"This setup has no OpenYR package inside it. Download the setup again, or build it with scripts\\package_modern.ps1.", AppTitle, MB_ICONERROR | MB_OK);
			DestroyWindow(Window);
			return(1);
		}
		Show_Page(PageType::Welcome);
	}

	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);

	if (!Options.ShotFolder.empty()) {
		SetTimer(Window, 1, 700, nullptr);
	}

	MSG message{};
	while (GetMessageW(&message, nullptr, 0, 0) > 0) {
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}

	return(ExitCode);
}


LRESULT WizardType::Handle(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IdNext:
					On_Next();
					return(0);

				case IdBack:
					On_Back();
					return(0);

				case IdCancel:
					On_Cancel();
					return(0);

				case IdBrowseFind:
					On_Browse_Find();
					return(0);

				case IdBrowseLocation:
					On_Browse_Location();
					return(0);

				case IdLocationEdit:
					if (HIWORD(wparam) == EN_CHANGE && Page == PageType::Location) {
						Refresh_Location_Status();
					}
					return(0);

				case IdFindList:
					if (HIWORD(wparam) == LBN_SELCHANGE) {
						Refresh_Page_Text();
					}
					return(0);

				default:
					break;
			}
			break;

		case WM_TIMER:
			if (wparam == 1) {
				Shot_Tick();
			}
			return(0);

		case WM_CLOSE:
			if (Busy) {
				return(0);
			}
			DestroyWindow(window);
			return(0);

		case WM_DESTROY:
			PostQuitMessage(0);
			return(0);

		default:
			break;
	}

	return(DefWindowProcW(window, message, wparam, lparam));
}


void WizardType::On_Next(void)
{
	switch (Page) {
		case PageType::Welcome:
			Show_Page(PageType::Find);
			break;

		case PageType::Find:
			{
				std::wstring const chosen = Selected_Yuri();
				if (chosen.empty() || !Is_Yuri_Folder(chosen)) {
					MessageBoxW(Window, L"Choose a Yuri's Revenge folder: one that holds ra2md.mix and ra2.mix.", AppTitle, MB_ICONWARNING | MB_OK);
					return;
				}
				Yuri = chosen;
				Show_Page(PageType::Location);
			}
			break;

		case PageType::Location:
			On_Install();
			break;

		case PageType::Done:
			On_Finish();
			break;

		case PageType::UninstallConfirm:
			On_Uninstall();
			break;

		case PageType::UninstallDone:
			On_Finish();
			break;

		default:
			break;
	}
}


void WizardType::On_Back(void)
{
	if (Page == PageType::Location) {
		Show_Page(PageType::Find);
	} else if (Page == PageType::Find) {
		Show_Page(PageType::Welcome);
	}
}


void WizardType::On_Cancel(void)
{
	if (Page == PageType::Done || Page == PageType::UninstallDone) {
		On_Finish();
		return;
	}

	ExitCode = 3;
	DestroyWindow(Window);
}


void WizardType::On_Browse_Find(void)
{
	std::wstring folder;
	if (!Pick_Folder(Window, L"Choose the Yuri's Revenge folder", folder)) {
		return;
	}

	if (!Is_Yuri_Folder(folder)) {
		MessageBoxW(Window, (L"That folder does not hold ra2md.mix and ra2.mix:\r\n\r\n" + folder).c_str(), AppTitle, MB_ICONWARNING | MB_OK);
		return;
	}

	for (std::size_t index = 0; index < Candidates.size(); index++) {
		if (Same_Path(Candidates[index].Folder, folder)) {
			SendMessageW(FindList, LB_SETCURSEL, static_cast<WPARAM>(index), 0);
			Refresh_Page_Text();
			return;
		}
	}

	Add_Yuri(folder, L"");
	SendMessageW(FindList, LB_SETCURSEL, static_cast<WPARAM>(Candidates.size() - 1), 0);
	Refresh_Page_Text();
}


void WizardType::On_Browse_Location(void)
{
	std::wstring folder;
	if (!Pick_Folder(Window, L"Choose the folder for OpenYR", folder)) {
		return;
	}

	SetWindowTextW(LocationEdit, folder.c_str());
}


void WizardType::On_Install(void)
{
	std::wstring const folder = Location_Folder();
	std::wstring const problem = Check_Install_Folder(folder, Yuri);
	if (!problem.empty()) {
		MessageBoxW(Window, problem.c_str(), AppTitle, MB_ICONWARNING | MB_OK);
		return;
	}

	Before = Read_Installed(folder);
	bool const desktop = SendMessageW(DesktopCheck, BM_GETCHECK, 0, 0) == BST_CHECKED;
	Options.Folder = folder;
	Options.Desktop = desktop;

	Busy = true;
	Show_Page(PageType::Progress);

	InstallType request;
	request.Yuri = Yuri;
	request.Folder = folder;
	request.Desktop = desktop;
	request.UninstallerSource = Options.Self;
	request.UninstallerBytes = Options.StubBytes;

	std::wstring error;
	bool const ok = Install_Package(request, Options.Files, [this](int percent, std::wstring const & line) {
		Report(percent, line);
	}, error);
	Busy = false;

	if (!ok) {
		MessageBoxW(Window, error.c_str(), AppTitle, MB_ICONERROR | MB_OK);
		Show_Page(PageType::Location);
		return;
	}

	Show_Page(PageType::Done);
}


void WizardType::On_Uninstall(void)
{
	std::wstring const folder = Options.Folder;
	Busy = true;
	Show_Page(PageType::Progress);

	UninstallType request;
	request.Folder = folder;
	request.DeleteData = SendMessageW(DeleteDataCheck, BM_GETCHECK, 0, 0) == BST_CHECKED;
	Options.DeleteData = request.DeleteData;

	std::wstring error;
	std::vector<std::wstring> left;
	bool const ok = Uninstall_Package(request, [this](int percent, std::wstring const & line) {
		Report(percent, line);
	}, error, left);
	Busy = false;

	// The uninstaller itself is still running, so it is the one file that is expected to remain.
	std::wstring remaining;
	for (std::wstring const & name : left) {
		if (_wcsicmp(name.c_str(), L"Uninstall OpenYR.exe") == 0) {
			continue;
		}
		remaining += (remaining.empty() ? L"" : L", ") + name;
	}
	Left_Behind = remaining;

	if (!ok && !error.empty()) {
		MessageBoxW(Window, error.c_str(), AppTitle, MB_ICONERROR | MB_OK);
	}
	Show_Page(PageType::UninstallDone);
}


void WizardType::On_Finish(void)
{
	if (Page == PageType::Done) {
		if (SendMessageW(StartCheck, BM_GETCHECK, 0, 0) == BST_CHECKED) {
			std::wstring error;
			if (!Launch_Game(Options.Folder, error)) {
				MessageBoxW(Window, error.c_str(), AppTitle, MB_ICONERROR | MB_OK);
			}
		}
	}

	if (Page == PageType::UninstallDone && Options.RunsFromInstall) {
		Schedule_Removal(Options.Folder, Options.Self);
	}

	DestroyWindow(Window);
}


void WizardType::Report(int percent, std::wstring const & line)
{
	if (!line.empty()) {
		Set_Text(Body[static_cast<int>(Page)], line.c_str());
	}
	if (percent >= 0) {
		SendMessageW(ProgressBar, PBM_SETPOS, static_cast<WPARAM>(percent), 0);
	}

	// The window is painted before anything reads it, so a saved page shows the bar and the line.
	RedrawWindow(Window, nullptr, nullptr, RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_INVALIDATE);
	Pump_Messages();

	if (!Options.ShotFolder.empty() && !Shot_Progress && percent >= 50) {
		Shot_Progress = true;
		Capture(PageType::Progress);
	}
}


void WizardType::Pump_Messages(void)
{
	MSG message{};
	while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
}


void WizardType::Capture(PageType page)
{
	std::wstring error;
	wchar_t const * name = PageNames[static_cast<int>(page)];
	if (page == PageType::Progress && Options.Uninstall) {
		name = L"u3-uninstall-progress";
	}
	std::wstring const path = Options.ShotFolder + L"\\" + name + L".png";
	Make_Folders(Options.ShotFolder);
	if (!Save_Window_Png(Window, path, error)) {
		Log_Line(Options.LogFile, L"Screenshot failed: " + error);
	}
}


/*
 * With /SHOTDIR the window walks its pages by itself, saving each one as it is shown. The install
 * runs during the Install page, which is why its progress is saved at half way.
 */
void WizardType::Shot_Tick(void)
{
	if (Busy || Shot_Done) {
		return;
	}

	if (Page == PageType::Progress) {
		return;
	}

	Capture(Page);

	if (Page == PageType::Done) {
		Shot_Done = true;
		DestroyWindow(Window);
		return;
	}

	// The uninstall ends through its finish step, so a copy running from the install folder removes itself.
	if (Page == PageType::UninstallDone) {
		Shot_Done = true;
		On_Finish();
		return;
	}

	On_Next();
}

}


int Run_Window(AppOptionsType & options)
{
	WizardType wizard(options);
	return(wizard.Run());
}


bool Save_Window_Png(HWND window, std::wstring const & path, std::wstring & error)
{
	RECT client{};
	GetClientRect(window, &client);
	int const width = client.right - client.left;
	int const height = client.bottom - client.top;
	if (width <= 0 || height <= 0) {
		error = L"The window has no size.";
		return(false);
	}

	HDC const screen = GetDC(window);
	HDC const memory = CreateCompatibleDC(screen);
	HBITMAP const bitmap = CreateCompatibleBitmap(screen, width, height);
	HGDIOBJ const old = SelectObject(memory, bitmap);
	PrintWindow(window, memory, PW_CLIENTONLY | PW_RENDERFULLCONTENT);
	SelectObject(memory, old);
	DeleteDC(memory);
	ReleaseDC(window, screen);

	ComScopeType com;
	IWICImagingFactory * factory = nullptr;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)))) {
		DeleteObject(bitmap);
		error = L"Windows Imaging Component is not available.";
		return(false);
	}

	bool ok = false;
	IWICBitmap * wic = nullptr;
	IWICStream * stream = nullptr;
	IWICBitmapEncoder * encoder = nullptr;
	IWICBitmapFrameEncode * frame = nullptr;
	IPropertyBag2 * options = nullptr;

	if (SUCCEEDED(factory->CreateBitmapFromHBITMAP(bitmap, nullptr, WICBitmapIgnoreAlpha, &wic))
		&& SUCCEEDED(factory->CreateStream(&stream))
		&& SUCCEEDED(stream->InitializeFromFilename(path.c_str(), GENERIC_WRITE))
		&& SUCCEEDED(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder))
		&& SUCCEEDED(encoder->Initialize(stream, WICBitmapEncoderNoCache))
		&& SUCCEEDED(encoder->CreateNewFrame(&frame, &options))
		&& SUCCEEDED(frame->Initialize(options))
		&& SUCCEEDED(frame->SetSize(static_cast<UINT>(width), static_cast<UINT>(height)))) {
		WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGR;
		ok = SUCCEEDED(frame->SetPixelFormat(&format))
			&& SUCCEEDED(frame->WriteSource(wic, nullptr))
			&& SUCCEEDED(frame->Commit())
			&& SUCCEEDED(encoder->Commit());
	}

	if (!ok) {
		error = L"Could not save " + path;
	}

	if (options != nullptr) options->Release();
	if (frame != nullptr) frame->Release();
	if (encoder != nullptr) encoder->Release();
	if (stream != nullptr) stream->Release();
	if (wic != nullptr) wic->Release();
	factory->Release();
	DeleteObject(bitmap);
	return(ok);
}


void Schedule_Self_Removal(std::wstring const & folder, std::wstring const & self)
{
	Schedule_Removal(folder, self);
}

}
