/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "hud.h"

#include "_mixfile.h"
#include "_rect.h"
#include "bsurface.h"
#include "csf.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "mixfile.h"
#include "houstype.h"
#include "sidebar.h"
#include "shapeset.h"
#include "tab.h"
#include "viewzoom.h"
#include "draw.h"

#include <algorithm>
#include <cmath>
#include <cstdio>


bool Modern_Interface(void)
{
	return(Options.ModernInterface && !Debug_Map);
}


Rect Tactical_View_Rect(void)
{
	Rect rect = VisibleRect;
	if (Modern_Interface()) {
		return(rect);
	}
	rect.X = (Options.IsSidebarOnRight || Debug_Map) ? 0 : SidebarClass::SIDE_WIDTH;
	rect.Y = SidebarClass::VIEW_TOP;
	rect.Width -= SidebarClass::SIDE_WIDTH;
	rect.Height -= SidebarClass::VIEW_TOP + SidebarClass::COMMAND_BAR_HEIGHT;
	return(rect);
}


int Sidebar_Screen_X(void)
{
	if (Modern_Interface()) {
		return(VisibleRect.Width - HUD_PANEL_WIDTH);
	}
	return(Options.IsSidebarOnRight ? ScreenTacticalRect.X + ScreenTacticalRect.Width : 0);
}


int Sidebar_Surface_Width(void)
{
	return(Modern_Interface() ? HUD_PANEL_WIDTH : SidebarClass::SIDE_WIDTH);
}


bool Point_Over_Map(Point2D const & point)
{
	if (!ScreenTacticalRect.Is_Point_Within(point)) {
		return(false);
	}
	if (!Modern_Interface()) {
		return(true);
	}
	if (Hud_Layout().Panel.Is_Point_Within(point)) {
		return(false);
	}
	return(!TabClass::Command_Bar_Frame().Is_Point_Within(point));
}


static int HudRows = 0;


bool Set_Hud_Rows(int rows)
{
	int const before = Hud_Layout().Rows;
	HudRows = rows;
	return(Hud_Layout().Rows != before);
}


HudLayout Hud_Layout(void)
{
	int const object_width = SidebarClass::StripClass::OBJECT_WIDTH;
	int const object_height = SidebarClass::StripClass::OBJECT_HEIGHT;
	int const inner = HUD_PANEL_WIDTH - HUD_MARGIN * 2;

	HudLayout layout {};
	int const buttons = 4;
	int const options_gap = 12;
	int const row = buttons * HUD_BUTTON_SIZE + (buttons - 1) * HUD_BUTTON_GAP + options_gap + HUD_BUTTON_SIZE;
	int const row_x = HUD_MARGIN + (inner - row) / 2;
	layout.Buttons = Rect(row_x, HUD_MARGIN, row - options_gap - HUD_BUTTON_SIZE, HUD_BUTTON_SIZE);
	layout.Options = Rect(row_x + row - HUD_BUTTON_SIZE, HUD_MARGIN, HUD_BUTTON_SIZE, HUD_BUTTON_SIZE);
	layout.Radar = Rect(HUD_MARGIN, layout.Buttons.Y + layout.Buttons.Height + 4, inner, 112);
	layout.Readouts = Rect(HUD_MARGIN, layout.Radar.Y + layout.Radar.Height + 4, inner, 20);
	int const half = (inner - 4) / 2;
	layout.Credits = Rect(layout.Readouts.X, layout.Readouts.Y, half, layout.Readouts.Height);
	layout.PowerReadout = Rect(layout.Readouts.X + layout.Readouts.Width - half, layout.Readouts.Y, half, layout.Readouts.Height);

	int const top = layout.Readouts.Y + layout.Readouts.Height + 6;
	int const arrows = 26;
	int const fit = (VisibleRect.Height - top - arrows - HUD_MARGIN) / object_height;
	int const most = SidebarClass::StripClass::MAX_SLOTS / HUD_GRID_COLUMNS;
	int const room = std::clamp(fit, 1, most);
	layout.Rows = (HudRows > 0) ? std::clamp(HudRows, std::min(int(HUD_MIN_ROWS), room), room) : room;

	layout.Power = Rect(HUD_MARGIN, top, 12, layout.Rows * object_height);
	layout.Tabs = Rect(layout.Power.X + layout.Power.Width + 2, top, HUD_TAB_WIDTH, layout.Rows * object_height);
	layout.Grid = Rect(layout.Tabs.X + layout.Tabs.Width + 2, top, HUD_GRID_COLUMNS * object_width, layout.Rows * object_height);
	layout.Arrows = Rect(layout.Grid.X, layout.Grid.Y + layout.Grid.Height + 2, layout.Grid.Width, arrows - 2);

	int const height = std::min(int(VisibleRect.Height), layout.Arrows.Y + layout.Arrows.Height + HUD_MARGIN);
	layout.Panel = Rect(VisibleRect.Width - HUD_PANEL_WIDTH, 0, HUD_PANEL_WIDTH, height);
	return(layout);
}


