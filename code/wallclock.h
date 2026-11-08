/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <ctime>

// Fills parts with the local calendar fields of stamp. Returns false when the platform cannot convert
// stamp; read parts only after a true result.
inline bool Local_Time(std::time_t stamp, std::tm & parts)
{
#if defined(_WIN32)
	return(localtime_s(&parts, &stamp) == 0);
#else
	return(localtime_r(&stamp, &parts) != nullptr);
#endif
}

// Fills parts with the UTC calendar fields of stamp. Returns false when the platform cannot convert
// stamp; read parts only after a true result.
inline bool Utc_Time(std::time_t stamp, std::tm & parts)
{
#if defined(_WIN32)
	return(gmtime_s(&parts, &stamp) == 0);
#else
	return(gmtime_r(&stamp, &parts) != nullptr);
#endif
}
