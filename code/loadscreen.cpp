/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "loadscreen.h"

#include "_pk.h"
#include "campaign.h"
#include "ccfile.h"
#include "ccini.h"
#include "convert.h"
#include "csf.h"
#include "data.h"
#include "dbgprint.h"
#include "dialog.h"
#include "draw.h"
#include "dsurface.h"
#include "font.h"
#include "globals.h"
#include "gscreen.h"
#include "mixfile.h"
#include "palette.h"
#include "progress.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "truecolour.h"

#include <algorithm>
#include <vector>


LoadScreenClass LoadScreen;


namespace {

// The height of the title bar above a campaign picture and of the strip below it.
int const STRIP_HEIGHT = 40;

// The width at which a campaign mission's loading briefing wraps.
int const BRIEFING_WIDTH = 400;

// How dark the picture is made behind text, in percent.
unsigned int const TEXT_SHADE = 62;


std::unique_ptr<MixFileClass> Mount(char const * name)
{
	if (!CCFileClass(name).Is_Available()) {
		return(nullptr);
	}
	return(std::make_unique<MixFileClass>(name, &FastKey));
}


std::unique_ptr<ConvertClass> Load_Drawer(std::string const & name, Surface const & surface)
{
	int const size = PaletteClass::COLOR_COUNT * 3;
	CCFileClass file(name.c_str());
	if (name.empty() || !file.Is_Available() || file.Size() < size) {
		DebugString("The loading screen palette %s is missing.\n", name.c_str());
		return(nullptr);
	}

	PaletteClass palette;
	file.Read(&palette[0], size);

	// Palette files hold six-bit colours, which the game widens to eight bits.
	for (int index = 0; index < PaletteClass::COLOR_COUNT; index++) {
		RGBClass const rgb = palette[index];
		palette[index] = RGBClass((unsigned char)(rgb.Get_Red() << 2), (unsigned char)(rgb.Get_Green() << 2), (unsigned char)(rgb.Get_Blue() << 2));
	}
	return(std::make_unique<ConvertClass>(palette, palette, surface));
}


// The colour a scheme prints text in on the loading screen: the colour it is defined with.
RGBClass Scheme_Text_Color(char const * name)
{
	ColorScheme const * scheme = Fetch_Scheme_By_Name(name);
	if (scheme == NULL) {
		return(RGBClass(255, 255, 255));
	}
	return(RGBClass(scheme->HSV));
}


FontClass const & Text_Font(void)
{
	return(*Font_From_TPF(TPF_EFNT));
}


// Prints one line of text in a single colour, with no background and no shadow.
void Print_Text(std::string const & text, Surface & surface, Point2D const & point, RGBClass const & color)
{
	PaletteClass palette;
	palette[1] = color;
	ConvertClass const drawer(palette, palette, surface);

	static unsigned char const remap[16] = {0, 1};
	Text_Font().Print(text.c_str(), surface, surface.Get_Rect(), point, drawer, remap);
}


// Breaks text into lines at its line breaks, and at the spaces that keep each line within width.
std::vector<std::string> Wrap_Text(std::string const & text, int width)
{
	FontClass const & font = Text_Font();
	std::vector<std::string> lines;

	std::size_t start = 0;
	while (start <= text.size()) {
		std::size_t end = text.find_first_of("\r\n", start);
		if (end == std::string::npos) {
			end = text.size();
		}

		std::string line;
		bool begun = false;
		std::size_t word = start;
		while (word <= end) {
			std::size_t space = text.find(' ', word);
			if (space == std::string::npos || space > end) {
				space = end;
			}
			std::string const piece = text.substr(word, space - word);
			std::string const longer = begun ? line + ' ' + piece : piece;
			if (begun && font.String_Pixel_Width(longer.c_str()) > width) {
				lines.push_back(line);
				line = piece;
			} else {
				line = longer;
			}
			begun = true;
			word = space + 1;
		}
		lines.push_back(line);

		if (end + 1 < text.size() && text[end] == '\r' && text[end + 1] == '\n') {
			end++;
		}
		start = end + 1;
	}
	return(lines);
}


// Prints wrapped text over a darkened box that reaches four pixels beyond it.
void Print_Shaded_Text(std::vector<std::string> const & lines, Surface & surface, Point2D const & point, RGBClass const & color)
{
	FontClass const & font = Text_Font();
	int width = 0;
	for (std::string const & line : lines) {
		width = std::max(width, font.String_Pixel_Width(line.c_str()));
	}
	int const height = font.Get_Height() * (int)lines.size();

	surface.Fill_Rect_Trans(Rect(point.X - 4, point.Y - 4, width + 8, height + 8), RGBClass(0, 0, 0), TEXT_SHADE);
	for (std::size_t index = 0; index < lines.size(); index++) {
		Print_Text(lines[index], surface, point + Point2D(0, font.Get_Height() * (int)index), color);
	}
}


// Copies part of a 16-bit surface onto another, each pixel made a square of factor pixels.
void Blit_Enlarged(Surface & dest, Point2D const & origin, Surface const & source, Rect const & area, int factor)
{
	Rect const from = Intersect(area, source.Get_Rect());
	if (!from.Is_Valid() || source.Bytes_Per_Pixel() != 2 || dest.Bytes_Per_Pixel() != 2) {
		return;
	}

	char const * spixels = (char const *)source.Lock();
	char * dpixels = (char *)dest.Lock();
	if (spixels != NULL && dpixels != NULL) {
		for (int y = from.Y; y < from.Y + from.Height; y++) {
			unsigned short const * srow = (unsigned short const *)(spixels + y * source.Stride());
			for (int repeat = 0; repeat < factor; repeat++) {
				int const dy = origin.Y + y * factor + repeat;
				if (dy < 0 || dy >= dest.Get_Height()) {
					continue;
				}
				unsigned short * drow = (unsigned short *)(dpixels + dy * dest.Stride());
				for (int x = from.X; x < from.X + from.Width; x++) {
					int const dx = origin.X + x * factor;
					for (int step = 0; step < factor; step++) {
						if (dx + step >= 0 && dx + step < dest.Get_Width()) {
							drow[dx + step] = srow[x];
						}
					}
				}
			}
		}
	}
	if (dpixels != NULL) {
		dest.Unlock();
	}
	if (spixels != NULL) {
		source.Unlock();
	}
}

}


