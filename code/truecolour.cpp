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
#include "ccini.h"
#include "cdfile.h"
#include "convert.h"
#include "data.h"
#include "dbgprint.h"
#include "mixfile.h"
#include "shapeset.h"
#include "xsurface.h"
#include "zbuffer.h"
#include "_zbuffer.h"
#include "_palette.h"
#include "rawfile.h"

#include <miniz/miniz.h>

#include <algorithm>
#include <climits>
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

// One per shape file name, so a shape loaded again at a new address reuses the decoded sheet.
struct SheetRecord
{
	int Width = 0;
	int Height = 0;
	int Count = 0;
	StateType State = STATE_UNKNOWN;
	std::string PngName;
	std::unique_ptr<SheetType> Sheet;
};

struct EntryType
{
	std::string Name;
	int Width = 0;
	int Height = 0;
	int Count = 0;
};

// A shape made for a PNG sheet that has no SHP. Its data is kept for the rest of the session, so
// the address the game holds stays valid.
struct MadeShapeType
{
	StateType State = STATE_UNKNOWN;
	std::vector<char> Data;
};

// An SHP frame record as ShapeSet reads it.
struct FrameRecord
{
	short X;
	short Y;
	short Width;
	short Height;
	short Flags;
	short Size;
	unsigned char Color[3];
	unsigned char Unused[5];
	int Data;
};
static_assert(sizeof(FrameRecord) == 24, "a SHP frame record is 24 bytes");

// ShapeSet's frame flags for a frame with transparent pixels, stored run-length encoded.
constexpr short FRAME_TRANSPARENT_RLE = 0x01 | 0x02;

std::unordered_map<void const *, EntryType> Entries;
std::unordered_map<std::string, SheetRecord> Sheets;
std::unordered_map<std::string, MadeShapeType> MadeShapes;


std::string Upper(std::string text)
{
	for (char & ch : text) {
		ch = (char)toupper((unsigned char)ch);
	}
	return(text);
}


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


void Load(SheetRecord & entry, std::string const & name)
{
	entry.PngName = Png_Name(name, "");
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
		DebugString("TRUECOLOUR: %s is %dx%d, not a whole number of %dx%d cells for %s\n", entry.PngName.c_str(), width, height, entry.Width, entry.Height, name.c_str());
		stbi_image_free((void *)pixels);
		return;
	}
	sheet->Columns = width / entry.Width;
	sheet->Frames = Fitting_Frames(entry.Count, sheet->Columns, height / entry.Height);
	if (sheet->Frames == 0) {
		DebugString("TRUECOLOUR: %s has %d rows of %d cells, which fits neither %d nor %d frames of %s\n", entry.PngName.c_str(), height / entry.Height, sheet->Columns,
			entry.Count, entry.Count / 2, name.c_str());
		stbi_image_free((void *)pixels);
		return;
	}
	sheet->Pixels.assign(pixels, pixels + (size_t)width * (size_t)height);
	stbi_image_free((void *)pixels);

	sheet->House.assign(sheet->Pixels.size(), 0);
	std::string maskname = Png_Name(name, "_hc");
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

	DebugString("TRUECOLOUR: %s replaces %s, %d of its %d frames%s\n", entry.PngName.c_str(), name.c_str(), sheet->Frames, entry.Count, sheet->HasHouse ? ", with house colour" : "");
	entry.Sheet = std::move(sheet);
	entry.State = STATE_LOADED;
}


SheetRecord * Record_For(void const * data)
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
	SheetRecord & record = Sheets[Upper(entry.Name)];
	if (record.Width != entry.Width || record.Height != entry.Height || record.Count != entry.Count) {
		record = SheetRecord();
		record.Width = entry.Width;
		record.Height = entry.Height;
		record.Count = entry.Count;
	}
	if (record.State == STATE_UNKNOWN) {
		Load(record, entry.Name);
	}
	return(&record);
}


