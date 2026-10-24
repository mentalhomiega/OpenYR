/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// One blocking HTTP or HTTPS request with a time limit: a GET for the tunnel
// master list, a POST for reports. Call them from a worker thread, never the
// game thread.

#pragma once

#include <string>


// Fetches the page. Returns false with the reason in error when the request
// fails, the status is not 200 or the body passes max_bytes. On platforms
// without a client the answer is always false.
bool CnCNet_Http_Get(std::string const & url, int timeout_ms, std::size_t max_bytes, std::string & body, std::string & error);

// Sends content with its type, and any extra header lines (each ending in
// \r\n), and fetches the answer. Status 200 or 201 counts as accepted. Fails
// as the GET does otherwise. When status is given, it receives the HTTP status
// of the answer, or 0 when no answer arrived.
bool CnCNet_Http_Post(std::string const & url, std::string const & content, std::string const & content_type,
	std::string const & headers, int timeout_ms, std::size_t max_bytes, std::string & body, std::string & error,
	int * status = nullptr);
