/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks the free player colors: the remap ramp step against the formula gamemd builds its
// ramps from, the packing and its value floor, the color-blind-aware distance, and the presets.
// Needs no game data.

#include "playercolor.h"

#include "visualc.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

int Failures = 0;
int Checked = 0;

void Check(bool condition, char const * what)
{
	Checked++;
	if (!condition) {
		Failures++;
		std::printf("FAIL: %s\n", what);
	}
}

/// The ramp exactly as the color scheme code built it before the helper existed. The helper
/// must reproduce it bit for bit, since the stock colors depend on it.
HSVClass Reference_Ramp_Entry(HSVClass const & hsv, int i)
{
	int const hue = hsv.Get_Hue();
	int const saturation = hsv.Get_Saturation();
	int const value = hsv.Get_Value();

	double const cos_step = DEG_TO_RAD(14.0 / 3.0);
	double const sin_step = DEG_TO_RAD(8.0 / 3.0);
	double cosval = DEG_TO_RAD(20) + (i * cos_step);
	double const sinval = DEG_TO_RAD(50) + (i * sin_step);
	if (i == 0) {
		cosval = DEG_TO_RAD(360.0/32.0);
	}

	int const sat = int(std::sin(sinval) * saturation);
	int const val = int(std::cos(cosval) * value);
	return(HSVClass((unsigned char)hue, (unsigned char)sat, (unsigned char)val));
}

/// The eight classic lobby colors as the lobby shows them.
RGBClass const CLASSIC[8] = {
	RGBClass(255, 223, 94),
	RGBClass(255, 26, 20),
	RGBClass(39, 60, 179),
	RGBClass(11, 148, 11),
	RGBClass(218, 137, 26),
	RGBClass(20, 177, 255),
	RGBClass(185, 20, 255),
	RGBClass(255, 70, 173),
};

void Test_Ramp_Matches_Reference(void)
{
	int mismatches = 0;
	for (int hue = 0; hue < 256; hue += 3) {
		for (int saturation = 0; saturation < 256; saturation += 5) {
			for (int value = 0; value < 256; value += 3) {
				HSVClass const base((unsigned char)hue, (unsigned char)saturation, (unsigned char)value);
				for (int step = 0; step < 16; step++) {
					HSVClass const want = Reference_Ramp_Entry(base, step);
					HSVClass const got = Player_Ramp_Entry(base, step);
					if (want != got) {
						mismatches++;
					}
				}
			}
		}
	}
	Check(mismatches == 0, "every ramp step matches the reference formula");
}

/// The ramp from the literal constants of gamemd's scheme builder (0068c500): the cosine angle
/// starts at 0.19634954 for step 0 and at 0.34906585 + 0.08144870 * i after, the sine angle at
/// 0.87266463 + 0.04654211 * i, and both products are truncated toward zero.
void Test_Ramp_Matches_Gamemd_Constants(void)
{
	int mismatches = 0;
	for (int hue = 0; hue < 256; hue += 5) {
		for (int saturation = 0; saturation < 256; saturation += 3) {
			for (int value = 0; value < 256; value += 3) {
				HSVClass const base((unsigned char)hue, (unsigned char)saturation, (unsigned char)value);
				for (int step = 0; step < 16; step++) {
					double cosine_angle = (double)step * 0.08144869842640204 + 0.3490658503988659;
					double const sine_angle = (double)step * 0.046542113386515455 + 0.8726646259971648;
					if (step == 0) {
						cosine_angle = 0.19634954084936207;
					}
					int const want_saturation = (int)(std::sin(sine_angle) * saturation);
					int const want_value = (int)(std::cos(cosine_angle) * value);
					HSVClass const got = Player_Ramp_Entry(base, step);
					if (got.Get_Hue() != hue || got.Get_Saturation() != want_saturation || got.Get_Value() != want_value) {
						mismatches++;
					}
				}
			}
		}
	}
	Check(mismatches == 0, "every ramp step matches the constants gamemd builds a color scheme from");
}

void Test_Ramp_Is_Sensible(void)
{
	// A bright base gives lighter steps with a lower saturation and a darker last step.
	HSVClass const base(20, 200, 200);
	HSVClass const first = Player_Ramp_Entry(base, 0);
	HSVClass const last = Player_Ramp_Entry(base, 15);
	Check(first.Get_Value() > last.Get_Value(), "the ramp darkens from its first step to its last");
	Check(first.Get_Hue() == 20 && last.Get_Hue() == 20, "the ramp keeps the base hue");
	Check(last.Get_Value() <= 1, "the last step of a ramp is nearly black");
}