// Fetches the sheet for a recorded shape, loading it on first use; NULL when the shape has none.
SheetType const * Sheet_For(void const * data)
{
	SheetRecord * record = Record_For(data);
	return(record != NULL ? record->Sheet.get() : NULL);
}


// The sidecar "<sheet>.ini" sets any of the values; without it a sheet is one row of square
// cells, or a single cell when its width is not a multiple of its height, and has twice as many
// frames as cells.
void Png_Only_Layout(std::string const & pngname, int width, int height, int & cellwidth, int & cellheight, int & frames)
{
	static char const * const SHEET = "Sheet";
	CCINIClass ini;
	CCFileClass file((pngname + ".ini").c_str());
	if (file.Is_Available()) {
		ini.Load(file, false);
	}
	cellheight = ini.Get_Int(SHEET, "FrameHeight", height);
	cellwidth = ini.Get_Int(SHEET, "FrameWidth", cellheight > 0 && width % cellheight == 0 ? cellheight : width);
	int const cells = cellwidth > 0 && cellheight > 0 ? (width / cellwidth) * (height / cellheight) : 0;
	frames = ini.Get_Int(SHEET, "Frames", cells * 2);
}


// Each frame of the shape covers the pixels of its cell that are not fully transparent, and every
// pixel of it is transparent; a frame with no such pixels, or with no cell, is empty.
void Build_Shape(std::vector<char> & data, SheetType const & sheet, int count)
{
	std::vector<FrameRecord> records((size_t)count, FrameRecord{});
	std::vector<char> lines;
	for (int frame = 0; frame < std::min(sheet.Frames, count); frame++) {
		int const left = (frame % sheet.Columns) * sheet.CellWidth;
		int const top = (frame / sheet.Columns) * sheet.CellHeight;
		int x0 = INT_MAX;
		int y0 = INT_MAX;
		int x1 = -1;
		int y1 = -1;
		long long sums[3] = {0, 0, 0};
		long long opaque = 0;
		for (int y = 0; y < sheet.CellHeight; y++) {
			std::uint32_t const * row = &sheet.Pixels[(size_t)(top + y) * sheet.Width + left];
			for (int x = 0; x < sheet.CellWidth; x++) {
				std::uint32_t const value = row[x];
				if ((value >> 24) == 0) {
					continue;
				}
				x0 = std::min(x0, x);
				x1 = std::max(x1, x);
				y0 = std::min(y0, y);
				y1 = std::max(y1, y);
				sums[0] += value & 0xFF;
				sums[1] += (value >> 8) & 0xFF;
				sums[2] += (value >> 16) & 0xFF;
				opaque++;
			}
		}
		if (opaque == 0) {
			continue;
		}

		FrameRecord & record = records[(size_t)frame];
		record.X = (short)x0;
		record.Y = (short)y0;
		record.Width = (short)(x1 - x0 + 1);
		record.Height = (short)(y1 - y0 + 1);
		record.Flags = FRAME_TRANSPARENT_RLE;
		for (int channel = 0; channel < 3; channel++) {
			record.Color[channel] = (unsigned char)(sums[channel] / opaque);
		}
		record.Data = (int)lines.size();

		// Each line is its length in bytes, then runs of at most 255 transparent pixels.
		int const length = 2 + 2 * ((record.Width + 254) / 255);
		for (int line = 0; line < record.Height; line++) {
			lines.push_back((char)(length & 0xFF));
			lines.push_back((char)(length >> 8));
			for (int remaining = record.Width; remaining > 0; remaining -= 255) {
				lines.push_back(0);
				lines.push_back((char)std::min(remaining, 255));
			}
		}
		record.Size = (short)std::min(length * record.Height, (int)SHRT_MAX);
	}

	size_t const recordsize = sizeof(FrameRecord) * records.size();
	size_t const start = sizeof(ShapeHeader) + recordsize;
	for (FrameRecord & record : records) {
		if (record.Width > 0) {
			record.Data += (int)start;
		}
	}
	ShapeHeader const header = {0, (short)sheet.CellWidth, (short)sheet.CellHeight, (short)count};
	data.assign(start + lines.size(), 0);
	std::memcpy(data.data(), &header, sizeof(header));
	std::memcpy(data.data() + sizeof(header), records.data(), recordsize);
	if (!lines.empty()) {
		std::memcpy(data.data() + start, lines.data(), lines.size());
	}
}


