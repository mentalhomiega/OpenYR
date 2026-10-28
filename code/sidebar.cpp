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

/* $Header: /counterstrike/SIDEBAR.CPP 2     3/17/97 1:05a Steve_tall $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : SIDEBAR.CPP                                                  *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : October 20, 1994                                             *
 *                                                                                             *
 *                  Last Update : October 9, 1996 [JLB]                                        *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   SidebarClass::AI -- Handles player clicking on sidebar area.                              *
 *   SidebarClass::Abandon_Production -- Stops production of the object specified.             *
 *   SidebarClass::Activate -- Controls the sidebar activation.                                *
 *   SidebarClass::Activate_Demolish -- Controls the demolish button on the sidebar.           *
 *   SidebarClass::Activate_Repair -- Controls the repair button on the sidebar.               *
 *   SidebarClass::Activate_Upgrade -- Controls the upgrade button on the sidebar.             *
 *   SidebarClass::Add -- Adds a game object to the sidebar list.                              *
 *   SidebarClass::Draw_It -- Renders the sidebar display.                                     *
 *   SidebarClass::Factory_Link -- Links a factory to a sidebar strip.                         *
 *   SidebarClass::Init_Clear -- Sets sidebar to a known (and deactivated) state               *
 *   SidebarClass::Init_IO -- Adds buttons to the button list                                  *
 *   SidebarClass::Init_Theater -- Performs theater-specific initialization                    *
 *   SidebarClass::One_Time -- Handles the one time game initializations.                      *
 *   SidebarClass::One_Time -- Handles the one time game initializations.                      *
 *   SidebarClass::Recalc -- Examines the sidebar data and updates it as necessary.            *
 *   SidebarClass::Refresh_Cells -- Intercepts the refresh, looking for sidebar controls.      *
 *   SidebarClass::SBGadgetClass::Action -- Special function that controls the mouse over the s*
 *   SidebarClass::Scroll -- Handles scrolling the sidebar object strip.                       *
 *   SidebarClass::Set_Current -- Sets a specified object that controls the sidebar display.   *
 *   SidebarClass::SidebarClass -- Default constructor for the sidebar.                        *
 *   SidebarClass::SidebarClass -- This is the no initialization constructor for the sidebar.  *
 *   SidebarClass::StripClass::AI -- Input and AI processing for the side strip.               *
 *   SidebarClass::StripClass::Abandon_Produ -- Abandons production associated with sidebar.   *
 *   SidebarClass::StripClass::Activate -- Adds the strip buttons to the input system.         *
 *   SidebarClass::StripClass::Add -- Add an object to the side strip.                         *
 *   SidebarClass::StripClass::Deactivate -- Removes the side strip buttons from the input syst*
 *   SidebarClass::StripClass::Draw_It -- Render the sidebar display.                          *
 *   SidebarClass::StripClass::Factory_Link -- Links a factory to a sidebar button.            *
 *   SidebarClass::StripClass::Flag_To_Redra -- Flags the sidebar strip to be redrawn.         *
 *   SidebarClass::StripClass::Get_Special_Cameo -- Fetches the special event cameo shape.     *
 *   SidebarClass::StripClass::Init_Clear -- Sets sidebar to a known (and deactivated) state   *
 *   SidebarClass::StripClass::Init_IO -- Adds buttons to the button list                      *
 *   SidebarClass::StripClass::Init_Theater -- Performs theater-specific initialization        *
 *   SidebarClass::StripClass::One_Time -- Performs one time actions necessary for the side str*
 *   SidebarClass::StripClass::Recalc -- Revalidates the current sidebar list of objects.      *
 *   SidebarClass::StripClass::Scroll -- Causes the side strip to scroll.                      *
 *   SidebarClass::StripClass::SelectClass:: -- Action function when buildable cameo is selecte*
 *   SidebarClass::StripClass::SelectClass:: -- Assigns special values to a buildable select bu*
 *   SidebarClass::StripClass::SelectClass::SelectClass -- Default constructor.                *
 *   SidebarClass::StripClass::StripClass -- Default constructor for the side strip class.     *
 *   SidebarClass::Which_Column -- Determines which column a given type should appear.         *
 *   SidebarClass::Zoom_Mode_Control -- Handles the zoom mode toggle operation.                *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "sidebar.h"
#include "sidebargrid.h"
#include "cameopcx.h"

#include "_bench.h"
#include "_convert.h"
#include "_keyboar.h"
#include "_map.h"
#include "_mixfile.h"
#include "_rtti.h"
#include "_rules.h"
#include "_surface.h"
#include "_tooltip.h"
#include "_ui.h"
#include "bench.h"
#include "building.h"
#include "builtype.h"
#include "cctooltip.h"
#include "conquer.h"
#include "convert.h"
#include "data.h"
#include "dialog.h"
#include "draw.h"
#include "event.h"
#include "factory.h"
#include "font.h"
#include "globals.h"
#include "goptions.h"
#include "house.h"
#include "houstype.h"
#include "hud.h"
#include "incdec.h"
#include "init.h"
#include "language/language.h"
#include "map.h"
#include "mixfile.h"
#include "movie.h"
#include "mpmode.h"
#include "queue.h"
#include "rect.h"
#include "rules.h"
#include "savestream.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "super.h"
#include "suprtype.h"
#include "surface.h"
#include "techtype.h"
#include "ui/uishell.h"
#include "utf8.h"
#include "viewzoom.h"
#include "voc.h"
#include "vox.h"

#include "bench.hh"
#include "color.hh"

#include <algorithm>
#include <compare>

ShapeSet const * SidebarClass::SidebarShape = NULL;
ShapeSet const * SidebarClass::SidebarMiddleShape = NULL;
ShapeSet const * SidebarClass::SidebarBottomShape = NULL;
ShapeSet const * SidebarClass::SidebarAddonShape = NULL;

/***************************************************************************
**	This points to the main sidebar shapes. These include the upgrade and
**	repair buttons.
*/
//TheaterType SidebarClass::StripClass::LastTheater = THEATER_NONE;

enum ButtonNumberType {
	BUTTON_RADAR = 100,
	BUTTON_REPAIR,
	BUTTON_POWER,
	BUTTON_SELL,
	BUTTON_UPGRADE,
	BUTTON_WAYPOINT,

	BUTTON_TAB = 230,		// One per tab, after the strips' scroll and select buttons.
	BUTTON_OPTIONS = BUTTON_TAB + 4,
	BUTTON_DIPLOMACY,
	BUTTON_BEACON
};

/*
**	Sidebar buttons
*/
SidebarClass::SBGadgetClass SidebarClass::Background;
ShapeButtonClass SidebarClass::Repair;
ShapeButtonClass SidebarClass::Power;
ShapeButtonClass SidebarClass::Upgrade;
ShapeButtonClass SidebarClass::Waypoint;
ShapeButtonClass SidebarClass::TabButton[COLUMNS];

// The modern panel's tabs lit on the current blink, one bit per tab.
static unsigned TabFlashMask = 0;

// Do the modern panel's tabs show the icons drawn for it, rather than Yuri's Revenge's art?
static bool TabIcons = false;
static ShapeSet const * HudIconSet = NULL;

// Whether each classic tab is disabled, as it was last set in AI. A gadget does not expose its own state.
static bool TabDisabled[SidebarClass::COLUMNS] = {};

// The cameo slots of the shown strip that are in the map's button list. Changing the rows of the
// modern build panel adds or removes the difference.
static int Registered_Slots = 0;

// Whether each classic strip's two scroll arrows are disabled, as they were last set in AI.
static bool ArrowsDisabled[SidebarClass::COLUMNS] = {};

// A classic tab or scroll arrow tooltip. A disabled gadget adds "(Disabled)" on a second line.
static std::string Classic_Tip(char const * label, char const * fallback, bool enabled)
{
	std::string text = Panel_Text(label, fallback);
	if (!enabled) {
		text += "\n(" + Panel_Text("Tip:Disabled", "Disabled") + ")";
	}
	return(text);
}

ShapeButtonClass SidebarClass::OptionsButton;
ShapeButtonClass SidebarClass::DiplomacyButton;
ShapeButtonClass SidebarClass::BeaconButton;
ShapeButtonClass SidebarClass::StripClass::UpButton[COLUMNS];
ShapeButtonClass SidebarClass::StripClass::DownButton[COLUMNS];
SidebarClass::StripClass::SelectClass
SidebarClass::StripClass::SelectButton[COLUMNS][MAX_SLOTS];

bool SidebarClass::IsGridLabelShown = false;

bool SidebarClass::IsToBlitSidebar = false;

/*
**	Shape data pointers
*/
ShapeSet * SidebarClass::StripClass::LogoShapes = NULL;
ShapeSet const * SidebarClass::StripClass::ClockShapes;
ShapeSet const * SidebarClass::StripClass::RechargeClockShapes;
ShapeSet const * SidebarClass::StripClass::DarkenShapes;

void Print_Cameo_Text(char const * string, Point2D const & point, Rect const & cliprect, int maxlinelen);


enum CameoCategoryType {
	CAMEO_CATEGORY_SUPERWEAPON,
	CAMEO_CATEGORY_INFANTRY,
	CAMEO_CATEGORY_AIRCRAFT,
	CAMEO_CATEGORY_UNIT,
	CAMEO_CATEGORY_BUILDING,
	CAMEO_CATEGORY_OTHER
};

enum CameoGroupType {
	CAMEO_GROUP_NORMAL,
	CAMEO_GROUP_WALL,
	CAMEO_GROUP_GATE,
	CAMEO_GROUP_DEFENSE
};

/*
 * The place a cameo takes on its strip. The members are compared in declaration order, so
 * that order is the ordering rule the sidebar publishes.
 */
struct CameoOrderType {
	int Category;
	int SortOrder;
	int Group;
	int RulesIndex;
	int Type;

	auto operator<=>(CameoOrderType const &) const = default;
};

struct CameoSortType {
	CameoOrderType Order;
	SidebarClass::StripClass::BuildType Entry;
};


static CameoOrderType Cameo_Order(SidebarClass::StripClass::BuildType const & entry)
{
	CameoOrderType order;

	order.Category = CAMEO_CATEGORY_OTHER;
	order.SortOrder = 0;
	order.Group = CAMEO_GROUP_NORMAL;
	order.RulesIndex = entry.BuildableID;
	order.Type = entry.BuildableType;

	// A super weapon has no techno type, and a scenario trigger may name one the rules dropped.
	if (entry.BuildableType == RTTI_SPECIAL) {
		order.Category = CAMEO_CATEGORY_SUPERWEAPON;
		if ((unsigned)entry.BuildableID < (unsigned)SuperWeaponTypes.Count()) {
			order.SortOrder = SuperWeaponTypes[entry.BuildableID]->CameoSortOrder;
		}
		return(order);
	}

	switch (entry.BuildableType) {
		case RTTI_INFANTRYTYPE:
		case RTTI_INFANTRY:
			order.Category = CAMEO_CATEGORY_INFANTRY;
			break;

		case RTTI_AIRCRAFTTYPE:
		case RTTI_AIRCRAFT:
			order.Category = CAMEO_CATEGORY_AIRCRAFT;
			break;

		case RTTI_UNITTYPE:
		case RTTI_UNIT:
			order.Category = CAMEO_CATEGORY_UNIT;
			break;

		case RTTI_BUILDINGTYPE:
		case RTTI_BUILDING:
			order.Category = CAMEO_CATEGORY_BUILDING;
			break;

		default:
			break;
	}

	TechnoTypeClass const * tech = Fetch_Techno_Type(entry.BuildableType, entry.BuildableID);
	if (tech == NULL) {
		return(order);
	}

	order.SortOrder = tech->CameoSortOrder;

	if (order.Category == CAMEO_CATEGORY_BUILDING) {
		BuildingTypeClass const * building = (BuildingTypeClass const *)tech;

		if (building->IsWall || building->IsFirestormWall || building->IsLaserFence || building->IsLaserFencePost) {
			order.Group = CAMEO_GROUP_WALL;
		} else if (building->IsGate) {
			order.Group = CAMEO_GROUP_GATE;
		} else if (building->IsSortCameoAsBaseDefense) {
			order.Group = CAMEO_GROUP_DEFENSE;
		}
	}

	return(order);
}


/// <summary>
/// Whether the left cameo goes before the right one in Yuri's Revenge's sidebar order
/// (BuildType::SortsBefore, 0x6A8420). Superweapons come first by recharge time; then the
/// player's own side's types; then ground types, aircraft and ships; then technology level,
/// cost to the player and name. Two entries that tie on all of these return true both ways.
/// </summary>
static bool Classic_Sorts_Before(SidebarClass::StripClass::BuildType const & left, SidebarClass::StripClass::BuildType const & right)
{
	if (right.BuildableType == RTTI_NONE) {
		return(true);
	}

	bool const left_special = left.BuildableType == RTTI_SPECIAL;
	bool const right_special = right.BuildableType == RTTI_SPECIAL;

	if (left_special || right_special) {
		if (!left_special) {
			return(false);
		}
		if (!right_special) {
			return(true);
		}
		if ((unsigned)left.BuildableID >= (unsigned)SuperWeaponTypes.Count() || (unsigned)right.BuildableID >= (unsigned)SuperWeaponTypes.Count()) {
			return(left.BuildableID <= right.BuildableID);
		}
		SuperWeaponTypeClass const * lsuper = SuperWeaponTypes[left.BuildableID];
		SuperWeaponTypeClass const * rsuper = SuperWeaponTypes[right.BuildableID];
		if (lsuper->RechargeTime != rsuper->RechargeTime) {
			return(lsuper->RechargeTime < rsuper->RechargeTime);
		}
		return(strcmp(lsuper->Full_Name(), rsuper->Full_Name()) <= 0);
	}

	TechnoTypeClass const * ltech = Fetch_Techno_Type(left.BuildableType, left.BuildableID);
	TechnoTypeClass const * rtech = Fetch_Techno_Type(right.BuildableType, right.BuildableID);
	if (ltech == NULL || rtech == NULL) {
		return(rtech == NULL);
	}

	if (PlayerPtr != NULL) {
		int const side = (int)PlayerPtr->Class->Side;
		bool const left_own = ltech->AIBasePlanningSide == side;
		bool const right_own = rtech->AIBasePlanningSide == side;
		if (left_own != right_own) {
			return(left_own);
		}
	}

	bool const left_mobile = left.BuildableType == RTTI_UNITTYPE || left.BuildableType == RTTI_AIRCRAFTTYPE;
	bool const right_mobile = right.BuildableType == RTTI_UNITTYPE || right.BuildableType == RTTI_AIRCRAFTTYPE;
	bool const left_air = left_mobile && ltech->IsConsideredAircraft;
	bool const left_naval = left_mobile && ltech->IsNaval;
	bool const right_air = right_mobile && rtech->IsConsideredAircraft;
	bool const right_naval = right_mobile && rtech->IsNaval;
	bool const right_ground = !right_air && !right_naval;

	// A type that is both an aircraft and naval takes gamemd's mixed path through these tests.
	if (!left_air && !left_naval) {
		if (right_air || right_naval) {
			return(true);
		}
	} else if (!left_air) {
		if (right_ground || right_air) {
			return(false);
		}
	} else {
		if (right_ground) {
			return(false);
		}
		if (right_naval) {
			return(true);
		}
	}

	if (ltech->Level != rtech->Level) {
		return(ltech->Level < rtech->Level);
	}

	int const lcost = ltech->Cost_Of(PlayerPtr);
	int const rcost = rtech->Cost_Of(PlayerPtr);
	if (lcost != rcost) {
		return(lcost < rcost);
	}

	return(strcmp(ltech->Full_Name(), rtech->Full_Name()) <= 0);
}


/// <summary>
/// Default constructor for the sidebar background gadget.
/// This gadget exists to cover the sidebar so that the mouse reverts to its normal shape
/// whenever it strays over there. The sidebar gives the gadget its real position and size
/// when it lays itself out.
/// </summary>
SidebarClass::SBGadgetClass::SBGadgetClass(void) :
	GadgetClass(0, 0, 1, 1, LEFTUP)
{
}


