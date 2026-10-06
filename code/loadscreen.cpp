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

#include "_map.h"
#include "_pk.h"
#include "_tactica.h"
#include "campaign.h"
#include "ccfile.h"
#include "cell.h"
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
#include "house.h"
#include "houstype.h"
#include "lightcon.h"
#include "map.h"
#include "mixfile.h"
#include "palette.h"
#include "pcx.h"
#include "preview.h"
#include "progress.h"
#include "scenario.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "tactical.h"
#include "truecolour.h"
#include "utf8.h"

#include <algorithm>
#include <cstdio>
#include <vector>


LoadScreenClass LoadScreen;


namespace {

// The height of the title bar above a campaign picture and of the strip below it.
int const STRIP_HEIGHT = 40;

// The width at which a campaign mission's loading briefing wraps.
int const BRIEFING_WIDTH = 400;

// How dark the picture is made behind text, in percent.
unsigned int const TEXT_SHADE = 62;


// The art and text of a match's loading screen for one country, in the stock [Countries] order.
struct CountryArtType {
	char const * Picture;
	char const * Palette;
	char const * Name;
	char const * Special;
	char const * Brief;
};

CountryArtType const COUNTRY_ART[] = {
	{"USTATES", "MPLSU.PAL", "Name:Americans", "Name:Para", "LoadBrief:USA"},
	{"KOREA", "MPLSK.PAL", "Name:Alliance", "Name:BEAGLE", "LoadBrief:Korea"},
	{"FRANCE", "MPLSF.PAL", "Name:French", "Name:GTGCAN", "LoadBrief:French"},
	{"GERMANY", "MPLSG.PAL", "Name:Germans", "Name:TNKD", "LoadBrief:Germans"},
	{"UKINGDOM", "MPLSUK.PAL", "Name:British", "Name:SNIPE", "LoadBrief:British"},
	{"LIBYA", "MPLSL.PAL", "Name:Africans", "Name:DTRUCK", "LoadBrief:Lybia"},
	{"IRAQ", "MPLSI.PAL", "Name:Arabs", "Name:DESO", "LoadBrief:Iraq"},
	{"CUBA", "MPLSC.PAL", "Name:Confederation", "Name:TERROR", "LoadBrief:Cuba"},
	{"RUSSIA", "MPLSR.PAL", "Name:Russians", "Name:TTNK", "LoadBrief:Russia"},
	{"YURI", "MPYLS.PAL", "Name:YuriCountry", "Name:YURI", "LoadBrief:YuriCountry"},
};

CountryArtType const OBSERVER_ART = {"OBS", "MPLSOBS.PAL", "Name:Observer", NULL, NULL};

// The flag a player row shows, for each stock country in [Countries] order.
char const * const FLAGS[] = {
	"USAI.PCX", "JAPI.PCX", "FRAI.PCX", "GERI.PCX", "GBRI.PCX", "DJBI.PCX", "ARBI.PCX", "LATI.PCX", "RUSI.PCX", "YRII.PCX"
};


// A match's layout positions, for the 800x600 and the 640x480 layout.
struct MatchLayoutType {
	Point2D Rows;
	int RowWidth;
	Rect CountryName;
	Point2D Special;
	Rect Brief;
	Point2D Loading;
	Rect Preview;
};

MatchLayoutType const MATCH_800 = {Point2D(16, 321), 406, Rect(540, 310, 200, 20), Point2D(20, 90), Rect(20, 158, 398, 130), Point2D(20, 300), Rect(499, 379, 216, 166)};
MatchLayoutType const MATCH_640 = {Point2D(12, 256), 326, Rect(385, 436, 200, 20), Point2D(16, 72), Rect(16, 126, 318, 104), Point2D(16, 235), Rect(385, 270, 200, 200)};


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
RGBClass Scheme_Text_Color(ColorScheme const * scheme)
{
	if (scheme == NULL) {
		return(RGBClass(255, 255, 255));
	}
	return(RGBClass(scheme->HSV));
}


FontClass const & Text_Font(void)
{
	return(*Font_From_TPF(TPF_EFNT));
}


int Text_Width(std::string const & text)
{
	return(Text_Font().String_Pixel_Width(text.c_str()));
}


// Prints one line of text in a single colour, with no background and no shadow.
void Print_Text(std::string const & text, Surface & surface, Point2D const & point, RGBClass const & color, Rect const & clip)
{
	PaletteClass palette;
	palette[1] = color;
	ConvertClass const drawer(palette, palette, surface);

	static unsigned char const remap[16] = {0, 1};
	Text_Font().Print(text.c_str(), surface, clip, point - clip.Top_Left(), drawer, remap);
}


void Print_Text(std::string const & text, Surface & surface, Point2D const & point, RGBClass const & color)
{
	Print_Text(text, surface, point, color, surface.Get_Rect());
}


// Breaks text into lines at its line breaks, and at the spaces that keep each line within width.
std::vector<std::string> Wrap_Text(std::string const & text, int width)
{
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
			if (begun && Text_Width(longer) > width) {
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


// Darkens the picture behind text in a box reaching four pixels beyond the area given.
void Shade(Surface & surface, Rect const & area)
{
	surface.Fill_Rect_Trans(Rect(area.X - 4, area.Y - 4, area.Width + 8, area.Height + 8), RGBClass(0, 0, 0), TEXT_SHADE);
}


void Print_Lines(std::vector<std::string> const & lines, Surface & surface, Point2D const & point, RGBClass const & color)
{
	int const height = Text_Font().Get_Height();
	for (std::size_t index = 0; index < lines.size(); index++) {
		Print_Text(lines[index], surface, point + Point2D(0, height * (int)index), color);
	}
}


int Widest_Line(std::vector<std::string> const & lines)
{
	int width = 0;
	for (std::string const & line : lines) {
		width = std::max(width, Text_Width(line));
	}
	return(width);
}


// Text with its small letters made capitals, among the letters of the Latin-1 range.
std::string Capitals(std::string const & text)
{
	std::string result;
	char const * cursor = text.c_str();
	while (*cursor != '\0') {
		char32_t code = UTF8::Decode(cursor);
		if ((code >= 'a' && code <= 'z') || (code >= 0xE0 && code <= 0xFE)) {
			code -= 0x20;
		}
		char sequence[UTF8::MAX_SEQUENCE];
		result.append(sequence, UTF8::Encode(code, sequence));
	}
	return(result);
}


// A PCX picture with the colour it shows as transparent, magenta.
struct FlagType {
	std::unique_ptr<Surface> Picture;
	PaletteClass Palette;
};


FlagType Load_Flag(char const * name)
{
	FlagType flag;
	if (name != NULL) {
		CCFileClass file(name);
		flag.Picture.reset(Read_PCX_File(file, &flag.Palette));
	}
	return(flag);
}


void Draw_Flag(Surface & dest, Point2D const & point, FlagType const & flag)
{
	Surface const * picture = flag.Picture.get();
	if (picture == NULL || dest.Bytes_Per_Pixel() != 2) {
		return;
	}

	int const magenta = DSurface::Build_Hicolor_Pixel(255, 0, 255);
	unsigned char const * pixels = (unsigned char const *)picture->Lock();
	char * dpixels = (char *)dest.Lock();
	if (pixels != NULL && dpixels != NULL) {
		for (int y = 0; y < picture->Get_Height(); y++) {
			int const dy = point.Y + y;
			if (dy < 0 || dy >= dest.Get_Height()) {
				continue;
			}
			unsigned short * drow = (unsigned short *)(dpixels + dy * dest.Stride());
			unsigned char const * row = pixels + y * picture->Stride();
			for (int x = 0; x < picture->Get_Width(); x++) {
				int const dx = point.X + x;
				if (dx < 0 || dx >= dest.Get_Width()) {
					continue;
				}
				int pixel;
				if (picture->Bytes_Per_Pixel() == 1) {
					RGBClass const & rgb = flag.Palette[row[x]];
					if (rgb.Get_Red() == 255 && rgb.Get_Green() == 0 && rgb.Get_Blue() == 255) {
						continue;
					}
					pixel = DSurface::Build_Hicolor_Pixel(rgb);
				} else {
					pixel = ((unsigned short const *)row)[x];
				}
				if (pixel != magenta) {
					drow[dx] = (unsigned short)pixel;
				}
			}
		}
	}
	if (dpixels != NULL) {
		dest.Unlock();
	}
	if (pixels != NULL) {
		picture->Unlock();
	}
}


// Copies a 16-bit surface into a rectangle of another, each pixel taken from the nearest one.
void Blit_Stretched(Surface & dest, Rect const & to, Surface const & source)
{
	if (!to.Is_Valid() || source.Get_Width() <= 0 || source.Get_Height() <= 0 || source.Bytes_Per_Pixel() != 2 || dest.Bytes_Per_Pixel() != 2) {
		return;
	}

	char const * spixels = (char const *)source.Lock();
	char * dpixels = (char *)dest.Lock();
	if (spixels != NULL && dpixels != NULL) {
		for (int y = 0; y < to.Height; y++) {
			int const dy = to.Y + y;
			if (dy < 0 || dy >= dest.Get_Height()) {
				continue;
			}
			unsigned short const * srow = (unsigned short const *)(spixels + (y * source.Get_Height() / to.Height) * source.Stride());
			unsigned short * drow = (unsigned short *)(dpixels + dy * dest.Stride());
			for (int x = 0; x < to.Width; x++) {
				int const dx = to.X + x;
				if (dx >= 0 && dx < dest.Get_Width()) {
					drow[dx] = srow[x * source.Get_Width() / to.Width];
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


ColorScheme const * Player_Scheme(NodeNameType const * node)
{
	int const index = Session.Color_Index_To_Scheme(node->Player.Color);
	if (index < 0 || index >= ColorSchemes.Count()) {
		return(NULL);
	}
	return(ColorSchemes[index]);
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
/// <param name="ini">The scenario file's contents, for a match's map preview, or NULL.</param>
/// <param name="players">The number of players whose progress is shown.</param>
/// <returns>False, with nothing drawn, when the art for the mission or the local player's
/// country cannot be found.</returns>
bool LoadScreenClass::Begin(char const * scenario, CCINIClass const * ini, int players)
{
	End();

	if (HiddenSurface == NULL) {
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

	Match = Session.Type != GAME_NORMAL;
	if (!(Match ? Begin_Match(ini, players) : Begin_Campaign(scenario))) {
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
	RGBClass const color = Scheme_Text_Color(Fetch_Scheme_By_Name(allied ? "AlliedLoad" : "SovietLoad"));

	std::string const message = ini.Get_String(scenario, "LSLoadMessage");
	if (!message.empty()) {
		Print_Text(Fetch_String_UTF8(message.c_str()), *Backdrop, title.Top_Left() + Point2D(10, 10), color);
	}

	std::string const briefing = ini.Get_String(scenario, "LSLoadBriefing");
	if (!briefing.empty()) {
		Point2D const at = art.Top_Left() + Point2D(ini.Get_Int(scenario, small ? "LS640BriefLocX" : "LS800BriefLocX"), ini.Get_Int(scenario, small ? "LS640BriefLocY" : "LS800BriefLocY"));
		std::vector<std::string> const lines = Wrap_Text(Fetch_String_UTF8(briefing.c_str()), BRIEFING_WIDTH);
		Shade(*Backdrop, Rect(at.X, at.Y, Widest_Line(lines), Text_Font().Get_Height() * (int)lines.size()));
		Print_Lines(lines, *Backdrop, at, color);
	}
	return(true);
}


bool LoadScreenClass::Begin_Match(CCINIClass const * ini, int players)
{
	if (Session.Players.Count() == 0) {
		return(false);
	}

	NodeNameType const * local = Session.Players[0];
	bool const observer = local->Player.IsObserver;
	int const country = local->Player.House;
	if (!observer && (country < 0 || country >= (int)ARRAY_SIZE(COUNTRY_ART))) {
		return(false);
	}
	CountryArtType const & art = observer ? OBSERVER_ART : COUNTRY_ART[country];
	MatchLayoutType const & layout = (Width == 640) ? MATCH_640 : MATCH_800;

	char picture[32];
	std::snprintf(picture, sizeof(picture), "LS%d%s.SHP", Width, art.Picture);
	PictureName = picture;

	ShapeFile const shape(picture);
	std::unique_ptr<ConvertClass> const drawer = Load_Drawer(art.Palette, *Backdrop);
	if (shape.Get() == NULL || drawer == nullptr) {
		return(false);
	}
	Draw_Shape(*Backdrop, *drawer, shape.Get(), 0, Point2D(0, 0), Backdrop->Get_Rect(), SHAPE_WIN_REL);

	PreviewBox = layout.Preview;
	if (ini != NULL && !Scen->IsRandom && ini->Is_Present("Preview")) {
		Preview = std::make_unique<MapPreviewClass>();
		if (Preview->Read_INI(*ini) && Preview->Get_Preview_Surface() != NULL) {
			Draw_Preview();
		} else {
			Preview.reset();
		}
	}

	// Allied countries print in the Allied colour and every other side's in the Soviet one.
	char const * scheme = "LightGrey";
	if (!observer) {
		scheme = (country < HouseTypes.Count() && HouseTypes[country]->Side == SIDE_FIRST) ? "AlliedLoad" : "SovietLoad";
	}
	RGBClass const color = Scheme_Text_Color(Fetch_Scheme_By_Name(scheme));

	std::string const name = Fetch_String_UTF8(art.Name);
	int const name_width = Text_Width(name);
	Point2D const name_at(layout.CountryName.X + layout.CountryName.Width - name_width, layout.CountryName.Y);
	Shade(*Backdrop, Rect(name_at.X, name_at.Y, name_width, Text_Font().Get_Height()));
	Print_Text(name, *Backdrop, name_at, color);

	if (art.Special != NULL) {
		Print_Text(Capitals(Fetch_String_UTF8(art.Special)), *Backdrop, layout.Special, RGBClass(0, 0, 0));

		std::vector<std::string> const lines = Wrap_Text(Fetch_String_UTF8(art.Brief), layout.Brief.Width);
		Shade(*Backdrop, Rect(layout.Brief.X, layout.Brief.Y, layout.Brief.Width, Text_Font().Get_Height() * (int)lines.size()));
		Print_Lines(lines, *Backdrop, layout.Brief.Top_Left(), color);
	}

	std::string const loading = Fetch_String_UTF8("GUI:LoadingEx");
	Shade(*Backdrop, Rect(layout.Loading.X, layout.Loading.Y, Text_Width(loading), Text_Font().Get_Height()));
	Print_Text(loading, *Backdrop, layout.Loading, color);

	BarShape = std::make_unique<ShapeFile>("PROGBARM.SHP");
	BarPos = layout.Rows;
	RowWidth = layout.RowWidth;
	Draw_Rows(players);
	return(true);
}


// Draws the map's preview on a black box, enlarged or reduced to fit it and centred in it.
void LoadScreenClass::Draw_Preview(void)
{
	Surface const & picture = *Preview->Get_Preview_Surface();
	if (picture.Get_Width() <= 0 || picture.Get_Height() <= 0) {
		return;
	}

	// The scale is kept in thousandths, as Yuri's Revenge keeps it.
	int const scale = std::min(PreviewBox.Width * 1000 / picture.Get_Width(), PreviewBox.Height * 1000 / picture.Get_Height());
	int const width = picture.Get_Width() * scale / 1000;
	int const height = picture.Get_Height() * scale / 1000;
	PreviewArea = Rect(PreviewBox.X + (PreviewBox.Width - width) / 2, PreviewBox.Y + (PreviewBox.Height - height) / 2, width, height);

	Backdrop->Fill_Rect(PreviewBox, 0);
	Blit_Stretched(*Backdrop, PreviewArea, picture);
}


// Draws each player's row: an outline in the player's colour for the bar, the flag, and the name.
void LoadScreenClass::Draw_Rows(int players)
{
	ShapeSet const * shape = BarShape->Get();
	if (shape == NULL) {
		return;
	}
	Rect const frame = shape->Get_Rect(0);

	// Every row is laid out for the size of the American flag, whatever flag it shows.
	FlagType const reference = Load_Flag(FLAGS[0]);
	int const flag_width = reference.Picture != nullptr ? reference.Picture->Get_Width() : 0;
	int const flag_height = reference.Picture != nullptr ? reference.Picture->Get_Height() : 0;
	int const font_height = Text_Font().Get_Height();

	Rows = std::min(players, Session.Players.Count());
	RowHeight = std::max({frame.Height + 6, flag_height, font_height}) + 4;

	for (int index = 0; index < Rows; index++) {
		NodeNameType const * node = Session.Players[index];
		RGBClass const color = Scheme_Text_Color(Player_Scheme(node));
		int const top = BarPos.Y + RowHeight * index;

		Rect const box(BarPos.X + 5, top + (RowHeight - (frame.Height + 6)) / 2, frame.Width + 6, frame.Height + 6);
		Backdrop->Draw_Rect(box, DSurface::Build_Hicolor_Pixel(color));

		char const * flag_name = NULL;
		if (node->Player.IsObserver) {
			flag_name = "OBSI.PCX";
		} else if (node->Player.House >= 0 && node->Player.House < (int)ARRAY_SIZE(FLAGS)) {
			flag_name = FLAGS[node->Player.House];
		}
		Point2D const flag_at(BarPos.X + frame.Width + 21, top + (RowHeight - flag_height) / 2);
		Draw_Flag(*Backdrop, flag_at, Load_Flag(flag_name));

		Point2D const name_at(flag_at.X + flag_width + 10, top + (RowHeight - font_height) / 2);
		Rect const clip(0, 0, BarPos.X + RowWidth - 3, Height);
		Print_Text(Session.Shown_Seat_Name(node), *Backdrop, name_at, color, clip);
	}
}


/// <summary>
/// Draws the progress bar for how much of the scenario has been read, or for a match a bar for
/// each player.
/// </summary>
void LoadScreenClass::Draw_Progress(ProgressScreenClass const & progress)
{
	if (!Active || BarShape == nullptr || BarShape->Get() == NULL) {
		return;
	}
	if (Match) {
		Draw_Match_Progress(progress);
	} else {
		Draw_Campaign_Progress(progress);
	}
}


void LoadScreenClass::Draw_Campaign_Progress(ProgressScreenClass const & progress)
{
	if (BarDrawer == nullptr) {
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


void LoadScreenClass::Draw_Match_Progress(ProgressScreenClass const & progress)
{
	ShapeSet const * shape = BarShape->Get();
	Rect const frame = shape->Get_Rect(0);
	int const rows = std::min({Rows, (int)progress.PlayerCount, Session.Players.Count()});

	Rect const area(BarPos.X, BarPos.Y, RowWidth, RowHeight * Rows);
	Screen->Blit_From(area, *Backdrop, area);

	for (int index = 0; index < rows; index++) {
		ColorScheme const * scheme = Player_Scheme(Session.Players[index]);
		if (scheme == NULL || scheme->Converter == NULL) {
			continue;
		}

		double fraction = progress.Get_Current_Progress(index);
		if (!(fraction > 0.0)) {
			fraction = 0.0;
		}
		fraction = std::min(fraction, 1.0);

		int const top = BarPos.Y + RowHeight * index + (RowHeight - (frame.Height + 6)) / 2;
		Rect const bar(BarPos.X + 5 + 3, top + 3, (int)(frame.Width * fraction), frame.Height);
		if (bar.Width > 0) {
			Draw_Shape(*Screen, *scheme->Converter, shape, 0, Point2D(0, 0), bar, SHAPE_WIN_REL);
		}
	}
	Present(area);
}


/// <summary>
/// Marks where each house starts on a match's map preview, in the house's colour. Call once the
/// houses have their start positions.
/// </summary>
void LoadScreenClass::Show_Start_Positions(void)
{
	if (!Active || Preview == nullptr || TacticalMap == NULL) {
		return;
	}

	// The preview spans the map's radar area: two pixels for each cell across, one down.
	int left = 10000;
	int top = 10000;
	Map.Reset_Iterator();
	for (CellClass * cell = Map.Iterate(); cell != NULL; cell = Map.Iterate()) {
		if (Map.In_Radar(cell->CellID)) {
			int x;
			int y;
			TacticalMap->Lepton_To_Map_Pixel(Coord(cell->CellID).X, Coord(cell->CellID).Y, x, y);
			left = std::min(left, x / ISO_TILE_PIXEL_W);
			top = std::min(top, y / ISO_TILE_PIXEL_H);
		}
	}
	auto preview_pixel = [&](Cell const & cell) {
		int x;
		int y;
		TacticalMap->Lepton_To_Map_Pixel(Coord(cell).X, Coord(cell).Y, x, y);
		return(Point2D((x / ISO_TILE_PIXEL_W - left) * 2, y / ISO_TILE_PIXEL_H - top));
	};

	/*
	** The marks a preview carries for the first start positions are blacked out, as Yuri's
	** Revenge does: one for each of the first eight waypoints that is set, counted from 0, so a
	** gap among them leaves the last ones marked.
	*/
	Surface & picture = *Preview->Get_Preview_Surface();
	int marked = 0;
	for (int waypoint = 0; waypoint < 8; waypoint++) {
		if (Scen->Is_Valid_Waypoint(waypoint)) {
			marked++;
		}
	}
	for (int waypoint = 0; waypoint < marked; waypoint++) {
		if (Scen->Is_Valid_Waypoint(waypoint)) {
			Point2D const pixel = preview_pixel(Scen->Get_Waypoint_Cell(waypoint));
			picture.Fill_Rect(Rect(pixel.X - 1, pixel.Y - 1, 4, 4), 0);
		}
	}
	Draw_Preview();

	ShapeFile const marker("MMPB.SHP");
	for (int index = 0; index < Houses.Count() && marker.Get() != NULL; index++) {
		HouseClass const * house = Houses[index];
		if (house->IsObserver || house->SpawnWaypoint < 0 || house->Scheme < 0 || house->Scheme >= ColorSchemes.Count()) {
			continue;
		}
		LightConvertClass * drawer = ColorSchemes[house->Scheme]->Converter;
		if (drawer == NULL) {
			continue;
		}

		Point2D const pixel = preview_pixel(Scen->Get_Waypoint_Cell(house->SpawnWaypoint));
		Point2D const at(PreviewArea.X + pixel.X * PreviewArea.Width / picture.Get_Width() - 3, PreviewArea.Y + pixel.Y * PreviewArea.Height / picture.Get_Height() - 2);
		Draw_Shape(*Backdrop, *drawer, marker.Get(), 0, at - PreviewBox.Top_Left(), PreviewBox, SHAPE_WIN_REL);
	}
	Preview.reset();

	Screen->Blit_From(PreviewBox, *Backdrop, PreviewBox);
	Present(PreviewBox);
}


/// <summary>
/// Takes the loading screen down and releases its art. The hidden surface keeps the last picture.
/// </summary>
void LoadScreenClass::End(void)
{
	Active = false;
	Match = false;
	PictureName.clear();
	BarShape.reset();
	BarDrawer.reset();
	Rows = 0;
	Preview.reset();
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
