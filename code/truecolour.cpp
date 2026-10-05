/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "truecolour.h"

#include "_alpha.h"
#include "abuffer.h"
#include "alphalighting.h"
#include "blit.h"
#include "blitter.h"
#include "bsurface.h"
#include "ccfile.h"
#include "cdfile.h"
#include "convert.h"
#include "dbgprint.h"
#include "mixfile.h"
#include "shapeset.h"
#include "zbuffer.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#define STBI_NO_STDIO
#include <stb_image.h>

namespace {

struct SheetType
{
	int CellWidth = 0;
	int CellHeight = 0;
	int Columns = 0;
	int Frames = 0;
	int Width = 0;
	int Height = 0;
	std::vector<std::uint32_t> Pixels;
	// Zero for an ordinary pixel, otherwise the house-colour palette index (16-31) it draws.
	std::vector<std::uint8_t> House;
	bool HasHouse = false;
};

enum StateType {
	STATE_UNKNOWN,
	STATE_MISSING,
	STATE_REJECTED,
	STATE_LOADED,
};

struct EntryType
{
	std::string Name;
	int Width = 0;
	int Height = 0;
	int Count = 0;
	StateType State = STATE_UNKNOWN;
	std::string PngName;
	std::unique_ptr<SheetType> Sheet;
};

std::unordered_map<void const *, EntryType> Entries;


struct ShapeHeader
{
	short Flags;
	short Width;
	short Height;
	short Count;
};


ShapeHeader Header_Of(void const * data)
{
	ShapeHeader header;
	std::memcpy(&header, data, sizeof(header));
	return(header);
}


bool Is_Shape_Name(char const * name, std::string & stem, std::string & extension)
{
	char const * dot = std::strrchr(name, '.');
	if (dot == NULL || dot == name) {
		return(false);
	}
	extension.assign(dot + 1);
	for (char & ch : extension) {
		ch = (char)toupper((unsigned char)ch);
	}
	static char const * const Extensions[] = {"SHP", "TEM", "SNO", "URB", "UBN", "DES", "LUN"};
	for (char const * known : Extensions) {
		if (extension == known) {
			stem.assign(name, dot);
			return(true);
		}
	}
	return(false);
}


std::string Png_Name(std::string const & name, char const * suffix)
{
	std::string stem;
	std::string extension;
	if (!Is_Shape_Name(name.c_str(), stem, extension)) {
		return(std::string());
	}
	if (extension != "SHP") {
		stem += "_" + extension;
	}
	return(stem + suffix + ".png");
}


bool Read_File(std::string const & name, std::vector<unsigned char> & bytes)
{
	CCFileClass file(name.c_str());
	if (!file.Is_Available()) {
		return(false);
	}
	int size = file.Size();
	if (size <= 0) {
		return(false);
	}
	bytes.resize((size_t)size);
	if (!file.Open(FileClass::READ)) {
		return(false);
	}
	bool ok = file.Read(bytes.data(), size) == size;
	file.Close();
	return(ok);
}


std::uint32_t const * Decode(std::vector<unsigned char> const & bytes, int & width, int & height)
{
	int channels = 0;
	return((std::uint32_t const *)stbi_load_from_memory(bytes.data(), (int)bytes.size(), &width, &height, &channels, 4));
}


int Fitting_Frames(int count, int columns, int rows)
{
	if (count > 0 && (count + columns - 1) / columns == rows) {
		return(count);
	}
	int half = count / 2;
	if (half > 0 && (half + columns - 1) / columns == rows) {
		return(half);
	}
	return(0);
}


void Load(EntryType & entry)
{
	entry.PngName = Png_Name(entry.Name, "");
	std::vector<unsigned char> bytes;
	if (entry.PngName.empty() || !Read_File(entry.PngName, bytes)) {
		entry.State = STATE_MISSING;
		return;
	}

	entry.State = STATE_REJECTED;
	int width = 0;
	int height = 0;
	std::uint32_t const * pixels = Decode(bytes, width, height);
	if (pixels == NULL) {
		DebugString("TRUECOLOUR: %s did not decode: %s\n", entry.PngName.c_str(), stbi_failure_reason());
		return;
	}

	auto sheet = std::make_unique<SheetType>();
	sheet->CellWidth = entry.Width;
	sheet->CellHeight = entry.Height;
	sheet->Width = width;
	sheet->Height = height;
	if (entry.Width <= 0 || entry.Height <= 0 || width % entry.Width != 0 || height % entry.Height != 0) {
		DebugString("TRUECOLOUR: %s is %dx%d, not a whole number of %dx%d cells for %s\n", entry.PngName.c_str(), width, height, entry.Width, entry.Height, entry.Name.c_str());
		stbi_image_free((void *)pixels);
		return;
	}
	sheet->Columns = width / entry.Width;
	sheet->Frames = Fitting_Frames(entry.Count, sheet->Columns, height / entry.Height);
	if (sheet->Frames == 0) {
		DebugString("TRUECOLOUR: %s has %d rows of %d cells, which fits neither %d nor %d frames of %s\n", entry.PngName.c_str(), height / entry.Height, sheet->Columns,
			entry.Count, entry.Count / 2, entry.Name.c_str());
		stbi_image_free((void *)pixels);
		return;
	}
	sheet->Pixels.assign(pixels, pixels + (size_t)width * (size_t)height);
	stbi_image_free((void *)pixels);

	sheet->House.assign(sheet->Pixels.size(), 0);
	std::string maskname = Png_Name(entry.Name, "_hc");
	if (Read_File(maskname, bytes)) {
		int maskwidth = 0;
		int maskheight = 0;
		std::uint32_t const * mask = Decode(bytes, maskwidth, maskheight);
		if (mask == NULL || maskwidth != width || maskheight != height) {
			DebugString("TRUECOLOUR: %s ignored; it must decode to the %dx%d of %s\n", maskname.c_str(), width, height, entry.PngName.c_str());
		} else {
			for (size_t index = 0; index < sheet->House.size(); index++) {
				std::uint32_t value = mask[index];
				int bright = std::max({(int)(value & 0xFF), (int)((value >> 8) & 0xFF), (int)((value >> 16) & 0xFF)});
				int shade = bright * (int)(value >> 24) / 255;
				if (shade > 0) {
					sheet->House[index] = (std::uint8_t)(16 + 15 - shade * 15 / 255);
					sheet->HasHouse = true;
				}
			}
		}
		if (mask != NULL) {
			stbi_image_free((void *)mask);
		}
	}

	DebugString("TRUECOLOUR: %s replaces %s, %d of its %d frames%s\n", entry.PngName.c_str(), entry.Name.c_str(), sheet->Frames, entry.Count, sheet->HasHouse ? ", with house colour" : "");
	entry.Sheet = std::move(sheet);
	entry.State = STATE_LOADED;
}


// Fetches the sheet for a recorded shape, loading it on first use; NULL when the shape has none.
SheetType const * Sheet_For(void const * data)
{
	auto found = Entries.find(data);
	if (found == Entries.end()) {
		return(NULL);
	}
	EntryType & entry = found->second;
	ShapeHeader header = Header_Of(data);
	if (header.Width != entry.Width || header.Height != entry.Height || header.Count != entry.Count) {
		Entries.erase(found);
		return(NULL);
	}
	if (entry.State == STATE_UNKNOWN) {
		Load(entry);
	}
	return(entry.Sheet.get());
}


inline unsigned short Pack_565(int red, int green, int blue)
{
	return((unsigned short)(((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3)));
}


// House pixels are drawn through the converter's lighting table, so they match the SHP art;
// other pixels are scaled by the tint of the same lighting band.
class TrueColourBlitter : public Blitter {
	public:
		std::uint32_t const * PixelBase = NULL;
		std::uint8_t const * HouseBase = NULL;
		unsigned short const * Table = NULL;
		unsigned char const * Remap = NULL;
		AlphaLightingRemapClass * Lighting = NULL;
		int (*Scale)[3] = NULL;
		int MiddleBand = 0;
		int Opacity = 256;
		bool ZTest = false;
		bool ZWrite = false;

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, unsigned short * z_buff = NULL, unsigned short * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			std::uint32_t const * pixel = (std::uint32_t const *)source;
			std::uint8_t const * house = HouseBase + (pixel - PixelBase);
			unsigned short * out = (unsigned short *)dest;
			unsigned short * zb = (ZTest && DepthBuffer != NULL) ? z_buff : NULL;
			unsigned short * ap = (Lighting != NULL && AlphaBuffer != NULL) ? a_buff : NULL;
			unsigned short const * light = ap != NULL ? Lighting->Get_Table(alpha_level) : NULL;

			for (int index = 0; index < length; index++) {
				std::uint32_t value = pixel[index];
				unsigned int alpha = value >> 24;
				if (alpha != 0 && (zb == NULL || z_min < *zb)) {
					int band = light != NULL ? (light[std::min<unsigned short>(*ap, 255)] >> 8) : MiddleBand;
					unsigned short colour;
					if (house[index] != 0) {
						int colourindex = Remap != NULL ? Remap[house[index]] : house[index];
						colour = Table[band * 256 + colourindex];
					} else {
						int const * scale = Scale[band];
						colour = Pack_565(std::min(255, (int)(((value & 0xFF) * scale[0]) >> 16)), std::min(255, (int)((((value >> 8) & 0xFF) * scale[1]) >> 16)),
							std::min(255, (int)((((value >> 16) & 0xFF) * scale[2]) >> 16)));
					}
					int weight = (int)(alpha * Opacity + 127) / 255;
					if (weight >= 256) {
						out[index] = colour;
					} else {
						unsigned short under = out[index];
						int red = (((colour >> 11) & 31) * weight + ((under >> 11) & 31) * (256 - weight)) >> 8;
						int green = (((colour >> 5) & 63) * weight + ((under >> 5) & 63) * (256 - weight)) >> 8;
						int blue = ((colour & 31) * weight + (under & 31) * (256 - weight)) >> 8;
						out[index] = (unsigned short)((red << 11) | (green << 5) | blue);
					}
					if (zb != NULL && ZWrite) {
						*zb = (unsigned short)z_min;
					}
				}
				if (zb != NULL) {
					zb = Blit_Wrap_Z_Buffer(zb + 1);
				}
				if (ap != NULL) {
					ap = Blit_Wrap_A_Buffer(ap + 1);
				}
			}
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, unsigned short * z_buff = NULL, unsigned short * a_buff = NULL, int alpha_level = 1000) const override
		{
			BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);
		}
};