/***********************************************************************************************
 * SidebarClass::SidebarClass -- Default constructor for the sidebar.                          *
 *                                                                                             *
 *    Constructor for the sidebar handler. It basically sets up the sidebar to the empty       *
 *    condition.                                                                               *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/17/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
SidebarClass::SidebarClass(void) :
	BASECLASS(),
	IsCameoText(false),
	IsSidebarActive(false),
	IsToRedrawCredits(false),
	IsToRedraw(true),
	IsRepairActive(false),
	IsUpgradeActive(false),
	IsDemolishActive(false),
	ActiveTab(0)
{
	CompletedBuilding[0] = NULL;
	CompletedBuilding[1] = NULL;

	// Every strip covers the same grid; only the active tab's strip is shown.
	for (int index = 0; index < COLUMNS; index++) {
		new (&Column[index]) StripClass(InitClass());
		Column[index].X = GRID_X;
		Column[index].Y = GRID_Y;
		Column[index].ObjectRect = Rect(Column[index].X, Column[index].Y, StripClass::OBJECT_WIDTH * StripClass::Slot_Columns(), StripClass::OBJECT_HEIGHT * Max_Visible() / StripClass::Slot_Columns());
	}
}


/// <summary>
/// Turns the cameo cost text on or off.
/// This routine is called when the player changes the option. With the text enabled, every
/// build cameo is captioned with its cost, so the sidebar is flagged to redraw itself.
/// </summary>
/// <param name="on">Should the cameo text be displayed?</param>
void SidebarClass::Toggle_Cameo_Text(bool on)
{
	if (on != IsCameoText) {
		IsCameoText = on;
		IsToRedraw = true;
		Flag_To_Redraw();
	}
}


/***********************************************************************************************
 * SidebarClass::One_Time -- Handles the one time game initializations.                        *
 *                                                                                             *
 *    This routine is used to load the graphic data that is needed by the sidebar display. It  *
 *    should only be called ONCE.                                                              *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Only call this routine once when the game first starts.                         *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/28/94   JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::One_Time(void)
{
	BASECLASS::One_Time();

	for (int index = 0; index < COLUMNS; index++) {
		Column[index].One_Time(index);
	}
}


/***********************************************************************************************
 * SidebarClass::Init_Clear -- Sets sidebar to a known (and deactivated) state                 *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::Init_Clear(void)
{

	BASECLASS::Init_Clear();

	IsToRedraw = true;
	IsRepairActive = false;
	IsUpgradeActive = false;
	IsDemolishActive = false;

	for (int index = 0; index < COLUMNS; index++) {
		Column[index].Init_Clear();
	}
	ActiveTab = 0;
	CompletedBuilding[0] = NULL;
	CompletedBuilding[1] = NULL;

	Activate(false);
}


/***********************************************************************************************
 * SidebarClass::Init_IO -- Adds buttons to the button list                                    *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::Init_IO(void)
{
	BASECLASS::Init_IO();

	SidebarRect.X = Sidebar_Screen_X();
	SidebarRect.Y = SIDE_Y;
	SidebarRect.Width = 641 - SidebarRect.X;
	SidebarRect.Height = (ScreenTacticalRect.Y - SidebarRect.Y) + ScreenTacticalRect.Height + COMMAND_BAR_HEIGHT;

	/*
	**	Add the sidebar's buttons only if we're not in editor mode.
	*/
	if (!Debug_Map) {
		int xoff = -480;
		int yoff = 3;

		Repair.IsSticky = true;
		Repair.ID = BUTTON_REPAIR;
		Repair.X = SidebarRect.X;
		Repair.Y = SidebarRect.Y;
		Repair.DrawOffsetX = xoff;
		Repair.DrawOffsetY = yoff;
		Repair.DrawOnSidebar = true;
		Repair.ShapeDrawer = SidebarDrawer;
		Repair.IsPressed = false;
		Repair.IsToggleType = true;
		Repair.ReflectButtonState = true;

		Upgrade.IsSticky = true;
		Upgrade.ID = BUTTON_UPGRADE;
		Upgrade.X = Repair.X + BUTTON_SPACING;
		Upgrade.Y = Repair.Y;
		Upgrade.DrawOffsetX = xoff;
		Upgrade.DrawOffsetY = yoff;
		Upgrade.DrawOnSidebar = true;
		Upgrade.ShapeDrawer = SidebarDrawer;
		Upgrade.IsPressed = false;
		Upgrade.IsToggleType = true;
		Upgrade.ReflectButtonState = true;

		Power.IsSticky = true;
		Power.ID = BUTTON_POWER;
		Power.X = Upgrade.X + BUTTON_SPACING;
		Power.Y = Upgrade.Y;
		Power.DrawOffsetX = xoff;
		Power.DrawOffsetY = yoff;
		Power.DrawOnSidebar = true;
		Power.ShapeDrawer = SidebarDrawer;
		Power.IsPressed = false;
		Power.IsToggleType = true;
		Power.ReflectButtonState = true;

		Waypoint.IsSticky = true;
		Waypoint.ID = BUTTON_WAYPOINT;
		Waypoint.X = Power.X + BUTTON_SPACING;
		Waypoint.Y = Power.Y;
		Waypoint.DrawOffsetX = xoff;
		Waypoint.DrawOffsetY = yoff;
		Waypoint.DrawOnSidebar = true;
		Waypoint.ShapeDrawer = SidebarDrawer;
		Waypoint.IsPressed = false;
		Waypoint.IsToggleType = true;
		Waypoint.ReflectButtonState = true;

		Waypoint.Enable();

		for (int index = 0; index < COLUMNS; index++) {
			Column[index].Init_IO(index);

			TabButton[index].ID = BUTTON_TAB + index;
			TabButton[index].IsSticky = true;
			TabButton[index].DrawOffsetX = xoff;
			TabButton[index].DrawOffsetY = yoff;
			TabButton[index].DrawOnSidebar = true;
			TabButton[index].ShapeDrawer = SidebarDrawer;
			TabButton[index].IsPressed = false;
		}

		OptionsButton.ID = BUTTON_OPTIONS;
		DiplomacyButton.ID = BUTTON_DIPLOMACY;
		for (ShapeButtonClass * button : {&OptionsButton, &DiplomacyButton}) {
			button->IsSticky = true;
			button->DrawOffsetX = xoff;
			button->DrawOffsetY = yoff;
			button->DrawOnSidebar = true;
			button->ShapeDrawer = SidebarDrawer;
			button->IsPressed = false;
		}

		Reposition_Sidebar();

		/*
		**	If a game was loaded & the sidebar was enabled, pop it up now
		*/
		if (IsSidebarActive) {
			IsSidebarActive = false;
			Activate(1);
//			Background.Zap();
//			Add_A_Button(Background);
		}
	}
}


/// <summary>
/// Loads the sidebar artwork that belongs to the player's house.
/// This routine is called whenever the house the player controls is established. It builds
/// the sidebar's own drawing translation table from the sidebar palette and then fetches
/// the frame, mode button and scroll arrow shapes that are drawn through it.
/// </summary>
void SidebarClass::Init_For_House(void)
{
	int i;

	BASECLASS::Init_For_House();

	// Yuri's Revenge keeps the build clock in each side's archives and has no separate recharge clock.
	StripClass::ClockShapes = (ShapeSet const *)MFCD::Retrieve("GCLOCK2.SHP");
	StripClass::RechargeClockShapes = StripClass::ClockShapes;

	PaletteClass pal;

	memmove(&pal, MFCD::Retrieve("SIDEBAR.PAL"), sizeof(pal));

	for (i = 0; i < PaletteClass::COLOR_COUNT; i++) {
		pal[i] = RGBClass(pal[i].Get_Red() * 4, pal[i].Get_Green() * 4, pal[i].Get_Blue() * 4);
	}

	if (SidebarDrawer != NULL) {
		delete SidebarDrawer;
		SidebarDrawer = NULL;
	}

	SidebarDrawer = new ConvertClass(pal, pal, *VisibleSurface, 1, false);

	Upgrade.Set_Shape((ShapeSet *)MFCD::Retrieve("SELL.SHP"));
	Upgrade.ShapeDrawer = SidebarDrawer;
	Power.Set_Shape((ShapeSet *)MFCD::Retrieve("POWER.SHP"));
	Power.ShapeDrawer = SidebarDrawer;
	Waypoint.Set_Shape((ShapeSet *)MFCD::Retrieve("WAYP.SHP"));
	Waypoint.ShapeDrawer = SidebarDrawer;
	Repair.Set_Shape((ShapeSet *)MFCD::Retrieve("REPAIR.SHP"));
	Repair.ShapeDrawer = SidebarDrawer;
	OptionsButton.Set_Shape((ShapeSet *)MFCD::Retrieve("OPTBTN.SHP"));
	OptionsButton.ShapeDrawer = SidebarDrawer;
	DiplomacyButton.Set_Shape((ShapeSet *)MFCD::Retrieve("DIPLOBTN.SHP"));
	DiplomacyButton.ShapeDrawer = SidebarDrawer;

	SidebarShape = (ShapeSet *)MFCD::Retrieve("SIDE1.SHP");
	SidebarMiddleShape = (ShapeSet *)MFCD::Retrieve("SIDE2.SHP");
	SidebarBottomShape = (ShapeSet *)MFCD::Retrieve("SIDE3.SHP");
	SidebarAddonShape = (ShapeSet *)MFCD::Retrieve("ADDON.SHP");

	for (i = 0; i < COLUMNS; i++) {
		StripClass::UpButton[i].Set_Shape((ShapeSet *)MFCD::Retrieve("R-UP.SHP"));
		StripClass::UpButton[i].ShapeDrawer = SidebarDrawer;
		StripClass::DownButton[i].Set_Shape((ShapeSet *)MFCD::Retrieve("R-DN.SHP"));
		StripClass::DownButton[i].ShapeDrawer = SidebarDrawer;

		char name[16];
		sprintf(name, "TAB%02d.SHP", i);
		TabButton[i].Set_Shape((ShapeSet *)MFCD::Retrieve(name));
		TabButton[i].ShapeDrawer = SidebarDrawer;
	}

	TabIcons = false;
	if (Modern_Interface()) {
		Use_Hud_Icons();
	} else {
		// The interface style can change at the main menu, after an earlier game gave these buttons icons.
		for (ShapeButtonClass * button : {&Repair, &Upgrade, &Power, &BeaconButton, &OptionsButton}) {
			button->IsHudIcon = false;
		}
		for (int i = 0; i < COLUMNS; i++) {
			StripClass::UpButton[i].IsHudIcon = false;
			StripClass::DownButton[i].IsHudIcon = false;
			TabButton[i].IsHudIcon = false;
		}
		OptionsButton.Label.clear();
	}
}


/// <summary>
/// Gives the modern build panel's tabs, repair and sell buttons and scroll buttons the icons
/// drawn for it, where the ui folder has them. A button without one keeps its Yuri's Revenge art.
/// </summary>
void SidebarClass::Use_Hud_Icons(void)
{
	auto use = [](ShapeButtonClass & button, char const * name) {
		ShapeSet const * shape = Hud_Icon(name);
		if (shape != NULL) {
			button.Set_Shape(const_cast<ShapeSet *>(shape));
		}
		button.IsHudIcon = shape != NULL;
		return(shape != NULL);
	};

	use(Repair, "RP");
	use(Upgrade, "SL");
	use(Power, "PW");
	BeaconButton.IsSticky = true;
	BeaconButton.ID = BUTTON_BEACON;
	BeaconButton.DrawOnSidebar = true;
	BeaconButton.ShapeDrawer = SidebarDrawer;
	BeaconButton.IsToggleType = true;
	BeaconButton.ReflectButtonState = true;
	use(BeaconButton, "BC");
	if (use(OptionsButton, "OP")) {
		OptionsButton.Label.clear();
	}
	bool tabs = true;
	for (int i = 0; i < COLUMNS; i++) {
		use(StripClass::UpButton[i], "UP");
		use(StripClass::DownButton[i], "DN");
		char name[4];
		sprintf(name, "T%d", i);
		tabs = use(TabButton[i], name) && tabs;
	}
	TabIcons = tabs;
	HudIconSet = Hud_Icon("T0");
}


void SidebarClass::Clear_For_House(void)
{
	// A strip removes as many buttons as the artwork's height let it add, so it goes first.
	Activate(false);

	Upgrade.Set_Shape(NULL);
	Power.Set_Shape(NULL);
	Waypoint.Set_Shape(NULL);
	Repair.Set_Shape(NULL);
	OptionsButton.Set_Shape(NULL);
	DiplomacyButton.Set_Shape(NULL);
	for (int i = 0; i < COLUMNS; i++) {
		StripClass::UpButton[i].Set_Shape(NULL);
		StripClass::DownButton[i].Set_Shape(NULL);
		TabButton[i].Set_Shape(NULL);
	}

	SidebarShape = NULL;
	SidebarMiddleShape = NULL;
	SidebarBottomShape = NULL;
	SidebarAddonShape = NULL;
	StripClass::ClockShapes = NULL;
	StripClass::RechargeClockShapes = NULL;

	BASECLASS::Clear_For_House();
}


/***********************************************************************************************
 * SidebarClass::Reload_Sidebar -- Loads appropriate sidebar shapes depending on house         *
 *                                                                                             *
 * INPUT:  none                                                                                *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   9/18/1996 BWG : Created.                                                                  *
 *=============================================================================================*/
void SidebarClass::Reload_Sidebar(void)
{
	for (int index = 0; index < COLUMNS; index++) {
		Column[index].X = GRID_X;
		Column[index].Y = GRID_Y;
		Column[index].ObjectRect = Rect(Column[index].X, Column[index].Y, StripClass::OBJECT_WIDTH * StripClass::Slot_Columns(), StripClass::OBJECT_HEIGHT * Max_Visible() / StripClass::Slot_Columns());
	}
}


/***********************************************************************************************
 * SidebarClass::Which_Column -- Determines which column a given type should appear.           *
 *                                                                                             *
 *    Use this function to resolve what column the specified object type should be placed      *
 *    into.                                                                                    *
 *                                                                                             *
 * INPUT:   otype -- Pointer to the object type class of the object in question.               *
 *                                                                                             *
 * OUTPUT:  Returns with the column number that the object should be placed in.                *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/01/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int SidebarClass::Which_Column(RTTIType type, int id)
{
	// Yuri's Revenge's tabs (SidebarClass::GetObjectTabIdx): combat structures and superweapons
	// on the defenses tab, other structures on the buildings tab.
	switch (type) {
		case RTTI_BUILDING:
		case RTTI_BUILDINGTYPE:
			return(BuildingTypes[id]->BuildCat == BUILDCAT_COMBAT ? 1 : 0);

		case RTTI_SPECIAL:
			return(1);

		case RTTI_INFANTRY:
		case RTTI_INFANTRYTYPE:
			return(2);

		default:
			return(3);
	}
}


/***********************************************************************************************
 * SidebarClass::Factory_Link -- Links a factory to a sidebar strip.                           *
 *                                                                                             *
 *    This routine will link the specified factory to the sidebar strip. A factory must be     *
 *    linked to the sidebar so that as the factory production progresses, the sidebar will     *
 *    show the production progress.                                                            *
 *                                                                                             *
 * INPUT:   factory  -- The factory number to attach.                                          *
 *                                                                                             *
 *          type     -- The object type number.                                                *
 *                                                                                             *
 *          id       -- The object sub-type number.                                            *
 *                                                                                             *
 * OUTPUT:  Was the factory successfully attached to the sidebar strip?                        *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SidebarClass::Factory_Link(FactoryClass * factory, RTTIType type, int id)
{
	assert((unsigned)type < RTTI_COUNT);
	assert(id >= 0);

	return(Column[Which_Column(type, id)].Factory_Link(factory, type, id));
}


/// <summary>
/// Reports whether an object type is offered on either side strip.
/// </summary>
bool SidebarClass::Is_On_Sidebar(RTTIType type, int id) const
{
	for (int column = 0; column < COLUMNS; column++) {
		StripClass const & strip = Column[column];
		for (int index = 0; index < strip.BuildableCount; index++) {
			if (strip.Buildables[index].BuildableType == type && strip.Buildables[index].BuildableID == id) {
				return(true);
			}
		}
	}
	return(false);
}


/***********************************************************************************************
 * SidebarClass::Activate_Repair -- Controls the repair button on the sidebar.                 *
 *                                                                                             *
 *    Use this routine to turn the repair sidebar button on and off. Typically, the button     *
 *    is enabled when the currently selected structure is friendly and damaged.                *
 *                                                                                             *
 * INPUT:   control  -- The controls how the button is to be activated or deactivated;         *
 *                      0  -- Turn button off.                                                 *
 *                      1  -- Turn button on.                                                  *
 *                      -1 -- Toggle button state.                                             *
 *                                                                                             *
 * OUTPUT:  bool; Was the button previously activated?                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SidebarClass::Activate_Repair(int control)
{
	bool old = IsRepairActive;

	if (control == -1) {
		control = IsRepairActive ? 0 : 1;
	}
	switch (control) {
		case 1:
			IsRepairActive = true;
			break;

		default:
		case 0:
			IsRepairActive = false;
			break;
	}
	if (old != IsRepairActive) {
		Flag_To_Redraw();
		IsToRedraw = true;

		if (!IsRepairActive) {
//			Help_Text(TXT_NONE);
			Set_Default_Mouse(MOUSE_NORMAL, false);
		}
	}
	return(old);
}


/***********************************************************************************************
 * SidebarClass::Activate_Upgrade -- Controls the upgrade button on the sidebar.               *
 *                                                                                             *
 *    Use this routine to turn the upgrade sidebar button on and off. Typically, the button    *
 *    is enabled when the currently selected structure can be upgraded and disabled otherwise. *
 *                                                                                             *
 * INPUT:   control  -- The controls how the button is to be activated or deactivated;         *
 *                      0  -- Turn button off.                                                 *
 *                      1  -- Turn button on.                                                  *
 *                      -1 -- Toggle button state.                                             *
 *                                                                                             *
 * OUTPUT:  bool; Was the button previously activated?                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SidebarClass::Activate_Upgrade(int control)
{
	bool old = IsUpgradeActive;
	if (control == -1) {
		control = IsUpgradeActive ? 0 : 1;
	}
	switch (control) {
		case 1:
			IsUpgradeActive = true;
			break;

		default:
		case 0:
			IsUpgradeActive = false;
			break;
	}
	if (old != IsUpgradeActive) {
		Flag_To_Redraw();
		IsToRedraw = true;
		if (!IsUpgradeActive) {
			Set_Default_Mouse(MOUSE_NORMAL, false);
		}
	}
	return(old);
}


/***********************************************************************************************
 * SidebarClass::Activate_Demolish -- Controls the demolish button on the sidebar.             *
 *                                                                                             *
 *    Use this routine to turn the demolish/dismantle sidebar button on and off. Typically,    *
 *    the button is enabled when a friendly building is selected and disabled otherwise.       *
 *                                                                                             *
 * INPUT:   control  -- The controls how the button is to be activated or deactivated;         *
 *                      0  -- Turn button off.                                                 *
 *                      1  -- Turn button on.                                                  *
 *                      -1 -- Toggle button state.                                             *
 *                                                                                             *
 * OUTPUT:  bool; Was the button previously activated?                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SidebarClass::Activate_Demolish(int control)
{
	bool old = IsDemolishActive;

	if (control == -1) {
		control = IsDemolishActive ? 0 : 1;
	}
	switch (control) {
		case 1:
			IsDemolishActive = true;
			break;

		default:
		case 0:
			IsDemolishActive = false;
			break;
	}
	if (old != IsDemolishActive) {
		Flag_To_Redraw();
		IsToRedraw = true;
		if (!IsDemolishActive) {
			Set_Default_Mouse(MOUSE_NORMAL, false);
		}
	}
	return(old);
}


/***********************************************************************************************
 * SidebarClass::Add -- Adds a game object to the sidebar list.                                *
 *                                                                                             *
 *    This routine is used to add a game object to the sidebar. Call this routine when a       *
 *    factory type building is created. It handles the case of adding an item that has already *
 *    been added -- it just ignores it.                                                        *
 *                                                                                             *
 * INPUT:   object   -- Pointer to the object that is being added.                             *
 *                                                                                             *
 * OUTPUT:  bool; Was the object added to the sidebar?                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/17/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SidebarClass::Add(RTTIType type, int id)
{
	assert((unsigned)type < RTTI_COUNT);

	/*
	**	Add the sidebar only if we're not in editor mode.
	*/
	if (!Debug_Map) {
		int column = Which_Column(type, id);

		bool empty = true;
		for (int index = 0; index < COLUMNS; index++) {
			if (Column[index].BuildableCount > 0) {
				empty = false;
			}
		}

		if (Column[column].Add(type, id)) {
			Activate(1);

			// In the classic sidebar the first cameo added shows its tab, as gamemd's AddCameo (0x6A6300) does.
			if (empty && !Modern_Interface()) {
				Set_Tab(column);
			}
			IsToRedraw = true;
			Flag_To_Redraw();
			return(true);
		}
		return(false);
	}

	return(false);
}


