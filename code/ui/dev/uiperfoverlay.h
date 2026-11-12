/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The performance overlay (docs/PERFORMANCE.md): frame rates, frame-time graphs and the time of each
// perfstat phase, drawn with Dear ImGui over the top left of the map view during a game.

#pragma once

// Sets Options.PerfOverlay, held to the levels, and turns the overlay's recording on or off with it.
void Perf_Overlay_Set_Level(int level);

// Steps to the next level: Off, Basic, Detailed, then Off again.
void Perf_Overlay_Cycle(void);

// Whether the overlay is to be drawn now: a level is set, a game is running and no menu is shown.
bool UIPerf_Overlay_Wanted(void);

// Adds the overlay's window to the ImGui frame being built. Returns false when it is not wanted.
bool UIPerf_Overlay_Build(void);

// Records how long the ImGui draw that showed the overlay took.
void UIPerf_Overlay_Note_Drawn(double milliseconds);

// Writes the overlay's numbers to the debug log as an AUTOTEST line.
void UIPerf_Overlay_Report(void);
