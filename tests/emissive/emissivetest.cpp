/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks the emissive defaults, the strength parser, the mask blend and the plane operations.
// Needs no game data.

#include "emissive.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

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


bool Near(float a, float b)
{
	return(std::fabs(a - b) < 0.0005f);
}


void Test_Table(void)
{
	Check(EMISSIVE_KIND_COUNT == 7, "the default table has one row per kind");
	Check(std::strcmp(EmissiveDefaults[EMISSIVE_LASER].Name, "Laser") == 0, "the laser row is named Laser");
	Check(std::strcmp(EmissiveDefaults[EMISSIVE_ELECTRIC_BOLT].Name, "ElectricBolt") == 0, "the bolt row is named ElectricBolt");
	bool named = true;
	for (EmissiveDefault const & row : EmissiveDefaults) {
		named = named && row.Name != NULL && row.Applies != NULL && row.Name[0] != '\0';
	}
	Check(named, "every row has a name and a description");
	Check(EmissiveDefaults[EMISSIVE_LASER].Strength == 220, "the laser default is 220");
}


void Test_Anim_Default(void)
{
	Check(Emissive_Anim_Default(false, false, 0, false) == 0, "an ordinary anim is not emissive");
	Check(Emissive_Anim_Default(true, false, 0, false) == EmissiveDefaults[EMISSIVE_ANIM_TRANSLUCENT].Strength, "a translucent anim takes the translucent default");
	Check(Emissive_Anim_Default(false, true, 0, false) == EmissiveDefaults[EMISSIVE_ANIM_SELF_LIT].Strength, "a self-lit anim takes the self-lit default");
	Check(Emissive_Anim_Default(false, false, 50, false) == EmissiveDefaults[EMISSIVE_ANIM_TRANSLUCENCY].Strength, "Translucency=50 takes the translucency default");
	Check(Emissive_Anim_Default(false, false, 10, false) == 0, "a Translucency that is not 25, 50 or 75 is not emissive");
	Check(Emissive_Anim_Default(false, false, 0, true) == EmissiveDefaults[EMISSIVE_ANIM_TIBERIUM].Strength, "an animated tiberium anim takes the tiberium default");
	Check(Emissive_Anim_Default(true, true, 0, false) == std::max(EmissiveDefaults[EMISSIVE_ANIM_TRANSLUCENT].Strength, EmissiveDefaults[EMISSIVE_ANIM_SELF_LIT].Strength), "the strongest matching default wins");
}


void Test_Parse(void)
{
	Check(Emissive_Parse_Strength("yes", 7) == 255, "yes is 255");
	Check(Emissive_Parse_Strength("YES", 7) == 255, "yes is accepted in any case");
	Check(Emissive_Parse_Strength("no", 7) == 0, "no is 0");
	Check(Emissive_Parse_Strength("128", 7) == 128, "a number is used as the strength");
	Check(Emissive_Parse_Strength("300", 7) == 255, "a number above 255 is clamped");
	Check(Emissive_Parse_Strength("-5", 7) == 0, "a negative number is clamped to 0");
	Check(Emissive_Parse_Strength("12x", 7) == 7, "text after the number gives the fallback");
	Check(Emissive_Parse_Strength("maybe", 7) == 7, "an unknown word gives the fallback");
	Check(Emissive_Parse_Strength("", 7) == 7, "an empty value gives the fallback");
	Check(Emissive_Parse_Strength(NULL, 9) == 9, "a missing value gives the fallback");
}


void Test_Voxel_Highlight(void)
{
	Check(Emissive_Voxel_Highlight(0) == 0, "shade row 0 is not a highlight");
	Check(Emissive_Voxel_Highlight(16) == 0, "neutral row 16 is not a highlight");
	Check(Emissive_Voxel_Highlight(17) == 8, "row 17 gives 128/15 rounded down");
	Check(Emissive_Voxel_Highlight(24) == 68, "row 24 is eight steps of 128/15 above neutral");
	Check(Emissive_Voxel_Highlight(31) == 128, "row 31 gives the table strength");
	Check(Emissive_Voxel_Highlight(31) >= Emissive_Voxel_Highlight(30), "the highlight does not fall as the row rises");
}


