/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "scorestage.h"

#include "_font.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_pk.h"
#include "_surface.h"
#include "ccfile.h"
#include "convert.h"
#include "color.hh"
#include "data.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsurface.h"
#include "font.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "mixfile.h"
#include "palette.h"
#include "pcx.h"
#include "shapeset.h"
#include "side.h"
#include "video.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>


namespace {

constexpr int STAGE_WIDTH = 800;
constexpr int STAGE_HEIGHT = 600;

/*
 * gamemd keeps the score screens' neutral art in these two archives. A match mounts only its
 * own side's archives, so the screens mount them while they are shown.
 */
int MountCount = 0;
MFCD * NeutralArchives[2] = { NULL, NULL };

void Mount_Neutral_Archives(void)
{
	static char const * const names[2] = { "NTRLMD.MIX", "NEUTRAL.MIX" };

	for (int i = 0; i < 2; i++) {
		if (NeutralArchives[i] == NULL && CCFileClass(names[i]).Is_Available()) {
			NeutralArchives[i] = new MFCD(names[i], &FastKey);
		}
	}
}

void Release_Neutral_Archives(void)
{
	for (int i = 0; i < 2; i++) {
		if (NeutralArchives[i] != NULL) {
			delete NeutralArchives[i];
			NeutralArchives[i] = NULL;
		}
	}
}

}


ScoreStageClass::ScoreStageClass(void) :
	Scale(1.0),
	FitOrigin(0, 0),
	StageSurface(NULL),
	BaseSurface(NULL),
	Font(NULL),
	TextConverter(NULL),
	Drawer(NULL),
	Mounted(false),
	StageWidth(STAGE_WIDTH),
	StageHeight(STAGE_HEIGHT),
	KeyedShape(NULL),
	KeyedWidth(0),
	KeyedHeight(0)
{
}


ScoreStageClass::~ScoreStageClass(void)
{
	Close();
}


/// <summary>
/// Creates the stage and its font, sizes the fit to the screen and mounts the neutral archives.
/// </summary>
/// <param name="fill_screen">When true the stage is the size of the screen at 1 to 1, rather than 800 by 600 fitted to it.</param>
/// <returns>bool; Was the stage created? Nothing is left allocated when it was not.</returns>
bool ScoreStageClass::Open(bool fill_screen)
{
	Close();

	int const screen_width = HiddenSurface->Get_Width();
	int const screen_height = HiddenSurface->Get_Height();
	StageWidth = fill_screen ? screen_width : STAGE_WIDTH;
	StageHeight = fill_screen ? screen_height : STAGE_HEIGHT;

	StageSurface = new DSurface(StageWidth, StageHeight);
	BaseSurface = new DSurface(StageWidth, StageHeight);
	if (StageSurface == NULL || BaseSurface == NULL) {
		DebugString("ScoreStage: Failed to create surface!\n");
		Close();
		return(false);
	}

	if (fill_screen) {
		Scale = 1.0;
		FitOrigin = Point2D(0, 0);
	} else {
		Scale = std::min(screen_width / (double)STAGE_WIDTH, screen_height / (double)STAGE_HEIGHT);
		int const fit_width = (int)(STAGE_WIDTH * Scale + 0.5);
		int const fit_height = (int)(STAGE_HEIGHT * Scale + 0.5);
		FitOrigin = Point2D((screen_width - fit_width) / 2, (screen_height - fit_height) / 2);
	}

	Font = Font6Ptr;
	if (Font == NULL) {
		DebugString("ScoreStage: Unable to create font!\n");
		Close();
		return(false);
	}
	Set_Text_Color(255, 255, 0);

	if (MountCount++ == 0) {
		Mount_Neutral_Archives();
	}
	Mounted = true;

	return(true);
}