AlphaLightingRemapClass * Lighting_For(int levels)
{
	static std::unordered_map<int, AlphaLightingRemapClass *> tables;
	AlphaLightingRemapClass *& table = tables[levels];
	if (table == NULL) {
		table = AlphaLightingRemapInit.Init(levels);
	}
	return(table);
}

}


/// <summary>
/// Records the file name a shape was retrieved under, so a PNG of the same stem can be drawn in
/// its place. Names that are not shape or theater art are ignored.
/// </summary>
void TrueColour_Note_Shape(void const * data, char const * name)
{
	std::string stem;
	std::string extension;
	if (data == NULL || name == NULL || !Is_Shape_Name(name, stem, extension)) {
		return;
	}

	ShapeHeader header = Header_Of(data);
	auto found = Entries.find(data);
	if (found != Entries.end() && stricmp(found->second.Name.c_str(), name) == 0 && found->second.Width == header.Width && found->second.Height == header.Height
		&& found->second.Count == header.Count) {
		return;
	}

	EntryType & entry = Entries[data];
	entry = EntryType();
	entry.Name = name;
	entry.Width = header.Width;
	entry.Height = header.Height;
	entry.Count = header.Count;
}


/// <summary>
/// Drops every recorded shape inside a block of memory that is about to be freed.
/// </summary>
void TrueColour_Forget_Range(void const * begin, std::size_t size)
{
	char const * start = (char const *)begin;
	char const * end = start + size;
	for (auto it = Entries.begin(); it != Entries.end();) {
		char const * key = (char const *)it->first;
		if (key >= start && key < end) {
			it = Entries.erase(it);
		} else {
			++it;
		}
	}
}


