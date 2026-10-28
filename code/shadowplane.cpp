/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "shadowplane.h"

#include "_surface.h"
#include "blitter.h"
#include "emissive_surface.h"

#include <algorithm>


namespace {

struct Binding
{
	void const * Surface = NULL;
	EmissivePlane Plane = {};
};

}

static bool _Enabled = false;
static bool _Shadows = false;
static bool _Water = false;
static Binding _Bindings[4];


static void Free_Plane(Binding & binding)
{
	delete [] binding.Plane.Pixels;
	binding = Binding();
}


bool ShadowPlane_Is_Enabled(void)
{
	return(_Enabled);
}


bool ShadowPlane_Keeps_Water(void)
{
	return(_Water);
}


bool ShadowPlane_Set_Enabled(bool shadows, bool water)
{
	if (shadows == _Shadows && water == _Water) {
		return(false);
	}
	_Shadows = shadows;
	_Water = water;
	_Enabled = shadows || water;
	if (!_Enabled) {
		for (Binding & binding : _Bindings) {
			Free_Plane(binding);
		}
	}
	return(true);
}


void ShadowPlane_Sync(Surface const * composite, Surface const * tile, Surface const * map_composite, Surface const * map_tile)
{
	Surface const * const surfaces[4] = { composite, tile, map_composite, map_tile };
	for (Binding & binding : _Bindings) {
		if (binding.Plane.Pixels != NULL && (!_Enabled || std::find(std::begin(surfaces), std::end(surfaces), binding.Surface) == std::end(surfaces))) {
			Free_Plane(binding);
		}
	}
	if (!_Enabled) {
		return;
	}
	for (Surface const * surface : surfaces) {
		if (surface == NULL) {
			continue;
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
			continue;
		}
		int const width = surface->Get_Width();
		int const height = surface->Get_Height();
		if (slot->Surface == surface && slot->Plane.Width == width && slot->Plane.Height == height) {
			continue;
		}
		Free_Plane(*slot);
		slot->Surface = surface;
		slot->Plane.Pixels = new unsigned char[(std::size_t)width * height]();
		slot->Plane.Width = width;
		slot->Plane.Height = height;
		slot->Plane.Pitch = width;
	}
}


EmissivePlane * ShadowPlane_For(void const * surface)
{
	if (!_Enabled || surface == NULL) {
		return(NULL);
	}
	for (Binding & binding : _Bindings) {
		if (binding.Surface == surface && binding.Plane.Pixels != NULL) {
			return(&binding.Plane);
		}
	}
	return(NULL);
}


void ShadowPlane_Forget(void const * surface)
{
	for (Binding & binding : _Bindings) {
		if (binding.Surface == surface && surface != NULL) {
			Free_Plane(binding);
		}
	}
}


EmissivePlane * ShadowPlane_For_RLE(Surface const & dest, RLEBlitter const & blitter, unsigned char & value, bool & depthtest, bool & depthwrite)
{
	if (!_Enabled || &dest != TileSurface) {
		return(NULL);
	}
	EmissivePlane * plane = ShadowPlane_For(&dest);
	if (plane == NULL) {
		return(NULL);
	}
	BlitDesc desc;
	depthwrite = false;
	if (blitter.Describe_Depth_Write(desc)) {
		depthwrite = true;
	} else if (!blitter.Describe(desc)) {
		// A blitter that neither description covers is taken as opaque.
		value = 0;
		depthtest = true;
		return(plane);
	}
	if (desc.Kind == BlitDesc::DARKEN) {
		if (!_Shadows) {
			return(NULL);
		}
		value = SHADOWPLANE_SHADOW;
	} else if (desc.Kind == BlitDesc::XLAT) {
		value = 0;
	} else {
		// A translucent draw blends with the shadow under it, so the shadow stays marked.
		return(NULL);
	}
	depthtest = desc.ZRead;
	return(plane);
}


void ShadowPlane_After_Bit_Blit(Surface & dest, int destx, int desty, Surface const & source, int sourcex, int sourcey, int width, int height)
{
	EmissivePlane * dplane = ShadowPlane_For(&dest);
	if (dplane == NULL) {
		return;
	}
	EmissivePlane const * splane = ShadowPlane_For(&source);
	if (splane != NULL) {
		Emissive_Copy_Rect(*dplane, destx, desty, *splane, sourcex, sourcey, width, height);
		return;
	}
	if (source.Bytes_Per_Pixel() == 1) {
		unsigned char const * pixels = (unsigned char const *)source.Lock(Point2D(sourcex, sourcey));
		if (pixels != NULL) {
			int const stride = source.Stride();
			for (int y = std::max(0, -desty); y < height && desty + y < dplane->Height; y++) {
				unsigned char * out = dplane->Pixels + (desty + y) * dplane->Pitch;
				for (int x = std::max(0, -destx); x < width && destx + x < dplane->Width; x++) {
					if (pixels[y * stride + x] != 0) {
						out[destx + x] = 0;
					}
				}
			}
			source.Unlock();
			return;
		}
	}
	Emissive_Fill_Rect(*dplane, destx, desty, width, height, 0);
}


