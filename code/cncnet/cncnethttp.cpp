/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnethttp.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winhttp.h>

#include <vector>


namespace {

std::wstring Wide(std::string const & text)
{
	if (text.empty()) {
		return(std::wstring());
	}
	int const length = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
	std::wstring wide(static_cast<std::size_t>(length), L'\0');
	MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), wide.data(), length);
	return(wide);
}


struct HandleType
{
	HINTERNET Value = nullptr;
	~HandleType(void) { if (Value != nullptr) WinHttpCloseHandle(Value); }
};

}


bool CnCNet_Http_Get(std::string const & url, int timeout_ms, std::size_t max_bytes, std::string & body, std::string & error)
{
	body.clear();
	std::wstring const wide = Wide(url);
	URL_COMPONENTS parts{};
	parts.dwStructSize = sizeof(parts);
	parts.dwSchemeLength = parts.dwHostNameLength = parts.dwUrlPathLength = parts.dwExtraInfoLength = static_cast<DWORD>(-1);
	if (!WinHttpCrackUrl(wide.c_str(), 0, 0, &parts)) {
		error = "The address " + url + " is not a web address.";
		return(false);
	}
	std::wstring const host(parts.lpszHostName, parts.dwHostNameLength);
	std::wstring path(parts.lpszUrlPath, parts.dwUrlPathLength);
	path.append(parts.lpszExtraInfo, parts.dwExtraInfoLength);

	HandleType session;
	session.Value = WinHttpOpen(L"OpenYR", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (session.Value == nullptr) {
		error = "The web client could not start.";
		return(false);
	}
	WinHttpSetTimeouts(session.Value, timeout_ms, timeout_ms, timeout_ms, timeout_ms);

	HandleType connection;
	connection.Value = WinHttpConnect(session.Value, host.c_str(), parts.nPort, 0);
	if (connection.Value == nullptr) {
		error = "Could not reach " + url + ".";
		return(false);
	}
	HandleType request;
	request.Value = WinHttpOpenRequest(connection.Value, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
		parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);
	if (request.Value == nullptr
		|| !WinHttpSendRequest(request.Value, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0)
		|| !WinHttpReceiveResponse(request.Value, nullptr)) {
		error = "Could not get " + url + " (error " + std::to_string(GetLastError()) + ").";
		return(false);
	}

	DWORD status = 0;
	DWORD size = sizeof(status);
	WinHttpQueryHeaders(request.Value, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX);
	if (status != 200) {
		error = url + " answered with status " + std::to_string(status) + ".";
		return(false);
	}

	for (;;) {
		DWORD available = 0;
		if (!WinHttpQueryDataAvailable(request.Value, &available)) {
			error = "The answer from " + url + " broke off.";
			return(false);
		}
		if (available == 0) {
			return(true);
		}
		std::vector<char> chunk(available);
		DWORD read = 0;
		if (!WinHttpReadData(request.Value, chunk.data(), available, &read)) {
			error = "The answer from " + url + " broke off.";
			return(false);
		}
		body.append(chunk.data(), read);
		if (body.size() > max_bytes) {
			error = "The answer from " + url + " is too long.";
			return(false);
		}
	}
}

#else

bool CnCNet_Http_Get(std::string const &, int, std::size_t, std::string &, std::string & error)
{
	error = "Fetching the tunnel list needs Windows for now.";
	return(false);
}

#endif