// A shape file read whole into memory, for art in archives the game does not keep loaded.
class LoadScreenClass::ShapeFile
{
	public:
		explicit ShapeFile(char const * name)
		{
			CCFileClass file(name);
			if (!file.Is_Available()) {
				DebugString("The loading screen shape %s is missing.\n", name);
				return;
			}

			int const size = file.Size();
			Data = Load_Alloc_Data(file);

			// A SHP file starts with a zero word, a size and a frame count, then 24 bytes per frame.
			short const * header = (short const *)Data;
			if (Data != NULL && (size < 8 || header[0] != 0 || header[1] <= 0 || header[2] <= 0 || header[3] <= 0 || size < 8 + header[3] * 24)) {
				DebugString("The loading screen shape %s is not a SHP file.\n", name);
				Release();
			}
		}

		~ShapeFile(void)
		{
			Release();
		}

		ShapeFile(ShapeFile const &) = delete;
		ShapeFile & operator=(ShapeFile const &) = delete;

		ShapeSet const * Get(void) const {return((ShapeSet const *)Data);}

	private:
		void Release(void)
		{
			if (Data != NULL) {
				TrueColour_Forget_Range(Data, 1);
				delete [] (char *)Data;
				Data = NULL;
			}
		}

		void * Data = NULL;
};


LoadScreenClass::LoadScreenClass(void)
{
}


LoadScreenClass::~LoadScreenClass(void)
{
}


/// <summary>
/// Draws the Yuri's Revenge loading screen for the scenario about to be read and shows it.
/// Call after the progress screen is initialized; the screen stays up until End.
/// </summary>
/// <param name="scenario">The scenario's file name, as MISSIONMD.INI names its section.</param>
/// <returns>False, with nothing drawn, outside a campaign mission or when the mission's
/// picture or its palette cannot be found.</returns>
bool LoadScreenClass::Begin(char const * scenario)
{
	End();

	if (HiddenSurface == NULL || Session.Type != GAME_NORMAL) {
		return(false);
	}

	Width = (HiddenSurface->Get_Width() == 640) ? 640 : 800;
	Height = (Width == 640) ? 480 : 600;
	Scale = std::max(1, std::min(HiddenSurface->Get_Width() / Width, HiddenSurface->Get_Height() / Height));
	Origin = Point2D((HiddenSurface->Get_Width() - Width * Scale) / 2, (HiddenSurface->Get_Height() - Height * Scale) / 2);

	LoadMix = Mount("LOADMD.MIX");
	BaseLoadMix = Mount("LOAD.MIX");

	Backdrop = std::make_unique<DSurface>(Width, Height);
	Screen = std::make_unique<DSurface>(Width, Height);
	Backdrop->Fill(0);

	if (!Begin_Campaign(scenario)) {
		End();
		return(false);
	}

	Screen->Blit_From(*Backdrop);
	Active = true;
	Present(Screen->Get_Rect());
	return(true);
}