/***********************************************************************************************
 * SidebarClass::Scroll -- Handles scrolling the sidebar object strip.                         *
 *                                                                                             *
 *    This routine is used to scroll the sidebar strip of objects. The strip appears whenever  *
 *    a building is selected that can produce units. If the number of units to produce is      *
 *    greater than what the sidebar can hold, this routine is used to scroll the other object  *
 *    into view so they can be selected.                                                       *
 *                                                                                             *
 * INPUT:   up -- Should the scroll be upwards? Upward scrolling reveals object that are       *
 *                later in the list of objects.                                                *
 *                                                                                             *
 * OUTPUT:  bool; Did scrolling occur?                                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/28/94   JLB : Created.                                                                 *
 *=============================================================================================*/
bool SidebarClass::Scroll(bool up, int column)
{
	if (UIShell.Screen_Shown()) {
		return(false);
	}

	if (column == -1) {
		bool scr = false;
		if (Column[ActiveTab].Scroll(up)) {
			scr = true;
		}
		if (scr) {
			IsToRedraw = true;
			Flag_To_Redraw();
			return(true);
		}
		if (!scr) {
			Sound_Effect(Rule->ScoldSound);
		}
		return(false);
	}

	// With one strip shown at a time, a command naming a strip moves the shown tab's.
	if (Column[ActiveTab].Scroll(up)) {
		// No need to redraw the whole sidebar juts because we scrolled a strip is there? ST - 10/15/96 7:29PM
		//IsToRedraw = true;
		Flag_To_Redraw();
		return(true);
	}

	return(false);
}


/// <summary>
/// Handles paging the sidebar object strips.
/// This routine works just like Scroll, save that the strips move a whole screenful of
/// objects at a time. The player is scolded with a sound effect when there is nothing left
/// in that direction to page to.
/// </summary>
/// <param name="up">Should the paging be upwards?</param>
/// <param name="column">Ignored unless -1, which also scolds when the shown strip cannot page.</param>
/// <returns>bool; Did any paging occur?</returns>
bool SidebarClass::Page(bool up, int column)
{
	if (column == -1) {
		bool scr = false;
		if (Column[ActiveTab].Page(up)) {
			scr = true;
		}
		if (scr) {
			IsToRedraw = true;
			Flag_To_Redraw();
			return(true);
		}
		if (!scr) {
			Sound_Effect(Rule->ScoldSound);
		}
		return(false);
	}

	if (Column[ActiveTab].Page(up)) {
		// No need to redraw the whole sidebar juts because we scrolled a strip is there? ST - 10/15/96 7:29PM
		//IsToRedraw = true;
		Flag_To_Redraw();
		return(true);
	}
	return(false);
}


/***********************************************************************************************
 * SidebarClass::Draw_It -- Renders the sidebar display.                                       *
 *                                                                                             *
 *    This routine performs the actual drawing of the sidebar display.                         *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  bool; Was the sidebar imagery changed at all?                                      *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/28/94   JLB : Created.                                                                 *
 *   12/31/1994 JLB : Split rendering off into the sidebar strip class.                        *
 *=============================================================================================*/
void SidebarClass::Draw_It(bool complete)
{
	complete = complete || IsForceCompleteRedraw;
	Map.LastDrawRect = RECT_NONE;

	// The power bar is drawn after the body below, as gamemd's SidebarClass::Draw (0x6A6C30) draws it
	// last, so a repaint of the body cannot cover the bar.
	RadarClass::Draw_It(complete);

	BStart(BENCH_SIDEBAR);

	Surface * old = LogicalSurface;
	LogicalSurface = SidebarSurface;

	Rect window(0, 0, SidebarSurface->Get_Width(), SidebarSurface->Get_Height());

	bool const body_drawn = IsSidebarActive && (IsToRedraw || complete) && !Debug_Map;
	if (body_drawn) {
		if (Modern_Interface() && (complete || Column[ActiveTab].IsToRedraw)) {
			Draw_Build_Area();
			Column[ActiveTab].IsToRedraw = true;
		} else if (complete || Column[ActiveTab].IsToRedraw) {

			int y = SidebarRect.Y;

			/*
			**	The sidebar shape is too big in 640x400 so it needs to be drawn in three chunks.
			*/
			Draw_Shape(*SidebarSurface, *SidebarDrawer, SidebarShape, 0, Point2D(0, y), window, SHAPE_WIN_REL);
			y = SidebarRect.Y + GRID_Y;

			for (int i = 0; i < Max_Visible() / StripClass::SLOT_COLUMNS; i++) {
				Draw_Shape(*SidebarSurface, *SidebarDrawer, SidebarMiddleShape, 0, Point2D(0, y), window, SHAPE_WIN_REL);
				y += SidebarMiddleShape->Get_Height();
			}

			Draw_Shape(*SidebarSurface, *SidebarDrawer, SidebarBottomShape, 0, Point2D(0, y), window, SHAPE_WIN_REL);
			Draw_Shape(*SidebarSurface, *SidebarDrawer, SidebarAddonShape, 0, Point2D(0, y + SidebarBottomShape->Get_Height()), window, SHAPE_WIN_REL);

			Column[ActiveTab].IsToRedraw = true;
		}

		Repair.Draw_Me(true);
		Upgrade.Draw_Me(true);
		if (Modern_Interface()) {
			Power.Draw_Me(true);
			BeaconButton.Draw_Me(true);
			OptionsButton.Draw_Me(true);
		}
		for (int index = 0; index < COLUMNS; index++) {
			TabButton[index].Draw_Me(true);
		}
		IsToBlitSidebar = true;
	}

	/*
	**	Draw the side strip elements by calling their respective draw functions.
	*/
	if (IsSidebarActive) {
		Column[ActiveTab].Draw_It(complete);
	}
	Draw_Power_Bar(complete || body_drawn);
	if (Repair.IsDrawn) {
		IsToBlitSidebar = true;
		Repair.IsDrawn = false;
	}
	if (Upgrade.IsDrawn) {
		IsToBlitSidebar = true;
		Upgrade.IsDrawn = false;
	}
	if (Power.IsDrawn) {
		IsToBlitSidebar = true;
		Power.IsDrawn = false;
	}
	if (Waypoint.IsDrawn) {
		IsToBlitSidebar = true;
		Waypoint.IsDrawn = false;
	}
	if (ToolTips != NULL) {
		ToolTips->Draw_Current(true);
	}
	IsToRedraw = false;
	IsForceCompleteRedraw = false;

	Blit_Sidebar(complete);
	LogicalSurface = old;

	BEnd(BENCH_SIDEBAR);
}


/// <summary>
/// Draws the modern build panel's backdrop under the tabs and the cameo slots. The radar, the
/// power bar and the credits keep what they drew.
/// </summary>
void SidebarClass::Draw_Build_Area(void)
{
	// Init_For_House can run before the player's side is known.
	if (Hud_Icon("T0") != HudIconSet) {
		Use_Hud_Icons();
	}

	HudLayout const layout = Hud_Layout();
	int const left = layout.Tabs.X - 1;
	int const top = layout.Grid.Y - 2;
	int const bottom = layout.Arrows.Y + layout.Arrows.Height + 1;
	Draw_Hud_Fill(*SidebarSurface, Rect(left, top, layout.Panel.Width - 2 - left, bottom - top));

	for (int index = 0; index < COLUMNS; index++) {
		ShapeButtonClass & tab = TabButton[index];
		int outline = -1;
		if (index == ActiveTab) {
			outline = Hud_Side_Pixel();
		} else if ((TabFlashMask & (1u << index)) != 0) {
			outline = Hud_Flash_Pixel();
		}
		tab.IsCutout = !TabIcons;
		tab.OutlinePixel = TabIcons ? -1 : outline;
		if (TabIcons && outline != -1) {
			int const pixel = (index == ActiveTab) ? Hud_Active_Pixel() : outline;
			Draw_Hud_Ring(*SidebarSurface, Rect(tab.X + tab.DrawOffsetX, tab.Y + tab.DrawOffsetY, tab.Width, tab.Height), HUD_ICON_RADIUS, pixel, 2);
		}
	}

	for (int row = 0; row < layout.Rows; row++) {
		for (int column = 0; column < HUD_GRID_COLUMNS; column++) {
			Point2D const slot(layout.Grid.X + column * StripClass::Column_Step(), layout.Grid.Y + row * StripClass::OBJECT_HEIGHT);
			Draw_Hud_Well(*SidebarSurface, Rect(slot.X - 1, slot.Y - 1, StripClass::OBJECT_WIDTH - 2, StripClass::OBJECT_HEIGHT));
		}
	}
}


/// <summary>
/// Copies the build panel onto the frame at its place on the right, over the map, for the modern
/// interface. The panel's background lets the map show through at Options.SidebarOpacity.
/// </summary>
void SidebarClass::Draw_Panels(Surface & surface)
{
	if (IsSidebarActive && GameActive && ScenarioActive && SidebarSurface != NULL) {
		Rect const panel = Hud_Layout().Panel;
		Rect const source(0, 0, panel.Width, panel.Height);
		Blit_Hud_Panel(surface, panel, *SidebarSurface, source);
	}
	BASECLASS::Draw_Panels(surface);
}


/// <summary>
/// Copies the sidebar onto the visible screen.
/// This routine is the last step of the sidebar's redraw. When nothing but the credits have
/// changed, only that much is copied, so an idle sidebar costs almost nothing.
/// </summary>
/// <param name="complete">Should the entire sidebar be copied rather than just the portion
/// that changed?</param>
void SidebarClass::Blit_Sidebar(bool complete)
{
	// The modern interface copies the sidebar onto the frame in Draw_Panels instead.
	if (Modern_Interface()) {
		IsToBlitSidebar = false;
		return;
	}

	if (IsSidebarActive && GameActive && ScenarioActive) {

		if (!IsToBlitSidebar && !complete) {
			IsToBlitSidebar = false;
			if (Map.LastDrawRect == RECT_NONE) {
				if (IsToRedrawCredits) {
					VisibleSurface->Blit_From(
						Rect((Options.IsSidebarOnRight ? ScreenTacticalRect.Width : 0), 0, SIDE_WIDTH, CREDITS_HEIGHT),
						*SidebarSurface,
						Rect(0, 0, SIDE_WIDTH, CREDITS_HEIGHT),
						false,
						true
					);
					IsToRedrawCredits = false;
				}
				IsToBlitSidebar = false;
				return;
			}
		} else {
			IsToBlitSidebar = true;
		}

		if (Map.LastDrawRect == RECT_NONE && !complete) {
			VisibleSurface->Blit_From(Rect((Options.IsSidebarOnRight ? ScreenTacticalRect.Width : 0), 0, SIDE_WIDTH, CREDITS_HEIGHT), *SidebarSurface, Rect(0, 0, SIDE_WIDTH, CREDITS_HEIGHT));
			VisibleSurface->Blit_From(Rect((Options.IsSidebarOnRight ? ScreenTacticalRect.Width : 0), SIDE_BODY_Y, SIDE_WIDTH, SidebarSurface->Get_Height() - SIDE_BODY_Y), *SidebarSurface, Rect(0, SIDE_BODY_Y, SIDE_WIDTH, SidebarSurface->Get_Height() - SIDE_BODY_Y));
		} else if (!IsToBlitSidebar) {
			VisibleSurface->Blit_From(Rect(Map.LastDrawRect.X + (Options.IsSidebarOnRight ? ScreenTacticalRect.Width : 0), Map.LastDrawRect.Y, Map.LastDrawRect.Width, Map.LastDrawRect.Height), *SidebarSurface, Map.LastDrawRect);
		} else {
			Rect sb_rect = SidebarSurface->Get_Rect();
			VisibleSurface->Blit_From(Rect((Options.IsSidebarOnRight ? ScreenTacticalRect.Width : 0), 0, sb_rect.Width, sb_rect.Height), *SidebarSurface, Rect(0, 0, sb_rect.Width, sb_rect.Height));
		}
	}
	IsToBlitSidebar = false;
}


/***********************************************************************************************
 * SidebarClass::AI -- Handles player clicking on sidebar area.                                *
 *                                                                                             *
 *    This routine handles the processing necessary when the player clicks on the sidebar.     *
 *    Typically, this is selection of the item to build.                                       *
 *                                                                                             *
 * INPUT:   input -- Reference to the keyboard input value.                                    *
 *                                                                                             *
 *          x,y   -- Mouse coordinates at time of input.                                       *
 *                                                                                             *
 * OUTPUT:  bool; Was the click handled?                                                       *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/28/94   JLB : Created.                                                                 *
 *   11/11/1994 JLB : Processes input directly.                                                *
 *   12/26/1994 JLB : Uses factory manager class for construction handling.                    *
 *   12/31/1994 JLB : Simplified to use the sidebar strip class handlers.                      *
 *   12/31/1994 JLB : Uses mouse coordinate parameters.                                        *
 *   06/27/1995 JLB : <TAB> key toggles sidebar.                                               *
 *=============================================================================================*/
void SidebarClass::AI(KeyNumType & input, Point2D const & xy)
{
	Point2D xy_rel = xy - Point2D(480, 0);

	if (!Debug_Map) {
		Activate(1);	// Force the sidebar always on in Win95 mode
	}

	if (!Debug_Map) {
		for (int index = 0; index < COLUMNS; index++) {
			Column[index].AI(input, xy_rel);
		}
	}

	Fit_Panel_To_Strip();

	// A classic tab with no items is disabled, as gamemd does when its last item goes (SidebarClass::RebuildScrollableStripList, 0x6AA600).
	if (!Modern_Interface()) {
		for (int index = 0; index < SidebarClass::COLUMNS; index++) {
			bool const enabled = Column[index].BuildableCount > 0;
			if (enabled == TabDisabled[index]) {
				if (enabled) {
					TabButton[index].Enable();
				} else {
					TabButton[index].Disable();
				}
				TabDisabled[index] = !enabled;
				TabButton[index].Flag_To_Redraw();
				Flag_To_Redraw();
			}
		}

		// The arrows of a classic strip are enabled only when it holds more cameos than fit, as gamemd's UpdateScrollButtons (0x6A6610) decides.
		int const visible = Max_Visible();
		for (int index = 0; index < SidebarClass::COLUMNS; index++) {
			bool const scrollable = Column[index].BuildableCount > visible;
			if (scrollable == ArrowsDisabled[index]) {
				if (scrollable) {
					StripClass::UpButton[index].Enable();
					StripClass::DownButton[index].Enable();
				} else {
					StripClass::UpButton[index].Disable();
					StripClass::DownButton[index].Disable();
				}
				ArrowsDisabled[index] = !scrollable;
				StripClass::UpButton[index].Flag_To_Redraw();
				StripClass::DownButton[index].Flag_To_Redraw();
				Flag_To_Redraw();
			}
		}
	}

	if (Modern_Interface()) {
		Flash_Ready_Tabs();
		for (ShapeButtonClass * button : {&OptionsButton, &DiplomacyButton, &Repair, &Upgrade, &Power, &BeaconButton}) {
			button->Track_Hover(xy);
		}
		for (int index = 0; index < COLUMNS; index++) {
			TabButton[index].Track_Hover(xy);
			StripClass::UpButton[index].Track_Hover(xy);
			StripClass::DownButton[index].Track_Hover(xy);
		}
	}

	if (IsSidebarActive) {

		/*
		**	If there are any buildings in the payer's inventory, then allow the repair
		**	option.
		*/
		if (PlayerPtr != NULL && PlayerPtr->CurBuildings > 0) {
			Activate_Repair(true);
		} else {
			Activate_Repair(false);
		}

		if (input == (BUTTON_REPAIR|KN_BUTTON)) {
			if (!Modern_Interface()) Sound_Effect(Rule->GUIMainButtonSound);
			Repair_Mode_Control(-1);
		}

		// Toggles beacon placement, as the PlaceBeacon command does.
		if (input == (BUTTON_BEACON|KN_BUTTON)) {
			BeaconButton.IsPressed = false;
			input = KN_NONE;
			Place_Beacon_Mode_Control(-1);
		}

		if (input == (BUTTON_POWER|KN_BUTTON)) {
			Power_Mode_Control(-1);
		}

		if (input == (BUTTON_WAYPOINT|KN_BUTTON)) {
			Waypoint_Mode_Control(-1, false);
		}

		if (input == (BUTTON_UPGRADE|KN_BUTTON)) {
			if (!Modern_Interface()) Sound_Effect(Rule->GUIMainButtonSound);
			Sell_Mode_Control(-1);
		}

		for (int index = 0; index < COLUMNS; index++) {
			if (input == KeyNumType((BUTTON_TAB + index)|KN_BUTTON)) {
				TabButton[index].IsPressed = false;
				if (!Modern_Interface()) Sound_Effect(Rule->GUITabSound);
				Set_Tab(index);
			}
		}

		if (input == (BUTTON_OPTIONS|KN_BUTTON)) {
			OptionsButton.IsPressed = false;
			input = KN_NONE;
			Execute_Command("Options");
		}

		// gamemd's 0xF2 opens the mission briefing in a campaign and in a Cooperative match against
		// other machines (RadarClass::Update, 0x653850). Other games open diplomacy, which OpenTS does
		// not have yet, so the button does nothing there.
		if (input == (BUTTON_DIPLOMACY|KN_BUTTON)) {
			DiplomacyButton.IsPressed = false;
			input = KN_NONE;
			if (Session.Type == GAME_NORMAL || MPMode_Is_Cooperative_Match()) {
				SpecialDialog = SDLG_BRIEFING;
			}
		}
	}

	// Each mode button is lit while its mode is on, however the mode was started.
	auto light = [](ShapeButtonClass & button, bool lit) {
		if (button.IsOn != lit) {
			if (lit) {
				button.Turn_On();
			} else {
				button.Turn_Off();
			}
		}
	};
	light(Repair, IsRepairMode);
	light(Upgrade, IsSellMode);
	light(Power, IsPowerMode);
	light(Waypoint, IsWaypointMode);
	light(BeaconButton, IsPlaceBeaconMode);

	BASECLASS::AI(input, xy);
}