/// <summary>
/// Adds a directory to the file search path and looks again for PNGs that were missing.
/// </summary>
void TrueColour_Add_Directory(char const * path)
{
	std::string directory = path;
	if (!directory.empty() && directory.back() != '\\' && directory.back() != '/') {
		directory += '\\';
	}
	CDFileClass::Add_Search_Drive(directory.c_str());
	for (auto & [data, entry] : Entries) {
		if (entry.State != STATE_LOADED) {
			entry.State = STATE_UNKNOWN;
		}
	}
	DebugString("TRUECOLOUR: searching %s\n", directory.c_str());
}


/// <summary>
/// Draws the PNG that replaces a shape frame, if there is one. Returns false, having drawn
/// nothing, when the caller must draw the SHP frame itself: the surface is not 16 bit, the
/// draw is a shadow, alpha-buffer or predator effect, or no PNG covers that frame.
/// </summary>
bool TrueColour_Draw(Surface & surface, ConvertClass & convert, ShapeSet const * shapefile, int shapenum, Point2D const & point, Rect const & window, ShapeFlags_Type flags,
	unsigned char const * remap, int height_offset, ZGradientType zgrad, int intensity)
{
	static constexpr int SkippedFlags = SHAPE_DARKEN | SHAPE_PREDATOR | SHAPE_NOTRANS | SHAPE_ALPHA_BLEND | SHAPE_ALPHA_WRITE_MULT | SHAPE_ALPHA_WRITE | SHAPE_ZERO_ALPHA
		| SHAPE_NONZERO_ALPHA;

	if (Entries.empty() || (flags & SkippedFlags) != 0 || surface.Bytes_Per_Pixel() != 2 || convert.Bytes_Per_Pixel() != 2) {
		return(false);
	}

	SheetType const * sheet = Sheet_For(shapefile);
	if (sheet == NULL || shapenum < 0 || shapenum >= sheet->Frames || convert.Get_Intensity_Table() == NULL) {
		return(false);
	}

	convert.Set_Remap(remap);

	int levels = std::max(convert.Get_Intensity_Levels(), 1);
	int red = 1000;
	int green = 1000;
	int blue = 1000;
	convert.Get_Tint(red, green, blue);
	int const tints[3] = {red < 0 ? 1000 : red, green < 0 ? 1000 : green, blue < 0 ? 1000 : blue};

	// The bands scale like the converter's own tables: twice the tint at the top band.
	int scale[NUM_INTENSITY_LEVELS + 1][3];
	int bands = std::min(levels, NUM_INTENSITY_LEVELS + 1);
	for (int band = 0; band < bands; band++) {
		for (int channel = 0; channel < 3; channel++) {
			long long step = (long long)tints[channel] * 65536 / 1000;
			scale[band][channel] = (int)(levels > 1 ? 2 * step * band / (levels - 1) : step);
		}
	}

	TrueColourBlitter blitter;
	blitter.PixelBase = sheet->Pixels.data();
	blitter.HouseBase = sheet->House.data();
	blitter.Table = (unsigned short const *)convert.Get_Intensity_Table();
	blitter.Remap = remap;
	blitter.Scale = scale;
	blitter.MiddleBand = std::min((levels - 1) >> 1, bands - 1);
	blitter.Lighting = (flags & SHAPE_ALPHA) != 0 && levels > 1 && levels == bands ? Lighting_For(levels) : NULL;
	switch (flags & SHAPE_TRANSLUCENT75) {
		case SHAPE_TRANSLUCENT25: blitter.Opacity = 192; break;
		case SHAPE_TRANSLUCENT50: blitter.Opacity = 128; break;
		case SHAPE_TRANSLUCENT75: blitter.Opacity = 64; break;
		default: break;
	}
	blitter.ZWrite = (flags & SHAPE_ZWRITE) != 0;
	blitter.ZTest = blitter.ZWrite || (flags & (SHAPE_ZREAD | SHAPE_ZGRAD)) != 0;

	int x = point.X;
	int y = point.Y;
	if (flags & SHAPE_CENTER) {
		x -= shapefile->Get_Width() / 2;
		y -= shapefile->Get_Height() / 2;
	}

	BSurface source(sheet->Width, sheet->Height, 4, (void *)sheet->Pixels.data());
	Rect cell((shapenum % sheet->Columns) * sheet->CellWidth, (shapenum / sheet->Columns) * sheet->CellHeight, sheet->CellWidth, sheet->CellHeight);
	Bit_Blit(surface, window, Rect(x, y, sheet->CellWidth, sheet->CellHeight), source, cell, Rect(0, 0, sheet->CellWidth, sheet->CellHeight), blitter, height_offset, zgrad, intensity, 0);
	return(true);
}