/// <summary>
/// Releases the stage, the font and the neutral archives. It is safe to call at any time.
/// </summary>
void ScoreStageClass::Close(void)
{
	if (Drawer != NULL) {
		delete Drawer;
		Drawer = NULL;
	}
	if (TextConverter != NULL) {
		delete TextConverter;
		TextConverter = NULL;
	}
	Font = NULL;
	KeyedShape = NULL;
	KeyedFrames.clear();
	if (BaseSurface != NULL) {
		delete BaseSurface;
		BaseSurface = NULL;
	}
	if (StageSurface != NULL) {
		delete StageSurface;
		StageSurface = NULL;
	}
	if (Mounted) {
		Mounted = false;
		if (--MountCount == 0) {
			Release_Neutral_Archives();
		}
	}
}


Surface * ScoreStageClass::Canvas(void) const
{
	return(StageSurface);
}


/// <summary>
/// Copies the whole stage to the base surface, which the stage restores its areas from.
/// </summary>
void ScoreStageClass::Save_Base(void)
{
	BaseSurface->Blit_From(BaseSurface->Get_Rect(), *StageSurface, StageSurface->Get_Rect());
}


/// <summary>
/// Copies an area of the stage to the base surface.
/// </summary>
void ScoreStageClass::Keep(Rect const & area)
{
	BaseSurface->Blit_From(area, *StageSurface, area);
}


/// <summary>
/// Restores an area of the stage from the base surface, erasing whatever was drawn on it since.
/// </summary>
void ScoreStageClass::Restore(Rect const & area)
{
	StageSurface->Blit_From(area, *BaseSurface, area);
}


/// <summary>
/// Maps an area of the stage onto the screen and shows it.
/// </summary>
/// <param name="area">The area of the stage that changed, in stage pixels.</param>
void ScoreStageClass::Present(Rect const & area)
{
	Rect const screen = Stage_To_Screen(area);
	HiddenSurface->Blit_From(screen, *StageSurface, area);
	VisibleSurface->Blit_From(screen, *HiddenSurface, screen);
	Video_Present_If_Dirty();
}


/// <summary>
/// Fills the whole screen with black and shows it, so the game's own interface does not show
/// around the stage.
/// </summary>
void ScoreStageClass::Clear_Screen(void)
{
	Rect const frame = HiddenSurface->Get_Rect();
	HiddenSurface->Fill(TBLACK);
	VisibleSurface->Blit_From(frame, *HiddenSurface, frame);
	Video_Mark_Dirty();
	Video_Present_Now();
}


/// <summary>
/// Shows the whole hidden screen surface, as it was last filled.
/// </summary>
void ScoreStageClass::Show_Screen(void)
{
	Rect const frame = HiddenSurface->Get_Rect();
	VisibleSurface->Blit_From(frame, *HiddenSurface, frame);
	Video_Mark_Dirty();
	Video_Present_Now();
}