bool LoadScreenClass::Begin_Campaign(char const * scenario)
{
	CCFileClass database("MISSIONMD.INI");
	if (!database.Is_Available()) {
		return(false);
	}
	CCINIClass ini;
	ini.Load(database, false);

	bool const small = (Width == 640);
	PictureName = ini.Get_String(scenario, small ? "LS640BkgdName" : "LS800BkgdName");
	if (PictureName.empty()) {
		return(false);
	}

	// Both picture sizes are drawn with the 800x600 picture's palette.
	ShapeFile const picture(PictureName.c_str());
	std::unique_ptr<ConvertClass> const picture_drawer = Load_Drawer(ini.Get_String(scenario, "LS800BkgdPal"), *Backdrop);
	if (picture.Get() == NULL || picture_drawer == nullptr) {
		return(false);
	}

	Rect const title(0, 0, Width, STRIP_HEIGHT);
	Rect const art(0, STRIP_HEIGHT, Width, Height - STRIP_HEIGHT * 2);
	Rect const strip(0, Height - STRIP_HEIGHT, Width, STRIP_HEIGHT);

	ShapeFile const title_bar(small ? "TTLBR640.SHP" : "TTLBR800.SHP");
	std::unique_ptr<ConvertClass> const title_drawer = Load_Drawer("LDSCRN.PAL", *Backdrop);
	if (title_bar.Get() != NULL && title_drawer != nullptr) {
		Draw_Shape(*Backdrop, *title_drawer, title_bar.Get(), 0, title.Top_Left(), Backdrop->Get_Rect(), SHAPE_WIN_REL);
	}

	Draw_Shape(*Backdrop, *picture_drawer, picture.Get(), 0, art.Top_Left(), Backdrop->Get_Rect(), SHAPE_WIN_REL);

	ShapeFile const strip_art(small ? "SPLDBRS.SHP" : "SPLDBRL.SHP");
	BarDrawer = Load_Drawer("SPLDBR.PAL", *Backdrop);
	if (strip_art.Get() != NULL && BarDrawer != nullptr) {
		Draw_Shape(*Backdrop, *BarDrawer, strip_art.Get(), 0, strip.Top_Left(), Backdrop->Get_Rect(), SHAPE_WIN_REL);
	}
	BarShape = std::make_unique<ShapeFile>("SPLDBR.SHP");
	BarPos = strip.Top_Left() + Point2D(small ? 84 : 164, 7);

	// Only a campaign with CD=0 prints in the Allied colour; the stock campaigns all have CD=2.
	bool const allied = Scen->Campaign == CAMPAIGN_NONE || Campaigns[Scen->Campaign]->CDNumber == 0;
	RGBClass const color = Scheme_Text_Color(allied ? "AlliedLoad" : "SovietLoad");

	std::string const message = ini.Get_String(scenario, "LSLoadMessage");
	if (!message.empty()) {
		Print_Text(Fetch_String_UTF8(message.c_str()), *Backdrop, title.Top_Left() + Point2D(10, 10), color);
	}

	std::string const briefing = ini.Get_String(scenario, "LSLoadBriefing");
	if (!briefing.empty()) {
		Point2D const offset(ini.Get_Int(scenario, small ? "LS640BriefLocX" : "LS800BriefLocX"), ini.Get_Int(scenario, small ? "LS640BriefLocY" : "LS800BriefLocY"));
		Print_Shaded_Text(Wrap_Text(Fetch_String_UTF8(briefing.c_str()), BRIEFING_WIDTH), *Backdrop, art.Top_Left() + offset, color);
	}
	return(true);
}


/// <summary>
/// Draws the progress bar for how much of the scenario has been read.
/// </summary>
void LoadScreenClass::Draw_Progress(ProgressScreenClass const & progress)
{
	if (!Active || BarShape == nullptr || BarShape->Get() == NULL || BarDrawer == nullptr) {
		return;
	}

	ShapeSet const * shape = BarShape->Get();
	Rect const frame = shape->Get_Rect(0);

	// The bar is centred in a row as tall as the text beside it, though no text is printed.
	int const row = std::max(frame.Height + 6, Text_Font().Get_Height()) + 4;
	Point2D const at = BarPos + Point2D(5 + 3, (row - (frame.Height + 6)) / 2 + 3);

	double fraction = progress.Get_Current_Progress(0);
	if (!(fraction > 0.0)) {
		fraction = 0.0;
	}
	fraction = std::min(fraction, 1.0);

	Rect const strip(0, Height - STRIP_HEIGHT, Width, STRIP_HEIGHT);
	Screen->Blit_From(strip, *Backdrop, strip);

	Rect const bar(at.X, at.Y, (int)(frame.Width * fraction), frame.Height);
	if (bar.Width > 0) {
		Draw_Shape(*Screen, *BarDrawer, shape, 0, Point2D(0, 0), bar, SHAPE_WIN_REL);
	}
	Present(strip);
}


/// <summary>
/// Takes the loading screen down and releases its art. The hidden surface keeps the last picture.
/// </summary>
void LoadScreenClass::End(void)
{
	Active = false;
	PictureName.clear();
	BarShape.reset();
	BarDrawer.reset();
	Screen.reset();
	Backdrop.reset();
	BaseLoadMix.reset();
	LoadMix.reset();
}


void LoadScreenClass::Present(Rect const & area)
{
	Blit_Enlarged(*HiddenSurface, Origin, *Screen, area, Scale);
	Update_Visible_Surface();
}