void Make_Shape(MadeShapeType & made, std::string const & name)
{
	made.State = STATE_MISSING;
	std::string const pngname = Png_Name(name, "");
	std::vector<unsigned char> bytes;
	if (pngname.empty() || !Read_File(pngname, bytes)) {
		return;
	}

	made.State = STATE_REJECTED;
	int width = 0;
	int height = 0;
	int channels = 0;
	if (!stbi_info_from_memory(bytes.data(), (int)bytes.size(), &width, &height, &channels) || width <= 0 || height <= 0) {
		DebugString("TRUECOLOUR: %s did not decode: %s\n", pngname.c_str(), stbi_failure_reason());
		return;
	}

	int cellwidth = 0;
	int cellheight = 0;
	int frames = 0;
	Png_Only_Layout(pngname, width, height, cellwidth, cellheight, frames);
	if (cellwidth <= 0 || cellheight <= 0 || cellwidth > SHRT_MAX || cellheight > SHRT_MAX || frames <= 0 || frames > SHRT_MAX) {
		DebugString("TRUECOLOUR: %s cannot stand in for %s: %d frames of %dx%d\n", pngname.c_str(), name.c_str(), frames, cellwidth, cellheight);
		return;
	}

	SheetRecord & record = Sheets[Upper(name)];
	record = SheetRecord();
	record.Width = cellwidth;
	record.Height = cellheight;
	record.Count = frames;
	Load(record, name);
	if (record.State != STATE_LOADED) {
		return;
	}

	Build_Shape(made.Data, *record.Sheet, frames);
	made.State = STATE_LOADED;
	DebugString("TRUECOLOUR: %s has no SHP; %s stands in for it with %d frames of %dx%d\n", name.c_str(), pngname.c_str(), frames, cellwidth, cellheight);
}


