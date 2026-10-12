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

#include "_rect.h"
#include "csf.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "sidebar.h"
#include "tab.h"
#include "viewzoom.h"

#include <algorithm>


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
	layout.Status = Rect(HUD_MARGIN, HUD_MARGIN, inner, 20);
	layout.Options = Rect(layout.Status.X, layout.Status.Y, 64, layout.Status.Height);
	layout.Diplomacy = Rect(layout.Status.X + layout.Status.Width - 64, layout.Status.Y, 64, layout.Status.Height);
	layout.Credits = Rect(layout.Options.X + layout.Options.Width + 2, layout.Status.Y, layout.Diplomacy.X - 2 - (layout.Options.X + layout.Options.Width + 2), layout.Status.Height);
	layout.Radar = Rect(HUD_MARGIN, layout.Status.Y + layout.Status.Height + 4, inner, 112);
	layout.Icons = Rect(HUD_MARGIN, layout.Radar.Y + layout.Radar.Height + 4, inner, 32);
	layout.PowerReadout = Rect(layout.Icons.X + layout.Icons.Width - 72, layout.Icons.Y + 6, 72, layout.Icons.Height - 12);

	int const top = layout.Icons.Y + layout.Icons.Height + 6;
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


void Draw_Hud_Flash(Surface & surface, Rect const & rect)
{
	unsigned const light = DSurface::Build_Hicolor_Pixel(RGBClass(230, 230, 230));
	surface.Draw_Rect(rect, light);
	surface.Draw_Rect(Rect(rect.X + 1, rect.Y + 1, rect.Width - 2, rect.Height - 2), light);
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
