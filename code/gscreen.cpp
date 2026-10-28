/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

/* $Header: /CounterStrike/GSCREEN.CPP 1     3/03/97 10:24a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : GSCREEN.CPP                                                  *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : 12/15/94                                                     *
 *                                                                                             *
 *                  Last Update : January 19, 1995 [JLB]                                       *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   GScreenClass::Add_A_Button -- Add a gadget to the game input system.                      *
 *   GScreenClass::Blit_Display -- Redraw the display from the hidpage to the seenpage.        *
 *   GScreenClass::Flag_To_Redraw -- Flags the display to be redrawn.                          *
 *   GScreenClass::GScreenClass -- Default constructor for GScreenClass.                       *
 *   GScreenClass::Init -- Init's the entire display hierarchy by calling all Init routines.   *
 *   GScreenClass::Init_Clear -- Sets the map to a known state.                                *
 *   GScreenClass::Init_IO -- Initializes the Button list ('Buttons').                         *
 *   GScreenClass::Init_Theater -- Performs theater-specific initializations.                  *
 *   GScreenClass::Input -- Fetches input and processes gadgets.                               *
 *   GScreenClass::One_Time -- Handles one time class setups.                                  *
 *   GScreenClass::Remove_A_Button -- Removes a gadget from the game input system.             *
 *   GScreenClass::Render -- General drawing dispatcher an display update function.            *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"
#include "perfstat.h"

#include "gscreen.h"

#include "gpudraw.h"
#include "gpumap.h"

#include "_bench.h"
#include "_convert.h"
#include "convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "_tooltip.h"
#include "_xmouse.h"
#include "bench.h"
#include "cctooltip.h"
#include "dialog.h"
#include "gadget.h"
#include "globals.h"
#include "hud.h"
#include "goptions.h"
#include "keyboard.h"
#include "logic.h"
#include "savestream.h"
#include "scheme.h"
#include "session.h"
#include "surface.h"
#include "tactical.h"
#include "lstorm.h"
#include "replay.h"
#include "scenario.h"
#include "theater.h"
#include "emissive.h"
#include "emissive_surface.h"
#include "shadowplane.h"
#include "dsurface.h"
#include "video.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>


// Composite surface pixels written since an event, from the surface's write watch: for each row, a mask
// of the column blocks written. Known is false when any pixel may have been written.
struct CompositeRows {
	std::vector<std::uint32_t> Rows;
	int BlockWidth = 1;
	bool Known = false;

	static int Block_Width(int width) { return(std::max(64, (width + 31) / 32)); }

	void Restart(int height, int width)
	{
		Rows.assign((size_t)std::max(height, 0), 0);
		BlockWidth = Block_Width(width);
		Known = true;
	}

	// Passes each written part of area to each, as rectangles that join the rows written in the same blocks.
	template<class F>
	void For_Each_Rect(Rect const & area, F each) const
	{
		int const left = std::max(area.X, 0);
		int const right = area.X + area.Width;
		if (right <= left || left >= BlockWidth * 32) {
			return;
		}
		int const first = left / BlockWidth;
		int const last = std::min((right - 1) / BlockWidth, 31);
		std::uint32_t const area_mask = (last >= 31 ? ~std::uint32_t(0) : (std::uint32_t(1) << (last + 1)) - 1) & ~((std::uint32_t(1) << first) - 1);
		int const bottom = std::min(area.Y + area.Height, (int)Rows.size());
		int y = std::max(area.Y, 0);
		while (y < bottom) {
			std::uint32_t const mask = Rows[y] & area_mask;
			if (mask == 0) {
				y++;
				continue;
			}
			int const top = y;
			while (y < bottom && (Rows[y] & area_mask) == mask) {
				y++;
			}
			for (int block = first; block <= last; block++) {
				if (!(mask & (std::uint32_t(1) << block))) {
					continue;
				}
				int end = block;
				while (end + 1 <= last && (mask & (std::uint32_t(1) << (end + 1)))) {
					end++;
				}
				int const x0 = std::max(left, block * BlockWidth);
				int const x1 = std::min(right, (end + 1) * BlockWidth);
				if (x1 > x0) {
					each(Rect(x0, top, x1 - x0, y - top));
				}
				block = end;
			}
		}
	}
};

// Rows written since the map layer key was last filled in, and since the composite was last copied to
// the visible surface.
static CompositeRows _SinceKeyFill;
static CompositeRows _SinceCopy;
static Rect _KeyFillRect;
static Rect _CopySource;
static Rect _CopyDest;
static unsigned _VisibleChangesAfterCopy;
static int _CopiesUntilFull;
static std::vector<unsigned char> _CopiedRows;


/// <summary>
/// Adds the composite rows written since the last call to each given set. Returns false, and marks the
/// sets unknown, when the composite cannot list its written rows.
/// </summary>
static bool Take_Composite_Rows(CompositeRows * first, CompositeRows * second)
{
	static std::vector<std::pair<std::size_t, std::size_t>> runs;
	DSurface * const composite = (DSurface *)CompositeSurface;
	int const height = composite->Get_Height();
	int const width = composite->Get_Width();
	int const pitch = composite->Stride();
	int const bpp = composite->Bytes_Per_Pixel();
	bool const listed = pitch > 0 && bpp > 0 && composite->Written_Runs(runs, true);
	// Scrolling swaps the composite with the tile layer, whose watch says nothing about what the composite last held.
	static unsigned last_composite_serial = 0;
	unsigned const composite_serial = composite->Get_Serial();
	if (composite_serial != last_composite_serial) {
		last_composite_serial = composite_serial;
		_SinceKeyFill.Known = false;
		_SinceCopy.Known = false;
	}
	for (CompositeRows * rows : {first, second}) {
		if (rows == NULL) {
			continue;
		}
		if (!listed || (int)rows->Rows.size() != height || rows->BlockWidth != CompositeRows::Block_Width(width)) {
			rows->Known = false;
			continue;
		}
		if (!rows->Known) {
			continue;
		}
		for (auto const & run : runs) {
			int const top = std::min((int)(run.first / pitch), height);
			int const bottom = std::min((int)((run.second + pitch - 1) / pitch), height);
			for (int y = top; y < bottom; y++) {
				std::size_t const row_start = (std::size_t)y * pitch;
				std::size_t const begin = std::max(run.first, row_start) - row_start;
				std::size_t const end = std::min(run.second, row_start + pitch) - row_start;
				int const x0 = (int)(begin / bpp);
				int const x1 = std::min(width, (int)((end + bpp - 1) / bpp));
				if (x1 <= x0) {
					continue;
				}
				int const b0 = std::min(x0 / rows->BlockWidth, 31);
				int const b1 = std::min((x1 - 1) / rows->BlockWidth, 31);
				rows->Rows[y] |= (b1 >= 31 ? ~std::uint32_t(0) : (std::uint32_t(1) << (b1 + 1)) - 1) & ~((std::uint32_t(1) << b0) - 1);
			}
		}
	}
	return(listed);
}


/// <summary>
/// Fills dest on the composite with the map layer key. Rows nothing has written since the last fill of
/// the same rectangle still hold the key and are left alone; the emissive values under all of dest are cleared.
/// </summary>
static void Fill_Map_Layer_Key(Rect const & dest)
{
	bool const listed = Take_Composite_Rows(&_SinceKeyFill, &_SinceCopy);
	if (!_SinceKeyFill.Known || dest != _KeyFillRect) {
		CompositeSurface->Fill_Rect(dest, VIDEO_LAYER_KEY);
	} else {
		_SinceKeyFill.For_Each_Rect(dest, [](Rect const & rect) {
			CompositeSurface->Fill_Rect(rect, VIDEO_LAYER_KEY);
		});
		Emissive_After_Fill(*CompositeSurface, Intersect(dest, CompositeSurface->Get_Rect()));
	}
	_KeyFillRect = dest;
	// The rows just filled go to the copy only; counting them for the next fill would refill them every frame.
	Take_Composite_Rows(NULL, &_SinceCopy);
	if (listed) {
		_SinceKeyFill.Restart(CompositeSurface->Get_Height(), CompositeSurface->Get_Width());
	}
}
#include "shellmap.h"
#include "video.h"
#include "viewzoom.h"

#include "bench.hh"

#include <algorithm>
#include <cmath>

void Multiplayer_Debug_Print(void);

GadgetClass * GScreenClass::Buttons = NULL;


/***********************************************************************************************
 * GScreenClass::GScreenClass -- Default constructor for GScreenClass.                         *
 *                                                                                             *
 *    This constructor merely sets the display system, so that it will redraw the first time   *
 *    the render function is called.                                                           *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/15/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
GScreenClass::GScreenClass(void) :
	ScreenX(0),
	ScreenY(0),
	DrawFlags(GS_REDRAW_ALL)
{
}


/// <summary>
/// Lists the members the game screen holds.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void GScreenClass::Serialize(SaveStreamClass & stream)
{
	// Buttons -- the input button list, rebuilt from scratch by Init_IO.
	stream.Serialize(ScreenX);
	stream.Serialize(ScreenY);
	// DrawFlags -- a redraw request covers a single frame, and the load asks for a full one.
}


/***********************************************************************************************
 * GScreenClass::One_Time -- Handles one time class setups.                                    *
 *                                                                                             *
 * This routine (and all those that overload it) must perform truly one-time initialization.   *
 * Such init's would normally be done in the constructor, but other aspects of the game may    *
 * not have been initialized at the time the constructors are called (such as the file system, *
 * the display, or other WWLIB subsystems), so many initializations should be deferred to the  *
 * One_Time init's.                                                                            *
 *                                                                                             *
 * Any variables set in this routine should be declared as static, so they won't be modified   *
 * by the load/save process.  Non-static variables will be over-written by a loaded game.      *
 *                                                                                             *
 * This function allocates the shadow buffer that is used for quick screen updates. If         *
 * there were any data files to load, they would be loaded at this time as well.               *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Call this routine only ONCE at the beginning of the game.                       *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/15/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void GScreenClass::One_Time(void)
{
	Buttons = 0;
}


/***********************************************************************************************
 * GScreenClass::Init -- Init's the entire display hierarchy by calling all Init routines.     *
 *                                                                                             *
 * This routine shouldn't be overloaded.  It's the main map initialization routine, and will   *
 * perform a complete map initialization, from mixfiles to clearing the buffers.  Calling this *
 * routine results in calling every initialization routine in the entire map hierarchy.        *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      theater      theater to initialize to                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/28/1994 BR : Created.                                                                  *
 *=============================================================================================*/