namespace {

struct HudColors {
	RGBClass Border;
	RGBClass Shade;
};

// The Allies' blue, the Soviets' red and Yuri's purple.
HudColors const & Side_Colors(void)
{
	static HudColors const _colors[] = {
		{RGBClass(72, 128, 200), RGBClass(28, 48, 80)},
		{RGBClass(196, 52, 40), RGBClass(80, 24, 20)},
		{RGBClass(150, 72, 196), RGBClass(60, 30, 84)},
	};
	int side = (PlayerPtr != NULL) ? int(PlayerPtr->Class->Side) : 0;
	if (side < 0 || side >= int(sizeof(_colors) / sizeof(_colors[0]))) {
		side = 0;
	}
	return(_colors[side]);
}

}


void Draw_Hud_Frame(Surface & surface, Rect const & rect)
{
	HudColors const & colors = Side_Colors();
	Draw_Hud_Fill(surface, rect);
	surface.Draw_Rect(rect, DSurface::Build_Hicolor_Pixel(colors.Border));
	Rect inner(rect.X + 1, rect.Y + 1, rect.Width - 2, rect.Height - 2);
	surface.Draw_Rect(inner, DSurface::Build_Hicolor_Pixel(colors.Shade));
}


void Draw_Hud_Well(Surface & surface, Rect const & rect)
{
	surface.Fill_Rect(rect, DSurface::Build_Hicolor_Pixel(RGBClass(8, 9, 11)));
	surface.Draw_Rect(rect, DSurface::Build_Hicolor_Pixel(RGBClass(48, 52, 60)));
}


void Draw_Hud_Highlight(Surface & surface, Rect const & rect)
{
	Draw_Hud_Well(surface, rect);
	surface.Draw_Rect(rect, DSurface::Build_Hicolor_Pixel(Side_Colors().Border));
}


int Hud_Side_Pixel(void)
{
	return(DSurface::Build_Hicolor_Pixel(Side_Colors().Border));
}


int Hud_Flash_Pixel(void)
{
	return(DSurface::Build_Hicolor_Pixel(RGBClass(235, 235, 235)));
}


namespace {

int Channel(int pixel, int right, int left)
{
	return(((pixel >> right) & (0xFF >> left)) << left);
}

// Is this hicolor pixel the near-black backing that Yuri's Revenge drew its sidebar buttons on?
bool Is_Backing(int pixel)
{
	int const most = 24;
	return(Channel(pixel, DSurface::RedRight, DSurface::RedLeft) <= most
		&& Channel(pixel, DSurface::GreenRight, DSurface::GreenLeft) <= most
		&& Channel(pixel, DSurface::BlueRight, DSurface::BlueLeft) <= most);
}

}


void Draw_Hud_Cutout(Surface & surface, ConvertClass & drawer, ShapeSet const * shape, int frame, Point2D const & at, int width, int height, int outline)
{
	if (shape == NULL || surface.Bytes_Per_Pixel() != 2 || width <= 0 || height <= 0) {
		return;
	}

	static BSurface * _scratch = NULL;
	int const w = width + 2;
	int const h = height + 2;
	if (_scratch == NULL || _scratch->Get_Width() < w || _scratch->Get_Height() < h) {
		delete _scratch;
		_scratch = new BSurface(std::max(w, 64), std::max(h, 64), 2);
	}

	int const key = DSurface::Build_Hicolor_Pixel(255, 0, 255);
	Rect const area(0, 0, w, h);
	_scratch->Fill_Rect(area, key);
	Draw_Shape(*_scratch, drawer, shape, frame, Point2D(1, 1), area, SHAPE_NORMAL);

	auto solid = [&](unsigned short const * row, int x) {
		int const pixel = row[x];
		return(pixel != key && !Is_Backing(pixel));
	};

	Rect const bounds = surface.Get_Rect();
	unsigned short const * source = static_cast<unsigned short const *>(_scratch->Lock());
	unsigned short * dest = static_cast<unsigned short *>(surface.Lock());
	if (source != NULL && dest != NULL) {
		int const sstride = _scratch->Stride() / 2;
		int const dstride = surface.Stride() / 2;
		for (int y = 0; y < h; y++) {
			int const dy = at.Y - 1 + y;
			if (dy < bounds.Y || dy >= bounds.Y + bounds.Height) {
				continue;
			}
			unsigned short const * row = source + y * sstride;
			for (int x = 0; x < w; x++) {
				int const dx = at.X - 1 + x;
				if (dx < bounds.X || dx >= bounds.X + bounds.Width) {
					continue;
				}
				if (solid(row, x)) {
					dest[dy * dstride + dx] = row[x];
				} else if (outline != -1) {
					bool const edge = (x > 0 && solid(row, x - 1)) || (x + 1 < w && solid(row, x + 1))
						|| (y > 0 && solid(row - sstride, x)) || (y + 1 < h && solid(row + sstride, x));
					if (edge) {
						dest[dy * dstride + dx] = static_cast<unsigned short>(outline);
					}
				}
			}
		}
	}
	if (source != NULL) _scratch->Unlock();
	if (dest != NULL) surface.Unlock();
}


