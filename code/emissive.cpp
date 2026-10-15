/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "emissive.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>


// The defaults are one table so tuning a value touches one row. Keep the row order equal to
// the EmissiveKind enum.
EmissiveDefault const EmissiveDefaults[EMISSIVE_KIND_COUNT] = {
	{"AnimTranslucent",		96,		"Anims with Translucent=yes, such as smoke."},
	{"AnimSelfLit",			200,	"Anims with UseNormalLight=yes, such as explosions and flames."},
	{"AnimTranslucency",	96,		"Anims with Translucency=25, 50 or 75."},
	{"AnimTiberium",		96,		"Anims with IsAnimatedTiberium=yes."},
	{"Laser",				220,	"Beams of weapons with IsLaser=yes, including Prism beams."},
	{"ElectricBolt",		230,	"Arcs of weapons with IsElectricBolt=yes."},
	{"VoxelSpecular",		128,	"Voxel specular highlight at row 31; lower rows scale down to zero at row 16."},
};

unsigned char Emissive_Draw_Strength = 0;
bool EmissiveActive = false;

namespace {

bool _DebugView = false;

// Planes are bound to a few long-lived surfaces, so a short list is enough.
constexpr int MaxBindings = 4;

struct Binding
{
	void const * Surface;
	EmissivePlane Plane;
};

Binding _Bindings[MaxBindings];

}


unsigned char Emissive_Anim_Default(bool translucent, bool self_lit, int translucency, bool tiberium)
{
	unsigned char strength = 0;
	if (translucent) {
		strength = std::max(strength, EmissiveDefaults[EMISSIVE_ANIM_TRANSLUCENT].Strength);
	}
	if (self_lit) {
		strength = std::max(strength, EmissiveDefaults[EMISSIVE_ANIM_SELF_LIT].Strength);
	}
	if (translucency == 25 || translucency == 50 || translucency == 75) {
		strength = std::max(strength, EmissiveDefaults[EMISSIVE_ANIM_TRANSLUCENCY].Strength);
	}
	if (tiberium) {
		strength = std::max(strength, EmissiveDefaults[EMISSIVE_ANIM_TIBERIUM].Strength);
	}
	return(strength);
}


namespace {

bool Equals_Ignore_Case(char const * text, char const * word)
{
	for (; *text != '\0' && *word != '\0'; text++, word++) {
		if (std::tolower((unsigned char)*text) != std::tolower((unsigned char)*word)) {
			return(false);
		}
	}
	return(*text == *word);
}

}


unsigned char Emissive_Parse_Strength(char const * text, unsigned char fallback)
{
	if (text == NULL || *text == '\0') {
		return(fallback);
	}
	if (Equals_Ignore_Case(text, "yes")) {
		return(255);
	}
	if (Equals_Ignore_Case(text, "no")) {
		return(0);
	}

	char * end = NULL;
	long value = std::strtol(text, &end, 10);
	if (end == text || *end != '\0') {
		return(fallback);
	}
	return((unsigned char)std::clamp(value, 0L, 255L));
}


unsigned char Emissive_Voxel_Highlight(int shade_row)
{
	int const top = EmissiveDefaults[EMISSIVE_VOXEL_SPECULAR].Strength;
	if (shade_row <= 16) {
		return(0);
	}
	if (shade_row >= 31) {
		return((unsigned char)top);
	}
	return((unsigned char)((shade_row - 16) * top / 15));
}


unsigned char Emissive_Scale_By_Opacity(unsigned char strength, int opacity)
{
	int const clamped = std::clamp(opacity, 0, 255);
	return((unsigned char)((strength * clamped + 127) / 255));
}


float Emissive_Gain(unsigned char mask, float peak_over_paper)
{
	return(1.0f + (mask / 255.0f) * (peak_over_paper - 1.0f));
}


float Emissive_Output_Nits(float linear, unsigned char mask, float paper_white, float peak)
{
	float const boosted = paper_white * linear * Emissive_Gain(mask, peak / paper_white);
	return(std::min(peak, boosted));
}


void Emissive_Set_Enabled(bool enabled)
{
	EmissiveActive = enabled;
}


bool Emissive_Debug_View(void)
{
	return(_DebugView);
}


void Emissive_Set_Debug_View(bool debug)
{
	_DebugView = debug;
}


EmissivePlane * Emissive_Plane_For(void const * surface)
{
	if (surface == NULL) {
		return(NULL);
	}
	for (Binding & binding : _Bindings) {
		if (binding.Surface == surface && binding.Plane.Pixels != NULL) {
			return(&binding.Plane);
		}
	}
	return(NULL);
}


