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

#include <cctype>
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
	unsigned char const * source = (unsigned char const *)picture->Lock();
	unsigned short * dest = (unsigned short *)hicolor->Lock();
	if (source != NULL && dest != NULL) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				dest[y * width + x] = (unsigned short)DSurface::Build_Hicolor_Pixel(palette[source[y * picture->Stride() + x]]);
			}
		}
	}
	if (dest != NULL) {
		hicolor->Unlock();
	}
	if (source != NULL) {
		picture->Unlock();
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
	unsigned char * dest = (unsigned char *)surface.Lock(Point2D(area.X, area.Y));
	unsigned char const * source = (unsigned char const *)cameo.Lock(Point2D(area.X - (cliprect.X + point.X), area.Y - (cliprect.Y + point.Y)));
	if (dest != NULL && source != NULL) {
		for (int row = 0; row < area.Height; row++) {
			memcpy(dest + row * surface.Stride(), source + row * cameo.Stride(), area.Width * 2);
		}
	}
	if (source != NULL) {
		cameo.Unlock();
	}
	if (dest != NULL) {
		surface.Unlock();
	}
}


Surface const * PCX_Cameo(std::string const & filename)
{
	if (filename.empty()) {
		return(NULL);
	}
	std::string key = filename;
	for (char & letter : key) {
		letter = (char)toupper((unsigned char)letter);
	}
	auto found = Cache.find(key);
	if (found == Cache.end()) {
		found = Cache.emplace(key, Load(key)).first;
	}
	return(found->second.get());
}
