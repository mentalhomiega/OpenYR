/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "setup_system.h"

#include <objbase.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <algorithm>
#include <cstdint>
#include <cwchar>
#include <filesystem>
#include <system_error>
#include <vector>

namespace setup {
namespace {

// Initializes COM on this thread for the call that holds it. A thread already in another COM
// mode keeps it, and the call goes ahead with that mode.
class ComScopeType
{
	public:
		ComScopeType(void) : Initialized(false)
		{
			HRESULT const status = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
			Initialized = SUCCEEDED(status);
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


std::wstring With_Separator(std::wstring const & folder)
{
	if (!folder.empty() && folder.back() == L'\\') {
		return(folder);
	}
	return(folder + L'\\');
}


/*
 * Clears a read-only flag so a file can be replaced or deleted. A missing file is left alone.
 */
void Clear_Read_Only(std::wstring const & path)
{
	DWORD const attributes = GetFileAttributesW(path.c_str());
	if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_READONLY) != 0) {
		SetFileAttributesW(path.c_str(), attributes & ~FILE_ATTRIBUTE_READONLY);
	}
}

}


std::wstring Known_Folder(REFKNOWNFOLDERID id)
{
	PWSTR path = nullptr;
	if (FAILED(SHGetKnownFolderPath(id, 0, nullptr, &path)) || path == nullptr) {
		return(std::wstring());
	}

	std::wstring const folder = Normalize_Path(path);
	CoTaskMemFree(path);
	return(folder);
}


std::wstring Normalize_Path(std::wstring path)
{
	std::replace(path.begin(), path.end(), L'/', L'\\');

	// A drive root keeps its separator, so "C:\" stays "C:\".
	while (path.size() > 3 && path.back() == L'\\') {
		path.pop_back();
	}
	return(path);
}


bool Same_Path(std::wstring const & left, std::wstring const & right)
{
	return(_wcsicmp(Normalize_Path(left).c_str(), Normalize_Path(right).c_str()) == 0);
}


bool Is_Inside(std::wstring const & path, std::wstring const & folder)
{
	std::wstring const place = Normalize_Path(path);
	std::wstring const home = Normalize_Path(folder);
	if (home.empty()) {
		return(false);
	}
	if (_wcsicmp(place.c_str(), home.c_str()) == 0) {
		return(true);
	}

	std::wstring const prefix = With_Separator(home);
	return(place.size() > prefix.size() && _wcsnicmp(place.c_str(), prefix.c_str(), prefix.size()) == 0);
}


bool Is_Directory(std::wstring const & path)
{
	DWORD const attributes = GetFileAttributesW(path.c_str());
	return(attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}


bool Is_File(std::wstring const & path)
{
	DWORD const attributes = GetFileAttributesW(path.c_str());
	return(attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
}


bool Make_Folders(std::wstring const & folder)
{
	std::error_code ec;
	std::filesystem::create_directories(folder, ec);
	return(!ec || Is_Directory(folder));
}


bool Registry_String(HKEY root, REGSAM view, wchar_t const * subkey, wchar_t const * name, std::wstring & value)
{
	HKEY key = nullptr;
	if (RegOpenKeyExW(root, subkey, 0, KEY_READ | view, &key) != ERROR_SUCCESS) {
		return(false);
	}

	DWORD type = 0;
	DWORD bytes = 0;
	LONG status = RegQueryValueExW(key, name, nullptr, &type, nullptr, &bytes);
	if (status == ERROR_SUCCESS && (type == REG_SZ || type == REG_EXPAND_SZ) && bytes >= sizeof(wchar_t)) {
		std::vector<wchar_t> buffer(bytes / sizeof(wchar_t) + 1, L'\0');
		status = RegQueryValueExW(key, name, nullptr, &type, reinterpret_cast<LPBYTE>(buffer.data()), &bytes);
		if (status == ERROR_SUCCESS) {
			value = buffer.data();
		}
	} else {
		status = ERROR_FILE_NOT_FOUND;
	}

	RegCloseKey(key);
	return(status == ERROR_SUCCESS);
}


bool Registry_Set_String(HKEY root, wchar_t const * subkey, wchar_t const * name, std::wstring const & value)
{
	HKEY key = nullptr;
	if (RegCreateKeyExW(root, subkey, 0, nullptr, 0, KEY_WRITE, nullptr, &key, nullptr) != ERROR_SUCCESS) {
		return(false);
	}

	LONG const status = RegSetValueExW(key, name, 0, REG_SZ, reinterpret_cast<BYTE const *>(value.c_str()), static_cast<DWORD>((value.size() + 1) * sizeof(wchar_t)));
	RegCloseKey(key);
	return(status == ERROR_SUCCESS);
}


bool Registry_Set_Dword(HKEY root, wchar_t const * subkey, wchar_t const * name, DWORD value)
{
	HKEY key = nullptr;
	if (RegCreateKeyExW(root, subkey, 0, nullptr, 0, KEY_WRITE, nullptr, &key, nullptr) != ERROR_SUCCESS) {
		return(false);
	}

	LONG const status = RegSetValueExW(key, name, 0, REG_DWORD, reinterpret_cast<BYTE const *>(&value), sizeof(value));
	RegCloseKey(key);
	return(status == ERROR_SUCCESS);
}


bool Registry_Delete_Tree(HKEY root, wchar_t const * subkey)
{
	LONG const status = RegDeleteTreeW(root, subkey);
	return(status == ERROR_SUCCESS || status == ERROR_FILE_NOT_FOUND || status == ERROR_PATH_NOT_FOUND);
}


bool Write_File_Replacing(std::wstring const & path, char const * data, std::size_t size, std::wstring & error)
{
	std::wstring const temp = path + L".openyr-new";
	DeleteFileW(temp.c_str());

	HANDLE const file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (file == INVALID_HANDLE_VALUE) {
		error = L"Could not write " + path + L" (error " + std::to_wstring(GetLastError()) + L").";
		return(false);
	}

	bool ok = true;
	std::size_t done = 0;
	while (ok && done < size) {
		DWORD const want = static_cast<DWORD>(std::min<std::size_t>(size - done, 1u << 20));
		DWORD wrote = 0;
		ok = WriteFile(file, data + done, want, &wrote, nullptr) && wrote == want;
		done += wrote;
	}

	ok = CloseHandle(file) && ok;
	if (!ok) {
		DeleteFileW(temp.c_str());
		error = L"Could not write " + path + L" (error " + std::to_wstring(GetLastError()) + L").";
		return(false);
	}

	Clear_Read_Only(path);
	if (!MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
		DWORD const status = GetLastError();
		DeleteFileW(temp.c_str());
		error = L"Could not replace " + path + L" (error " + std::to_wstring(status) + L").";
		return(false);
	}

	return(true);
}


bool Create_Shortcut(std::wstring const & link, std::wstring const & target, std::wstring const & working, std::wstring const & icon, int icon_index, std::wstring const & description, std::wstring & error)
{
	ComScopeType com;

	IShellLinkW * shortcut = nullptr;
	HRESULT status = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shortcut));
	if (FAILED(status)) {
		error = L"Could not create a shortcut for " + target + L".";
		return(false);
	}

	bool ok = SUCCEEDED(shortcut->SetPath(target.c_str()))
		&& SUCCEEDED(shortcut->SetWorkingDirectory(working.c_str()))
		&& SUCCEEDED(shortcut->SetIconLocation(icon.c_str(), icon_index))
		&& SUCCEEDED(shortcut->SetDescription(description.c_str()));

	IPersistFile * file = nullptr;
	if (ok) {
		status = shortcut->QueryInterface(IID_PPV_ARGS(&file));
		ok = SUCCEEDED(status);
	}

	if (ok) {
		ok = SUCCEEDED(file->Save(link.c_str(), TRUE));
	}

	if (file != nullptr) {
		file->Release();
	}
	shortcut->Release();

	if (!ok) {
		error = L"Could not save the shortcut " + link + L".";
	}
	return(ok);
}


bool Shortcut_Target(std::wstring const & link, std::wstring & target)
{
	ComScopeType com;

	IShellLinkW * shortcut = nullptr;
	if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&shortcut)))) {
		return(false);
	}

	IPersistFile * file = nullptr;
	bool ok = SUCCEEDED(shortcut->QueryInterface(IID_PPV_ARGS(&file)));
	if (ok) {
		ok = SUCCEEDED(file->Load(link.c_str(), STGM_READ));
	}

	if (ok) {
		wchar_t path[MAX_PATH] = {};
		ok = SUCCEEDED(shortcut->GetPath(path, MAX_PATH, nullptr, 0));
		target = path;
	}

	if (file != nullptr) {
		file->Release();
	}
	shortcut->Release();
	return(ok);
}