/***********************************************************************************************
 * SidebarClass::Recalc -- Examines the sidebar data and updates it as necessary.              *
 *                                                                                             *
 *    Occasionally a factory gets destroyed. This routine must be called in such a case        *
 *    because it might be possible that sidebar object need to be removed. This routine will   *
 *    examine all existing objects in the sidebar class and if no possible factory can         *
 *    produce it, then it will be removed.                                                     *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   This routine is exhaustive and thus time consuming. Only call it when really    *
 *             necessary. Such as when a factory is destroyed rather than when a non-factory   *
 *             is destroyed.                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   11/30/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::Recalc(void)
{
	bool redraw = false;

	for (int index = 0; index < COLUMNS; index++) {
		int const before = Column[index].BuildableCount;
		if (Column[index].Recalc()) {
			redraw = true;
		}

		// A classic strip that its recheck empties hands the shown tab to the first tab with cameos, as gamemd's RebuildScrollableStripList (0x6AA600) does.
		if (!Modern_Interface() && before > 0 && Column[index].BuildableCount == 0) {
			for (int shown = 0; shown < COLUMNS; shown++) {
				if (Column[shown].BuildableCount > 0) {
					Set_Tab(shown);
					break;
				}
			}
		}
	}
	if (redraw) {
		IsToRedraw = true;
		Flag_To_Redraw();
	}
}


/***********************************************************************************************
 * SidebarClass::Activate -- Controls the sidebar activation.                                  *
 *                                                                                             *
 *    Use this routine to turn the sidebar on or off. This routine handles updating the        *
 *    necessary flags.                                                                         *
 *                                                                                             *
 * INPUT:   control  -- Tells what to do with the sidebar according to the following:          *
 *                         0 = Turn sidebar off.                                               *
 *                         1 = Turn sidebar on.                                                *
 *                         -1= Toggle sidebar on or off.                                       *
 *                                                                                             *
 * OUTPUT:  bool; Was the sidebar already on?                                                  *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/09/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SidebarClass::Activate(int control)
{
	bool old = IsSidebarActive;

	if (Session.Play && Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH)
		return(old);

	/*
	**	Determine the new state of the sidebar.
	*/
	switch (control) {
		case -1:
			IsSidebarActive = IsSidebarActive == false;
			break;

		case 1:
			IsSidebarActive = true;
			break;

		default:
		case 0:
			IsSidebarActive = false;
			break;
	}

	/*
	**	Only if there is a change in the state of the sidebar will anything
	**	be done to change it.
	*/
	if (IsSidebarActive != old) {

		/*
		**	If the sidebar is activated but was on the right side of the screen, then
		**	activate it on the left side of the screen.
		*/
		if (IsSidebarActive) {
			Reposition_Sidebar();
			IsToRedraw = true;
			Repair.Zap();
			Add_A_Button(Repair);
			Upgrade.Zap();
			Add_A_Button(Upgrade);
			for (int index = 0; index < COLUMNS; index++) {
				TabButton[index].Zap();
				Add_A_Button(TabButton[index]);
			}
			OptionsButton.Zap();
			Add_A_Button(OptionsButton);
			if (Modern_Interface()) {
				Power.Zap();
				Add_A_Button(Power);
				BeaconButton.Zap();
				Add_A_Button(BeaconButton);
			} else {
				DiplomacyButton.Zap();
				Add_A_Button(DiplomacyButton);
			}
			Column[ActiveTab].Activate();
			Background.Zap();
			Add_A_Button(Background);
			RadarButton.Zap();
			Add_A_Button(RadarButton);
		} else {
			Stop_Ingame_Movie();
			Remove_A_Button(Repair);
			Remove_A_Button(Upgrade);
			for (int index = 0; index < COLUMNS; index++) {
				Remove_A_Button(TabButton[index]);
			}
			Remove_A_Button(OptionsButton);
			if (Modern_Interface()) {
				Remove_A_Button(Power);
				Remove_A_Button(BeaconButton);
			} else {
				Remove_A_Button(DiplomacyButton);
			}
			Remove_A_Button(Background);
			Column[ActiveTab].Deactivate();
			Remove_A_Button(RadarButton);
		}

		/*
		**	Since the sidebar status has changed, update the map so that the graphics
		**	will be rendered correctly.
		*/
		Flag_To_Redraw(GS_REDRAW_ALL);
	}

	return(old);
}


/***********************************************************************************************
 * SidebarClass::StripClass::StripClass -- Default constructor for the side strip class.       *
 *                                                                                             *
 *    This constructor is used to reset the side strip to default empty state.                 *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
SidebarClass::StripClass::StripClass(InitClass const &) :
	X(0),
	Y(0),
	ObjectRect(RECT_NONE),
	ID(0),
	IsToRedraw(true),
	IsToSort(true),
	IsBuilding(false),
	IsScrollingDown(false),
	IsScrolling(false),
	Flasher(-1),
	TopIndex(0),
	Scroller(0),
	Slid(0),
	LastSlid(0),
	BuildableCount(0)
{
	for (int index = 0; index < MAX_BUILDABLES; index++) {
		Buildables[index].BuildableID = 0;
		Buildables[index].BuildableType = RTTI_NONE;
		Buildables[index].Factory = NULL;
	}
}


/***********************************************************************************************
 * SidebarClass::StripClass::One_Time -- Performs one time actions necessary for the side stri *
 *                                                                                             *
 *    Call this routine ONCE at the beginning of the game. It handles retrieving pointers to   *
 *    the shape files it needs for rendering.                                                  *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::StripClass::One_Time(int )
{
	DarkenShapes = (ShapeSet*)MFCD::Retrieve("DARKEN.SHP");
}


/***********************************************************************************************
 * SidebarClass::StripClass::Get_Special_Cameo -- Fetches the special event cameo shape.       *
 *                                                                                             *
 *    This routine will return with a pointer to the cameo data for the special objects that   *
 *    can appear on the sidebar (e.g., nuclear bomb).                                          *
 *                                                                                             *
 * INPUT:   type  -- The special type to fetch the cameo imagery for.                          *
 *                                                                                             *
 * OUTPUT:  Returns with a pointer to the cameo imagery for the specified special object.      *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/19/1995 JLB : commented                                                                *
 *=============================================================================================*/
ShapeSet const * SidebarClass::StripClass::Get_Special_Cameo(SuperWeaponType type)
{
	if ((unsigned)type < (unsigned)SuperWeaponTypes.Count()) {
		return(SuperWeaponTypes[type]->CameoData);
	}
	return(NULL);
}


/***********************************************************************************************
 * SidebarClass::StripClass::Init_Clear -- Sets sidebar to a known (and deactivated) state     *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::StripClass::Init_Clear(void)
{
	IsScrollingDown = false;
	IsScrolling = false;
	IsBuilding = false;
	IsToSort = true;
	Flasher = -1;
	TopIndex = 0;
	Slid = 0;
	BuildableCount = 0;

	/*
	**	Since we're resetting the strips, clear out all the buildables & factory pointers.
	*/
	for (int index = 0; index < MAX_BUILDABLES; index++) {
		Buildables[index].BuildableID = 0;
		Buildables[index].BuildableType = RTTI_NONE;
		Buildables[index].Factory = NULL;
	}
}


/***********************************************************************************************
 * SidebarClass::StripClass::Init_IO -- Initializes the strip's buttons                        *
 *                                                                                             *
 * This routine doesn't actually add any buttons to the list.                                  *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/24/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::StripClass::Init_IO(int id)
{
	ID = id;

	UpButton[ID].IsSticky = true;
	UpButton[ID].ID = BUTTON_UP+id;
	UpButton[ID].DrawOnSidebar = true;
	UpButton[ID].ShapeDrawer = SidebarDrawer;
	UpButton[ID].Set_Flags(GadgetClass::RIGHTRELEASE | GadgetClass::RIGHTPRESS | GadgetClass::LEFTRELEASE | GadgetClass::LEFTPRESS);

	DownButton[ID].IsSticky = true;
	DownButton[ID].ID = BUTTON_DOWN+id;
	DownButton[ID].DrawOnSidebar = true;
	DownButton[ID].ShapeDrawer = SidebarDrawer;
	DownButton[ID].Set_Flags(GadgetClass::RIGHTRELEASE | GadgetClass::RIGHTPRESS | GadgetClass::LEFTRELEASE | GadgetClass::LEFTPRESS);

	for (int index = 0; index < Map.Max_Visible(); index++) {
		SelectClass & g = SelectButton[ID][index];
		g.ID = BUTTON_SELECT;
		g.X = SidebarRect.X + Map.Column[ID].X + (index % Slot_Columns()) * Column_Step();
		g.Y = SidebarRect.Y + Map.Column[ID].Y + (OBJECT_HEIGHT * (index / Slot_Columns()));
		g.Width = OBJECT_WIDTH;
		g.Height = OBJECT_HEIGHT;
		g.Set_Owner(*this, index);
	}

}


/***********************************************************************************************
 * SidebarClass::StripClass::Activate -- Adds the strip buttons to the input system.           *
 *                                                                                             *
 *    This routine will add the side strip buttons to the map's input system. This routine     *
 *    should be called once when the sidebar activates.                                        *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Never call this routine a second time without first calling Deactivate().       *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::StripClass::Activate(void)
{
	UpButton[ID].Zap();
	Map.Add_A_Button(UpButton[ID]);

	DownButton[ID].Zap();
	Map.Add_A_Button(DownButton[ID]);

	Registered_Slots = Map.Max_Visible();
	for (int index = 0; index < Registered_Slots; index++) {
		SelectButton[ID][index].Zap();
		Map.Add_A_Button(SelectButton[ID][index]);
	}
}


/***********************************************************************************************
 * SidebarClass::StripClass::Deactivate -- Removes the side strip buttons from the input syste *
 *                                                                                             *
 *    Call this routine to remove all the buttons on the side strip from the map's input       *
 *    system.                                                                                  *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Never call this routine unless the Activate() function was previously called.   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::StripClass::Deactivate(void)
{
	Map.Remove_A_Button(UpButton[ID]);
	Map.Remove_A_Button(DownButton[ID]);
	for (int index = 0; index < Registered_Slots; index++) {
		Map.Remove_A_Button(SelectButton[ID][index]);
	}
}


/***********************************************************************************************
 * SidebarClass::StripClass::Add -- Add an object to the side strip.                           *
 *                                                                                             *
 *    Use this routine to add a buildable object to the side strip.                            *
 *                                                                                             *
 * INPUT:   object   -- Pointer to the object type that can be built and is to be added to     *
 *                      the side strip.                                                        *
 *                                                                                             *
 * OUTPUT:  bool; Was the object successfully added to the side strip? Failure could be the    *
 *                result of running out of room in the side strip array or the object might    *
 *                already be in the list.                                                      *
 *                                                                                             *
 * WARNINGS:   none.                                                                           *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SidebarClass::StripClass::Add(RTTIType type, int id)
{
	int const capacity = Rule->IsLargeSidebarStrips ? MAX_BUILDABLES : STRIP_CAMEO_CAPACITY;
	if (BuildableCount < capacity) {
		for (int index = 0; index < BuildableCount; index++) {
			if (Buildables[index].BuildableType == type && Buildables[index].BuildableID == id) {
				return(false);
			}
		}
		if (!ScenarioInit && type != RTTI_SPECIAL) {
			Speak(VOX_NEW_CONSTRUCT);
		}
		BuildType const entry(id, type);
		int place = BuildableCount;

		// Yuri's Revenge inserts a new cameo before the first one it sorts before (0x6A8710).
		if (Options.SidebarSorting && !Rule->IsUseCameoSortOrder) {
			for (place = 0; place < BuildableCount; place++) {
				if (Classic_Sorts_Before(entry, Buildables[place])) {
					break;
				}
			}
			if (place < BuildableCount) {
				memmove(&Buildables[place+1], &Buildables[place], sizeof(Buildables[0]) * (BuildableCount - place));
			}
		}
		Buildables[place] = entry;
		BuildableCount++;
		IsToRedraw = true;
		IsToSort = true;
		return(true);
	}
	return(false);
}


/***********************************************************************************************
 * SidebarClass::StripClass::Scroll -- Causes the side strip to scroll.                        *
 *                                                                                             *
 *    Use this routine to flag the side strip to scroll. The direction scrolled is controlled  *
 *    by the parameter. Scrolling is merely initiated by this routine. Subsequent calls to     *
 *    the AI function and the Draw_It function are required to properly give the appearance    *
 *    of scrolling.                                                                            *
 *                                                                                             *
 * INPUT:   bool; Should the side strip scroll UP? If it is to scroll down then pass false.    *
 *                                                                                             *
 * OUTPUT:  bool; Was the side strip started to scroll in the desired direction?               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *   07/29/1995 JLB : Simplified scrolling logic.                                              *
 *=============================================================================================*/
bool SidebarClass::StripClass::Scroll(bool up)
{
	if (up) {
		if (!TopIndex) return(false);
		Scroller--;
	} else {
		if (TopIndex+Map.Max_Visible() >= BuildableCount) return(false);
		Scroller++;
	}
	return(true);
}


/// <summary>
/// The distance between the two cameos of a row: 63 pixels on the first side's sidebar and 64
/// on the others', as in Yuri's Revenge.
/// </summary>
int SidebarClass::StripClass::Column_Step(void)
{
	if (Modern_Interface()) {
		return(OBJECT_WIDTH);
	}
	return(PlayerPtr != NULL && PlayerPtr->Class->Side == SIDE_FIRST ? 63 : 64);
}


/// <summary>
/// The number of cameos side by side in each row: two on the classic sidebar, three on the
/// modern interface's build panel.
/// </summary>
int SidebarClass::StripClass::Slot_Columns(void)
{
	return(Modern_Interface() ? int(HUD_GRID_COLUMNS) : int(SLOT_COLUMNS));
}


/// <summary>
/// Causes the side strip to scroll by a whole page.
/// Use this routine to flag the side strip to move a full screenful of cameos rather than
/// the single one that Scroll would move. As with scrolling, this routine only starts the
/// motion; the AI and Draw_It routines carry it out.
/// </summary>
/// <param name="up">Should the side strip page UP? If it is to page down then pass false.</param>
/// <returns>bool; Was the side strip started to page in the desired direction?</returns>
bool SidebarClass::StripClass::Page(bool up)
{
	if (up) {
		if (!TopIndex) return(false);
		Scroller-=Map.Max_Visible() / Slot_Columns();
	} else {
		if (TopIndex+Map.Max_Visible() >= BuildableCount) return(false);
		Scroller+=Map.Max_Visible() / Slot_Columns();
	}
	return(true);
}


/***********************************************************************************************
 * SidebarClass::StripClass::Flag_To_Redraw -- Flags the sidebar strip to be redrawn.          *
 *                                                                                             *
 *    This utility routine is called when something changes on the sidebar and it must be      *
 *    reflected the next time drawing is performed.                                            *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/18/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::StripClass::Flag_To_Redraw(void)
{
	IsToRedraw = true;
	Map.Flag_To_Redraw();
}


/***********************************************************************************************
 * SidebarClass::StripClass::AI -- Input and AI processing for the side strip.                 *
 *                                                                                             *
 *    The side strip AI processing is performed by this function. This function not only       *
 *    checks for player input, but also handles any graphic logic updating necessary as a      *
 *    result of flashing or construction animation.                                            *
 *                                                                                             *
 * INPUT:   input -- The player input code.                                                    *
 *                                                                                             *
 *          x,y   -- Mouse coordinate to use.                                                  *
 *                                                                                             *
 * OUTPUT:  bool; Did the AI detect that it will need a rendering change? If this routine      *
 *                returns true, then the Draw_It function should be called at the              *
 *                earliest opportunity.                                                        *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *   12/31/1994 JLB : Uses mouse coordinate parameters.                                        *
 *=============================================================================================*/