/// <summary>
/// Fills a surface with an area of the stage, averaged on a coarse grid, blurred and dimmed. The area
/// is read from the stage's current pixels, so it must be drawn before anything that should not be blurred.
/// </summary>
/// <param name="target">The 16 bit surface to fill. It may be any size; the stage is not changed.</param>
/// <param name="source">The stage area to take the art from, stretched over the whole target.</param>
void ScoreStageClass::Fill_Backdrop(Surface & target, Rect const & source)
{
	constexpr int COLUMNS = 32;
	constexpr int ROWS = 24;
	constexpr int SAMPLES = 4;
	constexpr double DIM = 0.45;

	if (StageSurface == NULL || target.Bytes_Per_Pixel() != 2) {
		return;
	}

	double grid[ROWS][COLUMNS][3];
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLUMNS; col++) {
			double sum[3] = { 0.0, 0.0, 0.0 };
			for (int sy = 0; sy < SAMPLES; sy++) {
				for (int sx = 0; sx < SAMPLES; sx++) {
					int const x = source.X + (int)((col + (sx + 0.5) / SAMPLES) * source.Width / COLUMNS);
					int const y = source.Y + (int)((row + (sy + 0.5) / SAMPLES) * source.Height / ROWS);
					RGBClass const rgb = DSurface::Deconstruct_Hicolor_Pixel((unsigned short)StageSurface->Get_Pixel(Point2D(x, y)));
					sum[0] += rgb.Get_Red();
					sum[1] += rgb.Get_Green();
					sum[2] += rgb.Get_Blue();
				}
			}
			for (int k = 0; k < 3; k++) {
				grid[row][col][k] = sum[k] / (SAMPLES * SAMPLES);
			}
		}
	}

	// Two passes of a 3 by 3 box blur, with the edge cells repeated outward.
	for (int pass = 0; pass < 2; pass++) {
		double blurred[ROWS][COLUMNS][3];
		for (int row = 0; row < ROWS; row++) {
			for (int col = 0; col < COLUMNS; col++) {
				double sum[3] = { 0.0, 0.0, 0.0 };
				for (int dy = -1; dy <= 1; dy++) {
					for (int dx = -1; dx <= 1; dx++) {
						int const r = std::clamp(row + dy, 0, ROWS - 1);
						int const c = std::clamp(col + dx, 0, COLUMNS - 1);
						for (int k = 0; k < 3; k++) {
							sum[k] += grid[r][c][k];
						}
					}
				}
				for (int k = 0; k < 3; k++) {
					blurred[row][col][k] = sum[k] / 9.0;
				}
			}
		}
		for (int row = 0; row < ROWS; row++) {
			for (int col = 0; col < COLUMNS; col++) {
				for (int k = 0; k < 3; k++) {
					grid[row][col][k] = blurred[row][col][k];
				}
			}
		}
	}

	int const width = target.Get_Width();
	int const height = target.Get_Height();
	for (int y = 0; y < height; y++) {
		unsigned short * line = (unsigned short *)target.Lock(Point2D(0, y));
		if (line == NULL) {
			break;
		}
		double const gy = std::clamp((y + 0.5) * ROWS / height - 0.5, 0.0, ROWS - 1.0);
		int const r0 = (int)gy;
		int const r1 = std::min(r0 + 1, ROWS - 1);
		double const ty = gy - r0;
		for (int x = 0; x < width; x++) {
			double const gx = std::clamp((x + 0.5) * COLUMNS / width - 0.5, 0.0, COLUMNS - 1.0);
			int const c0 = (int)gx;
			int const c1 = std::min(c0 + 1, COLUMNS - 1);
			double const tx = gx - c0;
			int channel[3];
			for (int k = 0; k < 3; k++) {
				double const top = grid[r0][c0][k] * (1.0 - tx) + grid[r0][c1][k] * tx;
				double const bottom = grid[r1][c0][k] * (1.0 - tx) + grid[r1][c1][k] * tx;
				channel[k] = (int)((top * (1.0 - ty) + bottom * ty) * DIM + 0.5);
			}
			line[x] = (unsigned short)DSurface::Build_Hicolor_Pixel(channel[0], channel[1], channel[2]);
		}
		target.Unlock();
	}
}


int ScoreStageClass::Width(void) const
{
	return(StageWidth);
}


int ScoreStageClass::Height(void) const
{
	return(StageHeight);
}


namespace {

/*
 * The remap for BitFontClass::Print. Index 1 is the glyph color. Index 0 (no background fill) and index 2
 * (no shadow) are left empty, so a glyph's set bits are the only pixels drawn, as gamemd's BitFont does.
 */
unsigned char const TEXT_REMAP[16] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

}


void ScoreStageClass::Print_Left(char const * text, int x, int y)
{
	Font->Print(text, *StageSurface, Rect(0, 0, StageWidth, StageHeight), Point2D(x, y), *TextConverter, TEXT_REMAP);
}


