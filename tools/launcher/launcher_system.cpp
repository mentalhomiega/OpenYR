/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "launcher_system.h"

#include "launcher_core.h"

#include <bcrypt.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <winhttp.h>

#include <algorithm>
#include <cwchar>
#include <vector>

namespace launcher {
namespace {

wchar_t const * const AppTitle = L"OpenYR";
wchar_t const * const ProgressClass = L"OpenYRLauncherProgress";
std::uint64_t const FeedLimit = 1024 * 1024;
DWORD const UpdateTimeoutMs = 15 * 60 * 1000;


/*
 * Opens a GET request for an https URL. The feed passes short timeouts, so a dead network starts
 * the game within a few seconds. The response is checked for status 200. Redirects are
 * followed by WinHTTP, as GitHub sends release downloads through another host.
 */
bool Open_Request(std::wstring const & url, DWORD resolve_ms, DWORD connect_ms, DWORD send_ms, DWORD receive_ms, HINTERNET & session, HINTERNET & connection, HINTERNET & request, std::wstring & error)
{
	session = connection = request = nullptr;

	URL_COMPONENTS parts{};
	parts.dwStructSize = sizeof(parts);
	wchar_t host[256] = {};
	wchar_t path[2048] = {};
	wchar_t extra[2048] = {};
	parts.lpszHostName = host;
	parts.dwHostNameLength = 256;
	parts.lpszUrlPath = path;
	parts.dwUrlPathLength = 2048;
	parts.lpszExtraInfo = extra;
	parts.dwExtraInfoLength = 2048;
	if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts)) {
		error = L"The address is not a valid https:// address.";
		return false;
	}

	session = WinHttpOpen(L"OpenYR-Launcher/1.0 (+https://github.com/mentalhomiega/OpenYR)", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (session == nullptr) {
		error = L"Could not start the network connection.";
		return false;
	}
	WinHttpSetTimeouts(session, resolve_ms, connect_ms, send_ms, receive_ms);

	connection = WinHttpConnect(session, host, parts.nPort, 0);
	if (connection == nullptr) {
		error = L"Could not reach " + std::wstring(host) + L".";
		return false;
	}

	DWORD const flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
	std::wstring const target = std::wstring(path) + extra;
	request = WinHttpOpenRequest(connection, L"GET", target.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
	if (request == nullptr) {
		error = L"Could not make the request.";
		return false;
	}

	std::wstring const headers = L"Accept: application/vnd.github+json\r\n";
	if (!WinHttpSendRequest(request, headers.c_str(), static_cast<DWORD>(headers.size()), WINHTTP_NO_REQUEST_DATA, 0, 0, 0)
		|| !WinHttpReceiveResponse(request, nullptr)) {
		error = L"No answer from " + std::wstring(host) + L".";
		return false;
	}

	DWORD status = 0;
	DWORD size = sizeof(status);
	if (!WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX)
		|| status != 200) {
		error = L"The server answered with status " + std::to_wstring(status) + L".";
		return false;
	}

	return true;
}


void Close_Request(HINTERNET session, HINTERNET connection, HINTERNET request)
{
	if (request != nullptr) {
		WinHttpCloseHandle(request);
	}
	if (connection != nullptr) {
		WinHttpCloseHandle(connection);
	}
	if (session != nullptr) {
		WinHttpCloseHandle(session);
	}
}


bool Read_Local(std::wstring const & path, std::string & body, std::wstring & error)
{
	if (!Read_Bytes(path, body, FeedLimit)) {
		error = L"Could not read " + path + L".";
		return false;
	}
	return true;
}


void Pump_Pending(void)
{
	MSG message{};
	while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
}


LRESULT CALLBACK Progress_Procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	// The window has no close button, but Alt+F4 must not destroy it while the update runs.
	if (message == WM_CLOSE) {
		return 0;
	}
	return DefWindowProcW(window, message, wparam, lparam);
}