bool SidebarClass::StripClass::AI(KeyNumType & input, Point2D const & xy)
{
	KeyNumType key = KeyNumType(input & ~KN_UNK);
	bool redraw = false;

	// Ordering waits out a slide so that the strip never moves under a scroll in progress.
	if (IsToSort && !IsScrolling) {
		if (Options.SidebarSorting) {
			Sort();
			redraw = true;
		} else {
			IsToSort = false;
		}
	}

	/*
	**	If this is scroll button for this side strip, then scroll the strip as
	**	indicated.
	*/
	// Both arrows page the strip by a screenful, as the classic sidebar's do (SidebarClass::Update, 0x6A7780).
	// The modern panel's screenful is its row count, so a click moves it by the rows it shows.
	if (key == KeyNumType(UpButton[ID].ID|KN_BUTTON)) {
		UpButton[ID].IsPressed = false;
		if (Modern_Interface()) {
			if (!Page(true)) {
				Sound_Effect(Rule->ScoldSound);
			}
		} else if (Page(true)) {
			Sound_Effect(Rule->GUITabSound);
		} else {
			Sound_Effect(Rule->ScoldSound);
		}
	}
	else if (key == KeyNumType(DownButton[ID].ID|KN_BUTTON)) {
		DownButton[ID].IsPressed = false;
		if (Modern_Interface()) {
			if (!Page(false)) {
				Sound_Effect(Rule->ScoldSound);
			}
		} else if (Page(false)) {
			Sound_Effect(Rule->GUITabSound);
		} else {
			Sound_Effect(Rule->ScoldSound);
		}
	}

	/*
	**	Reflect the scroll desired direction/value into the scroll
	**	logic handler. This might result in up or down scrolling.
	*/
	if (!IsScrolling && Scroller) {
		if (BuildableCount <= Map.Max_Visible()) {
			Scroller = 0;
		} else {

			/*
			**	Top of list is moving toward lower ordered entries in the object list. It looks like
			**	the "window" to the object list is moving up even though the actual object images are
			**	scrolling downward.
			*/
			if (Scroller < 0) {
				if (!TopIndex) {
					Scroller = 0;
				} else {
					Scroller++;
					IsScrollingDown = false;
					IsScrolling = true;
					TopIndex = std::max(0, TopIndex - Slot_Columns());
					Slid = 0;
				}

			} else {
				if (TopIndex+Map.Max_Visible() >= BuildableCount) {
					Scroller = 0;
				} else {
					Scroller--;
					Slid = OBJECT_HEIGHT;
					IsScrollingDown = true;
					IsScrolling = true;
				}
			}
		}
	}

	/*
	**	Scroll logic is handled here.
	*/
	if (IsScrolling) {
		if (IsScrollingDown) {
			Slid -= SCROLL_RATE;
			if (Slid <= 0) {
				IsScrolling = false;
				Slid = 0;
				TopIndex += Slot_Columns();
			}
		} else {
			Slid += SCROLL_RATE;
			if (Slid >= OBJECT_HEIGHT) {
				IsScrolling = false;
				Slid = 0;
			}
		}
		redraw = true;
	}

	/*
	**	A cameo a trigger action set flashing blinks every few frames until its time is up.
	*/
	if (!CameoFlashes.empty()) {
		CameoFlashes.erase(std::remove_if(CameoFlashes.begin(), CameoFlashes.end(),
			[](CameoFlashType const & flash) {return(Frame >= flash.EndFrame);}), CameoFlashes.end());
		if (Frame % CAMEO_FLASH_RATE == 0 || CameoFlashes.empty()) {
			redraw = true;
		}
	}

	/*
	**	Handle any flashing logic. Flashing occurs when the player selects an object
	**	and provides the visual feedback of a recognized and legal selection.
	*/
	if (Flasher != -1) {
		if (Graphic_Logic()) {
			redraw = true;
			if (Fetch_Stage() >= 7) {
				Set_Rate(0);
				Set_Stage(0);
				Flasher = -1;
			}
		}
	}

	/*
	**	Handle any building clock animation logic.
	*/
	if (IsBuilding) {
		for (int index = 0; index < BuildableCount; index++) {
			FactoryClass * factory = Buildables[index].Factory;

			if (factory && factory->Has_Changed()) {
				redraw = true;
				if (factory->Has_Completed()) {

					/*
					**	Construction has been completed. Announce this fact to the player and
					**	try to get the object to automatically leave the factory. Buildings are
					**	the main exception to the ability to leave the factory under their own
					**	power.
					*/
					TechnoClass * pending = factory->Get_Object();
					if (pending != NULL) {
						switch ((RTTIType)pending->RTTI) {
							case RTTI_UNIT:
							case RTTI_AIRCRAFT:
								OutList.push_back(EventClass(pending->Owner(), EventClass::PLACE, pending->RTTI, pending->Class_Of()->Fetch_Heap_ID(), CELL_NONE));
								Speak(VOX_UNIT_READY);
								break;

							case RTTI_BUILDING:
								Map.Note_Completed_Building((BuildingClass *)pending);
								Speak(VOX_CONSTRUCTION);
								break;

							case RTTI_INFANTRY:
								OutList.push_back(EventClass(pending->Owner(), EventClass::PLACE, pending->RTTI, pending->Class_Of()->Fetch_Heap_ID(), CELL_NONE));
								Speak(VOX_UNIT_READY);
								break;
						}
					}
				}
			}
		}
	}

	/*
	**	If any of the logic determined that this side strip needs to be redrawn, then
	**	set the redraw flag for this side strip.
	*/
	if (redraw) {
		Flag_To_Redraw();
		IsToBlitSidebar = true;
	}

	return(redraw);
}


/// <summary>
/// Fetches the tooltip text for one of the strip's cameos.
/// This routine composes the name and cost line that appears when the mouse lingers over a
/// build cameo. Only the cost is given when the player has cameo text turned on, since the
/// name is already printed beneath the cameo itself.
/// </summary>
/// <param name="id">The cameo slot, counted from the top of the strip as it is displayed.</param>
/// <returns>Returns with a pointer to the help text, or NULL if that slot is empty.</returns>
char const * SidebarClass::StripClass::Help_Text(int id)
{
	static char _buffer[84];

	int i = id + TopIndex;

	if (GameActive) {
		if (i < BuildableCount) {
			if (Buildables[i].BuildableType == RTTI_SPECIAL) {
				return(SuperWeaponTypes[Buildables[i].BuildableID]->Full_Name());
			}

			TechnoTypeClass const * choice = Fetch_Techno_Type(Buildables[i].BuildableType, Buildables[i].BuildableID);
			if (choice == NULL) {
				return(NULL);
			}

			if (Modern_Interface()) {
				static std::string _text;
				_text = choice->Full_Name();
				_text += "\n$" + std::to_string(choice->Cost_Of(PlayerPtr));
				if (choice->RTTI == RTTI_BUILDINGTYPE) {
					int const power = static_cast<BuildingTypeClass const *>(choice)->Power;
					if (power != 0) {
						_text += "\n" + Panel_Text("GUI:Power", "Power") + (power > 0 ? " +" : " ") + std::to_string(power);
					}
				}
				return(_text.c_str());
			}

			if (Map.IsCameoText) {
				sprintf(_buffer, Fetch_String(TXT_MONEY_FORMAT_1), choice->Cost_Of(PlayerPtr));
			} else {
				sprintf(_buffer, Fetch_String(TXT_MONEY_FORMAT_2), choice->Full_Name(), choice->Cost_Of(PlayerPtr));
			}

			return(_buffer);
		}
	}

	return(NULL);
}


/***********************************************************************************************
 * SidebarClass::StripClass::Draw_It -- Render the sidebar display.                            *
 *                                                                                             *
 *    Use this routine to render the sidebar display. It checks to see if it needs to be       *
 *    redrawn and only redraw if necessary. If the "complete" parameter is true, then it       *
 *    will force redraw the entire strip.                                                      *
 *                                                                                             *
 * INPUT:   complete -- Should the redraw be forced? A force redraw will ignore the redraw     *
 *                      flag.                                                                  *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   12/31/1994 JLB : Created.                                                                 *
 *   08/06/1995 JLB : Handles multi factory tracking in same strip.                            *
 *=============================================================================================*/
void SidebarClass::StripClass::Draw_It(bool complete)
{
	if (IsToRedraw || complete) {
		IsToRedraw = false;
		IsToBlitSidebar = true;
		Rect cliprect = SidebarRect;
		cliprect.X = 0;

		/*
		**	Redraw the scroll buttons.
		*/
		UpButton[ID].Draw_Me(true);
		DownButton[ID].Draw_Me(true);

		/*
		**	Loop through all the buildable objects that are visible in the strip and render
		**	them. Their Y offset may be adjusted if the strip is in the process of scrolling.
		*/
		for (int i = 0; i < Map.Max_Visible() + (IsScrolling ? Slot_Columns() : 0); i++) {
			ShapeSet const * shapefile = NULL;

			int index = i+TopIndex;
			int x = X + (i % Slot_Columns()) * Column_Step();
			int y = Y + (i / Slot_Columns()) * OBJECT_HEIGHT;

			bool production = false;
			bool completed = false;
			int stage = 0;
			bool darken = false;
			FactoryClass * factory = NULL;
			char const * state = NULL;
			bool isready = false;

			/*
			**	If the strip is scrolling, then the offset is adjusted accordingly.
			*/
			if (IsScrolling) {
				y -= OBJECT_HEIGHT - Slid;
			}

			char const * name = NULL;
			TechnoTypeClass const * obj = NULL;
			Surface const * pcx = NULL;

			/*
			**	Fetch the shape number for the object type located at this current working
			**	slot. This shape pointer is used to draw the underlying graphic there.
			*/
			if (index < BuildableCount) {
				SuperWeaponType spc = SUPER_NONE;

				if (Buildables[index].BuildableType != RTTI_SPECIAL) {

					obj = Fetch_Techno_Type(Buildables[index].BuildableType, Buildables[index].BuildableID);
					if (obj != NULL) {

						name = obj->Full_Name();

						/*
						**	If there is already a factory producing this kind of object, then all
						**	objects of this type are displays in a disabled state.
						*/
						bool isbusy = false;
						if (obj->RTTI == RTTI_BUILDINGTYPE) {
							isbusy = (PlayerPtr->Fetch_Factory(Buildables[index].BuildableType, HouseClass::Build_Category(Buildables[index].BuildableType, Buildables[index].BuildableID), HouseClass::Is_Naval_Type(Buildables[index].BuildableType, Buildables[index].BuildableID)) != NULL);
						}

						if (obj->Who_Can_Build_Me(true, true, true, PlayerPtr) == NULL) {
							isbusy = true;
						}

						if (!isbusy && PlayerPtr->Can_Build(Fetch_Techno_Type(Buildables[index].BuildableType, Buildables[index].BuildableID), false, false) == -1) {
							isbusy = true;
						}

						shapefile = (ShapeSet const *)obj->Get_Cameo_Data();
						pcx = PCX_Cameo(obj->CameoPCX);
						factory = Buildables[index].Factory;
						if (factory != NULL) {
							production	= true;
							completed	= factory->Has_Completed();
							if (completed) {
								state = Fetch_String(TXT_READY);
							}
							stage		= factory->Completion();
							darken		= false;
						} else {
							production  = false;
						//							darken      = IsBuilding;

							/*
							**	Darken the imagery if a factory of a matching type is
							**	already busy.
							*/
							darken = isbusy;
						}
					} else {
						darken = false;
					}

				} else {

					spc = SuperWeaponType(Buildables[index].BuildableID);
					name = SuperWeaponTypes[spc]->Full_Name();
					shapefile = Get_Special_Cameo(spc);
					pcx = PCX_Cameo(SuperWeaponTypes[spc]->SidebarPCX);

					production = true;
					completed = PlayerPtr->SuperWeapon[spc]->Is_Charging() == false;
					isready = PlayerPtr->SuperWeapon[spc]->Can_Place();
					state = PlayerPtr->SuperWeapon[spc]->State_String();
					stage = PlayerPtr->SuperWeapon[spc]->Anim_Stage();
					darken = false;
				}

				if (obj != NULL || spc != SUPER_NONE) {
					/*
					**	If this item is flashing then take care of it.
					**
					*/
					// if (Flasher == index && (Fetch_Stage() & 0x01)) {
					// 	remapper = Map.FadingLight;
					// }

				} else {
					shapefile	= LogoShapes;
					// if (!darken) {
					// 	shapenum		= SB_BLANK;
					// }
				}
			} else {
				shapefile	= LogoShapes;
				production	= false;
			}

			/*
			**	Now that the shape of the object at the current working slot has been found,
			**	draw it and any graphic overlays as necessary.
			*/
			if (shapefile != LogoShapes) {

				if (pcx != NULL) {
					Draw_PCX_Cameo(*SidebarSurface, *pcx, Point2D(x, y), cliprect);
				} else if (shapefile != NULL) {
					Draw_Shape(*SidebarSurface, *CameoDrawer, shapefile, 0, Point2D(x, y), cliprect, ShapeFlags_Type(SHAPE_WIN_REL));
				}

				/*
				**	A cameo that a trigger action set flashing is brightened on every other beat.
				*/
				if (obj != NULL && Is_Cameo_Flashing(obj) && (Frame / CAMEO_FLASH_RATE) % 2 == 0) {
					Rect flashrect = Intersect(Rect(cliprect.X + x, cliprect.Y + y, OBJECT_WIDTH - 2, OBJECT_HEIGHT - 2), Intersect(cliprect, SidebarSurface->Get_Rect()));
					if (flashrect.Is_Valid()) {
						SidebarSurface->Fill_Rect_Trans(flashrect, RGBClass(255, 255, 255), 50);
					}
				}

				/*
				**	Darken this object because it cannot be produced or is otherwise
				**	unavailable.
				*/
				if (darken) {
					// The darkening shape is wider than a cameo, so the modern grid's narrower step clips it to the slot.
					Rect darkrect = cliprect;
					if (Modern_Interface()) {
						darkrect = Intersect(Rect(cliprect.X + x, cliprect.Y + y, OBJECT_WIDTH, OBJECT_HEIGHT), cliprect);
					}
					Draw_Shape(*SidebarSurface, *SidebarDrawer, DarkenShapes, 0, Point2D(cliprect.X + x - darkrect.X, cliprect.Y + y - darkrect.Y), darkrect, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_DARKEN));
				}
			}

			if (name != NULL) {
				Print_Cameo_Text(name, Point2D(x, y + CAMEO_TEXT_Y_OFFSET), cliprect, OBJECT_WIDTH-2);
			}

			bool hasqueuecount = false;
			if (obj != NULL) {
				FactoryClass * factory = PlayerPtr->Fetch_Factory(obj->RTTI, HouseClass::Build_Category(obj->RTTI, obj->Fetch_Heap_ID()), HouseClass::Is_Naval_Type(obj->RTTI, obj->Fetch_Heap_ID()));

				if (factory != NULL) {
					int total = factory->Total(obj);
					if (total > 1 || total > 0 && !factory->Is_Currently_Producing(obj)) {
						Fancy_Text_Print("%d", *SidebarSurface, cliprect, Point2D(x + QUEUE_COUNT_X_OFFSET, y + TEXT_Y_OFFSET), Fetch_Scheme_By_Name("LightGrey"), TBLACK, TextPrintType(TPF_RIGHT|TPF_FULLSHADOW|TPF_8POINT), total);
						hasqueuecount = true;
					}
				}
			}

			/*
			**	Draw the overlapping clock shape if this is object is being constructed.
			**	If the object is completed, then display "Ready" with no clock shape.
			*/
			if (production) {

				/*
				**	Display text showing that the object is ready to place.
				*/
				if (state != NULL) {
					Fancy_Text_Print(state, *SidebarSurface, cliprect, Point2D(x + TEXT_X_OFFSET, y + TEXT_Y_OFFSET), Fetch_Scheme_By_Name("LightBlue"), TBLACK, TextPrintType(TPF_CENTER|TPF_FULLSHADOW|TPF_8POINT));
				}

				if (!completed) {

					if (!isready) {
						Draw_Shape(*SidebarSurface, *SidebarDrawer, ClockShapes, stage + 1, Point2D(x, y), cliprect, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_TRANSLUCENT50));
					} else {
						Draw_Shape(*SidebarSurface, *SidebarDrawer, RechargeClockShapes, stage + 1, Point2D(x, y), cliprect, ShapeFlags_Type(SHAPE_WIN_REL|SHAPE_TRANSLUCENT50));
					}

					/*
					**	Display text showing that the construction is temporarily on hold.
					*/
					if (factory && !factory->Is_Building()) {
						if (!hasqueuecount) {
							Fancy_Text_Print(TXT_HOLD, *SidebarSurface, cliprect, Point2D(x + TEXT_X_OFFSET, y + TEXT_Y_OFFSET), Fetch_Scheme_By_Name("LightGrey"), TBLACK, TextPrintType(TPF_CENTER|TPF_FULLSHADOW|TPF_8POINT));
						} else {
							Fancy_Text_Print(TXT_HOLD, *SidebarSurface, cliprect, Point2D(x, y + TEXT_Y_OFFSET), Fetch_Scheme_By_Name("LightGrey"), TBLACK, TextPrintType(TPF_FULLSHADOW|TPF_8POINT));
						}
					}
				}
			}
			/*
			**	While the grid build modifier is held, each of the first cameos shows the grid key that starts it.
			*/
			if (IsGridLabelShown && i < Map.Max_Visible() && i < SIDEBAR_GRID_KEY_COUNT && index < BuildableCount) {
				char const letter[2] = { Sidebar_Grid_Letter(i), '\0' };
				Fancy_Text_Print(letter, *SidebarSurface, cliprect, Point2D(x + 3, y + 2), Fetch_Scheme_By_Name("Yellow"), TBLACK, TextPrintType(TPF_FULLSHADOW|TPF_8POINT));
			}
		}

		LastSlid = Slid;

	} else {

		if (UpButton[ID].IsDrawn) {
			IsToBlitSidebar = true;
			UpButton[ID].IsDrawn = false;
		}

		if (DownButton[ID].IsDrawn) {
			IsToBlitSidebar = true;
			DownButton[ID].IsDrawn = false;
		}
	}
}