void ScoreStageClass::Print_Right(char const * text, int right, int y)
{
	Print_Left(text, right - Text_Width(text), y);
}


void ScoreStageClass::Print_Centre(char const * text, int left, int width, int y)
{
	Print_Left(text, left + (width - Text_Width(text)) / 2, y);
}


int ScoreStageClass::Text_Width(char const * text) const
{
	return(Font->String_Pixel_Width(text));
}


int ScoreStageClass::Text_Height(void) const
{
	return(Font->Get_Height());
}


/// <summary>
/// Loads a palette stored in 6 bit values into the game palette, and makes the drawer that
/// converts shapes through it.
/// </summary>
/// <param name="name">The palette's file name.</param>
void ScoreStageClass::Load_Palette(char const * name)
{
	CCFileClass file(name);
	PaletteClass * palette = (PaletteClass *)Load_Alloc_Data(file);
	file.Close();
	if (palette == NULL) return;

	for (int i = 0; i < 256; i++) {
		CCPalette[i] = RGBClass(
			((unsigned char *)*palette)[i*3] << 2,
			((unsigned char *)*palette)[i*3+1] << 2,
			((unsigned char *)*palette)[i*3+2] << 2);
	}
	delete palette;

	if (Drawer != NULL) {
		delete Drawer;
	}
	// Cached frames were drawn through the previous palette's drawer.
	KeyedShape = NULL;
	KeyedFrames.clear();
	Drawer = new ConvertClass(CCPalette, CCPalette, *StageSurface);
}


/// <summary>
/// Makes a drawer that converts shapes through a palette stored in 6 bit values, without changing the
/// stage's own drawer.
/// </summary>
/// <param name="name">The palette's file name.</param>
/// <returns>Returns with the drawer, or NULL when the palette is missing. Free it with Free_Drawer.</returns>
/// <remarks>The global CCPalette is overwritten with the palette.</remarks>
ConvertClass * ScoreStageClass::Load_Drawer(char const * name) const
{
	CCFileClass file(name);
	PaletteClass * palette = (PaletteClass *)Load_Alloc_Data(file);
	file.Close();
	if (palette == NULL) return(NULL);

	for (int i = 0; i < 256; i++) {
		CCPalette[i] = RGBClass(
			((unsigned char *)*palette)[i*3] << 2,
			((unsigned char *)*palette)[i*3+1] << 2,
			((unsigned char *)*palette)[i*3+2] << 2);
	}
	delete palette;

	return(new ConvertClass(CCPalette, CCPalette, *StageSurface));
}


void ScoreStageClass::Free_Drawer(ConvertClass * drawer)
{
	delete drawer;
}


/// <summary>
/// Sets the color that the text printed after this call is drawn in. Open starts the stage in yellow, the
/// color of gamemd's dialog text.
/// </summary>
/// <param name="red">The red of the color, 0 to 255.</param>
/// <param name="green">The green of the color, 0 to 255.</param>
/// <param name="blue">The blue of the color, 0 to 255.</param>
void ScoreStageClass::Set_Text_Color(int red, int green, int blue)
{
	PaletteClass palette(RGBClass(0, 0, 0));
	palette[1] = RGBClass(red, green, blue);
	ConvertClass * converter = new ConvertClass(palette, palette, *VisibleSurface);
	if (converter != NULL) {
		delete TextConverter;
		TextConverter = converter;
	}
}


/// <summary>
/// Draws one frame of a shape set at its own size, with the shape's own offsets, through a drawer.
/// </summary>
/// <param name="x">The stage x of the shape set's top left corner.</param>
/// <param name="y">The stage y of the shape set's top left corner.</param>
/// <param name="drawer">The drawer that holds the palette the shape set is stored in.</param>
void ScoreStageClass::Draw_Shape_At(ShapeSet const * shapes, int frame, int x, int y, ConvertClass * drawer)
{
	if (shapes == NULL || drawer == NULL) {
		return;
	}
	Draw_Shape(*StageSurface, *drawer, shapes, frame, Point2D(x, y), StageSurface->Get_Rect(), SHAPE_WIN_REL);
}