void GScreenClass::Init(TheaterType theater)
{
	Init_Clear();
	Init_IO();
}


/***********************************************************************************************
 * GScreenClass::Init_Clear -- Sets the map to a known state.                                  *
 *                                                                                             *
 * This routine (and those that overload it) clears any buffers and variables to a known       *
 * state.  It assumes that all buffers are allocated & valid.  The map should be displayable   *
 * after calling this function, and should draw basically an empty display.                    *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/28/1994 BR : Created.                                                                  *
 *=============================================================================================*/
void GScreenClass::Init_Clear(void)
{
	DrawFlags = GS_REDRAW_ALL;
}


/***********************************************************************************************
 * GScreenClass::Init_IO -- Initializes the Button list ('Buttons').                           *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/28/1994 BR : Created.                                                                  *
 *=============================================================================================*/
void GScreenClass::Init_IO(void)
{
	/*
	**	Reset the button list.  This means that any other elements of the map that need
	**	buttons must attach them after this routine is called!
	*/
	Buttons = 0;
}


/***********************************************************************************************
 * GScreenClass::Flag_To_Redraw -- Flags the display to be redrawn.                            *
 *                                                                                             *
 *    This function is used to flag the display system whether any rendering is needed. The    *
 *    parameter tells the system either to redraw EVERYTHING, or just that something somewhere *
 *    has changed and the individual Draw_It functions must be called. When a sub system       *
 *    determines that it needs to render something local to itself, it would call this routine *
 *    with a false parameter. If the entire screen gets trashed or needs to be rebuilt, then   *
 *    this routine will be called with a true parameter.                                       *
 *                                                                                             *
 * INPUT:   complete -- bool; Should the ENTIRE screen be redrawn?                             *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   This doesn't actually draw the screen, it merely sets flags so that when the    *
 *             Render() function is called, the appropriate drawing steps will be performed.   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/15/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void GScreenClass::Flag_To_Redraw(GScreenRedrawFlags flags)
{
	if (TacticalMap != NULL) {
		TacticalMap->IsToRedraw = true;
	}

	if (flags != GS_REDRAW_DIRTY) {
		if (DrawFlags != GS_REDRAW_ALL) {
			DrawFlags = flags;
		}
		Map.Increment_Redraw_Counter();
	}
}


/***********************************************************************************************
 * GScreenClass::Input -- Fetches input and processes gadgets.                                 *
 *                                                                                             *
 *    This routine will fetch the keyboard/mouse input and dispatch this through the gadget    *
 *    system.                                                                                  *
 *                                                                                             *
 * INPUT:   key      -- Reference to the key code (for future examination).                    *
 *                                                                                             *
 *          x,y      -- Reference to mouse coordinates (for future examination).               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void GScreenClass::Input(KeyNumType & key, int & x, int & y)
{
	x = Get_Mouse_X();
	y = Get_Mouse_Y();

	if (Buttons != NULL) {

		/*
		**	If any buttons need redrawing, they will do so in the Input routine, and
		**	they should draw themselves to the HidPage.  So, flag ourselves for a Blit
		**	to show the newly drawn buttons.
		*/
		if (Buttons->Is_List_To_Redraw()) {
			Flag_To_Redraw();
		}

		Surface * oldpage = LogicalSurface;
		LogicalSurface = HiddenSurface;

		key = Buttons->Input();

		LogicalSurface = oldpage;

	} else {
		key = Keyboard->Check();

		if (key != 0) {
			key = Keyboard->Get();
		}
	}

	AI(key, Point2D(x, y));
}