bool Delete_File_Forced(std::wstring const & path)
{
	Clear_Read_Only(path);
	if (DeleteFileW(path.c_str())) {
		return(true);
	}
	return(GetLastError() == ERROR_FILE_NOT_FOUND || GetLastError() == ERROR_PATH_NOT_FOUND);
}


void Remove_Folder_If_Empty(std::wstring const & folder)
{
	DWORD const attributes = GetFileAttributesW(folder.c_str());
	if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_REPARSE_POINT) == 0) {
		RemoveDirectoryW(folder.c_str());
	}
}


bool Crosses_Link(std::wstring const & root, std::wstring const & relative)
{
	std::wstring path = Normalize_Path(root);
	std::wstring::size_type start = 0;
	while (true) {
		std::wstring::size_type const slash = relative.find(L'\\', start);
		if (slash == std::wstring::npos) {
			return(false);
		}

		path = With_Separator(path) + relative.substr(start, slash - start);
		DWORD const attributes = GetFileAttributesW(path.c_str());
		if (attributes == INVALID_FILE_ATTRIBUTES) {
			return(false);
		}
		if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0) {
			return(true);
		}
		start = slash + 1;
	}
}


bool Remove_Tree(std::wstring const & folder, std::wstring & error)
{
	DWORD const attributes = GetFileAttributesW(folder.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES) {
		return(true);
	}

	// A junction or symbolic link is removed as a link. What it points to is left alone.
	if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0) {
		if (RemoveDirectoryW(folder.c_str()) || DeleteFileW(folder.c_str())) {
			return(true);
		}
		error = L"Could not remove " + folder + L".";
		return(false);
	}

	WIN32_FIND_DATAW found{};
	HANDLE const search = FindFirstFileW((With_Separator(folder) + L"*").c_str(), &found);
	if (search != INVALID_HANDLE_VALUE) {
		bool ok = true;
		do {
			std::wstring const name = found.cFileName;
			if (name == L"." || name == L"..") {
				continue;
			}

			std::wstring const path = With_Separator(folder) + name;
			if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
				ok = Remove_Tree(path, error) && ok;
			} else if (!Delete_File_Forced(path)) {
				error = L"Could not remove " + path + L".";
				ok = false;
			}
		} while (ok && FindNextFileW(search, &found));
		FindClose(search);

		if (!ok) {
			return(false);
		}
	}

	if (!RemoveDirectoryW(folder.c_str())) {
		error = L"Could not remove " + folder + L".";
		return(false);
	}
	return(true);
}

}
