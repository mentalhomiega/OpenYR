/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

// Colour maths for HDR10 output. The composite shader performs the same steps on the GPU, and
// these functions are the reference that the unit test checks them against.

// The paper white setting's limits in nits, and the value used when the setting is automatic.
int const HDR_PAPER_WHITE_MIN = 80;
int const HDR_PAPER_WHITE_MAX = 480;
int const HDR_PAPER_WHITE_DEFAULT = 200;

// The luminance that ST.2084 encodes at full scale, in nits.
float const HDR_PQ_PEAK_NITS = 10000.0f;

// The peak brightness used when the display reports none.
int const HDR_PEAK_FALLBACK_NITS = 1000;

// Limits a paper white setting to its range.
int HDR_Clamp_Paper_White(int nits);

// Limits a peak brightness setting to the range from the paper white up to the ST.2084 peak.
int HDR_Clamp_Peak(int nits, int paperwhite);

// Decodes an sRGB channel value to linear light. Input outside [0, 1] is clamped first.
float HDR_SRGB_To_Linear(float encoded);

// Converts linear Rec.709 RGB to linear Rec.2020 RGB. The arrays hold three channels each.
void HDR_Rec709_To_Rec2020(float const * linear, float * out);

// The colour adjustment the composite pass applies to linear light. It runs in this order: the
// 3x3 matrix (row-major, applied to RGB as a column), then the brightness multiplier, then the gamma
// exponent. The identity leaves colour unchanged.
struct HDRColourAdjust
{
	float Matrix[9];
	float Brightness;
	float Gamma;
};

// Sets an adjustment to the identity.
void HDR_Colour_Adjust_Reset(HDRColourAdjust & adjust);

// True when the adjustment changes nothing, so the composite pass can be skipped.
bool HDR_Colour_Adjust_Is_Identity(HDRColourAdjust const & adjust);

// Applies the adjustment to one linear RGB pixel. Values above 1 stay above 1 after brightness,
// and a negative value is raised to zero before the gamma exponent.
void HDR_Apply_Colour_Adjust(HDRColourAdjust const & adjust, float const * linear, float * out);

// The paper white in nits for a paper white setting. Zero takes Windows' SDR content brightness
// when it is known, and HDR_PAPER_WHITE_DEFAULT otherwise.
int HDR_Resolve_Paper_White(int setting, bool sdrknown, float sdrnits);

// The peak brightness in nits for a peak setting. Zero takes the reported peak, or
// HDR_PEAK_FALLBACK_NITS when the display reports none. The result is never below the paper white.
int HDR_Resolve_Peak(int setting, float reported, int paperwhite);

// Encodes an absolute luminance in nits as ST.2084 (PQ) in [0, 1]. Luminance above the PQ peak
// encodes as 1.
float HDR_PQ_Encode(float nits);

// Converts one sRGB pixel to PQ code values in Rec.2020. SDR white maps to paperwhite nits, and each
// channel is limited to peak nits before it is encoded.
void HDR_Encode_Pixel(float const * srgb, float paperwhite, float peak, float * pq);
