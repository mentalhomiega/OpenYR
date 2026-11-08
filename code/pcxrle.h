/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Decoder for one line of PCX run-length data, shared by the PCX readers. A byte with its top two
// bits set is a run: the low six bits count the repeats of the byte that follows. Any other byte
// stands for itself.

#pragma once


/// <summary>
/// Decodes one line of run-length data into out, keeping only the first store bytes of the line.
/// A run that would pass the end of the line is cut there.
/// </summary>
/// <param name="get">Called as get(value) for each data byte. It returns false when no byte is left.</param>
/// <param name="length">The decoded size of the line in bytes.</param>
/// <param name="allow_empty_run">Whether a run with a count of zero is accepted. When false, such a run fails the line.</param>
/// <returns>bool; Did the data hold the whole line?</returns>
template<class Get>
bool Decode_PCX_RLE_Line(Get && get, unsigned char * out, int length, int store, bool allow_empty_run)
{
	int produced = 0;
	while (produced < length) {
		unsigned char value;
		if (!get(value)) {
			return(false);
		}

		int count = 1;
		if ((value & 0xC0) == 0xC0) {
			count = value & 0x3F;
			if (count == 0 && !allow_empty_run) {
				return(false);
			}
			if (!get(value)) {
				return(false);
			}
		}

		if (count > length - produced) {
			count = length - produced;
		}
		for (int step = 0; step < count && produced + step < store; step++) {
			out[produced + step] = value;
		}
		produced += count;
	}
	return(true);
}
