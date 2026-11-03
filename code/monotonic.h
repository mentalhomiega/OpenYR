/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <chrono>
#include <cstdint>

// The steady clock in milliseconds. Only the difference between two readings has a meaning.
// The countdown, the heartbeats and the CnCNet timers are measured on it.
inline std::int64_t Monotonic_Milliseconds(void)
{
	return(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}

// Milliseconds on the steady clock since begin was read.
inline std::int64_t Monotonic_Elapsed_Milliseconds(std::chrono::steady_clock::time_point begin)
{
	return(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count());
}
