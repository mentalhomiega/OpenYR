/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "rect.h"

#include <algorithm>
#include <cstddef>
#include <vector>


// The rectangles of a store that its writers named since a reader last took them. The store is rows of
// stride values, and a rectangle counts columns and rows of it. A new list covers the whole store.
class WrittenRects
{
	public:
		// Past this many rectangles the list covers the whole store instead.
		static constexpr std::size_t LIMIT = 512;

		void Note_All(void)
		{
			All = true;
			Rects.clear();
		}

		void Note(int column, int row, int width, int height)
		{
			if (All || width <= 0 || height <= 0) {
				return;
			}
			if (Rects.size() >= LIMIT) {
				Note_All();
				return;
			}
			Rects.push_back(Rect(column, row, width, height));
		}

		// Notes count values that follow one another from value first on, row after row.
		void Note_Run(std::size_t first, std::size_t count, int stride)
		{
			int row = (int)(first / stride);
			int const column = (int)(first % stride);
			if (column != 0) {
				std::size_t const head = (std::min)(count, (std::size_t)(stride - column));
				Note(column, row, (int)head, 1);
				count -= head;
				row++;
			}
			Note(0, row, stride, (int)(count / stride));
			Note(0, row + (int)(count / stride), (int)(count % stride), 1);
		}

		// Notes a picture rectangle of width by height values whose top left value lies start values into
		// a store of rows rows. A picture row that passes the end of a store row runs on into the next
		// one, and the store's last row runs on into its first.
		void Note_Picture(std::size_t start, int stride, int rows, int width, int height)
		{
			int const column = (int)(start % stride);
			int const row = (int)(start / stride);
			int const head = (std::min)(width, stride - column);
			Note_Rows(column, row, head, height, rows);
			Note_Rows(0, row + 1, width - head, height, rows);
		}

		// Moves the rectangles named since the last call into rects. False means the whole store may
		// have been written, and rects is then empty.
		bool Take(std::vector<Rect> & rects)
		{
			rects.clear();
			bool const listed = !All;
			if (listed) {
				rects.swap(Rects);
			}
			All = false;
			Rects.clear();
			return(listed);
		}

	private:
		void Note_Rows(int column, int row, int width, int height, int rows)
		{
			row %= rows;
			int const head = (std::min)(height, rows - row);
			Note(column, row, width, head);
			Note(column, 0, width, height - head);
		}

		std::vector<Rect> Rects;
		bool All = true;
};