/***********************************************************************************************
 * SidebarClass::StripClass::Recalc -- Revalidates the current sidebar list of objects.        *
 *                                                                                             *
 *    This routine will revalidate all the buildable objects in the sidebar. This routine      *
 *    comes in handy when a factory has been destroyed, and the sidebar needs to reflect any   *
 *    change that this requires. It checks every object to see if there is a factory available *
 *    that could produce it. If none can be found, then the object is removed from the         *
 *    sidebar.                                                                                 *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  bool; The sidebar has changed as a result of this call?                            *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *   06/26/1995 JLB : Doesn't collapse sidebar when buildables removed.                        *
 *=============================================================================================*/
bool SidebarClass::StripClass::Recalc(void)
{
	int ok;

	if (Debug_Map || !BuildableCount) {
		return(false);
	}

	bool scroll = false;
	int max_visible = Map.Max_Visible();
	BuildType * unshifted = new BuildType[max_visible];

	for (int i = 0; i < max_visible; i++) {
		if (i + TopIndex < BuildableCount) {
			unshifted[i] = Buildables[i + TopIndex];
		}
	}

	/*
	**	Sweep through all objects listed in the sidebar. If any of those object can
	**	not be created -- even in theory -- then they must be removed form the sidebar and
	**	any current production must be abandoned.
	*/
	bool redraw = false;
	for (int index = 0; index < BuildableCount; index++) {
		TechnoTypeClass const * tech = Fetch_Techno_Type(Buildables[index].BuildableType, Buildables[index].BuildableID);
		if (tech != NULL) {
			BuildingClass const * who = tech->Who_Can_Build_Me(true, false, false, PlayerPtr);
			// gamemd's sweep (0x6AA600) always applies every gate; RecheckPrerequisites=no skips the gates.
			ok = who != NULL && who->House->Can_Build(tech, !Rule->IsRecheckPrerequisites, true);
		} else {
			if ((unsigned)Buildables[index].BuildableID < (unsigned)PlayerPtr->SuperWeapon.Count()) {
				ok = PlayerPtr->SuperWeapon[Buildables[index].BuildableID]->Is_Present();
			} else {
				ok = false;
			}
		}

		if (!ok) {
			// This sweep runs for the local player alone, so the abandon travels as an event.
			if (Rule->IsRecheckPrerequisites && tech != NULL) {
				FactoryClass * fptr = PlayerPtr->Fetch_Factory(Buildables[index].BuildableType, HouseClass::Build_Category(Buildables[index].BuildableType, Buildables[index].BuildableID), HouseClass::Is_Naval_Type(Buildables[index].BuildableType, Buildables[index].BuildableID));
				int pending = (fptr != NULL) ? fptr->Total(tech) : 0;
				if (pending > 0) {
					OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::ABANDON_COUNT,
							Buildables[index].BuildableType, Buildables[index].BuildableID, pending));
				}
			}

			for (int i = 0; i < max_visible; i++) {
				if (unshifted[i] == Buildables[index]) {
					unshifted[i] = BuildType(0, RTTI_NONE);
				}
			}

			/*
			**	Removes this entry from the list.
			*/
			if (BuildableCount > 1 && index < BuildableCount-1) {
				memmove(&Buildables[index], &Buildables[index+1], sizeof(Buildables[0])*((BuildableCount-index)-1));
			}
			redraw = true;
			scroll = true;
			Buildables[BuildableCount-1].Factory = NULL;
			IsToRedraw = true;
			BuildableCount--;
			index--;
		}
	}

	if (scroll) {
		bool got_old = false;
		bool got_new = false;

		int oldpos;
		for (oldpos = 0; oldpos < max_visible; oldpos++) {
			if (unshifted[oldpos] != BuildType(0, RTTI_NONE)) {
				got_old = true;
				break;
			}
		}

		int newpos;
		if (got_old && BuildableCount != 0) {
			for (newpos = 0; newpos < BuildableCount; newpos++) {
				if (Buildables[newpos] == unshifted[oldpos]) {
					got_new = true;
					break;
				}
			}
		}

		if (got_old && got_new) {
			TopIndex = newpos - oldpos;
			TopIndex = std::max(0, std::min(TopIndex, BuildableCount - max_visible));
		} else {
			TopIndex = 0;
		}
	}

	delete[] unshifted;
	return(redraw);
}


/// <summary>
/// Puts this strip's entries into the order the sidebar specifies.
/// A strip that has been scrolled keeps the cameo on its top row there. The order is a
/// display concern only; membership and production are untouched.
/// </summary>
void SidebarClass::StripClass::Sort(void)
{
	IsToSort = false;

	if (BuildableCount < 2) {
		return;
	}

	BuildType const anchor = Buildables[std::max(0, std::min(TopIndex, BuildableCount - 1))];

	CameoSortType sorted[MAX_BUILDABLES];
	for (int index = 0; index < BuildableCount; index++) {
		sorted[index].Order = Cameo_Order(Buildables[index]);
		sorted[index].Entry = Buildables[index];
	}

	if (Rule->IsUseCameoSortOrder) {
		std::sort(&sorted[0], &sorted[BuildableCount], [](CameoSortType const & left, CameoSortType const & right) {
			return(left.Order < right.Order);
		});
	} else {
		// Ties keep their order, so a strip that Add already ordered is left as it is.
		std::stable_sort(&sorted[0], &sorted[BuildableCount], [](CameoSortType const & left, CameoSortType const & right) {
			return(Classic_Sorts_Before(left.Entry, right.Entry) && !Classic_Sorts_Before(right.Entry, left.Entry));
		});
	}

	bool moved = false;
	for (int index = 0; index < BuildableCount; index++) {
		if (Buildables[index] != sorted[index].Entry) {
			moved = true;
		}
		Buildables[index] = sorted[index].Entry;
	}

	if (!moved) {
		return;
	}

	// A strip resting at its top stays there rather than following what was on that row.
	if (TopIndex > 0) {
		for (int index = 0; index < BuildableCount; index++) {
			if (Buildables[index] == anchor) {
				TopIndex = index;
				break;
			}
		}
	}
	TopIndex = std::max(0, std::min(TopIndex, BuildableCount - Map.Max_Visible()));

	// A tooltip on screen holds a copy of its text and outlives the cameo it describes.
	if (ToolTips != NULL) {
		ToolTips->Reset_Current();
	}

	Flag_To_Redraw();
}


/***********************************************************************************************
 * SidebarClass::StripClass::SelectClass::SelectClass -- Default constructor.                  *
 *                                                                                             *
 *    This is the default constructor for the button that controls the buildable cameos on     *
 *    the sidebar strip.                                                                       *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   The coordinates are set to zero by this routine. They must be set to the        *
 *             correct values before this button will function.                                *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
SidebarClass::StripClass::SelectClass::SelectClass(void) :
	ControlClass(0, 0, 0, OBJECT_WIDTH-1, OBJECT_HEIGHT, LEFTPRESS|RIGHTPRESS|LEFTUP),
	Strip(0),
	Index(0)
{
}


/***********************************************************************************************
 * SidebarClass::StripClass::SelectClass:: -- Assigns special values to a buildable select but *
 *                                                                                             *
 *    Use this routine to set custom buildable vars for this particular select button. It      *
 *    uses this information to properly know what buildable object to start or stop production *
 *    on.                                                                                      *
 *                                                                                             *
 * INPUT:   strip    -- Reference to the strip that owns this buildable button.                *
 *                                                                                             *
 *          index    -- The index (0 .. MAX_VISIBLE-1) of this button. This is used to let     *
 *                      the owning strip know what index this button refers to.                *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::StripClass::SelectClass::Set_Owner(StripClass & strip, int index)
{
	Strip = &strip;
	Index = index;
}


/// <summary>
/// Runs the left press the cameo takes from a click, for the grid build keys to start it the same way.
/// </summary>
void SidebarClass::StripClass::SelectClass::Press_Left(void)
{
	KeyNumType key = KN_NONE;
	Action(LEFTPRESS, key);
}


/// <summary>
/// Runs the right press the cameo takes from a click, without a mouse event.
/// </summary>
void SidebarClass::StripClass::SelectClass::Press_Right(void)
{
	KeyNumType key = KN_NONE;
	Action(RIGHTPRESS, key);
}


/// <summary>
/// Returns how many PRODUCE events a left click on a unit or infantry cameo sends. Shift held in the modern
/// interface sends Options.ShiftQueueCount of them, limited to the places left in the factory's queue.
/// </summary>
/// <param name="factory">The factory that builds the type, or NULL when none is making anything yet.</param>
/// <returns>Returns the number of events to send, never less than one.</returns>
static int Cameo_Produce_Count(FactoryClass const * factory)
{
	if (!Modern_Interface() || Options.ShiftQueueCount <= 1 || !Keyboard->Down(KN_LSHIFT)) {
		return(1);
	}

	// A factory holds one object in production and up to MaximumQueuedObjects behind it.
	int room = Rule->MaximumQueuedObjects + 1;
	if (factory != NULL) {
		room = Rule->MaximumQueuedObjects - factory->Queued_Count() + (factory->Get_Object() == NULL ? 1 : 0);
	}
	return(std::max(1, std::min(Options.ShiftQueueCount, room)));
}


/// <summary>
/// Sends the abandon for a cameo's type. Shift held abandons every object of that type, queued or in
/// production, in one event; without Shift it abandons one.
/// </summary>
static void Cameo_Abandon(RTTIType otype, int oid, TechnoTypeClass const * choice)
{
	if (Keyboard->Down(KN_LSHIFT)) {
		FactoryClass * fptr = PlayerPtr->Fetch_Factory(otype, HouseClass::Build_Category(otype, oid), HouseClass::Is_Naval_Type(otype, oid));
		int const pending = (fptr != NULL) ? fptr->Total(choice) : 0;
		if (pending > 1) {
			OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::ABANDON_COUNT, otype, oid, pending));
			return;
		}
	}
	OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::ABANDON, otype, oid));
}


/***********************************************************************************************
 * SidebarClass::StripClass::SelectClass:: -- Action function when buildable cameo is selected *
 *                                                                                             *
 *    This function is called when the buildable icon (cameo) is clicked on. It handles        *
 *    starting and stopping production as indicated.                                           *
 *                                                                                             *
 * INPUT:   flags -- The input event that triggered the call.                                  *
 *                                                                                             *
 *          key   -- The keyboard value at the time of the input.                              *
 *                                                                                             *
 * OUTPUT:  Returns with whether the input list should be scanned further.                     *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *   10/09/1996 JLB : Sonar pulse converted to regular event type.                             *
 *=============================================================================================*/
int SidebarClass::StripClass::SelectClass::Action(unsigned flags, KeyNumType & key)
{
	if (Strip == NULL) {
		return(1);
	}

	int index = Strip->TopIndex + Index;
	RTTIType otype = Strip->Buildables[index].BuildableType;
	int oid = Strip->Buildables[index].BuildableID;
	FactoryClass * factory = Strip->Buildables[index].Factory;

	TechnoTypeClass const * choice = NULL;
	SuperWeaponType spc = SUPER_NONE;

	Map.Override_Mouse_Shape(MOUSE_NORMAL);

	if (index < Strip->BuildableCount) {
		if (otype != RTTI_SPECIAL) {
			choice = Fetch_Techno_Type(otype, oid);
		} else {
			spc = SuperWeaponType(oid);
		}
	}

	if (spc != SUPER_NONE) {

		/*
		**	Display the help text if the mouse is over the button.
		*/
		if (flags & LEFTUP) {
			flags &= ~LEFTUP;
		}

		/*
		**	A right mouse button signals "cancel".  If we are in targeting
		**	mode then we don't want to be any more.
		*/
		if (flags & RIGHTPRESS) {
			Map.IsTargettingMode = SUPER_NONE;
		}
		/*
		**	A left mouse press signal "activate".  If our weapon type is
		**	available then we should activate it.
		*/
		if (flags & LEFTPRESS) {

			if ((unsigned)spc < (unsigned)PlayerPtr->SuperWeapon.Count()) {
				if (PlayerPtr->SuperWeapon[spc]->Can_Place()) {
					if (PlayerPtr->SuperWeapon[spc]->Class->Action == ACTION_NONE) {
						OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::SPECIAL_PLACE, PlayerPtr->SuperWeapon[spc]->Class->HeapID, Cell(0, 0)));
					} else {
						Map.Place_Beacon_Mode_Control(0);
						Map.IsTargettingMode = spc;
						Unselect_All();
						Speak(VOX_SELECT_TARGET);
					}
				} else {
					PlayerPtr->SuperWeapon[spc]->Impatient_Click();
				}
			}
		}

	} else {

		if (choice != NULL) {

			/*
			**	Display the help text if the mouse is over the button.
			*/
			if (flags & LEFTUP) {
				flags &= ~LEFTUP;
			}

			/*
			**	A right mouse button signals "cancel".
			*/
			if (flags & RIGHTPRESS) {

				/*
				**	If production is in progress, put it on hold. If production is already
				**	on hold, then abandon it. Money will be refunded, the factory
				**	manager deleted, and the object under construction is returned to
				**	the free pool.
				*/
				if (factory != NULL) {

					/*
					**	Cancels placement mode if the sidebar factory is abandoned or
					**	suspended.
					*/
					if (Map.PendingObjectPtr && Dynamic_Cast<TechnoClass *>(Map.PendingObjectPtr)) {
						Map.PendingObjectPtr = NULL;
						Map.PendingObject = NULL;
						Map.PendingHouse = HOUSE_NONE;
						Map.Set_Cursor_Shape(NULL);
					}

					if (!factory->Is_Building()) {
						Speak(VOX_CANCELED);
						Cameo_Abandon(otype, oid, choice);
					} else {
						Speak(VOX_SUSPENDED);
						OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::SUSPEND, otype, oid));
						Map.Flag_Strips_To_Redraw();
					}
				} else {

					/*
					 * Cancel a queued production.
					 */
					FactoryClass * fptr = PlayerPtr->Fetch_Factory(otype, HouseClass::Build_Category(otype, oid), HouseClass::Is_Naval_Type(otype, oid));
					if (fptr != NULL && fptr->Is_Queued(choice)) {
						Cameo_Abandon(otype, oid, choice);
					}
				}
			}

			if (flags & LEFTPRESS) {

				if (factory != NULL && !factory->Is_Building()) {

					/*
					**	If production has completed, then attempt to have the object exit
					**	the factory or go into placement mode.
					*/
					if (factory->Has_Completed()) {

						TechnoClass * pending = factory->Get_Object();
						if (!pending) {
							if (factory->Get_Special_Item() != -1) {
								Map.IsTargettingMode = SUPER_ANY;
							}
						} else {
							BuildingClass * builder = pending->Who_Can_Build_Me(false, false);
							if (!builder) {
								OutList.push_back(EventClass(pending->Owner(), EventClass::ABANDON, otype, oid));
								Speak(VOX_NO_FACTORY);
							} else {

								/*
								**	If the completed object is a building, then change the
								**	game state into building placement mode. This fact is
								**	not transmitted to any linked computers until the moment
								**	the building is actually placed down.
								*/
								if (pending->RTTI == RTTI_BUILDING) {
							 		PlayerPtr->Manual_Place(builder, (BuildingClass *)pending);
								} else {

									/*
									**	For objects that can leave the factory under their own
									**	power, queue this event and process through normal house
									**	production channels.
									*/
									OutList.push_back(EventClass(pending->Owner(), EventClass::PLACE, otype, oid, CELL_NONE));
								}
							}
						}
					} else {

						/*
						**	The factory must have been in a suspended state. Resume construction
						**	normally.
						*/
						if (otype == RTTI_INFANTRYTYPE) {
							Speak(VOX_TRAINING);
						} else {
							Speak(VOX_BUILDING);
						}
						OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::PRODUCE, otype, oid));
					}

				} else {

					/*
					**	If there is already a factory attached to this strip but the player didn't click
					**	on the icon that has the attached factory, then say that the factory is busy and
					**	ignore the click.
					*/
					factory = PlayerPtr->Fetch_Factory(otype, HouseClass::Build_Category(otype, oid), HouseClass::Is_Naval_Type(otype, oid));
					bool produce = false;
					if (factory != NULL && (factory->Is_Building() || factory->Has_Production_Target())) {
						if (otype == RTTI_BUILDINGTYPE) {
							Speak(VOX_NO_FACTORY);
						} else {
							produce = true;
						}

					} else {

						/*
						**	If this side strip is already busy with production, then ignore the
						**	input and announce this fact.
						*/
						if (otype == RTTI_INFANTRYTYPE) {
							Speak(VOX_TRAINING);
						} else {
							Speak(VOX_BUILDING);
						}
						produce = true;
					}
					if (produce) {
						int const count = (otype != RTTI_BUILDINGTYPE) ? Cameo_Produce_Count(factory) : 1;
						for (int queued = 0; queued < count; queued++) {
							OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::PRODUCE, otype, oid));
						}
					}
				}
			}
		} else {
			flags = 0;
		}
	}

	ControlClass::Action(flags, key);

	return(true);
}


