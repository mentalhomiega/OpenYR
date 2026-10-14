/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Checks the HDR10 colour maths against values worked out from ST.2084, BT.2087 and the sRGB
// curve. Needs no GPU, display or game data.

#include "hdrcolour.h"

#include <cmath>
#include <cstdio>

namespace
{

int checks = 0;
int failures = 0;


void Expect_Near(char const * name, float actual, float expected, float tolerance)
{
	checks++;
	if (std::fabs(actual - expected) > tolerance) {
		failures++;
		std::printf("FAIL %s: got %.7f, expected %.7f\n", name, actual, expected);
	}
}


void Expect_True(char const * name, bool condition)
{
	checks++;
	if (!condition) {
		failures++;
		std::printf("FAIL %s\n", name);
	}
}


void Check_SRGB(void)
{
	Expect_Near("srgb 0 is black", HDR_SRGB_To_Linear(0.0f), 0.0f, 1e-7f);
	Expect_Near("srgb 1 is full scale", HDR_SRGB_To_Linear(1.0f), 1.0f, 1e-6f);
	Expect_Near("srgb knee", HDR_SRGB_To_Linear(0.04045f), 0.0031308f, 1e-6f);
	Expect_Near("srgb mid grey", HDR_SRGB_To_Linear(0.5f), 0.2140411f, 1e-5f);
	Expect_Near("srgb below zero clamps", HDR_SRGB_To_Linear(-0.5f), 0.0f, 1e-7f);
	Expect_Near("srgb above one clamps", HDR_SRGB_To_Linear(2.0f), 1.0f, 1e-6f);
}


void Check_PQ(void)
{
	Expect_Near("pq 0 nits", HDR_PQ_Encode(0.0f), 0.0f, 1e-5f);
	Expect_Near("pq 1 nit", HDR_PQ_Encode(1.0f), 0.1499457f, 1e-5f);
	Expect_Near("pq 80 nits", HDR_PQ_Encode(80.0f), 0.4858570f, 1e-5f);
	Expect_Near("pq 100 nits", HDR_PQ_Encode(100.0f), 0.5080784f, 1e-5f);
	Expect_Near("pq 200 nits", HDR_PQ_Encode(200.0f), 0.5791332f, 1e-5f);
	Expect_Near("pq 1000 nits", HDR_PQ_Encode(1000.0f), 0.7518270f, 1e-5f);
	Expect_Near("pq 10000 nits is full scale", HDR_PQ_Encode(10000.0f), 1.0f, 1e-5f);
	Expect_Near("pq above 10000 nits clamps", HDR_PQ_Encode(20000.0f), 1.0f, 1e-5f);
	Expect_True("pq rises with luminance", HDR_PQ_Encode(100.0f) < HDR_PQ_Encode(1000.0f));
}


void Check_Primaries(void)
{
	float const white[3] = {1.0f, 1.0f, 1.0f};
	float out[3];
	HDR_Rec709_To_Rec2020(white, out);
	for (int channel = 0; channel < 3; channel++) {
		Expect_Near("white keeps its level in Rec.2020", out[channel], 1.0f, 2e-5f);
	}

	float const red[3] = {1.0f, 0.0f, 0.0f};
	HDR_Rec709_To_Rec2020(red, out);
	Expect_Near("red primary red output", out[0], 0.627404f, 1e-6f);
	Expect_Near("red primary green output", out[1], 0.069097f, 1e-6f);
	Expect_Near("red primary blue output", out[2], 0.016391f, 1e-6f);
}


void Check_Pixels(void)
{
	float const sdr_white[3] = {1.0f, 1.0f, 1.0f};
	float const sdr_black[3] = {0.0f, 0.0f, 0.0f};
	float const sdr_grey[3] = {0.5f, 0.5f, 0.5f};
	float pq[3];

	// SDR white lands at the paper white, so 200 nits encodes as ST.2084 of 200 nits.
	HDR_Encode_Pixel(sdr_white, 200.0f, 10000.0f, pq);
	for (int channel = 0; channel < 3; channel++) {
		Expect_Near("paper white encodes at 200 nits", pq[channel], 0.5791332f, 1e-4f);
	}

	HDR_Encode_Pixel(sdr_black, 200.0f, 10000.0f, pq);
	for (int channel = 0; channel < 3; channel++) {
		Expect_Near("black stays black", pq[channel], 0.0f, 1e-5f);
	}

	// Mid grey is 0.2140411 of white in linear light, so 42.8082 nits.
	HDR_Encode_Pixel(sdr_grey, 200.0f, 10000.0f, pq);
	for (int channel = 0; channel < 3; channel++) {
		Expect_Near("mid grey encodes at 42.8 nits", pq[channel], 0.4256226f, 1e-4f);
	}

	// A peak below the paper white limits the encoded luminance.
	HDR_Encode_Pixel(sdr_white, 200.0f, 100.0f, pq);
	for (int channel = 0; channel < 3; channel++) {
		Expect_Near("peak clamps the output", pq[channel], 0.5080784f, 1e-4f);
	}
}


void Check_Adjust(void)
{
	HDRColourAdjust adjust;
	HDR_Colour_Adjust_Reset(adjust);
	Expect_True("reset adjustment is the identity", HDR_Colour_Adjust_Is_Identity(adjust));

	float const linear[3] = {0.2f, 0.4f, 0.6f};
	float out[3];
	HDR_Apply_Colour_Adjust(adjust, linear, out);
	for (int channel = 0; channel < 3; channel++) {
		Expect_Near("identity leaves linear light alone", out[channel], linear[channel], 1e-6f);
	}

	// A channel swap, then brightness 0.5 and gamma 2. In that order the first channel is
	// (0.4 * 0.5)^2 = 0.04; gamma before brightness would give 0.4^2 * 0.5 = 0.08.
	HDR_Colour_Adjust_Reset(adjust);
	adjust.Matrix[0] = 0.0f;
	adjust.Matrix[1] = 1.0f;
	adjust.Matrix[3] = 1.0f;
	adjust.Matrix[4] = 0.0f;
	adjust.Brightness = 0.5f;
	adjust.Gamma = 2.0f;
	Expect_True("swap is not the identity", !HDR_Colour_Adjust_Is_Identity(adjust));
	HDR_Apply_Colour_Adjust(adjust, linear, out);
	Expect_Near("matrix, brightness, gamma: red", out[0], 0.04f, 1e-6f);
	Expect_Near("matrix, brightness, gamma: green", out[1], 0.01f, 1e-6f);
	Expect_Near("matrix, brightness, gamma: blue", out[2], 0.09f, 1e-6f);

	// Brightness above one is kept in linear light for the paper white scale to limit later.
	HDR_Colour_Adjust_Reset(adjust);
	adjust.Brightness = 2.0f;
	float const bright[3] = {0.8f, 0.8f, 0.8f};
	HDR_Apply_Colour_Adjust(adjust, bright, out);
	Expect_Near("brightness above one is not clipped", out[0], 1.6f, 1e-6f);
}


void Check_Resolve(void)
{
	Expect_True("automatic paper white follows a known Windows level", HDR_Resolve_Paper_White(0, true, 80.0f) == 80);
	Expect_True("automatic paper white rounds the Windows level", HDR_Resolve_Paper_White(0, true, 159.6f) == 160);
	Expect_True("automatic paper white clamps a high Windows level", HDR_Resolve_Paper_White(0, true, 2000.0f) == 480);
	Expect_True("automatic paper white falls back to 200", HDR_Resolve_Paper_White(0, false, 0.0f) == 200);
	Expect_True("explicit paper white is used", HDR_Resolve_Paper_White(300, true, 80.0f) == 300);
	Expect_True("explicit paper white clamps up", HDR_Resolve_Paper_White(50, true, 80.0f) == 80);

	Expect_True("automatic peak follows the reported peak", HDR_Resolve_Peak(0, 1000.0f, 200) == 1000);
	Expect_True("automatic peak falls back to 1000", HDR_Resolve_Peak(0, 0.0f, 200) == HDR_PEAK_FALLBACK_NITS);
	Expect_True("automatic peak clamps to 10000", HDR_Resolve_Peak(0, 20000.0f, 200) == 10000);
	Expect_True("automatic peak rises to paper white", HDR_Resolve_Peak(0, 100.0f, 480) == 480);
	Expect_True("explicit peak is used", HDR_Resolve_Peak(1500, 400.0f, 200) == 1500);
	Expect_True("explicit peak rises to paper white", HDR_Resolve_Peak(100, 400.0f, 200) == 200);
}


void Check_Limits(void)
{
	Expect_True("paper white below range clamps up", HDR_Clamp_Paper_White(79) == 80);
	Expect_True("paper white above range clamps down", HDR_Clamp_Paper_White(481) == 480);
	Expect_True("paper white inside range is kept", HDR_Clamp_Paper_White(200) == 200);
	Expect_True("peak below paper white raises to it", HDR_Clamp_Peak(50, 200) == 200);
	Expect_True("peak inside its range is kept", HDR_Clamp_Peak(1000, 200) == 1000);
	Expect_True("peak above 10000 clamps down", HDR_Clamp_Peak(20000, 200) == 10000);
}

}


int main()
{
	Check_SRGB();
	Check_PQ();
	Check_Primaries();
	Check_Pixels();
	Check_Limits();
	Check_Adjust();
	Check_Resolve();

	std::printf("hdrcolour: %d checks, %d failures\n", checks, failures);
	return(failures == 0 ? 0 : 1);
}
