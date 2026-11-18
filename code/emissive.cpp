/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"
#include "emissive.h"

#include <algorithm>
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
	{"AnimLightPost",		160,	"Anims named NAPOST_A, NAPOST_AD and NAPOST_B, the glow of light posts."},
	{"VoxelSpecular",		128,	"Voxel specular highlight at row 31; lower rows scale down to zero at row 16."},
};

unsigned char Emissive_Draw_Strength = 0;
bool EmissiveActive = false;

namespace {

bool _DebugView = false;

// Emissive_Sync binds at most five surfaces (the screen pair, the map pair and the presented one); one slot is spare.
constexpr int MaxBindings = 6;

SurfacePlaneTable<MaxBindings> _Bindings;

// The one plane whose pixels the caller owns, and the surface it belongs to.
EmissivePlane _FixedPlane = {NULL, 0, 0, 0};
void const * _FixedSurface = NULL;

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


unsigned char Emissive_Parse_Strength(char const * text, unsigned char fallback)
{
	if (text == NULL || *text == '\0') {
		return(fallback);
	}
	if (stricmp(text, "yes") == 0) {
		return(255);
	}
	if (stricmp(text, "no") == 0) {
		return(0);
	}

	char * end = NULL;
	long value = std::strtol(text, &end, 10);
	if (end == text || *end != '\0') {
		return(fallback);
	}
	return((unsigned char)std::clamp(value, 0L, 255L));
}


unsigned char Emissive_Anim_Name_Default(char const * name)
{
	if (name == NULL) {
		return(0);
	}
	if (stricmp(name, "NAPOST_A") == 0 || stricmp(name, "NAPOST_AD") == 0 || stricmp(name, "NAPOST_B") == 0) {
		return(EmissiveDefaults[EMISSIVE_ANIM_LIGHT_POST].Strength);
	}
	return(0);
}


unsigned char Emissive_Voxel_Highlight(int shade_row)
{
	int const top = EmissiveDefaults[EMISSIVE_VOXEL_SPECULAR].Strength;
	if (shade_row <= EMISSIVE_VOXEL_LAST_PLAIN_ROW) {
		return(0);
	}
	if (shade_row >= 31) {
		return((unsigned char)top);
	}
	return((unsigned char)((shade_row - EMISSIVE_VOXEL_LAST_PLAIN_ROW) * top / (31 - EMISSIVE_VOXEL_LAST_PLAIN_ROW)));
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


void Emissive_Bind_Fixed(void const * surface, unsigned char * pixels, int width, int height)
{
	_FixedSurface = surface;
	_FixedPlane.Pixels = pixels;
	_FixedPlane.Width = width;
	_FixedPlane.Height = height;
	_FixedPlane.Pitch = width;
}


EmissivePlane * Emissive_Plane_For(void const * surface)
{
	if (surface == NULL) {
		return(NULL);
	}
	if (surface == _FixedSurface && _FixedPlane.Pixels != NULL) {
		return(EmissiveActive ? &_FixedPlane : NULL);
	}
	return(_Bindings.Find(surface));
}


EmissivePlane * Emissive_Bind(void const * surface, int width, int height)
{
	if (surface == NULL || width <= 0 || height <= 0) {
		return(NULL);
	}
	return(_Bindings.Acquire(surface, width, height, true));
}


void Emissive_Retain(void const * const * keep, int count)
{
	_Bindings.Retain(keep, count);
}


void Emissive_Forget(void const * surface)
{
	_Bindings.Forget(surface);
}


void Emissive_Release_All(void)
{
	_Bindings.Release_All();
}


// Records that the columns from left up to right of a row may now hold values.
static void Widen_Lit(EmissivePlane & plane, int row, int left, int right)
{
	if (plane.Lit == NULL || left >= right) {
		return;
	}
	if (plane.Lit[row] == 0) {
		plane.Lit[row] = 1;
		plane.LitLeft[row] = left;
		plane.LitRight[row] = right;
		return;
	}
	plane.LitLeft[row] = std::min(plane.LitLeft[row], left);
	plane.LitRight[row] = std::max(plane.LitRight[row], right);
}


// Sets the columns from left up to right of a row to zero, writing only those that may hold a value.
static void Clear_Columns(EmissivePlane & plane, int row, int left, int right)
{
	int from = left;
	int to = right;
	if (plane.Lit != NULL) {
		from = std::max(from, plane.LitLeft[row]);
		to = std::min(to, plane.LitRight[row]);
	}
	if (from < to) {
		std::memset(plane.Pixels + row * plane.Pitch + from, 0, (size_t)(to - from));
	}
}


// Records that the columns from left up to right of a row now hold zero.
static void Narrow_Lit(EmissivePlane & plane, int row, int left, int right)
{
	if (plane.Lit == NULL || plane.Lit[row] == 0 || left >= right) {
		return;
	}
	int & from = plane.LitLeft[row];
	int & to = plane.LitRight[row];
	if (left <= from && right >= to) {
		plane.Lit[row] = 0;
	} else if (left <= from && right > from) {
		from = right;
	} else if (right >= to && left < to) {
		to = left;
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
		if (value != 0) {
			std::memset(plane.Pixels + row * plane.Pitch + left, value, (size_t)(right - left));
			Widen_Lit(plane, row, left, right);
		} else if (Emissive_Row_Lit(plane, row)) {
			Clear_Columns(plane, row, left, right);
			Narrow_Lit(plane, row, left, right);
		}
	}
}


void Emissive_Mark_Rows(EmissivePlane & plane, int top, int count, int left, int right)
{
	int const first = std::max(top, 0);
	int const last = std::min(top + count, plane.Height);
	left = std::max(left, 0);
	right = std::min(right, plane.Width);
	for (int row = first; row < last; row++) {
		Widen_Lit(plane, row, left, right);
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
		unsigned char * out = dest.Pixels + (dy + row) * dest.Pitch + dx;
		if (Emissive_Row_Lit(source, sy + row)) {
			// The source's columns are read before the destination's change, as both can be the same row.
			int left = sx;
			int right = sx + width;
			if (source.Lit != NULL) {
				left = std::max(left, source.LitLeft[sy + row]);
				right = std::min(right, source.LitRight[sy + row]);
			}
			std::memmove(out, source.Pixels + (sy + row) * source.Pitch + sx, (size_t)width);
			Narrow_Lit(dest, dy + row, dx, dx + width);
			Widen_Lit(dest, dy + row, left + dx - sx, right + dx - sx);
		} else if (Emissive_Row_Lit(dest, dy + row)) {
			Clear_Columns(dest, dy + row, dx, dx + width);
			Narrow_Lit(dest, dy + row, dx, dx + width);
		}
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
			Widen_Lit(plane, y, x, x + 1);
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
