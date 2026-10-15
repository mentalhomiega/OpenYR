/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins the cell spread table that area damage, shrapnel and Tiberium-spread searches walk, and
// the radius a warhead's CellSpread selects. The table follows gamemd's CellSpreadTable, so a
// difference in order or membership changes which objects a blast reaches and in what order.
//
// Needs no game data.

#include "cellspread.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <utility>

namespace {

int Failures = 0;
int Checked = 0;


void Check(bool passed, char const * what)
{
	Checked++;
	if (!passed) {
		Failures++;
		std::printf("FAILED: %s\n", what);
	}
}


using Cell2 = std::pair<int, int>;

Cell2 Entry(int index)
{
	return(Cell2(CellSpread::Table[index].X, CellSpread::Table[index].Y));
}

// The longer offset plus half the shorter one, rounded down.
int Distance(Cell2 const & cell)
{
	int const x = std::abs(cell.first);
	int const y = std::abs(cell.second);
	return(std::max(x, y) + std::min(x, y) / 2);
}

int Ring_Start(int radius)
{
	return(radius == 0 ? 0 : CellSpread::Count(radius - 1));
}


void Test_Counts(void)
{
	static int const expected[CELL_SPREAD_MAX + 1] = {1, 9, 21, 37, 61, 89, 121, 161, 205, 253, 309, 369};
	bool all = true;
	for (int radius = 0; radius <= CELL_SPREAD_MAX; radius++) {
		all = all && CellSpread::Count(radius) == expected[radius];
	}
	Check(all, "the counts for radius 0 to 11 are 1, 9, 21, 37, 61, 89, 121, 161, 205, 253, 309, 369");
	Check(CELL_SPREAD_MAX == 11, "the largest radius is 11");
	Check(CellSpread::Count(-1) == 1 && CellSpread::Count(-100) == 1, "a negative radius counts as radius 0");
	Check(CellSpread::Count(12) == 369 && CellSpread::Count(1000) == 369, "a radius past the largest counts as the largest");
	Check(sizeof(CellSpread::Table) / sizeof(CellSpread::Table[0]) == 369, "the table holds 369 entries");
}


void Test_Order(void)
{
	Check(Entry(0) == Cell2(0, 0), "the first entry is the blast's own cell");

	static Cell2 const first_ring[8] = {
		{1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}
	};
	bool ring_one = true;
	for (int index = 0; index < 8; index++) {
		ring_one = ring_one && Entry(1 + index) == first_ring[index];
	}
	Check(ring_one, "ring 1 lists (1,-1), (0,-1), (-1,-1), (-1,0), (1,0), (-1,1), (0,1), (1,1) in that order");

	Check(Entry(9) == Cell2(-1, -2) && Entry(10) == Cell2(0, -2) && Entry(11) == Cell2(1, -2), "ring 2 begins (-1,-2), (0,-2), (1,-2)");
	Check(Entry(367) == Cell2(11, 0) && Entry(368) == Cell2(-11, 0), "the table ends (11,0), (-11,0)");

	bool nondecreasing = true;
	for (int index = 1; index < CellSpread::Count(10); index++) {
		nondecreasing = nondecreasing && Distance(Entry(index - 1)) <= Distance(Entry(index));
	}
	Check(nondecreasing, "within rings 0 to 10 no entry is nearer than the one before it");
}


void Test_Rings(void)
{
	bool distances = true;
	bool unique = true;
	bool complete = true;
	for (int radius = 1; radius <= 10; radius++) {
		std::set<Cell2> seen;
		for (int index = Ring_Start(radius); index < CellSpread::Count(radius); index++) {
			Cell2 const cell = Entry(index);
			distances = distances && Distance(cell) == radius;
			unique = unique && seen.insert(cell).second;
		}

		// Every cell at that distance within a 25 by 25 square is listed in the ring.
		std::set<Cell2> wanted;
		for (int x = -12; x <= 12; x++) {
			for (int y = -12; y <= 12; y++) {
				if (Distance(Cell2(x, y)) == radius) {
					wanted.insert(Cell2(x, y));
				}
			}
		}
		complete = complete && wanted == seen;
	}
	Check(distances, "every entry of rings 1 to 10 is at the ring's distance");
	Check(unique, "no cell repeats within rings 1 to 10");
	Check(complete, "rings 1 to 10 list every cell at their distance");

	Check(Distance(Entry(0)) == 0 && CellSpread::Count(0) == 1, "ring 0 is the one cell at distance 0");

	// A cell with the longer offset 2 and the shorter 1 is at distance 2, so it is in ring 2, not 3.
	bool two_one = true;
	int found = 0;
	for (int index = Ring_Start(2); index < CellSpread::Count(2); index++) {
		Cell2 const cell = Entry(index);
		if (std::abs(cell.first) == 2 && std::abs(cell.second) == 1) {
			found++;
			two_one = two_one && Distance(cell) == 2;
		}
	}
	Check(two_one && found == 4, "the four (+-2,+-1) cells are in ring 2");
}


void Test_Ring_Eleven(void)
{
	std::set<Cell2> ring;
	int listed_minus3_11 = 0;
	for (int index = Ring_Start(11); index < CellSpread::Count(11); index++) {
		Cell2 const cell = Entry(index);
		ring.insert(cell);
		if (cell == Cell2(-3, 11)) {
			listed_minus3_11++;
		}
	}
	Check(CellSpread::Count(11) - Ring_Start(11) == 60, "ring 11 has 60 entries");
	Check(ring.size() == 59, "ring 11 holds 59 different cells");
	Check(listed_minus3_11 == 2, "ring 11 lists (-3,11) twice");
	Check(ring.count(Cell2(3, -11)) == 0, "ring 11 leaves out (3,-11)");

	bool swapped_in = ring.count(Cell2(2, 11)) && ring.count(Cell2(-2, 11)) && ring.count(Cell2(2, -11)) && ring.count(Cell2(-2, -11))
		&& ring.count(Cell2(-3, -11)) && ring.count(Cell2(3, 11));
	Check(swapped_in, "ring 11 holds (+-2,+-11), (-3,-11) and (3,11)");

	bool swapped_out = true;
	for (int sx = -1; sx <= 1; sx += 2) {
		for (int sy = -1; sy <= 1; sy += 2) {
			swapped_out = swapped_out && ring.count(Cell2(2 * sx, 10 * sy)) == 0 && ring.count(Cell2(3 * sx, 10 * sy)) == 0;
		}
	}
	Check(swapped_out, "ring 11 leaves out (+-2,+-10) and (+-3,+-10)");

	// Those eight cells are in no earlier ring either, so a blast never reaches them.
	bool absent = true;
	for (int index = 0; index < 369; index++) {
		Cell2 const cell = Entry(index);
		absent = absent && !(std::abs(cell.first) >= 2 && std::abs(cell.first) <= 3 && std::abs(cell.second) == 10);
	}
	Check(absent, "no entry of the table is (+-2,+-10) or (+-3,+-10)");

	bool inside = true;
	for (int index = Ring_Start(11); index < CellSpread::Count(11); index++) {
		Cell2 const cell = Entry(index);
		inside = inside && std::max(std::abs(cell.first), std::abs(cell.second)) <= 11;
	}
	Check(inside, "every ring 11 entry is at most 11 cells out along an axis");
}


void Test_Symmetry(void)
{
	// Rings 0 to 10 are symmetric across both axes, so no side of a blast is favoured.
	std::set<Cell2> cells;
	for (int index = 0; index < CellSpread::Count(10); index++) {
		cells.insert(Entry(index));
	}
	bool mirrored = true;
	for (Cell2 const & cell : cells) {
		mirrored = mirrored && cells.count(Cell2(-cell.first, cell.second)) && cells.count(Cell2(cell.first, -cell.second))
			&& cells.count(Cell2(-cell.first, -cell.second));
	}
	Check(mirrored, "rings 0 to 10 are symmetric across both axes");
	Check(cells.size() == 309, "rings 0 to 10 hold 309 different cells");
}


void Test_Radius(void)
{
	Check(CellSpread::Radius_For(0.0f) == 0, "a CellSpread of 0 reaches radius 0");
	Check(CellSpread::Radius_For(0.1f) == 1, "a CellSpread of 0.1 reaches radius 1");
	Check(CellSpread::Radius_For(0.5f) == 1, "a CellSpread of 0.5 reaches radius 1");
	Check(CellSpread::Radius_For(1.0f) == 1, "a CellSpread of 1 reaches radius 1");
	Check(CellSpread::Radius_For(1.02f) == 2, "a CellSpread of 1.02 reaches radius 2");
	Check(CellSpread::Radius_For(1.5f) == 2, "a CellSpread of 1.5 reaches radius 2");
	Check(CellSpread::Radius_For(3.0f) == 3, "a CellSpread of 3 reaches radius 3");
	Check(CellSpread::Radius_For(10.5f) == 11, "a CellSpread of 10.5 reaches radius 11");
	Check(CellSpread::Radius_For(11.0f) == 11, "a CellSpread of 11 reaches radius 11");
	Check(CellSpread::Radius_For(50.0f) == CELL_SPREAD_MAX, "a CellSpread past 11 is held to radius 11");
	Check(CellSpread::Radius_For(-3.0f) == 0, "a negative CellSpread reaches radius 0");
}

}


int main(void)
{
	Test_Counts();
	Test_Order();
	Test_Rings();
	Test_Ring_Eleven();
	Test_Symmetry();
	Test_Radius();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