namespace {

// Returns one frame as premultiplied RGBA, four floats a pixel, with the dark blue surround keyed out.
std::vector<float> Key_Frame(ShapeSet const * shapes, int frame, ConvertClass & drawer, int width, int height)
{
	DSurface picture(width, height);
	picture.Fill(0);
	Draw_Shape(picture, drawer, shapes, frame, Point2D(0, 0), picture.Get_Rect(), SHAPE_WIN_REL);

	// Which pixels are the plate rather than its dark blue surround, then that spread over a few pixels so
	// that dark detail inside the plate stays solid and the plate's edge is soft.
	constexpr int REACH = 5;
	std::vector<float> colors((size_t)width * height * 3);
	std::vector<float> present((size_t)width * height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			RGBClass const rgb = DSurface::Deconstruct_Hicolor_Pixel((unsigned short)picture.Get_Pixel(Point2D(x, y)));
			float const r = rgb.Get_Red();
			float const g = rgb.Get_Green();
			float const b = rgb.Get_Blue();
			float const brightest = std::max(r, std::max(g, b));
			size_t const at = (size_t)y * width + x;
			colors[at * 3] = r;
			colors[at * 3 + 1] = g;
			colors[at * 3 + 2] = b;
			present[at] = (brightest >= 60.0f && !(b > r * 1.3f && brightest < 110.0f)) ? 1.0f : 0.0f;
		}
	}
	std::vector<float> spread((size_t)width * height);
	std::vector<float> across((size_t)width * height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float sum = 0.0f;
			for (int k = -REACH; k <= REACH; k++) {
				sum += present[(size_t)y * width + std::clamp(x + k, 0, width - 1)];
			}
			across[(size_t)y * width + x] = sum / (2 * REACH + 1);
		}
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float sum = 0.0f;
			for (int k = -REACH; k <= REACH; k++) {
				sum += across[(size_t)std::clamp(y + k, 0, height - 1) * width + x];
			}
			spread[(size_t)y * width + x] = sum / (2 * REACH + 1);
		}
	}

	// Premultiplied colors with alpha, 4 floats a source pixel.
	std::vector<float> source((size_t)width * height * 4);
	constexpr float FADE = 16.0f;
	for (size_t at = 0; at < (size_t)width * height; at++) {
		int const x = (int)(at % width);
		int const y = (int)(at / width);
		float const edge = (float)std::min(std::min(x, width - 1 - x), std::min(y, height - 1 - y));
		float const alpha = std::clamp((spread[at] - 0.15f) / 0.35f, 0.0f, 1.0f) * std::clamp(edge / FADE, 0.0f, 1.0f);
		source[at * 4] = colors[at * 3] * alpha;
		source[at * 4 + 1] = colors[at * 3 + 1] * alpha;
		source[at * 4 + 2] = colors[at * 3 + 2] * alpha;
		source[at * 4 + 3] = alpha;
	}
	return(source);
}

}


