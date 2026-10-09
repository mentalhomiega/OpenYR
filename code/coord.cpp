/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

/* $Header: /CounterStrike/COORD.CPP 1     3/03/97 10:24a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : COORD.CPP                                                    *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : September 10, 1993                                           *
 *                                                                                             *
 *                  Last Update : July 22, 1996 [JLB]                                          *
 *                                                                                             *
 * Support code to handle the coordinate system is located in this module.                     *
 * Routines here will be called QUITE frequently during play and must be                       *
 * as efficient as possible.                                                                   *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   Cardinal_To_Fixed -- Converts cardinal numbers into a fixed point number.                 *
 *   Coord_Cell -- Convert a coordinate into a cell number.                                    *
 *   Coord_Move -- Moves a coordinate an arbitrary direction for an arbitrary distance         *
 *   Coord_Scatter -- Determines a random coordinate from an anchor point.                     *
 *   Coord_Spillage_List -- Calculate a spillage list for the dirty rectangle specified.       *
 *   Coord_Spillage_List -- Determines the offset list for cell spillage/occupation.           *
 *   Distance -- Determines the cell distance between two cells.                               *
 *   Distance -- Determines the lepton distance between two coordinates.                       *
 *   Distance -- Fetch distance between two target values.                                     *
 *   Fixed_To_Cardinal -- Converts a fixed point number into a cardinal number.                *
 *   Normal_Move_Point -- Moves point with tilt compensation.                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "coord.h"

#include "_map.h"
#include "inline.h"
#include "mouse.h"


/***************************************************************************
**	This array is used to index a facing in order to retrieve a cell
**	offset that, when added to another cell, will achieve the adjacent cell
**	in the indexed direction.
*/
Cell const AdjacentCell[FACING_COUNT] = {
	Cell(0,  -1),		// North
	Cell(1,  -1),		// North East
	Cell(1,   0),		// East
	Cell(1,   1),		// South East
	Cell(0,   1),		// South
	Cell(-1,  1),		// South West
	Cell(-1,  0),		// West
	Cell(-1, -1),		// North West
};


Point2D const AdjacentPoint[FACING_COUNT] = {
	Point2D(0,              -CELL_LEPTON_H),
	Point2D(CELL_LEPTON_W,  -CELL_LEPTON_H),
	Point2D(CELL_LEPTON_W,  0             ),
	Point2D(CELL_LEPTON_W,  CELL_LEPTON_H ),
	Point2D(0,              CELL_LEPTON_H ),
	Point2D(-CELL_LEPTON_W, CELL_LEPTON_H ),
	Point2D(-CELL_LEPTON_W, 0             ),
	Point2D(-CELL_LEPTON_W, -CELL_LEPTON_H),
};


/***********************************************************************************************
 * Coord_Scatter -- Determines a random coordinate from an anchor point.                       *
 *                                                                                             *
 *    This routine will perform a scatter algorithm on the specified                           *
 *    anchor point in order to return with another coordinate that is                          *
 *    randomly nearby the original. Typical use of this would be for                           *
 *    missile targeting.                                                                       *
 *                                                                                             *
 * INPUT:   coord    -- This is the anchor coordinate.                                         *
 *                                                                                             *
 *          distance -- This is the distance in pixels that the scatter                        *
 *                      should fall within.                                                    *
 *                                                                                             *
 *          lock     -- bool; Convert the new coordinate into a center                         *
 *                      cell based coordinate?                                                 *
 *                                                                                             *
 * OUTPUT:  Returns with a new coordinate that is nearby the original.                         *
 *                                                                                             *
 * WARNINGS:   Maximum pixel scatter distance is 255.                                          *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   02/01/1992 JLB : Created.                                                                 *
 *   05/13/1992 JLB : Only uses Random().                                                      *
 *=============================================================================================*/