/***********************************************************************************************
 * SidebarClass::SBGadgetClass::Action -- Special function that controls the mouse over the si *
 *                                                                                             *
 *    This routine is called whenever the mouse is over the sidebar. It makes sure that the    *
 *    mouse is always the normal shape while over the sidebar.                                 *
 *                                                                                             *
 * INPUT:   flags -- The event flags that resulted in this routine being called.               *
 *                                                                                             *
 *          key   -- Reference the keyboard code that may be present.                          *
 *                                                                                             *
 * OUTPUT:  Returns that no further keyboard processing is necessary.                          *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   03/28/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int SidebarClass::SBGadgetClass::Action(unsigned , KeyNumType & )
{
	Map.Override_Mouse_Shape(MOUSE_NORMAL, false);
	return(true);
}


/***********************************************************************************************
 * SidebarClass::StripClass::Factory_Link -- Links a factory to a sidebar button.              *
 *                                                                                             *
 *    This routine will link the specified factory to this sidebar strip. The exact button to  *
 *    link to is determined from the object type and id specified. A linked button is one that *
 *    will show appropriate construction animation (clock shape) that matches the state of     *
 *    the factory.                                                                             *
 *                                                                                             *
 * INPUT:   factory  -- The factory number to link to the sidebar.                             *
 *                                                                                             *
 *          type     -- The object type that this factory refers to.                           *
 *                                                                                             *
 *          id       -- The object sub-type that this factory refers to.                       *
 *                                                                                             *
 * OUTPUT:  Was the factory successfully attached? Failure would indicate that there is no     *
 *          object of the specified type and sub-type in the sidebar list.                     *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/18/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SidebarClass::StripClass::Factory_Link(FactoryClass * factory, RTTIType type, int id)
{
	for (int index = 0; index < BuildableCount; index++) {
		if (Buildables[index].BuildableType == type && Buildables[index].BuildableID == id) {
			Buildables[index].Factory = factory;
			IsBuilding = true;
			/*
			**	Flag that all the icons on this strip need to be redrawn
			*/
			Flag_To_Redraw();
			return(true);
		}
	}
	return(false);
}


/***********************************************************************************************
 * SidebarClass::Abandon_Production -- Stops production of the object specified.               *
 *                                                                                             *
 *    This routine is used to abandon production of the object specified. The factory will     *
 *    be completely disabled by this call.                                                     *
 *                                                                                             *
 * INPUT:   type     -- The object type that is to be abandoned. The sub-type is not needed    *
 *                      since it is presumed there can be only one type in production at any   *
 *                      one time.                                                              *
 *                                                                                             *
 *          factory  -- The factory number that is doing the production.                       *
 *                                                                                             *
 * OUTPUT:  Was the factory successfully abandoned?                                            *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/18/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
bool SidebarClass::Abandon_Production(RTTIType type, FactoryClass * factory)
{
	// Only the strip holding the factory's cameo acts on it.
	for (int index = 0; index < COLUMNS; index++) {
		if (Column[index].Abandon_Production(factory)) {
			return(true);
		}
	}
	return(false);
}


/***********************************************************************************************
 * SidebarClass::StripClass::Abandon_Produ -- Abandons production associated with sidebar.     *
 *                                                                                             *
 *    Production of the object associated with this sidebar is abandoned when this routine is  *
 *    called.                                                                                  *
 *                                                                                             *
 * INPUT:   factory  -- The factory index that is to be suspended.                             *
 *                                                                                             *
 * OUTPUT:  Was the production abandonment successful?                                         *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   05/18/1995 JLB : Created.                                                                 *
 *   08/06/1995 JLB : More intelligent abandon logic for multiple factories.                   *
 *=============================================================================================*/
bool SidebarClass::StripClass::Abandon_Production(FactoryClass const * factory)
{
	bool noprod = true;
	bool abandon = false;
	for (int index = 0; index < BuildableCount; index++) {
		if (Buildables[index].Factory == factory) {
			((FactoryClass *)factory)->Abandon();
			Buildables[index].Factory = NULL;
			abandon = true;
		} else {
			if (Buildables[index].Factory != NULL) {
				noprod = false;
			}
		}
	}

	/*
	**	If there was a change to the strip, then flag the strip to be redrawn.
	*/
	if (abandon) {
		Flag_To_Redraw();
	}

	/*
	**	If there is no production whatsoever on this strip, then flag it so.
	*/
	if (noprod) {
		IsBuilding = false;
	}
	return(abandon);
}


/***********************************************************************************************
 * SidebarClass::Zoom_Mode_Control -- Handles the zoom mode toggle operation.                  *
 *                                                                                             *
 *    This is the function that is called when the map button is pressed. It will toggle       *
 *    between the different modes that the radar map can assume.                               *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/31/1996 JLB : Created.                                                                 *
 *=============================================================================================*/
void SidebarClass::Zoom_Mode_Control(void)
{
	/*
	**	If radar is active, cycle as follows:
	**	Zoomed => not zoomed
	**	not zoomed => player status (multiplayer only)
	**	player status => radar spying readout
	**	radar spying readout => zoomed
	*/
	if (Session.Type!=GAME_NORMAL) {
		if (Is_Radar_Existing()) {
			if (Is_Player_Names()) {
				Radar_Activate(1);
				return;
			}
		} else if (Is_Player_Names()) {
			Radar_Activate(0);
			return;
		}

		Radar_Activate(2);
	}
}


/// <summary>
/// Repositions the sidebar and everything that sits on it.
/// This routine is called whenever the sidebar changes which edge of the screen it lives
/// on, or the display it must fit into changes size. The sidebar rectangle is placed first
/// and then the mode buttons, scroll arrows, cameo buttons and their tooltip regions are
/// moved to suit.
/// </summary>
void SidebarClass::Reposition_Sidebar(void)
{
	int index;

	/*
	 * Position the sidebar.
	 */
	SidebarRect.X = Sidebar_Screen_X();
	SidebarRect.Y = SIDE_Y;
	SidebarRect.Width = SIDE_WIDTH;
	SidebarRect.Height = VisibleRect.Height - SIDE_Y;

	if (Modern_Interface()) {
		SidebarRect = Hud_Layout().Panel;
		BASECLASS::Reposition_Sidebar();
		Reposition_Panel();
		Add_Tool_Tips();
		return;
	}

	BASECLASS::Reposition_Sidebar();

	if (!SidebarShape) {
		SidebarShape = (ShapeSet const *)MFCD::Retrieve("SIDEGDI1.SHP");
		SidebarMiddleShape = (ShapeSet const *)MFCD::Retrieve("SIDEGDI2.SHP");
		SidebarBottomShape = (ShapeSet const *)MFCD::Retrieve("SIDEGDI3.SHP");
	}

	/*
	 * Yuri's Revenge lays the first side's sidebar out a little differently from the others'
	 * (FUN_006a5090 and FUN_006a5130 in gamemd).
	 */
	bool const first = PlayerPtr == NULL || PlayerPtr->Class->Side == SIDE_FIRST;
	int const repair_x = first ? 20 : 33;
	int const repair_y = first ? 8 : 7;
	int const sell_step = first ? 64 : 52;
	int const tab_x = first ? 26 : 20;
	int const tab_step = first ? 29 : 32;
	int const tab_y = 39;
	int const rows = Max_Visible() / StripClass::SLOT_COLUMNS;
	int const arrow_y = SidebarRect.Y + GRID_Y + rows * StripClass::OBJECT_HEIGHT + 7;
	int const up_step = first ? 46 : 45;

	Background.Set_Position(SidebarRect.X + 16, ScreenTacticalRect.Y);
	Background.Flag_To_Redraw();

	Repair.Set_Position(SidebarRect.X + repair_x, SidebarRect.Y + repair_y);
	Repair.Flag_To_Redraw();
	Repair.DrawOffsetX = -SidebarRect.X;

	Upgrade.Set_Position(Repair.X + sell_step, Repair.Y);
	Upgrade.Flag_To_Redraw();
	Upgrade.DrawOffsetX = -SidebarRect.X;

	/*
	 * The buttons on the piece above the radar sit at fixed screen rows (RadarClass::Init_For_House,
	 * 0x652E90 in gamemd).
	 */
	DiplomacyButton.Set_Position(SidebarRect.X + (first ? 11 : 14), first ? 20 : 21);
	DiplomacyButton.Flag_To_Redraw();
	DiplomacyButton.DrawOffsetX = -SidebarRect.X;
	OptionsButton.Set_Position(SidebarRect.X + (first ? 83 : 86), first ? 20 : 21);
	OptionsButton.Flag_To_Redraw();
	OptionsButton.DrawOffsetX = -SidebarRect.X;

	for (index = 0; index < COLUMNS; index++) {
		TabButton[index].Set_Position(SidebarRect.X + tab_x + index * tab_step, SidebarRect.Y + tab_y);
		TabButton[index].Flag_To_Redraw();
		TabButton[index].DrawOffsetX = -SidebarRect.X;
	}

	/*
	 * Every strip's scroll arrows and cameo slots sit in the same place; only the shown strip's
	 * are in the button list.
	 */
	for (int col = 0; col < COLUMNS; col++) {
		// gamemd places the page-down arrow (R-DN, button 0xC9) left of the page-up arrow (R-UP, 0xC8) (0x6ABD30).
		StripClass::UpButton[col].Set_Position(SidebarRect.X + 39 + up_step, arrow_y);
		StripClass::UpButton[col].Flag_To_Redraw();
		StripClass::UpButton[col].DrawOffsetX = -SidebarRect.X;

		StripClass::DownButton[col].Set_Position(SidebarRect.X + 39, arrow_y);
		StripClass::DownButton[col].Flag_To_Redraw();
		StripClass::DownButton[col].DrawOffsetX = -SidebarRect.X;

		for (int i = 0; i < Max_Visible(); i++) {
			StripClass::SelectButton[col][i].Set_Position(
				SidebarRect.X + Column[col].X + (i % StripClass::Slot_Columns()) * StripClass::Column_Step(),
				SidebarRect.Y + Column[col].Y + StripClass::OBJECT_HEIGHT * (i / StripClass::Slot_Columns()));
			StripClass::SelectButton[col][i].Flag_To_Redraw();
		}
	}

	Add_Tool_Tips();

	int x = 0;
	int y = RadarButton.Y + RadarButton.Height;
	if (Options.IsSidebarOnRight) {
		Background.Set_Position(Sidebar_Screen_X(), y);
	} else {
		Background.Set_Position(x, y);
	}
	Background.Set_Size(SidebarSurface->Get_Width(), SidebarSurface->Get_Height() - y);
}


/// <summary>
/// Places the build panel's buttons and cameo slots for the modern interface: repair and sell
/// on the left of the icon row and options and diplomacy on its right, the tabs one above the
/// other, and the scroll buttons under the slots.
/// </summary>
void SidebarClass::Reposition_Panel(void)
{
	HudLayout const layout = Hud_Layout();
	Point2D const origin(SidebarRect.X, SidebarRect.Y);

	auto place = [&](ShapeButtonClass & button, int x, int y) {
		button.Set_Position(origin.X + x, origin.Y + y);
		button.DrawOffsetX = -origin.X;
		button.DrawOffsetY = -origin.Y;
		button.Flag_To_Redraw();
	};
	auto middle = [](Rect const & rect, int height) {
		return(rect.Y + (rect.Height - height) / 2);
	};

	int x = layout.Buttons.X;
	for (ShapeButtonClass * button : {&BeaconButton, &Upgrade, &Power, &Repair}) {
		place(*button, x + (HUD_BUTTON_SIZE - button->Width) / 2, middle(layout.Buttons, button->Height));
		x += HUD_BUTTON_SIZE + HUD_BUTTON_GAP;
	}
	if (OptionsButton.Label.empty() && OptionsButton.Get_Shape_Data() != NULL) {
		place(OptionsButton, layout.Options.X, layout.Options.Y);
	} else {
		OptionsButton.Label = Panel_Text("GUI:Options", "Options");
		place(OptionsButton, layout.Options.X - 36, layout.Options.Y);
		OptionsButton.Set_Size(layout.Options.Width + 36, layout.Options.Height);
	}

	int y = layout.Tabs.Y;
	for (int index = 0; index < COLUMNS; index++) {
		place(TabButton[index], layout.Tabs.X + (layout.Tabs.Width - TabButton[index].Width) / 2, y);
		y += TabButton[index].Height + 4;
	}

	int const centre = layout.Arrows.X + layout.Arrows.Width / 2;
	for (int col = 0; col < COLUMNS; col++) {
		ShapeButtonClass & up = StripClass::UpButton[col];
		ShapeButtonClass & down = StripClass::DownButton[col];
		place(up, centre - 4 - up.Width, middle(layout.Arrows, up.Height));
		place(down, centre + 4, middle(layout.Arrows, down.Height));

		Column[col].X = layout.Grid.X;
		Column[col].Y = layout.Grid.Y;
		Column[col].ObjectRect = layout.Grid;
		for (int i = 0; i < Max_Visible(); i++) {
			StripClass::SelectButton[col][i].Set_Position(
				origin.X + layout.Grid.X + (i % HUD_GRID_COLUMNS) * StripClass::Column_Step(),
				origin.Y + layout.Grid.Y + (i / HUD_GRID_COLUMNS) * StripClass::OBJECT_HEIGHT);
			StripClass::SelectButton[col][i].Flag_To_Redraw();
		}
	}

	Background.Set_Position(origin.X, origin.Y + layout.Readouts.Y);
	Background.Set_Size(layout.Panel.Width, layout.Panel.Height - layout.Readouts.Y);
	Background.Flag_To_Redraw();
}


/// <summary>
/// Gives the cameo slots and the repair and sell buttons their tooltips at their current places.
/// </summary>
void SidebarClass::Add_Tool_Tips(void)
{
	int index;

	if (ToolTips) {
		ToolTip tooltip;

		for (index = 0; index < COLUMNS; index++) {
			for (int j = 0; j < 100; j++) {
				ToolTips->Remove((j | (index << 8)) + GADGET_CAMEO);
			}
		}

		for (int i = 0; i < Max_Visible(); i++) {
			ToolTip tmp;
			tmp.Text = TXT_NONE;
			tmp.ID = (i | (ActiveTab << 8)) + GADGET_CAMEO;
			tmp.Region.Set(StripClass::SelectButton[ActiveTab][i].X, StripClass::SelectButton[ActiveTab][i].Y,
							   StripClass::SelectButton[ActiveTab][i].Width, StripClass::SelectButton[ActiveTab][i].Height);
			ToolTips->Add(&tmp);
		}

		tooltip.ID = BUTTON_REPAIR;
		tooltip.Text = TXT_REPAIR_MODE;
		tooltip.Region.Set(Repair.X, Repair.Y, Repair.Width, Repair.Height);
		ToolTips->Remove(tooltip.ID);
		ToolTips->Add(&tooltip);

		tooltip.ID = BUTTON_SELL;
		tooltip.Text = TXT_SELL_MODE;
		tooltip.Region.Set(Upgrade.X, Upgrade.Y, Upgrade.Width, Upgrade.Height);
		ToolTips->Remove(tooltip.ID);
		ToolTips->Add(&tooltip);

		if (Modern_Interface()) {
			for (ShapeButtonClass const * button : {&Power, &BeaconButton, &OptionsButton}) {
				tooltip.ID = button->ID;
				tooltip.Text = TXT_NONE;
				tooltip.Region.Set(button->X, button->Y, button->Width, button->Height);
				ToolTips->Remove(tooltip.ID);
				ToolTips->Add(&tooltip);
			}
		} else {
			for (ShapeButtonClass const * button : {&OptionsButton, &DiplomacyButton}) {
				tooltip.ID = button->ID;
				tooltip.Text = TXT_NONE;
				tooltip.Region.Set(button->X, button->Y, button->Width, button->Height);
				ToolTips->Remove(tooltip.ID);
				ToolTips->Add(&tooltip);
			}

			// Only the shown strip's arrows are on screen, so only they need a region.
			for (ShapeButtonClass const * button : {&TabButton[0], &TabButton[1], &TabButton[2], &TabButton[3], &StripClass::UpButton[ActiveTab], &StripClass::DownButton[ActiveTab]}) {
				tooltip.ID = button->ID;
				tooltip.Text = TXT_NONE;
				tooltip.Region.Set(button->X, button->Y, button->Width, button->Height);
				ToolTips->Remove(tooltip.ID);
				ToolTips->Add(&tooltip);
			}
		}
	}
}


/// <summary>
/// Which tabs the modern panel is flashing now, one bit per tab; for the autotests.
/// </summary>
unsigned SidebarClass::Tab_Flash_Mask(void)
{
	return(TabFlashMask);
}