std::wstring Hex_Encode(BYTE const * data, DWORD size)
{
	std::wstring text;
	for (DWORD index = 0; index < size; index++) {
		wchar_t pair[3] = {};
		swprintf(pair, 3, L"%02x", static_cast<unsigned>(data[index]));
		text += pair;
	}
	return text;
}

}


bool Read_Bytes(std::wstring const & path, std::string & bytes, std::uint64_t limit)
{
	bytes.clear();

	HANDLE const file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (file == INVALID_HANDLE_VALUE) {
		return false;
	}

	LARGE_INTEGER size{};
	bool ok = GetFileSizeEx(file, &size) && size.QuadPart >= 0 && static_cast<std::uint64_t>(size.QuadPart) <= limit;
	if (ok) {
		bytes.assign(static_cast<std::size_t>(size.QuadPart), '\0');
		std::size_t done = 0;
		while (ok && done < bytes.size()) {
			DWORD const want = static_cast<DWORD>(std::min<std::size_t>(bytes.size() - done, 1u << 20));
			DWORD got = 0;
			ok = ReadFile(file, bytes.data() + done, want, &got, nullptr) && got > 0;
			done += got;
		}
	}

	CloseHandle(file);
	return ok;
}


bool Fetch_Text(std::wstring const & source, std::string & body, std::wstring & error)
{
	body.clear();

	switch (Source_Kind(source)) {
		case SourceType::LocalFile:
			return Read_Local(source, body, error);

		case SourceType::Refused:
			error = L"The feed address must start with https://, or be a full local path.";
			return false;

		case SourceType::Https:
			break;
	}

	HINTERNET session = nullptr;
	HINTERNET connection = nullptr;
	HINTERNET request = nullptr;
	bool ok = Open_Request(source, 3000, 3000, 5000, 5000, session, connection, request, error);
	if (ok) {
		DWORD available = 0;
		while (ok && WinHttpQueryDataAvailable(request, &available) && available > 0) {
			std::vector<char> chunk(available);
			DWORD read = 0;
			ok = WinHttpReadData(request, chunk.data(), available, &read) && read > 0;
			if (ok) {
				body.append(chunk.data(), read);
				ok = body.size() <= FeedLimit;
			}
		}
		if (!ok && error.empty()) {
			error = L"The feed did not finish downloading.";
		}
	}

	Close_Request(session, connection, request);
	return ok;
}


bool Download_File(std::wstring const & source, std::wstring const & path, ProgressType const & progress, std::wstring & error)
{
	if (Source_Kind(source) == SourceType::LocalFile) {
		if (!CopyFileW(source.c_str(), path.c_str(), FALSE)) {
			error = L"Could not copy " + source + L".";
			return false;
		}
		WIN32_FILE_ATTRIBUTE_DATA info{};
		std::uint64_t total = 0;
		if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &info)) {
			total = (static_cast<std::uint64_t>(info.nFileSizeHigh) << 32) | info.nFileSizeLow;
		}
		progress(total, total);
		return true;
	}

	if (Source_Kind(source) != SourceType::Https) {
		error = L"The download address must start with https://.";
		return false;
	}

	HINTERNET session = nullptr;
	HINTERNET connection = nullptr;
	HINTERNET request = nullptr;
	if (!Open_Request(source, 5000, 10000, 30000, 30000, session, connection, request, error)) {
		Close_Request(session, connection, request);
		return false;
	}

	std::uint64_t total = 0;
	DWORD length = 0;
	DWORD length_size = sizeof(length);
	if (WinHttpQueryHeaders(request, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &length, &length_size, WINHTTP_NO_HEADER_INDEX)) {
		total = length;
	}

	HANDLE const file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (file == INVALID_HANDLE_VALUE) {
		error = L"Could not write " + path + L".";
		Close_Request(session, connection, request);
		return false;
	}

	bool ok = true;
	std::uint64_t done = 0;
	std::vector<char> buffer(64 * 1024);
	while (ok) {
		DWORD available = 0;
		if (!WinHttpQueryDataAvailable(request, &available)) {
			ok = false;
			break;
		}
		if (available == 0) {
			break;
		}

		if (available > buffer.size()) {
			buffer.resize(available);
		}
		DWORD read = 0;
		if (!WinHttpReadData(request, buffer.data(), available, &read)) {
			ok = false;
			break;
		}

		DWORD wrote = 0;
		ok = WriteFile(file, buffer.data(), read, &wrote, nullptr) && wrote == read;
		done += read;
		progress(done, total);
	}

	ok = CloseHandle(file) && ok && (total == 0 || done == total);
	Close_Request(session, connection, request);

	if (!ok) {
		DeleteFileW(path.c_str());
		error = L"The download stopped before it was complete.";
	}
	return ok;
}