/***********************************************************************************************
 * GScreenClass::Add_A_Button -- Add a gadget to the game input system.                        *
 *                                                                                             *
 *    This will add a gadget to the game input system. The gadget will be processed in         *
 *    subsequent calls to the GScreenClass::Input() function.                                  *
 *                                                                                             *
 * INPUT:   gadget   -- Reference to the gadget that will be added to the input system.        *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void GScreenClass::Add_A_Button(GadgetClass & gadget)
{
	/*
	**	If this gadget is already in the list, remove it before adding it in:
	**	- If 1st gadget in list, use Remove_A_Button to remove it, to reset the
	**	value of 'Buttons' appropriately
	**	- Otherwise, just call the Remove function for that gadget to remove it
	**	from any list it may be in
	*/
	if (Buttons == &gadget) {
		Remove_A_Button(gadget);
	} else {
		gadget.Remove();
	}

	/*
	**	Now add the gadget to our list:
	**	- If there are not buttons, start the list with this one
	**	- Otherwise, add it to the tail of the existing list
	*/
	if (Buttons) {
		gadget.Add_Tail(*Buttons);
	} else {
		Buttons = &gadget;
	}
}


/***********************************************************************************************
 * GScreenClass::Remove_A_Button -- Removes a gadget from the game input system.               *
 *                                                                                             *
 * INPUT:   gadget   -- Reference to the gadget that will be removed from the input system.    *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   'gadget' MUST be already a part of 'Buttons', or the new value of 'Buttons'     *
 *               will be invalid!                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void GScreenClass::Remove_A_Button(GadgetClass & gadget)
{
	Buttons = (GadgetClass *)gadget.Remove();
}


/// <summary>
/// The part of the zoomed map surface that shows a part of the screen view, mapped as Shown_Map_Rect
/// maps the whole view onto the screen. The presenter draws the map layer the same way.
/// </summary>
/// <param name="screen_part">A rectangle inside ScreenTacticalRect, in screen pixels.</param>
static Rect Shown_Map_Part(Rect const & screen_part)
{
	Rect const shown = Shown_Map_Rect();
	double const scale_x = double(shown.Width) / ScreenTacticalRect.Width;
	double const scale_y = double(shown.Height) / ScreenTacticalRect.Height;
	int const left = shown.X + int(std::floor((screen_part.X - ScreenTacticalRect.X) * scale_x));
	int const top = shown.Y + int(std::floor((screen_part.Y - ScreenTacticalRect.Y) * scale_y));
	int const right = shown.X + int(std::ceil((screen_part.X + screen_part.Width - ScreenTacticalRect.X) * scale_x));
	int const bottom = shown.Y + int(std::ceil((screen_part.Y + screen_part.Height - ScreenTacticalRect.Y) * scale_y));
	return(Rect(left, top, right - left, bottom - top));
}


/// <summary>
/// Draws the frame-time graph: one column for each gap between presents, newest at the right.
/// A column is 1 pixel per millisecond, and a gap of 40 ms or more fills the graph's full height.
/// Gaps that have not yet been recorded leave their columns black.
/// </summary>
/// <param name="surface">The surface the graph is drawn on.</param>
/// <param name="origin">The top left corner of the graph.</param>
static void Draw_Frame_Graph(Surface & surface, Point2D const & origin)
{
	constexpr int height = 40;
	int const width = VIDEO_FRAME_TIME_HISTORY;

	surface.Fill_Rect(Rect(origin.X, origin.Y, width, height), NormalDrawer->Convert_Pixel(TBLACK));

	int const count = Video_Frame_Time_Count();
	for (int index = 0; index < count; index++) {
		int const bar = std::clamp((int)Video_Frame_Time(index), 1, height);
		int const x = origin.X + width - count + index;
		surface.Fill_Rect(Rect(x, origin.Y + height - bar, 1, bar), NormalDrawer->Convert_Pixel(WHITE));
	}

	Fancy_Text_Print("40 ms", surface, surface.Get_Rect(), Point2D(origin.X + width + 4, origin.Y), Fetch_Scheme_By_Name("Grey"), TBLACK, (TextPrintType)(TPF_EFNT | TPF_NOSHADOW));
}


/// <summary>
/// Draws the ShowFPS counts at the top left of the game view.
/// The first line is the rendered rate: the presents per second that reached the display in
/// the last complete second. The second line is the game rate, the game frames run in the last
/// 960 ms window (LastFramesPerSecond), which also gates the detail effects.
/// </summary>
/// <param name="surface">The surface the counts are drawn on.</param>
static void Draw_Frame_Rates(Surface & surface)
{
	char buffer[32];

	snprintf(buffer, sizeof(buffer), "Render %u FPS", Video_Presents_Per_Second());
	Fancy_Text_Print(buffer, surface, surface.Get_Rect(), Point2D(4, 4),Fetch_Scheme_By_Name("Grey"), TBLACK, (TextPrintType)(TPF_EFNT | TPF_NOSHADOW));

	snprintf(buffer, sizeof(buffer), "Game %u ticks/s", LastFramesPerSecond);
	Fancy_Text_Print(buffer, surface, surface.Get_Rect(), Point2D(4, 14), Fetch_Scheme_By_Name("Grey"), TBLACK, (TextPrintType)(TPF_EFNT | TPF_NOSHADOW));

	if (Options.ShowFrameGraph) {
		Draw_Frame_Graph(surface, Point2D(4, 26));
	}
}


/// <summary>
/// Draws the replay label at the top centre of the game view, so that a replay is never mistaken for a live game.
/// </summary>
static void Draw_Replay_Label(Surface & surface)
{
	std::string const label = Replay_Label_Text();
	Fancy_Text_Print("%s", surface, surface.Get_Rect(), Point2D(ScreenTacticalRect.X + ScreenTacticalRect.Width / 2, ScreenTacticalRect.Y + 6),
		Fetch_Scheme_By_Name("Yellow"), TBLACK, (TextPrintType)(TPF_EFNT | TPF_NOSHADOW | TPF_CENTER), label.c_str());
}


/***********************************************************************************************
 * GScreenClass::Render -- General drawing dispatcher an display update function.              *
 *                                                                                             *
 *    This routine should be called in the main game loop (once every game frame). It will     *
 *    call the Draw_It() function if necessary. All rendering is performed to the LogicPage    *
 *    which is set to the HIDPAGE. After rendering has been performed, the HIDPAGE is          *
 *    copied to the visible page.                                                              *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   This actually updates the graphic display. As a result it can take quite a      *
 *             while to perform.                                                               *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/15/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void GScreenClass::Render(void)
{
	BStart(BENCH_GSCREEN_RENDER);
	PerfDrawSample draw_sample;
	PerfScope draw_scope(PERF_DRAW_TOTAL);

	Update_Display_Zoom();
	Set_View_Zoom_Drawing(true);

	Surface * oldpage = LogicalSurface;
	LogicalSurface = CompositeSurface;

	bool redraw = DrawFlags != GS_REDRAW_DIRTY;
	bool complete = DrawFlags == GS_REDRAW_ALL;

	// Masks follow the composite: planes are bound here, and a full redraw starts them clear.
	Emissive_Sync(CompositeSurface, TileSurface, MapCompositeSurface, MapTileSurface, VisibleSurface);
	if (complete && Emissive_Is_Enabled() && CompositeSurface != NULL) {
		Emissive_After_Fill(*CompositeSurface, CompositeSurface->Get_Rect());
	}

	// While zoomed, the map's passes draw into the map's own surfaces; the panning pass may swap them.
	bool const zoomed = (MapCompositeSurface != NULL && MapTileSurface != NULL);
	Surface * screen_composite = CompositeSurface;
	Surface * screen_tile = TileSurface;
	auto use_map_surfaces = [&](bool map) {
		if (!zoomed) {
			return;
		}
		if (map) {
			CompositeSurface = MapCompositeSurface;
			TileSurface = MapTileSurface;
		} else {
			MapCompositeSurface = CompositeSurface;
			MapTileSurface = TileSurface;
			CompositeSurface = screen_composite;
			TileSurface = screen_tile;
		}
		LogicalSurface = CompositeSurface;
	};

	// The chronoshift effect swirls a snapshot of the finished map view, which only the software pass holds.
	bool const gpu_render = Options.GpuRender && Modern_Interface() && Video_Map_Layer_Supported() && !Chrono_Screen_Effect_Is_Running();
	bool const gpu_planes = Options.GpuRender && Modern_Interface() && Video_Map_Layer_Supported() && GpuMap_Supported();
	if (ShadowPlane_Set_Enabled(gpu_planes && Options.SoftShadows, gpu_planes && Options.WaterRipples)) {
		redraw = true;
		TacticalMap->IsToRedraw = true;
	}
	ShadowPlane_Sync(CompositeSurface, TileSurface, MapCompositeSurface, MapTileSurface);
	GpuDraw_Plan(gpu_render);
	use_map_surfaces(true);
	TacticalMap->Render(*CompositeSurface, redraw, DRAW_PASS_PAN);
	TacticalMap->Render(*CompositeSurface, redraw, DRAW_PASS_BACKGROUND);
	use_map_surfaces(false);
	// The menu battle is drawn bare: the menus stand where the sidebar and messages would be.
	bool const hud = !Shellmap_In_Scope();
	if (hud && complete && Modern_Interface() && SidebarSurface != NULL) {
		Draw_Hud_Panel(*SidebarSurface);
	}
	if (hud) {
		PerfScope hud_scope(PERF_DRAW_HUD);
		Draw_It(complete);
	}
	use_map_surfaces(true);
	Video_Draw_Probe_Target(CompositeSurface);
	// While zoomed, the GPU draws only the part of the map that is shown, and the presenter scales it.
	Rect const gpu_area = zoomed ? Shown_Map_Rect() : TacticalRect;
	GpuMap_Set_Soft_Shadows(Options.SoftShadows ? Options.SoftShadowRadius : 0);
	GpuMap_Set_Bloom(Options.Bloom ? Options.BloomStrength : 0);
	GpuMap_Set_Water(Options.WaterRipples ? Options.WaterRippleStrength : 0);
	GpuMap_Set_Clouds(Options.CloudShadows ? Options.CloudShadowStrength : 0);
	GpuWeather weather = GPU_WEATHER_NONE;
	if (Options.Weather == WEATHER_RAIN || (Options.Weather == WEATHER_MAP && LightningStormClass::Is_Active())) {
		weather = GPU_WEATHER_RAIN;
	} else if (Options.Weather == WEATHER_SNOW || (Options.Weather == WEATHER_MAP && Scen != NULL && TheaterClass::As_Reference(Scen->Theater).IsArctic)) {
		weather = GPU_WEATHER_SNOW;
	}
	GpuMap_Set_Weather(weather, Options.WeatherStrength);
	GpuDraw_Begin(*CompositeSurface, gpu_area, gpu_render);
	TacticalMap->Render(*CompositeSurface, redraw, DRAW_PASS_FOREGROUND);
	bool gpu_drawn = false;
	{
		PerfScope gpumap_scope(PERF_DRAW_GPUMAP);
		gpu_drawn = GpuDraw_End();
	}
	Video_Draw_Probe_Target(NULL);
	Video_Note_Draw_Digest(*CompositeSurface, TacticalRect);
	use_map_surfaces(false);
	if (gpu_drawn) {
		Rect const dest = zoomed ? ScreenTacticalRect : TacticalRect;
		Fill_Map_Layer_Key(dest);
		Video_Set_Gpu_Map_Layer(gpu_area, dest);
		// The new map pass is shown at the next present even when nothing in the frame changed.
		Video_Mark_Dirty();
	} else if (zoomed && Video_Map_Layer_Supported()) {
		// The presenter draws the map under the frame at the window's own resolution. The translucent panel
		// blends with the map, so the frame under the panel holds a copy of the map instead of the key.
		CompositeSurface->Fill_Rect(ScreenTacticalRect, VIDEO_LAYER_KEY);
		if (hud && Modern_Interface()) {
			Rect const panel = Hud_Layout().Panel;
			CompositeSurface->Blit_From(panel, *MapCompositeSurface, Shown_Map_Part(panel));
		}
		Video_Set_Map_Layer(MapCompositeSurface, Shown_Map_Rect(), ScreenTacticalRect);
	} else {
		if (zoomed) {
			CompositeSurface->Blit_From(ScreenTacticalRect, *MapCompositeSurface, Shown_Map_Rect());
		}
		Video_Set_Map_Layer(NULL, Rect(), Rect());
	}

	{
		PerfScope ui_scope(PERF_DRAW_UI);
		if (hud && Modern_Interface()) {
			Draw_Panel_Backdrops(*CompositeSurface);
		}
		if (hud && Buttons) Buttons->Draw_All(false);
		if (hud && Modern_Interface()) {
			Draw_Panels(*CompositeSurface);
		}

#ifdef _DEBUG
		/*
		**	Draw the Editor's buttons
		*/
		if (Debug_Map) {
			if (Buttons) {
				Buttons->Draw_All();
			}
		}