/// <summary>
/// Writes to the log whether a shape has a PNG in its place and, if so, the sheet's size, the
/// frames it covers and the centre pixel of its first frame. The shape is fetched by name.
/// </summary>
void TrueColour_Report(char const * name)
{
	void const * data = MixFileClass::Retrieve(name);
	if (data == NULL) {
		DebugString("AUTOTEST   truecolour %s: no such shape\n", name);
		return;
	}
	ShapeHeader header = Header_Of(data);
	SheetType const * sheet = Sheet_For(data);
	auto found = Entries.find(data);
	if (sheet == NULL || found == Entries.end()) {
		DebugString("AUTOTEST   truecolour %s shp %d frames %dx%d: no override (%s)\n", name, header.Count, header.Width, header.Height,
			found != Entries.end() && found->second.State == STATE_REJECTED ? "rejected" : "no png");
		return;
	}
	int x = sheet->CellWidth / 2;
	int y = sheet->CellHeight / 2;
	std::uint32_t pixel = sheet->Pixels[(size_t)y * sheet->Width + x];
	DebugString("AUTOTEST   truecolour %s shp %d frames %dx%d: png %s sheet %dx%d frames %d house %d pixel %d,%d rgba %d,%d,%d,%d\n", name, header.Count, header.Width,
		header.Height, found->second.PngName.c_str(), sheet->Width, sheet->Height, sheet->Frames, (int)sheet->HasHouse, x, y, (int)(pixel & 0xFF), (int)((pixel >> 8) & 0xFF),
		(int)((pixel >> 16) & 0xFF), (int)(pixel >> 24));
}