bool Sha256_File(std::wstring const & path, std::string & digest, std::wstring & error)
{
	digest.clear();

	BCRYPT_ALG_HANDLE algorithm = nullptr;
	if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0))) {
		error = L"SHA-256 is not available on this system.";
		return false;
	}

	DWORD object_size = 0;
	DWORD hash_size = 0;
	DWORD result_size = 0;
	bool ok = BCRYPT_SUCCESS(BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&object_size), sizeof(object_size), &result_size, 0))
		&& BCRYPT_SUCCESS(BCryptGetProperty(algorithm, BCRYPT_HASH_LENGTH, reinterpret_cast<PUCHAR>(&hash_size), sizeof(hash_size), &result_size, 0));

	std::vector<UCHAR> object(ok ? object_size : 0);
	BCRYPT_HASH_HANDLE hash = nullptr;
	ok = ok && BCRYPT_SUCCESS(BCryptCreateHash(algorithm, &hash, object.data(), object_size, nullptr, 0, 0));

	HANDLE const file = ok ? CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr) : INVALID_HANDLE_VALUE;
	ok = ok && file != INVALID_HANDLE_VALUE;
	if (!ok) {
		error = L"Could not read " + path + L" to check it.";
	}

	std::vector<UCHAR> buffer(64 * 1024);
	while (ok) {
		DWORD read = 0;
		if (!ReadFile(file, buffer.data(), static_cast<DWORD>(buffer.size()), &read, nullptr)) {
			ok = false;
			break;
		}
		if (read == 0) {
			break;
		}
		ok = BCRYPT_SUCCESS(BCryptHashData(hash, buffer.data(), read, 0));
	}

	std::vector<UCHAR> sum(ok ? hash_size : 0);
	ok = ok && BCRYPT_SUCCESS(BCryptFinishHash(hash, sum.data(), hash_size, 0));
	if (ok) {
		digest = Wide_To_Utf8(Hex_Encode(sum.data(), hash_size));
	}

	if (file != INVALID_HANDLE_VALUE) {
		CloseHandle(file);
	}
	if (hash != nullptr) {
		BCryptDestroyHash(hash);
	}
	BCryptCloseAlgorithmProvider(algorithm, 0);
	return ok;
}


