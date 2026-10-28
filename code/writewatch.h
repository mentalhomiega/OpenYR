/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cstddef>
#include <utility>
#include <vector>


// Allocates size bytes whose written pages WriteWatch_Take can list. Free it with WriteWatch_Free.
void * WriteWatch_Alloc(std::size_t size);

void WriteWatch_Free(void * memory);

// Fills runs with the byte ranges, relative to memory, of the pages written since the last call
// (all of them on the first call) and starts listing anew. False means writes to the block are not
// listed on this system, and every byte must be treated as written.
// With steady set, a page written in two takes in a row is reported by every take until a full reset
// that happens every 32 takes, whether or not it is written again, and its writes stop costing a page fault.
// Set it for a block that many of the same pages are written in every frame.
bool WriteWatch_Take(void const * memory, std::size_t size, std::vector<std::pair<std::size_t, std::size_t>> & runs, bool steady = false);