void Test_Blend(void)
{
	Check(Near(Emissive_Gain(0, 4.0f), 1.0f), "mask 0 leaves paper white alone");
	Check(Near(Emissive_Gain(255, 4.0f), 4.0f), "mask 255 reaches peak over paper white");
	Check(Near(Emissive_Gain(128, 4.0f), 1.0f + (128.0f / 255.0f) * 3.0f), "mask 128 scales the boost linearly");

	Check(Near(Emissive_Output_Nits(1.0f, 0, 200.0f, 800.0f), 200.0f), "mask 0 gives paper white for linear 1");
	Check(Near(Emissive_Output_Nits(0.5f, 255, 200.0f, 800.0f), 400.0f), "mask 255 doubles a half-bright pixel");
	Check(Near(Emissive_Output_Nits(1.0f, 255, 200.0f, 800.0f), 800.0f), "mask 255 reaches the peak");
	Check(Near(Emissive_Output_Nits(2.0f, 255, 200.0f, 800.0f), 800.0f), "a result above the peak is clamped to it");
	Check(Near(Emissive_Output_Nits(2.0f, 0, 200.0f, 800.0f), 400.0f), "mask 0 gives paper white times linear, whatever the peak");

	// The composite shader writes the gain as paper + mask * (peak - paper). It must match the CPU form.
	bool shader_form = true;
	for (int mask = 0; mask <= 255; mask += 51) {
		for (float linear = 0.0f; linear <= 1.0f; linear += 0.25f) {
			float const shader = std::min(800.0f, linear * (200.0f + (mask / 255.0f) * (800.0f - 200.0f)));
			shader_form = shader_form && Near(Emissive_Output_Nits(linear, (unsigned char)mask, 200.0f, 800.0f), shader);
		}
	}
	Check(shader_form, "the composite's gain form matches the CPU blend");
}


void Test_Opacity(void)
{
	Check(Emissive_Scale_By_Opacity(200, 255) == 200, "a whole picture keeps its strength");
	Check(Emissive_Scale_By_Opacity(200, 0) == 0, "an invisible picture keeps no strength");
	Check(Emissive_Scale_By_Opacity(255, 192) == 192, "a 25 percent translucent picture keeps three quarters of its strength");
	Check(Emissive_Scale_By_Opacity(96, 64) == 24, "a 75 percent translucent picture keeps a quarter of its strength");
	Check(Emissive_Scale_By_Opacity(96, 1000) == 96, "an opacity above 255 is a whole picture");
}


void Test_Lifecycle(void)
{
	int a = 0;
	int b = 0;
	Emissive_Release_All();
	Check(Emissive_Plane_For(&a) == NULL, "no plane before a bind");

	EmissivePlane * first = Emissive_Bind(&a, 4, 3);
	Check(first != NULL && first->Width == 4 && first->Height == 3 && first->Pitch == 4, "a bind makes a plane of the surface size");
	Check(Emissive_Bind(&a, 4, 3) == first, "a bind of the same size reuses the plane");
	Check(first->Pixels[0] == 0, "a new plane starts clear");
	Check(Emissive_Plane_For(&a) == first, "the bound plane is found by its surface");

	Check(Emissive_Bind(&b, 2, 2) != NULL, "a second surface gets its own plane");
	void const * keep[1] = {&b};
	Emissive_Retain(keep, 1);
	Check(Emissive_Plane_For(&a) == NULL, "a retain frees a surface that is not kept");
	Check(Emissive_Plane_For(&b) != NULL, "a retain keeps a listed surface");

	Emissive_Release_All();
	Check(Emissive_Plane_For(&b) == NULL, "release all frees every plane");

	Emissive_Set_Enabled(true);
	Check(Emissive_Is_Enabled(), "the enabled flag turns on");
	Emissive_Set_Enabled(false);
	Check(!Emissive_Is_Enabled(), "the enabled flag turns off");
}