bool Game_Running(std::wstring const & folder)
{
	std::wstring const game = folder + L"\\Game.exe";

	HANDLE const snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		return false;
	}

	bool running = false;
	PROCESSENTRY32W entry{};
	entry.dwSize = sizeof(entry);
	if (Process32FirstW(snapshot, &entry)) {
		do {
			if (_wcsicmp(entry.szExeFile, L"Game.exe") != 0) {
				continue;
			}

			HANDLE const process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ProcessID);
			if (process == nullptr) {
				continue;
			}

			wchar_t path[MAX_PATH * 4] = {};
			DWORD size = MAX_PATH * 4;
			if (QueryFullProcessImageNameW(process, 0, path, &size) && _wcsicmp(path, game.c_str()) == 0) {
				running = true;
			}
			CloseHandle(process);
		} while (!running && Process32NextW(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return running;
}


std::wstring Module_Path(void)
{
	wchar_t path[MAX_PATH * 4] = {};
	GetModuleFileNameW(nullptr, path, MAX_PATH * 4);
	return path;
}


std::wstring Folder_Of(std::wstring const & path)
{
	std::wstring::size_type slash = path.find_last_of(L'\\');
	return slash == std::wstring::npos ? std::wstring() : path.substr(0, slash);
}


bool Make_Update_Folder(std::wstring & folder, std::wstring & error)
{
	wchar_t temp[MAX_PATH] = {};
	DWORD const length = GetTempPathW(MAX_PATH, temp);
	if (length == 0 || length >= MAX_PATH) {
		error = L"There is no usable temporary folder.";
		return false;
	}

	std::wstring base = temp;
	if (!base.empty() && base.back() == L'\\') {
		base.pop_back();
	}

	// The name cannot be guessed, and the folder is closed to other users, because the update copy
	// runs from it and loads from it.
	std::wstring const random = Random_Hex(8);
	if (random.empty()) {
		error = L"The system could not make a random name for the update folder.";
		return false;
	}

	folder = base + L"\\OpenYR-Update-" + std::to_wstring(GetCurrentProcessId()) + L"-" + random;
	return Create_Private_Folder(folder, error);
}


bool Start_Process(std::wstring const & command_line, std::wstring const & folder, DWORD flags, PROCESS_INFORMATION & info, std::wstring & error)
{
	std::vector<wchar_t> buffer(command_line.begin(), command_line.end());
	buffer.push_back(L'\0');

	STARTUPINFOW startup{};
	startup.cb = sizeof(startup);
	info = PROCESS_INFORMATION{};
	if (!CreateProcessW(nullptr, buffer.data(), nullptr, nullptr, FALSE, flags, nullptr, folder.empty() ? nullptr : folder.c_str(), &startup, &info)) {
		error = L"Could not start a process (error " + std::to_wstring(GetLastError()) + L").";
		return false;
	}
	return true;
}


bool Wait_For_Process(HANDLE process, DWORD timeout_ms, DWORD & code)
{
	ULONGLONG const deadline = GetTickCount64() + timeout_ms;
	while (true) {
		DWORD const wait = MsgWaitForMultipleObjects(1, &process, FALSE, 100, QS_ALLINPUT);
		if (wait == WAIT_OBJECT_0) {
			return GetExitCodeProcess(process, &code) && code != STILL_ACTIVE;
		}

		Pump_Pending();
		if (GetTickCount64() >= deadline) {
			return false;
		}
	}
}


void Schedule_Folder_Removal(std::wstring const & folder)
{
	wchar_t system[MAX_PATH] = {};
	GetSystemDirectoryW(system, MAX_PATH);

	// The delay lets this process exit first, so the folder's launcher copy is no longer in use.
	std::wstring const command = std::wstring(L"/d /c \"ping -n 3 127.0.0.1 >nul & rmdir /s /q \"") + folder + L"\"\"";
	std::wstring const line = std::wstring(system) + L"\\cmd.exe " + command;
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


void Write_Stdout(std::wstring const & text)
{
	HANDLE const out = GetStdHandle(STD_OUTPUT_HANDLE);
	if (out == nullptr || out == INVALID_HANDLE_VALUE) {
		return;
	}

	std::string const bytes = Wide_To_Utf8(text + L"\r\n");
	DWORD written = 0;
	WriteFile(out, bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr);
}


namespace {

bool Silent_Run = false;

}


void Set_Silent(bool silent)
{
	Silent_Run = silent;
}


bool Ask_Update(std::wstring const & question)
{
	if (Silent_Run) {
		return false;
	}

	TASKDIALOG_BUTTON const buttons[] = {
		{IDYES, L"Yes"},
		{IDNO, L"Not now"},
	};

	TASKDIALOGCONFIG config{};
	config.cbSize = sizeof(config);
	config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
	config.dwCommonButtons = 0;
	config.pszWindowTitle = AppTitle;
	config.pszMainIcon = TD_INFORMATION_ICON;
	config.pszMainInstruction = question.c_str();
	config.pButtons = buttons;
	config.cButtons = 2;
	config.nDefaultButton = IDYES;

	int chosen = IDNO;
	if (FAILED(TaskDialogIndirect(&config, &chosen, nullptr, nullptr))) {
		// Without the dialog the answer is no; the launcher must not update unasked.
		return false;
	}
	return chosen == IDYES;
}


void Show_Message(std::wstring const & text, bool is_error)
{
	if (Silent_Run) {
		return;
	}
	MessageBoxW(nullptr, text.c_str(), AppTitle, MB_OK | (is_error ? MB_ICONERROR : MB_ICONINFORMATION) | MB_TOPMOST);
}


ProgressWindowType::ProgressWindowType(void)
	: Window(nullptr), Line(nullptr), Bar(nullptr), Font(nullptr)
{
}


ProgressWindowType::~ProgressWindowType(void)
{
	Close();
}


bool ProgressWindowType::Create(std::wstring const & title)
{
	static bool registered = false;
	if (!registered) {
		WNDCLASSEXW cls{};
		cls.cbSize = sizeof(cls);
		cls.lpfnWndProc = Progress_Procedure;
		cls.hInstance = GetModuleHandleW(nullptr);
		cls.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		cls.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
		cls.lpszClassName = ProgressClass;
		registered = RegisterClassExW(&cls) != 0;
		if (!registered) {
			return false;
		}
	}

	HDC const screen = GetDC(nullptr);
	int const dpi = GetDeviceCaps(screen, LOGPIXELSY);
	ReleaseDC(nullptr, screen);
	auto scale = [dpi](int value) { return MulDiv(value, dpi, 96); };

	RECT frame{0, 0, scale(440), scale(120)};
	AdjustWindowRectEx(&frame, WS_OVERLAPPED | WS_CAPTION, FALSE, 0);
	Window = CreateWindowExW(0, ProgressClass, title.c_str(), WS_OVERLAPPED | WS_CAPTION,
		CW_USEDEFAULT, CW_USEDEFAULT, frame.right - frame.left, frame.bottom - frame.top, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
	if (Window == nullptr) {
		return false;
	}

	Font = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
	Line = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT, scale(16), scale(16), scale(408), scale(20), Window, nullptr, GetModuleHandleW(nullptr), nullptr);
	Bar = CreateWindowExW(0, PROGRESS_CLASSW, L"", WS_CHILD | WS_VISIBLE, scale(16), scale(50), scale(408), scale(18), Window, nullptr, GetModuleHandleW(nullptr), nullptr);
	SendMessageW(Line, WM_SETFONT, reinterpret_cast<WPARAM>(Font), TRUE);
	SendMessageW(Bar, PBM_SETRANGE32, 0, 100);

	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);
	Pump();
	return true;
}


void ProgressWindowType::Set_Line(std::wstring const & text)
{
	if (Line != nullptr) {
		SetWindowTextW(Line, text.c_str());
	}
	Pump();
}


void ProgressWindowType::Set_Percent(int percent)
{
	if (Bar == nullptr) {
		return;
	}

	bool const marquee = percent < 0;
	LONG_PTR style = GetWindowLongPtrW(Bar, GWL_STYLE);
	style = marquee ? (style | PBS_MARQUEE) : (style & ~static_cast<LONG_PTR>(PBS_MARQUEE));
	SetWindowLongPtrW(Bar, GWL_STYLE, style);
	SendMessageW(Bar, PBM_SETMARQUEE, marquee ? TRUE : FALSE, 30);
	if (!marquee) {
		SendMessageW(Bar, PBM_SETPOS, static_cast<WPARAM>(percent > 100 ? 100 : percent), 0);
	}
	Pump();
}


void ProgressWindowType::Pump(void)
{
	Pump_Pending();
}


void ProgressWindowType::Close(void)
{
	if (Window != nullptr) {
		DestroyWindow(Window);
		Window = nullptr;
		Line = nullptr;
		Bar = nullptr;
	}
}

}
