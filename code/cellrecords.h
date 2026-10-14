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
#include <cstdint>
#include <cstring>


/// <summary>
/// Reads the cell records of a terrain pack: a coordinate pair of two 16-bit values, then
/// BodySize bytes of cell data, repeated until the end marker.
/// </summary>
/// <remarks>
/// A stream that ends on a record boundary with no end marker is whole, as gamemd reads it. A
/// record cut off in its coordinates or its body is not. The caller still checks the source
/// for damage.
/// </remarks>
/// <param name="source">The straw supplying the decompressed records.</param>
/// <param name="end_x">The X coordinate of the end marker.</param>
/// <param name="end_y">The Y coordinate of the end marker.</param>
/// <param name="visit">Called for each record other than the marker, with its coordinates and
/// a pointer to its BodySize bytes of data.</param>
/// <returns>bool; Did the records end whole?</returns>
template <std::size_t BodySize, typename Source, typename Visit>
bool Read_Cell_Records(Source & source, std::int16_t end_x, std::int16_t end_y, Visit && visit)
{
	unsigned char coords[4];
	unsigned char body[BodySize];

	for (;;) {
		int const got = source.Get(coords, sizeof(coords));
		if (got == 0) {
			return(true);
		}
		if (got != static_cast<int>(sizeof(coords))) {
			return(false);
		}

		std::int16_t x;
		std::int16_t y;
		std::memcpy(&x, &coords[0], sizeof(x));
		std::memcpy(&y, &coords[2], sizeof(y));
		if (x == end_x && y == end_y) {
			return(true);
		}

		if (source.Get(body, sizeof(body)) != static_cast<int>(sizeof(body))) {
			return(false);
		}
		visit(x, y, static_cast<unsigned char const *>(body));
	}
}


/// <summary>
/// Copies the next field of a record's body into the field, and returns the bytes that follow it.
/// </summary>
/// <param name="body">The position in the record body.</param>
/// <param name="field">The field to fill, which holds the field's own size.</param>
/// <returns>The position just past the field.</returns>
template <typename Field>
unsigned char const * Take_Cell_Field(unsigned char const * body, Field & field)
{
	std::memcpy(&field, body, sizeof(field));
	return(body + sizeof(field));
}
