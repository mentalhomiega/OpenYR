/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Widening of the 16 bit RGB565 pixels that the surfaces hold.

#pragma once

#include <cstdint>


// Scales each channel's level to 0 to 255 as level * 255 / max, so a channel at its largest level gives 255.
inline void Rgb565_Widen(std::uint16_t pixel, std::uint8_t & red, std::uint8_t & green, std::uint8_t & blue)
{
	red = (std::uint8_t)(((pixel >> 11) & 31) * 255 / 31);
	green = (std::uint8_t)(((pixel >> 5) & 63) * 255 / 63);
	blue = (std::uint8_t)((pixel & 31) * 255 / 31);
}