/// <summary>
/// Draws one frame of a shape set into a box on the stage, stretched to fill it, with its dark blue
/// background left out so that it blends into what is already on the stage. The background is the dark blue
/// pixels that have no bright pixel within a few pixels of them, and the frame's own edge fades out.
/// </summary>
/// <param name="box">The stage area the frame fills.</param>
/// <remarks>The frame is drawn through the drawer Load_Palette made. Each frame is keyed on first use and kept until the stage closes, the palette changes, or another shape is drawn.</remarks>
void ScoreStageClass::Draw_Keyed_Frame(ShapeSet const * shapes, int frame, Rect const & box)
{
	if (shapes == NULL || Drawer == NULL || box.Width <= 0 || box.Height <= 0) {
		return;
	}

	int const width = shapes->Get_Width();
	int const height = shapes->Get_Height();
	int const count = shapes->Get_Count();
	if (width <= 0 || height <= 0 || frame < 0 || frame >= count) {
		return;
	}

	// Frames are cached by the shape's address; Close or Load_Palette must run before a new shape can reuse a freed address.
	if (KeyedShape != shapes || KeyedWidth != width || KeyedHeight != height || KeyedFrames.size() != (size_t)count) {
		KeyedShape = shapes;
		KeyedWidth = width;
		KeyedHeight = height;
		KeyedFrames.assign((size_t)count, std::vector<float>());
	}
	std::vector<float> & source = KeyedFrames[(size_t)frame];
	if (source.empty()) {
		source = Key_Frame(shapes, frame, *Drawer, width, height);
	}

	for (int dy = 0; dy < box.Height; dy++) {
		int const row = box.Y + dy;
		if (row < 0 || row >= StageHeight) {
			continue;
		}
		unsigned short * line = (unsigned short *)StageSurface->Lock(Point2D(0, row));
		if (line == NULL) {
			break;
		}
		double const fy = std::clamp((dy + 0.5) * height / box.Height - 0.5, 0.0, height - 1.0);
		int const y0 = (int)fy;
		int const y1 = std::min(y0 + 1, height - 1);
		double const ty = fy - y0;
		for (int dx = 0; dx < box.Width; dx++) {
			int const column = box.X + dx;
			if (column < 0 || column >= StageWidth) {
				continue;
			}
			double const fx = std::clamp((dx + 0.5) * width / box.Width - 0.5, 0.0, width - 1.0);
			int const x0 = (int)fx;
			int const x1 = std::min(x0 + 1, width - 1);
			double const tx = fx - x0;
			double sample[4];
			for (int k = 0; k < 4; k++) {
				double const top = source[((size_t)y0 * width + x0) * 4 + k] * (1.0 - tx) + source[((size_t)y0 * width + x1) * 4 + k] * tx;
				double const bottom = source[((size_t)y1 * width + x0) * 4 + k] * (1.0 - tx) + source[((size_t)y1 * width + x1) * 4 + k] * tx;
				sample[k] = top * (1.0 - ty) + bottom * ty;
			}
			if (sample[3] <= 0.0) {
				continue;
			}
			RGBClass const behind = DSurface::Deconstruct_Hicolor_Pixel(line[column]);
			double const keep = 1.0 - sample[3];
			int const red = std::clamp((int)(behind.Get_Red() * keep + sample[0] + 0.5), 0, 255);
			int const green = std::clamp((int)(behind.Get_Green() * keep + sample[1] + 0.5), 0, 255);
			int const blue = std::clamp((int)(behind.Get_Blue() * keep + sample[2] + 0.5), 0, 255);
			line[column] = (unsigned short)DSurface::Build_Hicolor_Pixel(red, green, blue);
		}
		StageSurface->Unlock();
	}
}


/// <summary>
/// Reads a shape file from the mix archives.
/// </summary>
/// <returns>Returns with the shape set, or NULL when the file is missing. Free it with Free_Shape.</returns>
ShapeSet * ScoreStageClass::Load_Shape(char const * name) const
{
	CCFileClass file(name);
	ShapeSet * shapes = (ShapeSet *)Load_Alloc_Data(file);
	file.Close();
	return(shapes);
}


/// <summary>
/// Frees a shape set that Load_Shape returned.
/// </summary>
void ScoreStageClass::Free_Shape(ShapeSet const * shapes)
{
	delete [] (char *)shapes;
}