/// <summary>
/// Makes the modern panel's tabs blink while they hold a finished item and are not the tab shown.
/// </summary>
void SidebarClass::Flash_Ready_Tabs(void)
{
	unsigned mask = 0;
	if ((Frame / (StripClass::CAMEO_FLASH_RATE * 2)) % 2 == 0) {
		for (int index = 0; index < COLUMNS; index++) {
			StripClass const & strip = Column[index];
			for (int item = 0; index != ActiveTab && item < strip.BuildableCount; item++) {
				if (strip.Buildables[item].BuildableType == RTTI_SPECIAL) {
					int const spc = strip.Buildables[item].BuildableID;
					if (spc >= 0 && spc < PlayerPtr->SuperWeapon.Count() && PlayerPtr->SuperWeapon[spc]->Should_Flash_Tab()) {
						mask |= 1u << index;
						break;
					}
					continue;
				}
				FactoryClass * factory = strip.Buildables[item].Factory;
				if (factory != NULL && factory->Has_Completed()) {
					mask |= 1u << index;
					break;
				}
			}
		}
	}

	if (mask != TabFlashMask) {
		TabFlashMask = mask;
		IsToRedraw = true;
		Column[ActiveTab].Flag_To_Redraw();
	}
}


/// <summary>
/// Lays the modern build panel out again when the number of cameo slots it shows changes, such as
/// after the Sidebar rows setting changes. The shown strip's buttons follow, and its scroll position
/// is held within the cameos that remain.
/// </summary>
void SidebarClass::Fit_Panel_To_Strip(void)
{
	if (!Modern_Interface()) {
		return;
	}

	int const after = Max_Visible();
	int const before = Registered_Slots;
	if (after == before) {
		return;
	}
	Registered_Slots = after;

	StripClass & strip = Column[ActiveTab];
	if (IsSidebarActive) {
		for (int i = before; i < after; i++) {
			StripClass::SelectButton[ActiveTab][i].Zap();
			Add_A_Button(StripClass::SelectButton[ActiveTab][i]);
		}
		for (int i = after; i < before; i++) {
			Remove_A_Button(StripClass::SelectButton[ActiveTab][i]);
		}
	}

	int const hidden = std::max(0, strip.BuildableCount - after);
	int const last = (hidden + HUD_GRID_COLUMNS - 1) / HUD_GRID_COLUMNS * HUD_GRID_COLUMNS;
	strip.TopIndex = std::clamp(strip.TopIndex, 0, last);

	Reposition_Sidebar();
	IsToRedraw = true;
	strip.Flag_To_Redraw();
	Flag_To_Redraw(GS_REDRAW_ALL);
}


/// <summary>
/// Lays the modern build panel out again after the rows it shows change, whether the setting was
/// saved or is being previewed, and draws the screen at once. A settings page open over the game
/// does not run the game's frames, so the screen would otherwise keep the old panel.
/// </summary>
void Modern_Panel_Rows_Changed(void)
{
	if (!GameActive || SidebarSurface == NULL || !Modern_Interface()) {
		return;
	}
	Map.SidebarClass::Fit_Panel_To_Strip();
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
	Map.Render();
}


/// <summary>
/// Shows the strip of the tab given in place of the one shown before. The other strips keep
/// building and counting down out of sight.
/// </summary>
void SidebarClass::Set_Tab(int tab)
{
	if (tab < 0 || tab >= COLUMNS || tab == ActiveTab) {
		return;
	}

	if (IsSidebarActive) {
		Column[ActiveTab].Deactivate();
	}
	ActiveTab = tab;
	if (IsSidebarActive) {
		Column[ActiveTab].Activate();
	}

	Reposition_Sidebar();
	IsToRedraw = true;
	Column[ActiveTab].Flag_To_Redraw();
	Flag_To_Redraw();
	Fit_Panel_To_Strip();
}


/// <summary>
/// Records a building that has just finished production, for the structure or defense tab that
/// lists it. The tab keeps it until it is placed, or until its factory no longer holds it.
/// </summary>
void SidebarClass::Note_Completed_Building(BuildingClass * building)
{
	CompletedBuilding[Which_Column(RTTI_BUILDINGTYPE, building->Class->Fetch_Heap_ID())] = building;
}


/// <summary>
/// Forgets the building given wherever a tab holds it as its completed building.
/// </summary>
void SidebarClass::Forget_Completed_Building(ObjectClass const * building)
{
	for (int tab = 0; tab < 2; tab++) {
		if (CompletedBuilding[tab] == building) {
			CompletedBuilding[tab] = NULL;
		}
	}
}


/// <summary>
/// Returns the tab's completed building if its factory still holds it as a finished product.
/// </summary>
BuildingClass * SidebarClass::Completed_Building(int tab)
{
	BuildingClass * building = CompletedBuilding[tab];
	if (building != NULL) {
		for (int item = 0; item < Column[tab].BuildableCount; item++) {
			FactoryClass * factory = Column[tab].Buildables[item].Factory;
			if (factory != NULL && factory->Has_Completed() && factory->Get_Object() == building) {
				return(building);
			}
		}
	}
	return(NULL);
}


/// <summary>
/// Starts placing the tab's completed building through the house, as clicking its cameo does.
/// Nothing happens when no building is waiting or the house is already placing one.
/// </summary>
void SidebarClass::Place_Completed_Building(int tab)
{
	BuildingClass * building = Completed_Building(tab);
	if (building != NULL) {
		PlayerPtr->Manual_Place(building->Who_Can_Build_Me(false, false), building);
	}
}


/// <summary>
/// Makes the cameo of an object type flash for a number of frames, wherever on the sidebar it is.
/// </summary>
/// <param name="type">The object type whose cameo flashes.</param>
/// <param name="frames">How many frames the flashing lasts.</param>
void SidebarClass::Flash_Cameo(TechnoTypeClass const * type, int frames)
{
	for (int index = 0; index < COLUMNS; index++) {
		Column[index].Flash_Cameo(type, frames);
	}
}


/// <summary>
/// Makes the cameo of an object type flash if this strip holds it.
/// </summary>
/// <param name="type">The object type whose cameo flashes.</param>
/// <param name="frames">How many frames the flashing lasts.</param>
void SidebarClass::StripClass::Flash_Cameo(TechnoTypeClass const * type, int frames)
{
	for (int index = 0; index < BuildableCount; index++) {
		if (Buildables[index].BuildableType != RTTI_SPECIAL && Fetch_Techno_Type(Buildables[index].BuildableType, Buildables[index].BuildableID) == type) {
			for (CameoFlashType & flash : CameoFlashes) {
				if (flash.Type == type) {
					flash.EndFrame = Frame + frames;
					Flag_To_Redraw();
					return;
				}
			}
			CameoFlashes.push_back({type, Frame + frames});
			Flag_To_Redraw();
			return;
		}
	}
}


/// <summary>
/// Is the cameo of an object type flashing on this strip?
/// </summary>
/// <param name="type">The object type to look up.</param>
bool SidebarClass::StripClass::Is_Cameo_Flashing(TechnoTypeClass const * type) const
{
	for (CameoFlashType const & flash : CameoFlashes) {
		if (flash.Type == type && Frame < flash.EndFrame) {
			return(true);
		}
	}
	return(false);
}


void SidebarClass::Flag_Strips_To_Redraw(void)
{
	for (int index = 0; index < COLUMNS; index++) {
		Column[index].Flag_To_Redraw();
	}
}


/// <summary>
/// Shows or hides the grid keys on the cameos. A change flags every strip to redraw, so the keys appear or go at once.
/// </summary>
/// <param name="shown">Should the keys show?</param>
void SidebarClass::Set_Grid_Labels(bool shown)
{
	if (IsGridLabelShown != shown) {
		IsGridLabelShown = shown;
		Flag_Strips_To_Redraw();
	}
}


/// <summary>
/// Starts the cameo of the visible slot at the grid index of the active strip, as a left press on it does.
/// </summary>
/// <param name="index">The grid index, from 0 for the first key to 11 for the last.</param>
void SidebarClass::Grid_Build(int index)
{
	if (!IsSidebarActive || PlayerPtr == NULL) {
		return;
	}

	StripClass & strip = Column[ActiveTab];
	if (Sidebar_Grid_Buildable(index, Max_Visible(), strip.TopIndex, strip.BuildableCount) < 0) {
		return;
	}

	// A slot that was never given its strip, such as one added when the panel grew, must still press.
	StripClass::SelectClass & button = StripClass::SelectButton[ActiveTab][index];
	button.Set_Owner(strip, index);
	button.Press_Left();
}


/// <summary>
/// Fetches the tooltip text for a sidebar gadget.
/// This routine is called by the tooltip manager. Any gadget the map layers underneath do
/// not claim is taken to be a build cameo and handed to the strip that owns it.
/// </summary>
/// <param name="id">The gadget identifier that text is wanted for.</param>
/// <returns>Returns with a pointer to the help text, or NULL if the gadget has none.</returns>
const char * SidebarClass::Help_Text(int id)
{
	if (Modern_Interface()) {
		static std::string _text;
		switch (id) {
			case BUTTON_POWER:
				_text = Panel_Text("GUI:PowerDown", "Power down");
				return(_text.c_str());
			case BUTTON_BEACON:
				_text = Panel_Text("GUI:PlaceBeacon", "Place beacon");
				return(_text.c_str());
			case BUTTON_OPTIONS:
				_text = Panel_Text("GUI:Options", "Options");
				return(_text.c_str());
		}
	} else {
		// gamemd's tips for tabs and scroll arrows (0x6AC210) use these CSF labels.
		static std::string _tip;
		static char const * const tab_labels[COLUMNS] = {"Tip:Tab1", "Tip:Tab2", "Tip:Tab3", "Tip:Tab4"};
		static char const * const tab_fallbacks[COLUMNS] = {"Structures Tab", "Armory Tab", "Infantry Tab", "Vehicles Tab"};
		if (id >= BUTTON_TAB && id < BUTTON_TAB + COLUMNS) {
			int const index = id - BUTTON_TAB;
			_tip = Classic_Tip(tab_labels[index], tab_fallbacks[index], !TabDisabled[index]);
			return(_tip.c_str());
		}
		if (id >= StripClass::BUTTON_UP && id < StripClass::BUTTON_UP + COLUMNS) {
			_tip = Classic_Tip("Tip:ScrollUp", "Up Scroll Arrow", !ArrowsDisabled[id - StripClass::BUTTON_UP]);
			return(_tip.c_str());
		}
		if (id >= StripClass::BUTTON_DOWN && id < StripClass::BUTTON_DOWN + COLUMNS) {
			_tip = Classic_Tip("Tip:ScrollDown", "Down Scroll Arrow", !ArrowsDisabled[id - StripClass::BUTTON_DOWN]);
			return(_tip.c_str());
		}

		// gamemd's tips for 0xF2 and 0xF3 (0x654320): the briefing in a campaign, else diplomacy.
		static std::string _text;
		switch (id) {
			case BUTTON_OPTIONS:
				_text = Panel_Text("Tip_OptionsButton", "Options");
				return(_text.c_str());
			case BUTTON_DIPLOMACY:
				if (Session.Type == GAME_NORMAL) {
					_text = Panel_Text("Tip_BriefingButton", "Briefing");
				} else {
					_text = Panel_Text("Tip_DiplomacyButton", "Diplomacy");
				}
				return(_text.c_str());
		}
	}

	const char *text = BASECLASS::Help_Text(id);
	if (text == NULL) {
		id = id - GADGET_CAMEO;
		int index = id >> 8;
		if (index < COLUMNS && index >= 0) {
			text = Column[index].Help_Text(id % 256U);
		}
	}
	return(text);
}


/// <summary>
/// Determines how many cameos a sidebar strip can show.
/// The answer follows from the height of the sidebar, so it varies with the screen
/// resolution. It is used to lay the strips out and to decide when scrolling is called for.
/// </summary>
/// <returns>Returns with the number of build cameos that will fit in one strip.</returns>
/// <remarks>A strip has MAX_SLOTS select buttons, so a taller picture than those can fill
/// shows the slots it has rather than the slots that would fit.</remarks>
int SidebarClass::Max_Visible(void)
{
	if (Modern_Interface()) {
		return(Hud_Layout().Rows * HUD_GRID_COLUMNS);
	}
	if (SidebarSurface != NULL) {
		bool const first = PlayerPtr == NULL || PlayerPtr->Class->Side == SIDE_FIRST;
		int const rows = (SidebarRect.Height - GRID_Y - (first ? 26 : 18) - 7) / StripClass::OBJECT_HEIGHT;
		return(std::clamp(rows * int(StripClass::SLOT_COLUMNS), int(StripClass::SLOT_COLUMNS), int(StripClass::MAX_SLOTS)));
	}
	return(StripClass::MAX_VISIBLE);
}


/// <summary>
/// Prints the descriptive text that goes with a sidebar cameo.
/// This routine is used by the strip renderer to caption a cameo with its name and cost.
/// Text too wide for the space is broken at a space or a hyphen and carried onto as many
/// lines as it takes.
/// </summary>
/// <param name="point">Where to print the last line. Earlier lines stack upward from
/// there.</param>
/// <param name="cliprect">The clipping rectangle to print within.</param>
/// <param name="maxlinelen">The width, in pixels, a line may reach before it is broken.</param>
/// <remarks>Nothing at all is printed unless the player has cameo text turned on.</remarks>
void Print_Cameo_Text(char const * string, Point2D const & point, Rect const & cliprect, int maxlinelen)
{
	char buffer[64];

	if (Map.IsCameoText == true && string != NULL) {
		FontClass * font = Font_From_TPF(TextPrintType(TPF_6POINT|TPF_METAL12|TPF_FULLSHADOW));
		Point2D drawpoint = point;

		if (font->String_Pixel_Width(string) > maxlinelen) {
			int len = strlen(string);
			strcpy(buffer, string);
			string = buffer;

			int w = 0;
			do {
				/*
				**	While the current line is less then the max length...
				*/
				int linelen = font->Char_Pixel_Width(UTF8::Peek(buffer + len));
				while (linelen <= maxlinelen && len > 0) {
					len = (int)(UTF8::Previous(buffer, buffer + len) - buffer);
					linelen += font->Char_Pixel_Width(UTF8::Peek(buffer + len));
				}

				/*
				**	Back up to an appropriate location to break.
				*/
				while (linelen > maxlinelen) {
					while (buffer[len] != ' ' && buffer[len] != '-' && buffer[len] != '\0') {
						int length;
						linelen -= font->Char_Pixel_Width(UTF8::Peek(buffer + len, length));
						len += length;
					}

					if (buffer[len] == '\0') {
						len = 0;
						break;
					}

					if (linelen <= maxlinelen) {
						break;
					}

					linelen -= font->Char_Pixel_Width(UTF8::Peek(buffer + len));
					len++;
				}

				if (len <= 0) {
					break;
				}

				if (buffer[len] == ' ') {
					buffer[len] = '\0';
					len++;
				} else if (buffer[len] == '-') {
					len++;
				}

				Fancy_Text_Print(&buffer[len], *SidebarSurface, cliprect, drawpoint, Fetch_Scheme_By_Name("LightGrey"), TBLACK, TextPrintType(TPF_FULLSHADOW|TPF_EFNT));
				buffer[len] = '\0';
				drawpoint.Y -= font->Get_Height();
				w = font->String_Pixel_Width(buffer);

			} while (len > 0 && w > maxlinelen);

			/*
			 * Print the rest of the string.
			 */
			finish:;
		}

		Fancy_Text_Print(string, *SidebarSurface, cliprect, drawpoint, Fetch_Scheme_By_Name("LightGrey"), TBLACK, TextPrintType(TPF_FULLSHADOW|TPF_EFNT));
	}
}


/// <summary>
/// Lists the members the sidebar holds.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void SidebarClass::Serialize(SaveStreamClass & stream)
{
	BASECLASS::Serialize(stream);

	// SidebarShape -- backdrop artwork, fetched for the player's house by Init_For_House.
	// SidebarMiddleShape
	// SidebarBottomShape
	// SidebarAddonShape
	stream.Serialize(Column);
	// IsToRedraw -- a redraw flag; the load asks for a complete draw anyway.
	stream.Serialize(IsCameoText);
	stream.Serialize(IsSidebarActive);

	// IsForceCompleteRedraw -- likewise redraw flags.
	// IsToRedrawCredits
	// Repair -- the sidebar buttons and their blit flag, all set up again by Init_IO.
	// Upgrade
	// Power
	// Waypoint
	// Background
	// IsToBlitSidebar
	stream.Serialize(IsRepairActive);
	stream.Serialize(IsUpgradeActive);
	stream.Serialize(IsDemolishActive);
}


/// <summary>
/// Lists the members one side strip holds.
/// </summary>
/// <param name="stream">The stream carrying the members.</param>
void SidebarClass::StripClass::Serialize(SaveStreamClass & stream)
{
	StageClass::Serialize(stream);

	/*
	 * not saved: X, Y, ObjectRect, ID -- where the strip sits on the sidebar and which strip it
	 * is, both established by the sidebar constructor and Init_IO for the current screen.
	 *
	 * not saved: IsToRedraw -- a redraw flag; the load asks for a complete draw anyway.
	 *
	 * not saved: IsToSort -- raised on load below, because the rules and the ordering setting
	 * can both differ from the ones the game was saved under.
	 */
	stream.Serialize(IsBuilding);
	stream.Serialize(IsScrollingDown);
	stream.Serialize(IsScrolling);
	stream.Serialize(Flasher);
	stream.Serialize(TopIndex);
	stream.Serialize(Scroller);
	stream.Serialize(Slid);
	stream.Serialize(LastSlid);
	stream.Serialize(BuildableCount);
	stream.Serialize(Buildables);

	if (stream.Is_Loading()) {
		IsToSort = true;
	}

	/*
	 * not saved: LogoShapes, ClockShapes, RechargeClockShapes, DarkenShapes, UpButton,
	 * DownButton, SelectButton -- cameo artwork and the strip's buttons, all set up again by
	 * One_Time, Init_For_House and Init_IO.
	 */
}