#endif
		/*
		**	Draw the multiplayer message system to the Hidpage at this point.
		**	This way, they'll Blit along with the rest of the map.
		*/
		if (hud) {
			Session.Messages.Draw();
			TacticalMap->Draw_Super_Timers();
		}
		if (Session.DrawMPDebugStats) {
			Multiplayer_Debug_Print();
		}

		if (Options.ShowFPS) {
			Draw_Frame_Rates(*LogicalSurface);
		}

		if (hud && Replay_Is_Playing()) {
			Draw_Replay_Label(*LogicalSurface);
		}

		if (hud && ToolTips != NULL) {
			ToolTips->Draw_Current();
		}
	}

	Blit_Display();
	DrawFlags = GS_REDRAW_DIRTY;

	BEnd(BENCH_GSCREEN_RENDER);
	LogicalSurface = oldpage;
	Set_View_Zoom_Drawing(false);
}


/***********************************************************************************************
 * GScreenClass::Blit_Display -- Redraw the display from the hidpage to the seenpage.          *
 *                                                                                             *
 *    This routine is used to copy the correct display from the HIDPAGE                        *
 *    to the SEENPAGE.                                                                         *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   02/14/1994 JLB : Created.                                                                 *
 *   05/01/1994 JLB : Converted to member function.                                            *
 *=============================================================================================*/
