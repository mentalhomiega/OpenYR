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


/*
 * Saved wide text is stored as UTF-16 code units, so a save holds the same bytes on every
 * platform. One wchar_t is one code unit on Windows but one code point elsewhere, so an
 * array copied raw is a different size on each platform. Both functions convert element by
 * element between two arrays of the same length and never write past either one.
 */
namespace UTF16
{
	/*
	 * Writes the elements in order, one unit each, or two for an element above U+FFFF. An
	 * element that does not fit is left out with the rest after it, and the units after the
	 * last one written are zero. A wide type of 16 bits is copied unit for unit, including
	 * the elements after its terminator.
	 */
	template<typename Wide, std::size_t Count>
	void From_Wide(Wide const (&text)[Count], char16_t (&units)[Count])
	{
		if constexpr (sizeof(Wide) == sizeof(char16_t)) {
			for (std::size_t index = 0; index < Count; index++) {
				units[index] = (char16_t)text[index];
			}
		} else {
			std::size_t out = 0;
			for (std::size_t in = 0; in < Count; in++) {
				char32_t code = (char32_t)text[in];
				if (code > 0x10FFFF) {
					code = 0xFFFD;
				}
				std::size_t const width = code > 0xFFFF ? 2 : 1;
				if (out + width > Count) {
					break;
				}
				if (width == 1) {
					units[out++] = (char16_t)code;
				} else {
					code -= 0x10000;
					units[out++] = (char16_t)(0xD800 + (code >> 10));
					units[out++] = (char16_t)(0xDC00 + (code & 0x3FF));
				}
			}
			while (out < Count) {
				units[out++] = 0;
			}
		}
	}

	/*
	 * Reads the units in order as elements. On a wide type of 32 bits, a high surrogate that
	 * is followed by a low one becomes a single element; on a wide type of 16 bits each unit
	 * is an element of its own. The last element is zero afterwards, so a damaged array cannot
	 * leave the text unterminated.
	 */
	template<typename Wide, std::size_t Count>
	void To_Wide(char16_t const (&units)[Count], Wide (&text)[Count])
	{
		if constexpr (sizeof(Wide) == sizeof(char16_t)) {
			for (std::size_t index = 0; index < Count; index++) {
				text[index] = (Wide)units[index];
			}
		} else {
			std::size_t out = 0;
			for (std::size_t in = 0; in < Count && out < Count; in++) {
				char32_t code = units[in];
				if (code >= 0xD800 && code <= 0xDBFF && in + 1 < Count && units[in + 1] >= 0xDC00 && units[in + 1] <= 0xDFFF) {
					in++;
					code = 0x10000 + ((code - 0xD800) << 10) + (units[in] - 0xDC00);
				}
				text[out++] = (Wide)code;
			}
			while (out < Count) {
				text[out++] = 0;
			}
		}
		text[Count - 1] = 0;
	}
}