void Test_Packing(void)
{
	Check(!Player_Color_Is_Custom(0), "zero is a stock color, not a custom one");

	unsigned int const packed = Player_Color_Pack(HSVClass(10, 200, 180));
	Check(Player_Color_Is_Custom(packed), "a packed color is custom");

	HSVClass const back = Player_Color_Unpack(packed);
	Check(back.Get_Hue() == 10 && back.Get_Saturation() == 200 && back.Get_Value() == 180, "packing round trips the HSV");

	unsigned int const dark = Player_Color_Pack(HSVClass(0, 0, 0));
	Check(Player_Color_Is_Custom(dark), "a black color packs as custom");
	Check(Player_Color_Unpack(dark).Get_Value() == PLAYER_COLOR_MIN_VALUE, "a dark value is raised to the minimum");

	unsigned int const from_rgb = Player_Color_From_RGB(RGBClass(0, 0, 255));
	Check(Player_Color_Unpack(from_rgb).Get_Value() == 255, "a pure blue keeps full value");
	Check(Player_Color_Preview(from_rgb) == RGBClass(0, 0, 255), "the preview of a pure blue is pure blue");
}

void Test_Distance(void)
{
	RGBClass const red(255, 0, 0);
	RGBClass const black(0, 0, 0);
	RGBClass const white(255, 255, 255);

	Check(Player_Color_Distance(red, red) == 0.0, "a color is zero distance from itself");
	Check(Player_Color_Distance(black, white) > 80.0, "black and white are far apart");
	Check(Player_Color_Too_Close(red, RGBClass(250, 4, 4)), "two nearly equal reds are too close");
	Check(!Player_Color_Too_Close(black, white), "black and white are not too close");

	// The distance is the smallest over the three views, so a pair that a color-blind viewer
	// confuses measures small even when it is clearly different to normal vision.
	RGBClass const green(0, 160, 0);
	Check(Player_Color_Too_Close(red, green), "red and green are too close for a color-blind viewer");
}

void Test_Text_Round_Trip(void)
{
	char text[32];

	int const stock = Player_Color_Text(text, sizeof(text), 5, 0);
	Check(stock == 1 && std::strcmp(text, "5") == 0, "a stock choice is written as its index alone");
	int seat = -1;
	unsigned int custom = 1;
	Check(Player_Color_Parse(text, seat, custom) && seat == 5 && custom == 0, "a stock index reads back with no custom color");

	unsigned int const picked = Player_Color_Pack(HSVClass(77, 130, 201));
	Player_Color_Text(text, sizeof(text), 3, picked);
	Check(std::strcmp(text, "3x4D82C9") == 0, "a custom choice is written as index x hex");
	Check(Player_Color_Parse(text, seat, custom) && seat == 3 && custom == picked, "a custom choice reads back unchanged");

	// The index part alone still reads for a reader that ignores the custom part.
	Check(std::atol(text) == 3, "the index comes first, so an older reader takes it");

	unsigned int dark = 0;
	Check(Player_Color_Parse("2x000000", seat, dark) && seat == 2 && dark == Player_Color_Pack(HSVClass(0, 0, PLAYER_COLOR_MIN_VALUE)), "a dark custom value is raised on read");

	custom = 99;
	Check(!Player_Color_Parse("", seat, custom), "an empty color text is refused");
	Check(!Player_Color_Parse("x4", seat, custom), "a color text with no index is refused");
	Check(!Player_Color_Parse("3y123456", seat, custom), "an unknown suffix is refused");
	Check(Player_Color_Parse("3x012345", seat, custom) && custom == Player_Color_Pack(HSVClass(0x01, 0x23, PLAYER_COLOR_MIN_VALUE)),
		"a hex part reads as hue, saturation and value, with the value raised");
	Check(!Player_Color_Parse("3x1234567", seat, custom), "a long hex part is refused");
}

void Test_Presets(void)
{
	int const count = Player_Color_Preset_Count();
	Check(count == 22, "the lobby has twenty-two presets");

	for (int index = 0; index < 8; index++) {
		PlayerColorPreset const & preset = Player_Color_Preset_At(index);
		Check(preset.Stock == index, "the first eight presets are the stock colors in order");
	}

	// The Okabe-Ito picks must all be allowed together, so none may fall inside the minimum.
	for (int index = 15; index < count; index++) {
		PlayerColorPreset const & a = Player_Color_Preset_At(index);
		for (int other = index + 1; other < count; other++) {
			PlayerColorPreset const & b = Player_Color_Preset_At(other);
			RGBClass const first((a.RGB >> 16) & 0xFF, (a.RGB >> 8) & 0xFF, a.RGB & 0xFF);
			RGBClass const second((b.RGB >> 16) & 0xFF, (b.RGB >> 8) & 0xFF, b.RGB & 0xFF);
			Check(!Player_Color_Too_Close(first, second), "two Okabe-Ito picks are distinct");
		}
	}

	// Print the stock colors' pairwise distances, so the threshold can be judged against them.
	for (int a = 0; a < 8; a++) {
		for (int b = a + 1; b < 8; b++) {
			std::printf("stock %d vs %d: %.1f\n", a, b, Player_Color_Distance(CLASSIC[a], CLASSIC[b]));
		}
	}
}

}


int main(void)
{
	Test_Ramp_Matches_Reference();
	Test_Ramp_Matches_Gamemd_Constants();
	Test_Ramp_Is_Sensible();
	Test_Packing();
	Test_Distance();
	Test_Text_Round_Trip();
	Test_Presets();

	std::printf("playercolor: %d checks, %d failures\n", Checked, Failures);
	return(Failures == 0 ? 0 : 1);
}
