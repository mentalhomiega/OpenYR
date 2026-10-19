/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <utility>
#include <vector>


/*
 * Projectile decisions that depend on numbers only, apart from the objects involved, so a test can run
 * them on their own.
 */
namespace ProjectileRules
{
	// The range an inaccurate arcing shell draws its scatter from, in leptons (Ares BallisticScatter.Min and .Max). A key
	// the projectile does not set is negative. A shell fired along the ground path (not a flak shell, or an invisible one)
	// scatters from half to all of the global value unless it says otherwise, and one of the other path from none to all of
	// it. A Max below the lower bound is raised to it.
	inline std::pair<int, int> Ballistic_Scatter_Bounds(int global, int minimum, int maximum, bool ground_path)
	{
		int const low = minimum >= 0 ? minimum : (ground_path ? global / 2 : 0);
		int const high = maximum >= 0 ? maximum : global;
		return(std::pair<int, int>(low, high < low ? low : high));
	}

	// The height, in leptons, up to which a building stops a SubjectToBuildings projectile (Ares SolidHeight): SolidHeight cells
	// of 256 leptons, the building's whole height when it is negative, and none when it is 0.
	inline int Solid_Height(int solid_height, int building_height_levels, int level_leptons)
	{
		return(solid_height < 0 ? building_height_levels * level_leptons : solid_height * 256);
	}

	// Whether a projectile this high above the ground is stopped by a building with this solid height.
	inline bool Building_Stops(int solid_leptons, int projectile_height)
	{
		return(solid_leptons > 0 && projectile_height < solid_leptons);
	}

	// The cells an Airburst projectile without AirburstSpread targets with its AirburstWeapon, in the order the stock game
	// does: the eight neighbours of the cell it burst over from north clockwise, then that cell.
	inline std::vector<std::pair<int, int>> Airburst_Stock_Offsets()
	{
		return(std::vector<std::pair<int, int>>{{0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, 0}});
	}

	// The cell offsets an Airburst projectile with this AirburstSpread targets with its AirburstWeapon (Ares): every cell
	// whose centre is within the spread, in cells, of the cell at the centre of the burst. A spread under 1 targets the
	// centre cell alone.
	inline std::vector<std::pair<int, int>> Airburst_Offsets(double spread)
	{
		std::vector<std::pair<int, int>> offsets;
		int const reach = spread > 0.0 ? (int)spread + 1 : 0;
		for (int y = -reach; y <= reach; y++) {
			for (int x = -reach; x <= reach; x++) {
				if ((double)(x * x + y * y) <= spread * spread + 1e-9 || (x == 0 && y == 0)) {
					offsets.push_back(std::pair<int, int>(x, y));
				}
			}
		}
		return(offsets);
	}
}
