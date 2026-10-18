/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_ui.h"
#include "cncnetcopy.h"
#include "ui/screens/cncnet/uicncnet.h"
#include "ui/screens/cncnetlobby/uicncnetlobby.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uiview.h"

#include <filesystem>
#include <memory>
#include <string>

#include <objbase.h>
#include <shlobj.h>


namespace
{

std::string To_Narrow(std::wstring const & wide)
{
	int const size = WideCharToMultiByte(CP_ACP, 0, wide.c_str(), static_cast<int>(wide.size()), nullptr, 0, nullptr, nullptr);
	if (size <= 0) {
		return(std::string());
	}
	std::string text(static_cast<std::size_t>(size), '\0');
	WideCharToMultiByte(CP_ACP, 0, wide.c_str(), static_cast<int>(wide.size()), text.data(), size, nullptr, nullptr);
	return(text);
}


// The folder the running Game.exe is in, which holds Language.dll and ui beside it.
std::string Build_Folder(void)
{
	wchar_t path[MAX_PATH];
	DWORD const length = GetModuleFileNameW(nullptr, path, MAX_PATH);
	return(std::filesystem::path(std::wstring(path, length)).parent_path().string());
}


// The Games folder on the drive Windows is installed on, or an empty string when Windows does not name one.
std::string Games_Folder(void)
{
	wchar_t path[MAX_PATH];
	UINT const length = GetSystemWindowsDirectoryW(path, MAX_PATH);
	if (length == 0 || length >= MAX_PATH) {
		return(std::string());
	}
	std::wstring const root = std::filesystem::path(std::wstring(path, length)).root_path().wstring();
	return(To_Narrow(root + L"Games"));
}


// Asks for a folder with the system dialog. False when the player cancels.
bool Browse_For_Folder(std::string & folder, wchar_t const * title)
{
	HRESULT const init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	wchar_t name[MAX_PATH] = L"";
	BROWSEINFOW info {};
	info.hwndOwner = MainWindow;
	info.pszDisplayName = name;
	info.lpszTitle = title;
	info.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	bool picked = false;
	PIDLIST_ABSOLUTE const item = SHBrowseForFolderW(&info);
	if (item != nullptr) {
		wchar_t path[MAX_PATH];
		if (SHGetPathFromIDListW(item, path)) {
			folder = To_Narrow(path);
			picked = true;
		}
		CoTaskMemFree(item);
	}

	if (SUCCEEDED(init)) {
		CoUninitialize();
	}
	return(picked);
}

}


bool UI_CNCNet_Dialog(void)
{
	UICNCNetSessionClass session(Build_Folder(), Games_Folder());
	session.Restore_Choices();

	while (true) {
		UICNCNetPresenterClass presenter(session);
		presenter.State.NativeLobby = UI_CnCNet_Lobby_Enabled();
		std::unique_ptr<UIViewClass> view = UI_CNCNet_View(presenter);
		UI_Run_Modal(*view);

		if (presenter.Choice == UI_CNCNET_CHOOSE) {
			std::string folder;
			if (Browse_For_Folder(folder, L"Choose the CnCNet Yuri's Revenge folder, the one with CnCNetYRLauncher.exe in it.")) {
				session.Choose_Install(folder);
			}
			continue;
		}

		if (presenter.Choice == UI_CNCNET_DESTINATION) {
			std::string folder;
			if (Browse_For_Folder(folder, L"Choose the folder the OpenYR copy is made in. It must be empty, or hold an OpenYR copy of this install.")) {
				session.Choose_Destination(folder);
			}
			continue;
		}

		if (presenter.Choice == UI_CNCNET_LOBBY) {
			if (!UI_CnCNet_Lobby_Dialog()) {
				session.Message = "The lobby has no server it may use. Check Servers= in the [CnCNet] section of the settings.";
				session.Problem = true;
			}
			continue;
		}

		if (presenter.Choice == UI_CNCNET_START) {
			std::string error;
			if (CNCNet_Launch(session.Status.Copy_Folder, error)) {
				return(true);
			}
			session.Message = error;
			session.Problem = true;
			continue;
		}

		// A copy still running when the page closes is stopped and its files removed before the game goes on.
		if (session.Busy()) {
			session.Cancel_Copy();
			session.Job.Wait();
		}
		return(false);
	}
}