EmissivePlane * Emissive_Bind(void const * surface, int width, int height)
{
	if (surface == NULL || width <= 0 || height <= 0) {
		return(NULL);
	}

	Binding * slot = NULL;
	for (Binding & binding : _Bindings) {
		if (binding.Surface == surface) {
			slot = &binding;
			break;
		}
		if (slot == NULL && binding.Plane.Pixels == NULL) {
			slot = &binding;
		}
	}
	if (slot == NULL) {
		return(NULL);
	}

	EmissivePlane & plane = slot->Plane;
	if (slot->Surface == surface && plane.Pixels != NULL && plane.Width == width && plane.Height == height) {
		return(&plane);
	}

	delete [] plane.Pixels;
	plane.Pixels = new unsigned char[(size_t)width * height];
	std::memset(plane.Pixels, 0, (size_t)width * height);
	plane.Width = width;
	plane.Height = height;
	plane.Pitch = width;
	slot->Surface = surface;
	return(&plane);
}


void Emissive_Retain(void const * const * keep, int count)
{
	for (Binding & binding : _Bindings) {
		if (binding.Plane.Pixels == NULL) {
			continue;
		}
		bool kept = false;
		for (int index = 0; index < count; index++) {
			if (keep[index] == binding.Surface) {
				kept = true;
			}
		}
		if (!kept) {
			delete [] binding.Plane.Pixels;
			binding.Plane.Pixels = NULL;
			binding.Plane.Width = 0;
			binding.Plane.Height = 0;
			binding.Plane.Pitch = 0;
			binding.Surface = NULL;
		}
	}
}


void Emissive_Release_All(void)
{
	for (Binding & binding : _Bindings) {
		delete [] binding.Plane.Pixels;
		binding.Plane.Pixels = NULL;
		binding.Plane.Width = 0;
		binding.Plane.Height = 0;
		binding.Plane.Pitch = 0;
		binding.Surface = NULL;
	}
}


void Emissive_Fill_Rect(EmissivePlane & plane, int x, int y, int width, int height, unsigned char value)
{
	int const left = std::max(x, 0);
	int const top = std::max(y, 0);
	int const right = std::min(x + width, plane.Width);
	int const bottom = std::min(y + height, plane.Height);
	if (left >= right || top >= bottom) {
		return;
	}
	for (int row = top; row < bottom; row++) {
		std::memset(plane.Pixels + row * plane.Pitch + left, value, (size_t)(right - left));
	}
}


void Emissive_Copy_Rect(EmissivePlane & dest, int dx, int dy, EmissivePlane const & source, int sx, int sy, int width, int height)
{
	// Clip the source first, then the destination, moving the origin with each cut.
	if (sx < 0) { dx -= sx; width += sx; sx = 0; }
	if (sy < 0) { dy -= sy; height += sy; sy = 0; }
	if (dx < 0) { sx -= dx; width += dx; dx = 0; }
	if (dy < 0) { sy -= dy; height += dy; dy = 0; }
	width = std::min(width, std::min(source.Width - sx, dest.Width - dx));
	height = std::min(height, std::min(source.Height - sy, dest.Height - dy));
	if (width <= 0 || height <= 0) {
		return;
	}

	// Rows run bottom up when a copy within one plane moves down, so no source row is overwritten first.
	bool const reverse = dest.Pixels == source.Pixels && dy > sy;
	for (int step = 0; step < height; step++) {
		int const row = reverse ? height - 1 - step : step;
		std::memmove(dest.Pixels + (dy + row) * dest.Pitch + dx, source.Pixels + (sy + row) * source.Pitch + sx, (size_t)width);
	}
}


void Emissive_Stamp_Segment(EmissivePlane & plane, int clipx, int clipy, int clipwidth, int clipheight, int x0, int y0, int x1, int y1, unsigned char value)
{
	// Bresenham walk over the whole line; points outside the clip rectangle are skipped.
	int const dx = std::abs(x1 - x0);
	int const dy = -std::abs(y1 - y0);
	int const sx = x0 < x1 ? 1 : -1;
	int const sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;
	int x = x0;
	int y = y0;
	for (;;) {
		if (x >= clipx && x < clipx + clipwidth && y >= clipy && y < clipy + clipheight && x >= 0 && x < plane.Width && y >= 0 && y < plane.Height) {
			plane.Pixels[y * plane.Pitch + x] = value;
		}
		if (x == x1 && y == y1) {
			break;
		}
		int const e2 = 2 * err;
		if (e2 >= dy) {
			err += dy;
			x += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y += sy;
		}
	}
}
