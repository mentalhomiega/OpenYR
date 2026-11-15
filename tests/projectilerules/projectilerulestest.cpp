/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the projectile decisions in projectilerules.h to what the manual pages describe: the
// scatter range an inaccurate shell draws from and the cells an Airburst projectile targets.
//
// Needs no game data.

#include "projectilerules.h"

#include <cstdio>

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


void Test_Ballistic_Scatter(void)
{
	auto const ground = ProjectileRules::Ballistic_Scatter_Bounds(256, -1, -1, true);
	Check(ground.first == 128 && ground.second == 256, "the ground path scatters from half to all of the global value");

	auto const other = ProjectileRules::Ballistic_Scatter_Bounds(256, -1, -1, false);
	Check(other.first == 0 && other.second == 256, "the other path scatters from none to all of the global value");

	auto const minimum = ProjectileRules::Ballistic_Scatter_Bounds(256, 64, -1, true);
	Check(minimum.first == 64 && minimum.second == 256, "Min replaces the lower bound alone");

	auto const maximum = ProjectileRules::Ballistic_Scatter_Bounds(256, -1, 640, true);
	Check(maximum.first == 128 && maximum.second == 640, "Max replaces the upper bound alone");

	auto const both = ProjectileRules::Ballistic_Scatter_Bounds(256, 0, 0, true);
	Check(both.first == 0 && both.second == 0, "a Min and Max of 0 give no scatter");

	auto const inverted = ProjectileRules::Ballistic_Scatter_Bounds(256, 512, 128, false);
	Check(inverted.first == 512 && inverted.second == 512, "a Max under the Min is raised to it");
}


void Test_Solid_Buildings(void)
{
	Check(ProjectileRules::Solid_Height(2, 5, 104) == 512, "a SolidHeight of 2 is two cells of 256 leptons");
	Check(ProjectileRules::Solid_Height(-1, 5, 104) == 520, "a negative SolidHeight is the whole building height");
	Check(ProjectileRules::Solid_Height(0, 5, 104) == 0, "a SolidHeight of 0 is not solid");
	Check(ProjectileRules::Building_Stops(512, 100), "a shot below the solid height is stopped");
	Check(!ProjectileRules::Building_Stops(512, 512), "a shot at the solid height flies over");
	Check(!ProjectileRules::Building_Stops(0, 0), "a building that is not solid stops nothing");
}


void Test_Airburst_Offsets(void)
{
	Check(ProjectileRules::Airburst_Offsets(0.5).size() == 1, "a spread of 0.5 targets the centre cell alone");
	Check(ProjectileRules::Airburst_Offsets(0.25).size() == 1, "a spread of 0.25 targets the centre cell alone");
	Check(ProjectileRules::Airburst_Offsets(0.0).size() == 1, "a spread of 0 targets the centre cell alone");
	Check(ProjectileRules::Airburst_Offsets(1.0).size() == 5, "a spread of 1 targets the centre and its four neighbours");
	Check(ProjectileRules::Airburst_Offsets(1.5).size() == 9, "a spread of 1.5 targets the three-by-three block");
	Check(ProjectileRules::Airburst_Offsets(2.0).size() == 13, "a spread of 2 targets thirteen cells");
	Check(ProjectileRules::Airburst_Offsets(3.0).size() == 29, "a spread of 3 targets twenty-nine cells");

	auto const stock = ProjectileRules::Airburst_Stock_Offsets();
	Check(stock.size() == 9, "the stock airburst targets nine cells");
	Check(stock.front() == std::make_pair(0, -1) && stock.back() == std::make_pair(0, 0), "the stock airburst runs from the north neighbour to the centre cell");
}

}


int main(void)
{
	Test_Ballistic_Scatter();
	Test_Airburst_Offsets();
	Test_Solid_Buildings();

	std::printf("%d of %d checks passed\n", Checked - Failures, Checked);
	return(Failures == 0 ? 0 : 1);
}