void ShadowPlane_After_Fill(Surface & dest, Rect const & rect)
{
	EmissivePlane * plane = ShadowPlane_For(&dest);
	if (plane != NULL && rect.Is_Valid()) {
		Emissive_Fill_Rect(*plane, rect.X, rect.Y, rect.Width, rect.Height, 0);
	}
}


void ShadowPlane_After_Tile(Surface & dest, int x, int y, Rect const & clip, unsigned char const * mask, int width, int height, bool water)
{
	EmissivePlane * plane = ShadowPlane_For(&dest);
	if (plane == NULL) {
		return;
	}
	unsigned char const value = water && _Water ? SHADOWPLANE_WATER : 0;
	Rect const area = Intersect(Intersect(clip, Rect(x, y, width, height)), Rect(0, 0, plane->Width, plane->Height));
	for (int py = area.Y; py < area.Y + area.Height; py++) {
		unsigned char const * row = mask + (py - y) * width - x;
		unsigned char * out = plane->Pixels + py * plane->Pitch;
		for (int px = area.X; px < area.X + area.Width; px++) {
			if (row[px] != 32) {
				out[px] = value;
			}
		}
	}
}


void ShadowPlane_After_Extra(Surface & dest, int x, int y, Rect const & clip, unsigned char const * pixels, int width, int height)
{
	EmissivePlane * plane = ShadowPlane_For(&dest);
	if (plane == NULL) {
		return;
	}
	Rect const area = Intersect(Intersect(clip, Rect(x, y, width, height)), Rect(0, 0, plane->Width, plane->Height));
	for (int py = area.Y; py < area.Y + area.Height; py++) {
		unsigned char const * row = pixels + (py - y) * width - x;
		unsigned char * out = plane->Pixels + py * plane->Pitch;
		for (int px = area.X; px < area.X + area.Width; px++) {
			if (row[px] != 0) {
				out[px] = 0;
			}
		}
	}
}


void ShadowPlane_Darken(Surface & surface, Rect const & area, unsigned short halfmask)
{
	EmissivePlane const * plane = ShadowPlane_For(&surface);
	Rect const clipped = Intersect(area, surface.Get_Rect());
	if (plane == NULL || !clipped.Is_Valid()) {
		return;
	}
	unsigned short * pixels = (unsigned short *)surface.Lock(Point2D(clipped.X, clipped.Y));
	if (pixels == NULL) {
		return;
	}
	for (int y = 0; y < clipped.Height; y++) {
		unsigned short * row = (unsigned short *)((char *)pixels + y * surface.Stride());
		unsigned char const * marks = plane->Pixels + (clipped.Y + y) * plane->Pitch + clipped.X;
		for (int x = 0; x < clipped.Width; x++) {
			if ((marks[x] & SHADOWPLANE_SHADOW) != 0) {
				row[x] = (unsigned short)((row[x] >> 1) & halfmask);
			}
		}
	}
	surface.Unlock();
}


void ShadowPlane_Darken_Copy(Surface const & surface, Rect const & area, unsigned short * pixels, unsigned short halfmask, bool terrain_only)
{
	EmissivePlane const * plane = ShadowPlane_For(&surface);
	if (plane == NULL) {
		return;
	}
	unsigned short const * terrain = NULL;
	if (terrain_only) {
		if (surface.Bytes_Per_Pixel() != 2 || surface.Get_Width() < plane->Width || surface.Get_Height() < plane->Height) {
			return;
		}
		terrain = (unsigned short const *)surface.Lock();
		if (terrain == NULL) {
			return;
		}
	}
	for (int y = 0; y < area.Height; y++) {
		if (area.Y + y < 0 || area.Y + y >= plane->Height) {
			continue;
		}
		unsigned char const * marks = plane->Pixels + (area.Y + y) * plane->Pitch;
		unsigned short const * ground = terrain != NULL ? (unsigned short const *)((char const *)terrain + (std::size_t)(area.Y + y) * surface.Stride()) : NULL;
		unsigned short * row = pixels + (std::size_t)y * area.Width;
		for (int x = std::max(0, -area.X); x < area.Width && area.X + x < plane->Width; x++) {
			if ((marks[area.X + x] & SHADOWPLANE_SHADOW) != 0 && (ground == NULL || ground[area.X + x] == row[x])) {
				row[x] = (unsigned short)((row[x] >> 1) & halfmask);
			}
		}
	}
	if (terrain != NULL) {
		surface.Unlock();
	}
}
