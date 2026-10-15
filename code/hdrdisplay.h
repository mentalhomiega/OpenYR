/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


// What Windows reports for the display that holds the game window.
struct HDRDisplayInfo
{
	// True when a DXGI output matches the window's monitor.
	bool Found = false;

	// True when Windows has HDR (advanced colour) on for that display.
	bool HDREnabled = false;

	// The peak and full-frame luminance the display reports, in nits. Zero means not reported.
	float PeakNits = 0.0f;
	float FullFrameNits = 0.0f;

	// The SDR content brightness Windows is set to, in nits. Known only when Windows reports it.
	bool SDRWhiteKnown = false;
	float SDRWhiteNits = 0.0f;
};


// Reads the display that holds the window and keeps the result for HDR_Current_Display. A NULL
// window reads nothing. Only Windows reports HDR; elsewhere the result never has Found set.
void HDR_Query_Window_Display(void * window);

// The result of the last HDR_Query_Window_Display, or an empty result before any query.
HDRDisplayInfo const & HDR_Current_Display(void);