Coord Coord_Scatter(Coord const & coord, int distance, bool lock)
{
	Coord newcoord;

	newcoord = Move_Coord(coord, Random_Dir(DIR_N, DIR_MAX), distance);

	unsigned int cell_x = newcoord.X / CELL_LEPTON_W;
	unsigned int cell_y = newcoord.Y / CELL_LEPTON_H;
	if (cell_x >= MAP_CELL_W || cell_y >= MAP_CELL_H) newcoord = coord;

	if (lock) {
		newcoord = Coord_Snap(newcoord);
	}

	return(newcoord);
}


/// <summary>
/// Gives the sine table gamemd reads for its trigonometry, the table at 0x84F084 in gamemd.exe.
/// Entry k is sin(2 pi k / 8192) for k from 0 to 10240, so entry k + 2048 is the cosine of the
/// same angle. Some entries differ from the correctly rounded float by one unit in the last place.
/// </summary>
static float const * Ballistic_Sine_Table(void)
{
	static float table[10241];
	static bool filled = false;

	if (!filled) {
		for (int index = 0; index < 10241; index++) {
			table[index] = (float)sin(2.0 * M_PI * index / 8192.0);
		}
		filled = true;
	}
	return(table);
}


/// <summary>
/// Moves an aim point by a random amount, as gamemd moves the aim of an inaccurate ballistic or
/// flak projectile (BulletClass::MoveTo at 0x468670 and TechnoClass::Fire at 0x6FDD50).
/// The draw picks the direction and the spread gives the distance in leptons. X moves by the
/// spread times the cosine and Y by the spread times the negated sine, each truncated to whole
/// leptons. Z is unchanged.
/// </summary>
/// <param name="point">The aim point to move.</param>
/// <param name="spread">The distance to move, in leptons.</param>
/// <param name="draw">A random number from 0 to 0x7FFFFFFE from the scenario generator.</param>
/// <returns>Returns with the moved aim point.</returns>
Coord Ballistic_Offset(Coord const & point, int spread, int draw)
{
	/*
	**	The draw becomes a turn in units of 65536 for a full circle, folded into a signed 16-bit
	**	value and shifted by 0x3FFF to give the direction.
	*/
	double const turn = (((double)draw * 4.656612877414201e-10) * 6.283185307179586 - 1.5707963267948966) * -10430.060040584269;
	int const low = (int)turn & 0xFFFF;
	int const direction = (low >= 0x8000 ? low - 0x10000 : low) - 0x3FFF;
	double const radians = (double)direction * -9.587672516830327e-05;

	/*
	**	The index counts half steps of pi / 8192, halved again to a table entry. An odd half step
	**	moves the entry up by one, except at the top of the table.
	*/
	int const index = (int)(radians * 2607.594482421875);
	int const base = ((index / 2) % 8192 + 8192) % 8192;
	bool const odd = (index & 1) != 0;

	int sine_index = base;
	if (odd && sine_index < 0x1FFF) {
		sine_index++;
	}
	int cosine_index = base + 2048;
	if (odd && cosine_index < 0x27FF) {
		cosine_index++;
	}

	float const * table = Ballistic_Sine_Table();
	Coord moved(point);
	moved.X = (int)(point.X + spread * (double)table[cosine_index]);
	moved.Y = (int)(point.Y - spread * (double)table[sine_index]);
	return(moved);
}


/// <summary>
/// Fetches the adjacent coordinate, following the lay of the land.
/// This routine steps one cell in the direction specified and then shifts the result
/// vertically by the difference in ground height, so it stays the same distance above
/// the terrain as the coordinate it came from. Use this routine when walking across
/// sloped ground, where a plain adjacent coordinate would end up buried in a hill or
/// hanging over a cliff.
/// </summary>
/// <returns>Returns with the adjacent coordinate, adjusted to match the ground.</returns>
Coord Adjacent_Coord_With_Height(Coord const & coord, FacingType dir)
{
	Coord newcoord = (coord + AdjacentPoint[(int)dir]);
	newcoord.Z += Map.Get_Height_GL(newcoord) - Map.Get_Height_GL(coord);
	return(newcoord);
}