void GScreenClass::Blit_Display(void)
{
	BStart(BENCH_BLIT_DISPLAY);
	Update_Visible_Surface(CompositeSurface, NULL);
	BEnd(BENCH_BLIT_DISPLAY);
}


/// <summary>
/// Presents a rendered surface onto the visible surface.
/// This is the low level routine that gets a finished frame in front of the player. The
/// destination is the visible surface, adjusted for the screen shake and for a sidebar
/// sitting on the left, and the source is narrowed when the tactical map is zoomed. Any
/// strip that the shake has left uncovered is filled with black.
/// </summary>
/// <param name="surface">The surface holding the frame to present.</param>
/// <param name="rect">The portion of the surface to present, or NULL for all of it.</param>
void Update_Visible_Surface(Surface *surface, Rect *rect)
{
	Rect fill_rect;

	if (rect == NULL) {
		fill_rect = surface->Get_Rect();
		rect = &fill_rect;
	}

	Rect dest_rect(0, 0, surface->Get_Width(), surface->Get_Height());

	// With screen shake off, the picture is drawn unshaken; the map keeps the offsets for the simulation.
	int const shake_x = Options.ScreenShake ? Map.ScreenX : 0;
	int const shake_y = Options.ScreenShake ? Map.ScreenY : 0;

	/// Screen shake handling
	if (shake_x == 0 && shake_y == 0) {
		// Do nothing
	} else {
		if (shake_x > 0) {
			dest_rect.X += shake_x;
			dest_rect.Width -= shake_x;
		} else if (shake_x < 0) {
			dest_rect.Width += shake_x;
		}
		if (shake_y > 0) {
			dest_rect.Y += shake_y;
			dest_rect.Height -= shake_y;
		} else if (shake_y < 0) {
			dest_rect.Height += shake_y;
		}
	}

	/// Adjust for sidebar position
	if (!Options.IsSidebarOnRight && !Debug_Map) {
		dest_rect.X += std::max(std::min(SidebarSurface->Get_Width(), VisibleRect.Width - dest_rect.Width), 0);
	}

	/*
	 * Copy input rect to source rect
	 */
	Rect src_rect = *rect;

	/// Apply zoom factor if tactical map is zoomed
	if (TacticalMap && (TacticalMap->ZoomFactor != 1.0)) {

		/*
		 * Compute zoomed source rect centered
		 */
		int zoom_surface_width = surface->Get_Width();
		int zoom_surface_height = surface->Get_Height();

		Rect tmp;
		double zoomed_width = (double)zoom_surface_width / TacticalMap->ZoomFactor;
		tmp.X = (int)(((double)zoom_surface_width - zoomed_width) / 2.0);
		double zoomed_height = (double)zoom_surface_height / TacticalMap->ZoomFactor;
		tmp.Y = (int)(((double)zoom_surface_height - zoomed_height) / 2.0);
		tmp.Width = (int)zoomed_width;
		tmp.Height = (int)zoomed_height;

		src_rect = tmp;

	} else {

		if (src_rect.Width >= dest_rect.Width) {
			src_rect.Width = dest_rect.Width;
		}
		if (src_rect.Height >= dest_rect.Height) {
			src_rect.Height = dest_rect.Height;
		}

	}

	if (shake_y < 0) {
		src_rect.Y -= shake_y;
	}

	if (shake_x < 0) {
		src_rect.X -= shake_x;
	}

	/// Draw filler for X offset
	if (shake_x != 0) {
		fill_rect.Set(fill_rect.X, 0, abs(shake_x), surface->Get_Height());
		fill_rect.X = shake_x < 0 ? dest_rect.X + dest_rect.Width : dest_rect.X - shake_x;

		VisibleSurface->Fill_Rect(VisibleSurface->Get_Rect(), fill_rect, 0);
	}

	/// Draw filler for Y offset
	if (shake_y != 0) {
		fill_rect.Set(0, fill_rect.Y, surface->Get_Width(), abs(shake_y));
		fill_rect.Y = shake_y < 0 ? dest_rect.Y + dest_rect.Height : dest_rect.Y - shake_y;

		VisibleSurface->Fill_Rect(VisibleSurface->Get_Rect(), fill_rect, 0);
	}

	/*
	 * Now blit the source surface to the visible surface. A composite frame copies only the rows written
	 * since its last copy while nothing else has changed the visible surface and the frame sits where it did;
	 * every 120th copy is whole.
	 */
	DSurface * const visible = (DSurface *)VisibleSurface;
	unsigned const before = visible->Change_Count();
	bool const composite = (surface == CompositeSurface);
	bool partial = false;
	bool listed = false;
	if (composite) {
		listed = Take_Composite_Rows(&_SinceKeyFill, &_SinceCopy);
		Rect const bounds = visible->Get_Rect();
		bool const inside = dest_rect.X >= 0 && dest_rect.Y >= 0 && dest_rect.X + dest_rect.Width <= bounds.Width && dest_rect.Y + dest_rect.Height <= bounds.Height;
		bool const unzoomed = (TacticalMap == NULL || TacticalMap->ZoomFactor == 1.0);
		partial = listed && _SinceCopy.Known && inside && unzoomed && shake_x == 0 && shake_y == 0
			&& before == _VisibleChangesAfterCopy && src_rect == _CopySource && dest_rect == _CopyDest && --_CopiesUntilFull > 0;
	}
	if (partial) {
		_CopiedRows.assign((size_t)visible->Get_Height(), 0);
		int const width = std::min(src_rect.Width, dest_rect.Width);
		int const height = std::min(src_rect.Height, dest_rect.Height);
		int const shift_x = dest_rect.X - src_rect.X;
		int const shift = dest_rect.Y - src_rect.Y;
		_SinceCopy.For_Each_Rect(Rect(src_rect.X, src_rect.Y, width, height), [&](Rect const & rect) {
			VisibleSurface->Blit_From(Rect(rect.X + shift_x, rect.Y + shift, rect.Width, rect.Height), *surface, rect, false, true);
			std::fill(_CopiedRows.begin() + (rect.Y + shift), _CopiedRows.begin() + (rect.Y + rect.Height + shift), (unsigned char)1);
		});
		// Emissive values can change where the pixels did not, so all of them follow.
		Emissive_After_Bit_Blit(*VisibleSurface, dest_rect.X, dest_rect.Y, *surface, src_rect.X, src_rect.Y, width, height);
		Video_Note_Visible_Rows(_CopiedRows.data(), (int)_CopiedRows.size(), before, visible->Change_Count());
	} else {
		VisibleSurface->Blit_From(dest_rect, *surface, src_rect, false, true);
		Video_Note_Visible_Rows(NULL, 0, before, visible->Change_Count());
		_CopiesUntilFull = 120;
	}
	if (composite) {
		if (listed) {
			_SinceCopy.Restart(CompositeSurface->Get_Height(), CompositeSurface->Get_Width());
		}
		_CopySource = src_rect;
		_CopyDest = dest_rect;
		_VisibleChangesAfterCopy = visible->Change_Count();
	}

	Video_Present_If_Dirty();
}


/// <summary>
/// Performs the per frame logic for the game screen.
/// This routine eases the screen shake offset back toward its resting position, so that a
/// jolt requested by an explosion or a superweapon settles down of its own accord. Derived
/// screens call this routine from their own logic pass.
/// </summary>
/// <param name="xy">The current mouse position.</param>
void GScreenClass::AI(KeyNumType &, Point2D const & xy)
{
	// On every other game frame the shake swings to the other side, one pixel smaller (GScreenClass::Update, 0x4F4BB0).
	// This runs several times per game frame, so it remembers the last frame it acted on.
	static int _lastframe = -1;
	if (Frame % 2 != 0 || Frame == _lastframe) {
		return;
	}
	_lastframe = Frame;
	if (ScreenX != 0) {
		ScreenX = (ScreenX < 0 ? -1 : 1) - ScreenX;
	}
	if (ScreenY != 0) {
		ScreenY = (ScreenY < 0 ? -1 : 1) - ScreenY;
	}
}
