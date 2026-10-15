/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "hdrcolour.h"

#include <algorithm>
#include <cmath>


namespace
{

// ITU-R BT.2087 conversion from linear Rec.709 to linear Rec.2020. Each row gives one output channel.
double const Rec709_To_Rec2020[3][3] = {
	{0.627404, 0.329282, 0.043313},
	{0.069097, 0.919540, 0.011362},
	{0.016391, 0.088013, 0.895595},
};

// ST.2084 constants, as published in the standard.
double const PQ_M1 = 2610.0 / 16384.0;
double const PQ_M2 = 2523.0 / 4096.0 * 128.0;
double const PQ_C1 = 3424.0 / 4096.0;
double const PQ_C2 = 2413.0 / 4096.0 * 32.0;
double const PQ_C3 = 2392.0 / 4096.0 * 32.0;

}


int HDR_Clamp_Paper_White(int nits)
{
	return(std::min(std::max(nits, HDR_PAPER_WHITE_MIN), HDR_PAPER_WHITE_MAX));
}


int HDR_Clamp_Peak(int nits, int paperwhite)
{
	return(std::min(std::max(nits, paperwhite), (int)HDR_PQ_PEAK_NITS));
}


float HDR_SRGB_To_Linear(float encoded)
{
	double const value = std::clamp((double)encoded, 0.0, 1.0);
	if (value <= 0.04045) {
		return((float)(value / 12.92));
	}
	return((float)std::pow((value + 0.055) / 1.055, 2.4));
}


void HDR_Rec709_To_Rec2020(float const * linear, float * out)
{
	for (int row = 0; row < 3; row++) {
		out[row] = (float)(Rec709_To_Rec2020[row][0] * linear[0]
			+ Rec709_To_Rec2020[row][1] * linear[1]
			+ Rec709_To_Rec2020[row][2] * linear[2]);
	}
}


float HDR_PQ_Encode(float nits)
{
	double const normal = std::clamp((double)nits / (double)HDR_PQ_PEAK_NITS, 0.0, 1.0);
	double const power = std::pow(normal, PQ_M1);
	return((float)std::pow((PQ_C1 + PQ_C2 * power) / (1.0 + PQ_C3 * power), PQ_M2));
}


void HDR_Encode_Pixel(float const * srgb, float paperwhite, float peak, float * pq)
{
	float linear[3];
	float rec2020[3];
	for (int channel = 0; channel < 3; channel++) {
		linear[channel] = HDR_SRGB_To_Linear(srgb[channel]) * paperwhite;
	}
	HDR_Rec709_To_Rec2020(linear, rec2020);
	for (int channel = 0; channel < 3; channel++) {
		pq[channel] = HDR_PQ_Encode(std::min(std::max(rec2020[channel], 0.0f), peak));
	}
}


void HDR_Colour_Adjust_Reset(HDRColourAdjust & adjust)
{
	for (int index = 0; index < 9; index++) {
		adjust.Matrix[index] = (index % 4 == 0) ? 1.0f : 0.0f;
	}
	adjust.Brightness = 1.0f;
	adjust.Gamma = 1.0f;
}


bool HDR_Colour_Adjust_Is_Identity(HDRColourAdjust const & adjust)
{
	for (int index = 0; index < 9; index++) {
		if (adjust.Matrix[index] != ((index % 4 == 0) ? 1.0f : 0.0f)) {
			return(false);
		}
	}
	return(adjust.Brightness == 1.0f && adjust.Gamma == 1.0f);
}


void HDR_Apply_Colour_Adjust(HDRColourAdjust const & adjust, float const * linear, float * out)
{
	float mixed[3];
	for (int row = 0; row < 3; row++) {
		mixed[row] = adjust.Matrix[row * 3] * linear[0]
			+ adjust.Matrix[row * 3 + 1] * linear[1]
			+ adjust.Matrix[row * 3 + 2] * linear[2];
	}
	for (int channel = 0; channel < 3; channel++) {
		float const bright = std::max(mixed[channel] * adjust.Brightness, 0.0f);
		out[channel] = std::pow(bright, adjust.Gamma);
	}
}


int HDR_Resolve_Paper_White(int setting, bool sdrknown, float sdrnits)
{
	if (setting > 0) {
		return(HDR_Clamp_Paper_White(setting));
	}
	if (sdrknown) {
		return(HDR_Clamp_Paper_White((int)std::lround(sdrnits)));
	}
	return(HDR_PAPER_WHITE_DEFAULT);
}


int HDR_Resolve_Peak(int setting, float reported, int paperwhite)
{
	if (setting > 0) {
		return(HDR_Clamp_Peak(setting, paperwhite));
	}
	if (reported > 0.0f) {
		return(HDR_Clamp_Peak((int)std::lround(reported), paperwhite));
	}
	return(HDR_Clamp_Peak(HDR_PEAK_FALLBACK_NITS, paperwhite));
}
