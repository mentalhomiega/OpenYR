/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds the voxel shading arithmetic to the ranges the voxel drawers rely on. The drawers index a
// palette row with the result, so a value outside the range would shade from the wrong row. Needs
// no game data.

#include <cmath>
#include <cstdio>
#include <limits>

#include "voxshade.h"

namespace {

int Failures = 0;


void Report(char const * what, bool ok)
{
	std::printf("%-72s %s\n", what, ok ? "ok" : "FAILED");
	if (!ok) Failures++;
}


float const Strengths[] = {0.01f, 0.1f, 0.25f, 0.4f, 0.49f, 0.5f, 0.75f, 1.0f, 3.0f, 10.0f, 1000.0f};


// A surface turned away from the halfway direction takes no highlight. Below a strength of 0.5
// the bare formula h / (s - h * s + h) is positive again for h below -s / (1 - s).
void Test_Turned_Away(void)
{
	bool none = true;
	for (float const strength : Strengths) {
		for (int step = -100; step <= 0; step++) {
			float const halfway_dot = static_cast<float>(step) / 100.0f;
			if (Voxel_Specular_Term(halfway_dot, strength) != 0.0f) none = false;
		}
	}
	Report("a surface turned away from the halfway direction takes no highlight", none);

	float const bare = -0.5f / (0.25f - (-0.5f) * 0.25f + (-0.5f));
	Report("strength 0.25 at a dot of -0.5 takes none, where the bare formula gives a highlight",
		Voxel_Specular_Term(-0.5f, 0.25f) == 0.0f && bare > 1.0f);
}


void Test_Range(void)
{
	bool in_range = true;
	for (float const strength : Strengths) {
		for (int step = -100; step <= 100; step++) {
			float const halfway_dot = static_cast<float>(step) / 100.0f;
			float const term = Voxel_Specular_Term(halfway_dot, strength);
			if (!std::isfinite(term) || term < 0.0f || term > 1.0f) in_range = false;
		}
	}
	Report("the highlight stays between 0 and 1 for every strength and every dot", in_range);
}


void Test_Rises_With_Alignment(void)
{
	bool rising = true;
	for (float const strength : Strengths) {
		float last = 0.0f;
		for (int step = 0; step <= 100; step++) {
			float const term = Voxel_Specular_Term(static_cast<float>(step) / 100.0f, strength);
			if (term < last) rising = false;
			last = term;
		}
	}
	Report("the highlight does not fall as the surface lines up with the halfway direction", rising);

	Report("a surface facing the halfway direction takes the full highlight at every strength",
		Voxel_Specular_Term(1.0f, 0.25f) == 1.0f && Voxel_Specular_Term(1.0f, 3.0f) == 1.0f);
}


void Test_Strength_One_Is_Linear(void)
{
	bool linear = true;
	for (int step = 1; step <= 100; step++) {
		float const halfway_dot = static_cast<float>(step) / 100.0f;
		if (std::fabs(Voxel_Specular_Term(halfway_dot, 1.0f) - halfway_dot) > 0.0001f) linear = false;
	}
	Report("at strength 1 the highlight equals the dot", linear);
}


void Test_Off_And_Bad_Values(void)
{
	Report("a strength of zero turns the highlight off", Voxel_Specular_Term(0.8f, 0.0f) == 0.0f);
	Report("a negative strength turns the highlight off", Voxel_Specular_Term(0.8f, -2.0f) == 0.0f);

	float const nan = std::numeric_limits<float>::quiet_NaN();
	Report("a dot that is not a number takes no highlight", Voxel_Specular_Term(nan, 3.0f) == 0.0f);
	Report("a strength that is not a number takes no highlight", Voxel_Specular_Term(0.8f, nan) == 0.0f);
}


}


int main(void)
{
	Test_Turned_Away();
	Test_Range();
	Test_Rises_With_Alignment();
	Test_Strength_One_Is_Linear();
	Test_Off_And_Bad_Values();

	std::printf("%s\n", Failures == 0 ? "voxelshade: all checks passed" : "voxelshade: FAILURES");
	return Failures == 0 ? 0 : 1;
}