/// <summary>
/// Draws one frame of a shape set into a box on the stage. A frame that is not the box's size is
/// stretched to fill it.
/// </summary>
/// <param name="shapes">The shape set to draw from.</param>
/// <param name="frame">The frame number.</param>
/// <param name="box">The stage area the frame fills.</param>
void ScoreStageClass::Draw_Shape_Frame(ShapeSet const * shapes, int frame, Rect const & box)
{
	if (shapes == NULL || Drawer == NULL) {
		return;
	}

	if (shapes->Get_Width() == box.Width && shapes->Get_Height() == box.Height) {
		Draw_Shape(*StageSurface, *Drawer, shapes, frame, Point2D(box.X, box.Y), StageSurface->Get_Rect(), SHAPE_WIN_REL);
		return;
	}

	DSurface picture(shapes->Get_Width(), shapes->Get_Height());
	picture.Fill(0);
	Draw_Shape(picture, *Drawer, shapes, frame, Point2D(0, 0), picture.Get_Rect(), SHAPE_WIN_REL);
	StageSurface->Blit_From(box, picture, picture.Get_Rect());
}


/// <summary>
/// Draws a picture from its own palette onto the stage at a position. The picture is converted
/// through the palette it was saved with, so it keeps its colors whatever the game palette is.
/// </summary>
/// <param name="name">The picture's file name.</param>
/// <param name="x">The stage x of the picture's top left corner.</param>
/// <param name="y">The stage y of the picture's top left corner.</param>
void ScoreStageClass::Draw_Band(char const * name, int x, int y)
{
	CCFileClass file(name);
	PaletteClass palette;
	Surface * band = Read_PCX_File(file, &palette);
	if (band != NULL) {
		ConvertClass * drawer = new ConvertClass(palette, palette, *StageSurface);
		Blit_Block(*StageSurface, *drawer, *band, band->Get_Rect(), Point2D(x, y), StageSurface->Get_Rect());
		delete drawer;
		delete band;
	}
}


/// <summary>
/// Is the screen the 640 wide size? gamemd gives that size its small art.
/// </summary>
bool ScoreStageClass::Is_Small(void) const
{
	return(HiddenSurface->Get_Width() == 640);
}


/// <summary>
/// Names the side's art by the player's side: A for the first side in the rules' [Sides] list, S
/// for the second, and Y for any other side.
/// </summary>
char ScoreStageClass::Side_Letter(void)
{
	int index = -1;
	if (PlayerPtr != NULL && PlayerPtr->Class != NULL) {
		index = SideClass::Place_In_Sides(PlayerPtr->Class->Side);
	}
	if (index == 0) return('A');
	if (index == 1) return('S');
	return('Y');
}


int ScoreStageClass::Dialog_X(int units)
{
	return(units * 6 / 4);
}


int ScoreStageClass::Dialog_Y(int units)
{
	return(units * 13 / 8);
}


Rect ScoreStageClass::Dialog_Rect(int x, int y, int width, int height)
{
	return(Rect(Dialog_X(x), Dialog_Y(y), Dialog_X(x + width) - Dialog_X(x), Dialog_Y(y + height) - Dialog_Y(y)));
}


/// <summary>
/// Tests whether a screen point is inside an area of the stage as the stage is drawn on the screen.
/// </summary>
bool ScoreStageClass::Contains_Screen_Point(Rect const & area, int x, int y) const
{
	Rect const screen = Stage_To_Screen(area);
	return(x >= screen.X && x < screen.X + screen.Width && y >= screen.Y && y < screen.Y + screen.Height);
}


/// <summary>
/// Maps an area of the stage onto the screen rectangle it is stretched to.
/// </summary>
Rect ScoreStageClass::Stage_To_Screen(Rect const & area) const
{
	int const left = FitOrigin.X + (int)std::floor(area.X * Scale);
	int const top = FitOrigin.Y + (int)std::floor(area.Y * Scale);
	int const right = FitOrigin.X + (int)std::ceil((area.X + area.Width) * Scale);
	int const bottom = FitOrigin.Y + (int)std::ceil((area.Y + area.Height) * Scale);
	return(Rect(left, top, right - left, bottom - top));
}
