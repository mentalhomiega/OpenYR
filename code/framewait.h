/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstdint>

// Waits until the steady clock (Monotonic_Nanoseconds) reaches deadline. It sleeps in coarse steps and spins
// only for the last stretch, so a core is not held while the wait is long. Returns at once if the time has passed.
void Fine_Wait_Until(std::int64_t deadline);

// CPU time the process has used so far, in microseconds, for the pacing log.
std::uint64_t Process_Cpu_Microseconds(void);