void Test_Fill_And_Copy(void)
{
	int tag = 0;
	EmissivePlane * plane = Emissive_Bind(&tag, 4, 4);
	Check(plane != NULL, "a plane for the fill test");

	Emissive_Fill_Rect(*plane, -1, 2, 3, 5, 255);
	bool clipped = plane->Pixels[2 * 4 + 0] == 255 && plane->Pixels[2 * 4 + 1] == 255 && plane->Pixels[2 * 4 + 2] == 0
		&& plane->Pixels[3 * 4 + 1] == 255 && plane->Pixels[1 * 4 + 0] == 0 && plane->Pixels[3 * 4 + 3] == 0;
	Check(clipped, "a fill is clipped to the plane");

	for (int i = 0; i < 16; i++) {
		plane->Pixels[i] = (unsigned char)(i + 1);
	}
	unsigned char snapshot[16];
	std::memcpy(snapshot, plane->Pixels, sizeof(snapshot));
	Emissive_Copy_Rect(*plane, 1, 1, *plane, 0, 0, 2, 2);
	bool moved = plane->Pixels[1 * 4 + 1] == snapshot[0] && plane->Pixels[1 * 4 + 2] == snapshot[1]
		&& plane->Pixels[2 * 4 + 1] == snapshot[4] && plane->Pixels[2 * 4 + 2] == snapshot[5];
	Check(moved, "an overlapping copy down and right moves the original values");

	EmissivePlane other = {NULL, 4, 4, 4};
	std::vector<unsigned char> pixels(16, 9);
	other.Pixels = pixels.data();
	Emissive_Copy_Rect(other, 2, 2, *plane, 3, 3, 4, 4);
	Check(other.Pixels[2 * 4 + 2] == snapshot[15] && other.Pixels[3 * 4 + 3] == 9, "a copy between planes is clipped to both");

	Emissive_Release_All();
}


void Test_Segment(void)
{
	int tag = 0;
	EmissivePlane * plane = Emissive_Bind(&tag, 4, 3);
	Emissive_Fill_Rect(*plane, 0, 0, 4, 3, 0);
	Emissive_Stamp_Segment(*plane, 0, 0, 4, 3, 0, 1, 3, 1, 200);
	bool row = plane->Pixels[4] == 200 && plane->Pixels[7] == 200 && plane->Pixels[0] == 0 && plane->Pixels[8] == 0;
	Check(row, "a horizontal segment writes its own row only");

	Emissive_Fill_Rect(*plane, 0, 0, 4, 3, 0);
	Emissive_Stamp_Segment(*plane, 0, 0, 4, 3, 0, 0, 3, 2, 99);
	int diagonal = 0;
	for (int i = 0; i < 12; i++) {
		diagonal += plane->Pixels[i] == 99 ? 1 : 0;
	}
	Check(diagonal == 4 && plane->Pixels[0] == 99 && plane->Pixels[11] == 99, "a diagonal segment writes one pixel per step of its long side");

	Emissive_Fill_Rect(*plane, 0, 0, 4, 3, 0);
	Emissive_Stamp_Segment(*plane, 1, 0, 2, 3, 0, 1, 3, 1, 50);
	Check(plane->Pixels[4] == 0 && plane->Pixels[5] == 50 && plane->Pixels[6] == 50 && plane->Pixels[7] == 0, "a segment is clipped to the clip rectangle");

	Emissive_Release_All();
}

}


int main(void)
{
	Test_Table();
	Test_Anim_Default();
	Test_Parse();
	Test_Voxel_Highlight();
	Test_Blend();
	Test_Opacity();
	Test_Lifecycle();
	Test_Fill_And_Copy();
	Test_Segment();
	std::printf("%d checks, %d failed\n", Checked, Failures);
	return(Failures == 0 ? 0 : 1);
}