bool Is_Made_Shape(void const * data)
{
	for (auto const & [name, made] : MadeShapes) {
		if (made.State == STATE_LOADED && made.Data.data() == data) {
			return(true);
		}
	}
	return(false);
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

		// A darkening draw halves what is under the frame's opaque pixels, as the shadow blitters do.
		bool Darken = false;
		unsigned short HalfMask = 0;

		// With a depth shape, each pixel's depth is ZBase less the shape's signed offset under it.
		signed char const * ZShape = NULL;
		long ZShapeSize = 0;
		int ZShapeWidth = 0;
		int ZOriginX = 0;
		int ZOriginY = 0;
		int ZBase = 0;
		int SheetWidth = 0;
		int CellLeft = 0;
		int CellTop = 0;

		virtual void BlitForward(void * dest, void const * source, int length, int z_min = 0, unsigned short * z_buff = NULL, unsigned short * a_buff = NULL, int alpha_level = 1000, int warp_offset = 0) const override
		{
			std::uint32_t const * pixel = (std::uint32_t const *)source;
			std::uint8_t const * house = HouseBase + (pixel - PixelBase);
			unsigned short * out = (unsigned short *)dest;
			unsigned short * zb = (ZTest && DepthBuffer != NULL) ? z_buff : NULL;
			unsigned short * ap = (Lighting != NULL && AlphaBuffer != NULL) ? a_buff : NULL;
			unsigned short const * light = ap != NULL ? Lighting->Get_Table(alpha_level) : NULL;

			long zat = 0;
			if (ZShape != NULL && zb != NULL) {
				long first = (long)(pixel - PixelBase);
				zat = (long)(ZOriginY + first / SheetWidth - CellTop) * ZShapeWidth + ZOriginX + first % SheetWidth - CellLeft;
			}

			for (int index = 0; index < length; index++) {
				std::uint32_t value = pixel[index];
				unsigned int alpha = value >> 24;
				int z = z_min;
				if (ZShape != NULL && zb != NULL) {
					long at = zat + index;
					z = ZBase - (at >= 0 && at < ZShapeSize ? ZShape[at] : 0);
				}
				if (alpha != 0 && (zb == NULL || z < *zb)) {
					unsigned short under = out[index];
					unsigned short colour;
					int weight;
					if (Darken) {
						colour = (unsigned short)((under >> 1) & HalfMask);
						weight = (int)(alpha * 256 + 127) / 255;
					} else {
						colour = Colour_Of(value, house[index], light != NULL ? (light[std::min<unsigned short>(*ap, 255)] >> 8) : MiddleBand);
						weight = (int)(alpha * Opacity + 127) / 255;
					}
					out[index] = weight >= 256 ? colour : Mix(colour, under, weight);
					if (zb != NULL && ZWrite) {
						*zb = (unsigned short)z;
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

		unsigned short Colour_Of(std::uint32_t value, std::uint8_t house, int band) const
		{
			if (house != 0) {
				return(Table[band * 256 + (Remap != NULL ? Remap[house] : house)]);
			}
			int const * scale = Scale[band];
			return(Pack_565(std::min(255, (int)(((value & 0xFF) * scale[0]) >> 16)), std::min(255, (int)((((value >> 8) & 0xFF) * scale[1]) >> 16)),
				std::min(255, (int)((((value >> 16) & 0xFF) * scale[2]) >> 16))));
		}

		static unsigned short Mix(unsigned short colour, unsigned short under, int weight)
		{
			int red = (((colour >> 11) & 31) * weight + ((under >> 11) & 31) * (256 - weight)) >> 8;
			int green = (((colour >> 5) & 63) * weight + ((under >> 5) & 63) * (256 - weight)) >> 8;
			int blue = ((colour & 31) * weight + (under & 31) * (256 - weight)) >> 8;
			return((unsigned short)((red << 11) | (green << 5) | blue));
		}

		virtual void BlitBackward(void * dest, void const * source, int length, int z_min = 0, unsigned short * z_buff = NULL, unsigned short * a_buff = NULL, int alpha_level = 1000) const override
		{
			BlitForward(dest, source, length, z_min, z_buff, a_buff, alpha_level);
		}
};


// A shape read from disk for a report, freed and forgotten when it goes out of scope.
struct LooseShape
{
	char * Data = NULL;
	~LooseShape()
	{
		if (Data != NULL) {
			TrueColour_Forget_Range(Data, 1);
			delete [] Data;
		}
	}
};


// Fetches a shape from a cached MIX file or, failing that, reads it as the game reads demand-loaded art.
void const * Fetch_Shape(char const * name, LooseShape & loose)
{
	void const * data = MixFileClass::Retrieve(name);
	if (data == NULL) {
		CCFileClass file(name);
		loose.Data = (char *)Load_Alloc_Data(file);
		data = loose.Data;
	}
	return(data);
}


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
/// Returns a shape for a PNG sheet that has no SHP, or NULL when the name is not a shape file
/// name, no sheet of that name is found, or the sheet does not fit its layout. The shape has the
/// cell size and frame count the sheet's layout gives, see docs/TRUECOLOUR.md, and only
/// transparent pixels; its frames are drawn from the sheet. Every request for one name returns
/// the same shape, which stays valid for the rest of the session.
/// </summary>
void const * TrueColour_Png_Only_Shape(char const * name)
{
	std::string stem;
	std::string extension;
	if (name == NULL || !Is_Shape_Name(name, stem, extension)) {
		return(NULL);
	}
	MadeShapeType & made = MadeShapes[Upper(name)];
	if (made.State == STATE_UNKNOWN) {
		Make_Shape(made, name);
	}
	if (made.State != STATE_LOADED) {
		return(NULL);
	}
	TrueColour_Note_Shape(made.Data.data(), name);
	return(made.Data.data());
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
	for (auto & [name, record] : Sheets) {
		if (record.State != STATE_LOADED) {
			record.State = STATE_UNKNOWN;
		}
	}
	for (auto & [name, made] : MadeShapes) {
		if (made.State != STATE_LOADED) {
			made.State = STATE_UNKNOWN;
		}
	}
	DebugString("TRUECOLOUR: searching %s\n", directory.c_str());
}


/// <summary>
/// Draws the PNG that replaces a shape frame, if there is one. Returns false, having drawn
/// nothing, when the caller must draw the SHP frame itself: the surface is not 16 bit, the
/// draw is an alpha-buffer or predator effect, a translucent or remapped shadow, or no PNG
/// covers that frame. A shadow draw darkens under the PNG frame's alpha. A depth shape gives
/// each pixel the depth the SHP's pixel at that spot would get.
/// </summary>
bool TrueColour_Draw(Surface & surface, ConvertClass & convert, ShapeSet const * shapefile, int shapenum, Point2D const & point, Rect const & window, ShapeFlags_Type flags,
	unsigned char const * remap, int height_offset, ZGradientType zgrad, int intensity, ShapeSet const * z_shapefile, int z_shapenum, Point2D const & z_off)
{
	static constexpr int SkippedFlags = SHAPE_PREDATOR | SHAPE_NOTRANS | SHAPE_ALPHA_BLEND | SHAPE_ALPHA_WRITE_MULT | SHAPE_ALPHA_WRITE | SHAPE_ZERO_ALPHA | SHAPE_NONZERO_ALPHA;

	if (Entries.empty() || (flags & SkippedFlags) != 0 || surface.Bytes_Per_Pixel() != 2 || convert.Bytes_Per_Pixel() != 2) {
		return(false);
	}

	// The SHP draws a translucent or remapped shadow with a non-darkening blitter.
	bool const darken = (flags & SHAPE_DARKEN) != 0;
	if (darken && (remap != NULL || (flags & SHAPE_TRANSLUCENT75) != 0)) {
		return(false);
	}

	SheetType const * sheet = Sheet_For(shapefile);
	if (sheet == NULL || shapenum < 0 || shapenum >= sheet->Frames || (!darken && convert.Get_Intensity_Table() == NULL)) {
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
	if (darken) {
		blitter.Darken = true;
		blitter.HalfMask = (unsigned short)convert.Get_Halfbright_Mask();
		blitter.Lighting = NULL;
	}

	int x = point.X;
	int y = point.Y;
	if (flags & SHAPE_CENTER) {
		x -= shapefile->Get_Width() / 2;
		y -= shapefile->Get_Height() / 2;
	}

	BSurface source(sheet->Width, sheet->Height, 4, (void *)sheet->Pixels.data());
	Rect cell((shapenum % sheet->Columns) * sheet->CellWidth, (shapenum / sheet->Columns) * sheet->CellHeight, sheet->CellWidth, sheet->CellHeight);

	// Matches RLE_Blit: the depth comes from the SHP frame's own rectangle, not the PNG cell.
	if (z_shapefile != NULL && DepthBuffer != NULL && zgrad >= ZGRAD_FIRST && zgrad < ZGRAD_COUNT && shapefile->Is_RLE_Compressed(shapenum)) {
		Rect rect = shapefile->Get_Rect(shapenum);
		Rect zrect = z_shapefile->Get_Rect(z_shapenum);
		signed char const * zdata = (signed char const *)z_shapefile->Get_Data(z_shapenum);
		Rect drawn(x + rect.X, y + rect.Y, rect.Width, rect.Height);
		Rect shaperect(0, 0, rect.Width, rect.Height);
		Rect clipped = shaperect;
		ZGradStruct const & grad = ZGradients[zgrad];
		if (zdata != NULL && Blit_Clip(drawn, window, clipped, shaperect)) {
			int base;
			if (grad.IsTopDown) {
				base = (unsigned short)DepthBuffer->Get_Scroll_Delta(window.Y + drawn.Y - DepthBuffer->Bounds.Y) + height_offset;
				base = (base / grad.LineIncrement) * grad.LineIncrement;
			} else {
				base = (unsigned short)DepthBuffer->Get_Scroll_Delta(window.Y + y + rect.Y + rect.Height - 1 - DepthBuffer->Bounds.Y) + height_offset;
			}
			blitter.ZShape = zdata;
			blitter.ZShapeWidth = zrect.Width;
			blitter.ZShapeSize = (long)zrect.Width * zrect.Height;
			blitter.ZOriginX = z_off.X - shapefile->Get_Width() / 2 + zrect.X;
			blitter.ZOriginY = z_off.Y - shapefile->Get_Height() / 2 + zrect.Y;
			blitter.ZBase = base;
			blitter.SheetWidth = sheet->Width;
			blitter.CellLeft = cell.X;
			blitter.CellTop = cell.Y;
		}
	}
	Bit_Blit(surface, window, Rect(x, y, sheet->CellWidth, sheet->CellHeight), source, cell, Rect(0, 0, sheet->CellWidth, sheet->CellHeight), blitter, height_offset, zgrad, intensity, 0);
	return(true);
}


/// <summary>
/// Writes to the log whether a shape has a PNG in its place and, if so, the sheet's size, the
/// frames it covers and the centre pixel of its first frame. The shape is fetched by name.
/// </summary>
void TrueColour_Report(char const * name)
{
	LooseShape loose;
	void const * data = Fetch_Shape(name, loose);
	if (data == NULL) {
		DebugString("AUTOTEST   truecolour %s: no such shape\n", name);
		return;
	}
	ShapeHeader header = Header_Of(data);
	SheetRecord const * record = Record_For(data);
	SheetType const * sheet = record != NULL ? record->Sheet.get() : NULL;
	if (sheet == NULL) {
		DebugString("AUTOTEST   truecolour %s shp %d frames %dx%d: no override (%s)\n", name, header.Count, header.Width, header.Height,
			record != NULL && record->State == STATE_REJECTED ? "rejected" : "no png");
		return;
	}
	int x = sheet->CellWidth / 2;
	int y = sheet->CellHeight / 2;
	std::uint32_t pixel = sheet->Pixels[(size_t)y * sheet->Width + x];
	DebugString("AUTOTEST   truecolour %s %s %d frames %dx%d: png %s sheet %dx%d frames %d house %d shadows %s pixel %d,%d rgba %d,%d,%d,%d\n", name,
		Is_Made_Shape(data) ? "png-only" : "shp", header.Count, header.Width, header.Height, record->PngName.c_str(), sheet->Width, sheet->Height, sheet->Frames,
		(int)sheet->HasHouse, sheet->Frames == header.Count ? "png" : "shp", x, y,
		(int)(pixel & 0xFF), (int)((pixel >> 8) & 0xFF), (int)((pixel >> 16) & 0xFF), (int)(pixel >> 24));
}


/// <summary>
/// Writes every frame of a shape, fetched by name, to a PNG sheet in the layout this file
/// reads, coloured with the unit palette of the current theater. Index 0 is written fully
/// transparent. When the shape uses house-colour indices 16-31, a mask of the same size is
/// written beside the sheet with "_hc" before the extension. Returns false when the shape is
/// not found or a file cannot be written.
/// </summary>
bool TrueColour_Export(char const * name, char const * path)
{
	LooseShape loose;
	ShapeSet const * shape = (ShapeSet const *)Fetch_Shape(name, loose);
	if (shape == NULL) {
		DebugString("AUTOTEST   exportshape %s: no such shape\n", name);
		return(false);
	}

	int const width = shape->Get_Width();
	int const height = shape->Get_Height();
	int const count = shape->Get_Count();
	if (width <= 0 || height <= 0 || count <= 0) {
		return(false);
	}
	int const columns = std::min(count, std::max(1, 4096 / width));
	int const rows = (count + columns - 1) / columns;
	int const sheetwidth = columns * width;
	int const sheetheight = rows * height;

	std::vector<std::uint8_t> indices((size_t)sheetwidth * sheetheight, 0);
	for (int frame = 0; frame < count; frame++) {
		Rect rect = shape->Get_Rect(frame);
		unsigned char const * data = (unsigned char const *)shape->Get_Data(frame);
		if (data == NULL || rect.Width <= 0 || rect.Height <= 0) {
			continue;
		}
		int const left = (frame % columns) * width + rect.X;
		int const top = (frame / columns) * height + rect.Y;
		bool const rle = shape->Is_RLE_Compressed(frame);		for (int line = 0; line < rect.Height; line++) {
			std::uint8_t * out = &indices[(size_t)(top + line) * sheetwidth + left];
			if (!rle) {
				std::memcpy(out, data, (size_t)rect.Width);
				data += rect.Width;
				continue;
			}
			unsigned short length = 0;
			std::memcpy(&length, data, sizeof(length));
			unsigned char const * run = data + sizeof(length);
			unsigned char const * end = data + length;
			int column = 0;
			while (run < end && column < rect.Width) {
				if (*run == 0) {
					column += run + 1 < end ? run[1] : rect.Width;
					run += 2;
				} else {
					out[column++] = *run++;
				}
			}
			data = end;
		}
	}

	std::vector<unsigned char> pixels(indices.size() * 4, 0);
	std::vector<unsigned char> mask(indices.size() * 4, 0);
	bool house = false;
	for (size_t index = 0; index < indices.size(); index++) {
		int colour = indices[index];
		if (colour == 0) {
			continue;
		}
		pixels[index * 4 + 0] = SchemePalette[colour].Get_Red();
		pixels[index * 4 + 1] = SchemePalette[colour].Get_Green();
		pixels[index * 4 + 2] = SchemePalette[colour].Get_Blue();
		pixels[index * 4 + 3] = 255;
		if (colour >= 16 && colour <= 31) {
			// The loader turns shade s back into index 31 - s * 15 / 255.
			unsigned char shade = (unsigned char)std::max((31 - colour) * 17, 1);
			mask[index * 4 + 0] = mask[index * 4 + 1] = mask[index * 4 + 2] = shade;
			mask[index * 4 + 3] = 255;
			house = true;
		}
	}

	auto write = [&](std::string const & filename, std::vector<unsigned char> const & image) {
		size_t size = 0;
		void * png = tdefl_write_image_to_png_file_in_memory_ex(image.data(), sheetwidth, sheetheight, 4, &size, MZ_DEFAULT_LEVEL, false);
		if (png == NULL) {
			return(false);
		}
		RawFileClass file(filename.c_str());
		bool ok = file.Open(FileClass::WRITE) && file.Write(png, (int)size) == (int)size;
		file.Close();
		mz_free(png);
		return(ok);
	};

	std::string sheetname = path;
	std::string maskname = sheetname;
	size_t dot = maskname.find_last_of('.');
	size_t slash = maskname.find_last_of("\\/");
	if (dot != std::string::npos && (slash == std::string::npos || dot > slash)) {
		maskname.insert(dot, "_hc");
	} else {
		maskname += "_hc";
	}

	bool ok = write(sheetname, pixels) && (!house || write(maskname, mask));
	DebugString("AUTOTEST   exportshape %s frames %d cell %dx%d sheet %dx%d house %d -> %s %s\n", name, count, width, height, sheetwidth, sheetheight, (int)house, path,
		ok ? "ok" : "failed");
	return(ok);
}
