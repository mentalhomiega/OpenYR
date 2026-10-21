/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Connects the emissive planes to the surfaces the blitters draw into. Every entry point here
// returns at once when no plane is bound, which is the case whenever masks are off.

#include "always.h"
#include "blit.h"
#include "_alpha.h"
#include "_zbuffer.h"

#include "emissive.h"
#include "emissive_surface.h"

#include "point.h"
#include "rect.h"
#include "rlerle.h"
#include "surface.h"
#include "zbuffer.h"


/// <summary>
/// Binds planes to the surfaces the presenter reads and releases any other plane. Surfaces
/// that are NULL are skipped.
/// </summary>
void Emissive_Sync(Surface const * composite, Surface const * map_composite, Surface const * map_tile, Surface const * visible)
{
	Surface const * surfaces[4] = {composite, map_composite, map_tile, visible};
	void const * keep[4] = {NULL, NULL, NULL, NULL};
	int count = 0;

	if (Emissive_Is_Enabled()) {
		for (Surface const * surface : surfaces) {
			if (surface != NULL) {
				keep[count++] = surface;
			}
		}
	}
	Emissive_Retain(keep, count);

	for (int index = 0; index < count; index++) {
		Emissive_Bind(keep[index], ((Surface const *)keep[index])->Get_Width(), ((Surface const *)keep[index])->Get_Height());
	}
}


/// <summary>
/// Copies the emissive values of the source rectangle to the destination after a Bit_Blit, or
/// clears the destination when the source has no plane.
/// </summary>
void Emissive_After_Bit_Blit(Surface & dest, int destx, int desty, Surface const & source, int sourcex, int sourcey, int width, int height)
{
	EmissivePlane * dplane = Emissive_Plane_For(&dest);
	if (dplane == NULL) {
		return;
	}
	EmissivePlane const * splane = Emissive_Plane_For(&source);
	if (splane != NULL) {
		Emissive_Copy_Rect(*dplane, destx, desty, *splane, sourcex, sourcey, width, height);
	} else {
		Emissive_Fill_Rect(*dplane, destx, desty, width, height, 0);
	}
}


/// <summary>
/// Clears the emissive values under a rectangle that was filled with a plain colour.
/// </summary>
void Emissive_After_Fill(Surface & dest, Rect const & rect)
{
	EmissivePlane * dplane = Emissive_Plane_For(&dest);
	if (dplane != NULL) {
		Emissive_Fill_Rect(*dplane, rect.X, rect.Y, rect.Width, rect.Height, 0);
	}
}


/// <summary>
/// Writes the draw strength to the pixels of one RLE row that the blitter draws. The row starts
/// at mask and the source is the RLE data after the row's length word. zbuf and zshape follow
/// the blitter's depth test and are NULL when the surface has no depth buffer.
/// </summary>
void Emissive_RLE_Row(unsigned char * mask, void const * source, int length, int leadskip, unsigned char strength, unsigned short * zbuf, signed char const * zshape, int zmin)
{
	unsigned char const * sptr = (unsigned char const *)source;

	if (leadskip > 0) {
		int transcount = Skip_Leading_Pixels(sptr, leadskip);
		mask += transcount;
		length -= transcount;
		if (zbuf != NULL) {
			zbuf = Blit_Wrap_Z_Buffer(zbuf + transcount);
		}
		if (zshape != NULL) {
			zshape += transcount;
		}
	}

	while (length > 0) {
		unsigned char value = *sptr++;
		if (value == 0) {
			value = *sptr++;
			length -= value;
			mask += value;
			if (zbuf != NULL) {
				zbuf = Blit_Wrap_Z_Buffer(zbuf + value);
			}
			if (zshape != NULL) {
				zshape += value;
			}
			continue;
		}

		// The blitter draws an opaque pixel only where its depth passes the depth buffer.
		bool drawn = true;
		if (zbuf != NULL) {
			int const offset = zshape != NULL ? *zshape : 0;
			drawn = (zmin - offset) < (int)*zbuf;
		}
		if (drawn) {
			*mask = strength;
		}

		length--;
		mask++;
		if (zbuf != NULL) {
			zbuf = Blit_Wrap_Z_Buffer(zbuf + 1);
		}
		if (zshape != NULL) {
			zshape++;
		}
	}
}


/// <summary>
/// Writes the draw strength along a line of pixels, for effects that draw lines rather than
/// shapes. The line is clipped to the rectangle.
/// </summary>
void Emissive_Stamp_Line_On(Surface & surface, Rect const & clip, Point2D const & start, Point2D const & end, unsigned char strength)
{
	EmissivePlane * plane = Emissive_Plane_For(&surface);
	if (plane != NULL) {
		Emissive_Stamp_Segment(*plane, clip.X, clip.Y, clip.Width, clip.Height, start.X, start.Y, end.X, end.Y, strength);
	}
}
