/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "cameopcx.h"

#include "bsurface.h"
#include "ccfile.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "palette.h"
#include "pcx.h"
#include "strutil.h"

#include <cstring>
#include <map>
#include <memory>


namespace {

std::map<std::string, std::unique_ptr<Surface>> Cache;


// A paletted picture becomes hicolor through its own palette; a hicolor one is kept as it is.
std::unique_ptr<Surface> Load(std::string const & filename)
{
	CCFileClass file(filename.c_str());
	PaletteClass palette;
	std::unique_ptr<Surface> picture(Read_PCX_File(file, &palette));
	if (picture == nullptr) {
		DebugString("Cameo picture %s could not be read.\n", filename.c_str());
		return(nullptr);
	}
	if (picture->Bytes_Per_Pixel() == 2) {
		return(picture);
	}

	int const width = picture->Get_Width();
	int const height = picture->Get_Height();
	auto hicolor = std::make_unique<BSurface>(width, height, 2);
	SurfaceLock picture_lock(*picture);
	SurfaceLock hicolor_lock(*hicolor);
	unsigned char const * source = picture_lock.As<unsigned char const>();
	unsigned short * dest = hicolor_lock.As<unsigned short>();
	if (source != NULL && dest != NULL) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				dest[y * width + x] = (unsigned short)DSurface::Build_Hicolor_Pixel(palette[source[y * picture->Stride() + x]]);
			}
		}
	}
	return(hicolor);
}

}


void Draw_PCX_Cameo(Surface & surface, Surface const & cameo, Point2D const & point, Rect const & cliprect)
{
	if (surface.Bytes_Per_Pixel() != 2 || cameo.Bytes_Per_Pixel() != 2) {
		return;
	}
	Rect const clip = Intersect(cliprect, surface.Get_Rect());
	Rect const area = Intersect(Rect(cliprect.X + point.X, cliprect.Y + point.Y, cameo.Get_Width(), cameo.Get_Height()), clip);
	if (!area.Is_Valid()) {
		return;
	}
	SurfaceLock dest_lock(surface, Point2D(area.X, area.Y));
	SurfaceLock source_lock(cameo, Point2D(area.X - (cliprect.X + point.X), area.Y - (cliprect.Y + point.Y)));
	unsigned char * dest = dest_lock.As<unsigned char>();
	unsigned char const * source = source_lock.As<unsigned char const>();
	if (dest != NULL && source != NULL) {
		for (int row = 0; row < area.Height; row++) {
			memcpy(dest + row * surface.Stride(), source + row * cameo.Stride(), area.Width * 2);
		}
	}
}


Surface const * PCX_Cameo(std::string const & filename)
{
	if (filename.empty()) {
		return(NULL);
	}
	std::string const key = Upper(filename);
	auto found = Cache.find(key);
	if (found == Cache.end()) {
		found = Cache.emplace(key, Load(key)).first;
	}
	return(found->second.get());
}
