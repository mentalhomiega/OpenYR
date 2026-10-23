/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// One blocking HTTP or HTTPS GET with a time limit, for the tunnel master
// list. Call it from a worker thread, never the game thread.

#pragma once

#include <string>


// Fetches the page. Returns false with the reason in error when the request
// fails, the status is not 200 or the body passes max_bytes. On platforms
// without a client the answer is always false.
bool CnCNet_Http_Get(std::string const & url, int timeout_ms, std::size_t max_bytes, std::string & body, std::string & error);