void Draw_Hud_Ring(Surface & surface, Rect const & tile, int radius, int pixel, int thickness)
{
	int const r = radius + thickness;
	int const w = tile.Width + thickness * 2;
	int const h = tile.Height + thickness * 2;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			double const dx = x - std::clamp(x, r, w - 1 - r);
			double const dy = y - std::clamp(y, r, h - 1 - r);
			double const distance = std::sqrt(dx * dx + dy * dy);
			if (distance >= radius + 0.5 && distance < r + 0.5) {
				surface.Put_Pixel(Point2D(tile.X - thickness + x, tile.Y - thickness + y), pixel);
			}
		}
	}
}


void Draw_Hud_Glyph(Surface & surface, Rect const & well, HudGlyphType glyph, RGBClass const & color)
{
	static char const * const _glyphs[][10] = {
		{
			"...##...",
			".######.",
			"##.##.##",
			"##.##...",
			".#####..",
			"...####.",
			"...##.##",
			"##.##.##",
			".######.",
			"...##..."
		},
		{
			"....####",
			"...####.",
			"..####..",
			".####...",
			"########",
			"...####.",
			"..####..",
			".####...",
			".##.....",
			"#......."
		}
	};

	char const * const * rows = _glyphs[glyph];
	int const left = well.X + 5;
	int const top = well.Y + (well.Height - 10) / 2;
	int const shadow = DSurface::Build_Hicolor_Pixel(RGBClass(0, 0, 0));
	int const pixel = DSurface::Build_Hicolor_Pixel(color);
	for (int pass = 0; pass < 2; pass++) {
		for (int y = 0; y < 10; y++) {
			for (int x = 0; x < 8; x++) {
				if (rows[y][x] == '#') {
					surface.Put_Pixel(Point2D(left + x + 1 - pass, top + y + 1 - pass), pass == 0 ? shadow : pixel);
				}
			}
		}
	}
}


int Hud_Active_Pixel(void)
{
	RGBClass const & border = Side_Colors().Border;
	return(DSurface::Build_Hicolor_Pixel(RGBClass((border.Get_Red() + 255) / 2, (border.Get_Green() + 255) / 2, (border.Get_Blue() + 255) / 2)));
}


ShapeSet const * Hud_Icon(char const * name)
{
	static char const _letters[] = {'A', 'S', 'Y'};
	int side = (PlayerPtr != NULL) ? int(PlayerPtr->Class->Side) : 0;
	if (side < 0 || side >= int(sizeof(_letters))) {
		side = 0;
	}
	char file[16];
	std::snprintf(file, sizeof(file), "HUD%c%s.SHP", _letters[side], name);
	return(static_cast<ShapeSet const *>(MFCD::Retrieve(file)));
}


void Draw_Hud_Fill(Surface & surface, Rect const & rect)
{
	surface.Fill_Rect(rect, DSurface::Build_Hicolor_Pixel(RGBClass(20, 22, 26)));
}


void Draw_Hud_Panel(Surface & surface)
{
	HudLayout const layout = Hud_Layout();
	Draw_Hud_Frame(surface, Rect(0, 0, layout.Panel.Width, layout.Panel.Height));
	Draw_Hud_Well(surface, layout.Credits);
	Draw_Hud_Well(surface, layout.PowerReadout);
	Draw_Hud_Well(surface, layout.Radar);
	Draw_Hud_Well(surface, layout.Power);
}


std::string Panel_Text(char const * label, char const * fallback)
{
	std::string const text = StringTable.Find_UTF8(label);
	return(text.empty() ? std::string(fallback) : text);
}
