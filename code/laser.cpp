/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "autotest.h"
#include "dbgprint.h"

#include "laser.h"

#include "_map.h"
#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "ccrand.h"
#include "coord.h"
#include "dsurface.h"
#include "globals.h"
#include "goptions.h"
#include "inline.h"
#include "logic.h"
#include "mouse.h"
#include "rgb.h"
#include "scenario.h"
#include "tactical.h"

#include <algorithm>

#include "emissive.h"
#include "emissive_surface.h"

DynamicVectorClass<LaserDrawClass *> LaserDrawClass::LaserDraws;


/// <summary>
/// Creates a laser beam between two points in the world.
/// This routine is used by the weapon code whenever a laser, railgun or similar beam
/// weapon fires. The new beam adds itself to the master list, so the caller may forget
/// about it entirely -- it will be updated, drawn and finally retired on its own.
/// </summary>
/// <param name="zadjust">The depth bias applied to the beam's starting end.</param>
/// <param name="unknown">Vestigial flag; recorded on the beam but never consulted.</param>
/// <param name="outer_spread">The maximum random color deviation of the outer glow.</param>
/// <param name="duration">The lifetime of the beam, in game frames.</param>
/// <param name="blinks">Should the beam flicker on and off as it ages?</param>
/// <param name="fades">Should the beam's intensity slide from start to end over its life?</param>
LaserDrawClass::LaserDrawClass(Coord start, Coord end, int zadjust, bool unknown, RGBClass inner_color, RGBClass outer_color, RGBClass outer_spread, int duration, bool blinks, bool fades, float start_intensity, float end_intensity) :
	StageClass(),
	Thickness(1),
	IsHouseColor(false),
	IsSupported(false),
	Start(start),
	End(end),
	ZAdjust(zadjust),
	UnusedBool1(unknown),
	EmissiveStrength(EmissiveDefaults[EMISSIVE_LASER].Strength),
	InnerColor(),
	OuterColor(),
	OuterSpread(),
	Duration(duration),
	Blinks(blinks),
	BlinkState(false),
	Fades(fades),
	StartIntensity(start_intensity),
	EndIntensity(end_intensity)
{
	Set_Stage(0);
	Set_Rate(1);

	InnerColor = inner_color;
	OuterColor = outer_color;
	OuterSpread = outer_spread;

	LaserDraws.Add(this);

	if (AutoTest_Active()) {
		DebugString("AUTOTEST laser beam from %d,%d,%d to %d,%d,%d duration %d\n", start.X, start.Y, start.Z, end.X, end.Y, end.Z, duration);
	}
}


/// <summary>
/// Destroys the laser beam.
/// This routine removes the beam from the master list, so that it is no longer updated
/// or drawn.
/// </summary>
LaserDrawClass::~LaserDrawClass(void)
{
	LaserDraws.Delete(this);
}


/// <summary>
/// Removes every laser beam that is currently active.
/// This routine is used when the scenario is torn down, so that no beam survives into
/// the next game.
/// </summary>
void LaserDrawClass::All_Clear(void)
{
	while (LaserDraws.Count()) {
		delete LaserDraws[0];
	}
}


/// <summary>
/// Handles the per frame logic for this laser beam.
/// This routine advances the animation stage, flips the blink state for a blinking beam,
/// and retires the beam once it has lived out its duration.
/// </summary>
/// <remarks>The laser may delete itself here, so do not touch it after this call.</remarks>
void LaserDrawClass::AI(void)
{
	Graphic_Logic();
	if (Blinks) {
		BlinkState = BlinkState == false;
	}

	if (Fetch_Stage() >= Duration) {
		delete this;
	}
}


/// <summary>
/// Processes the logic for every laser beam that is currently active.
/// This routine is called once per game logic loop. Beams that have outlived their
/// duration remove themselves from the list during this call.
/// </summary>
void LaserDrawClass::Update_All(void)
{
	for (int i = LaserDraws.Count() - 1; i >= 0; i--) {
		LaserDraws[i]->AI();
	}
}


/// <summary>
/// Draws every laser beam that is currently active.
/// This routine is called by the tactical map rendering pass so that beams appear over
/// the objects they were fired between.
/// </summary>
void LaserDrawClass::Draw_All(void)
{
	for (int i = LaserDraws.Count() - 1; i >= 0; i--) {
		LaserDraws[i]->Draw_It();
	}
}


/// <summary>
/// Draws the laser beam onto the logical surface.
/// This routine draws an ordinary beam as a thin inner core and, when an outer color was specified, a pair
/// of parallel glow lines flanking it. A beam of a house colored weapon is drawn by Draw_House_Color
/// instead. A beam that lies under the fog of war, that is currently blinked off, or that has no duration
/// draws nothing at all. At the lowest detail level the beam degrades to a plain depth shaded line.
/// </summary>
/// <remarks>This is gamemd's LaserDrawClass draw routine (0x550260). Only the detail level gates the
/// ordinary beam; the frame rate gates the house colored one.</remarks>
void LaserDrawClass::Draw_It(void)
{
	if (Duration <= 0) {
		return;
	}

	if (!Scen->Special.IsFogOfWar || !Map.Is_Fogged(Start) || !Map.Is_Fogged(End)) {

		if (IsHouseColor) {
			Draw_House_Color();
			return;
		}

		static Point2D _glow_offsets[16] = {
			Point2D(0, -1),
			Point2D(1, 0),
			Point2D(0, -1),
			Point2D(0, 1),
			Point2D(1, 0),
			Point2D(0, 1),
			Point2D(-1, 0),
			Point2D(1, 0),
			Point2D(-1, 0),
			Point2D(0, 1),
			Point2D(0, -1),
			Point2D(1, 0),
			Point2D(0, -1),
			Point2D(-1, 0),
			Point2D(-1, 0),
			Point2D(1, 0)
		};

		if (!BlinkState) {

			/// The "thin" core (InnerColor) is drawn once with no offset. The "thick"
			/// outer glow (OuterColor) is two extra lines, each a 1-pixel parallel copy
			/// of the core shifted by _glow_offsets[2*facing] / [2*facing+1].
			///
			/// The glow can look like it "leans" a slightly different way than the core at
			/// some angles. That is deliberate, not a defect here. Two reasons:
			/// 1. facing is the WORLD-space beam direction quantized to just 8 compass
			/// points (As_Dir8), so in-between angles get the nearest sector's offset.
			/// 2. facing is world-space but the offsets are applied in ISOMETRIC screen
			/// space (Y compressed ~2:1), so the offset is only approximately
			/// perpendicular to the on-screen beam.
			/// Per-facing the offset PAIR straddles the core symmetrically only for
			/// NE(1)/SE(3)/NW(7); for N(0)/E(2)/S(4)/SW(5)/W(6) the pair is an L-corner
			/// (both on one side), biasing the glow off the core axis -- this is the
			/// "thick and thin face different ways" artifact. Do NOT "fix" the table or
			/// switch to screen-space facing without opting into a deliberate deviation.
			DirType dir = Direction(Start, End);
			FacingType facing = dir.As_Facing();

			Point2D start_pixel;
			TacticalMap->Coord_To_Pixel(Start, start_pixel);
			Point2D end_pixel;
			TacticalMap->Coord_To_Pixel(End, end_pixel);
			if (Emissive_Is_Enabled() && EmissiveStrength != 0) {
				Emissive_Stamp_Line_On(*LogicalSurface, TacticalRect, start_pixel, end_pixel, EmissiveStrength);
			}

			int start_z = ZAdjust - TacticalMap->Z_Lepton_To_Pixel(Start.Z) - 2;
			int end_z = -TacticalMap->Z_Lepton_To_Pixel(End.Z) - 2;

			RGBClass outer_color;
			int outer_hicolor = 0;
			bool has_outer_glow = true;

			if (OuterColor == RGBClass(0, 0, 0)) {
				has_outer_glow = false;
			} else {
				int red = Sim_Random_Pick(-OuterSpread.Get_Red(), OuterSpread.Get_Red());
				int green = Sim_Random_Pick(-OuterSpread.Get_Green(), OuterSpread.Get_Green());
				int blue = Sim_Random_Pick(-OuterSpread.Get_Blue(), OuterSpread.Get_Blue());

				red += OuterColor.Get_Red();
				red = std::max(0, red);
				red = std::min(255, red);

				green += OuterColor.Get_Green();
				green = std::max(0, green);
				green = std::min(255, green);

				blue += OuterColor.Get_Blue();
				blue = std::max(0, blue);
				blue = std::min(255, blue);

				outer_hicolor = DSurface::Build_Hicolor_Pixel(red, green, blue);
				outer_color.Set_Red(red);
				outer_color.Set_Green(green);
				outer_color.Set_Blue(blue);
			}

			float current_intensity = 1.0;

			bool has_r = InnerColor.Get_Red() > 0;
			bool has_g = InnerColor.Get_Green() > 0;
			bool has_b = InnerColor.Get_Blue() > 0;

			if (Fades) {
				current_intensity = (StartIntensity - EndIntensity) * (Duration - Fetch_Stage()) / Duration + EndIntensity;
			}

			if (Options.DetailLevel != 0) {
				if (Blinks) {
					LogicalSurface->Draw_Depth_Antialiased_Line(TacticalRect, start_pixel, end_pixel, InnerColor, start_z, end_z, false, true, true, true, current_intensity);
					if (has_outer_glow) {
						LogicalSurface->Draw_Depth_Antialiased_Line(
							TacticalRect,
							start_pixel + _glow_offsets[2 * facing],
							end_pixel + _glow_offsets[2 * facing],
							outer_color,
							start_z, end_z, false, true, true, true, 1.0
						);

						LogicalSurface->Draw_Depth_Antialiased_Line(
							TacticalRect,
							start_pixel + _glow_offsets[2 * facing + 1],
							end_pixel + _glow_offsets[2 * facing + 1],
							outer_color,
							start_z, end_z, false, true, true, true, 1.0
						);
					}
				} else if (has_outer_glow) {
					/// A steady beam with a glow adds all three of its lines to the pixels beneath
					/// (0x4BDF00), the glow lines first, and the glow takes the beam's intensity as the core does.
					LogicalSurface->Draw_Depth_Additive_Line(
						TacticalRect,
						start_pixel + _glow_offsets[2 * facing],
						end_pixel + _glow_offsets[2 * facing],
						outer_color,
						current_intensity, start_z, end_z
					);

					LogicalSurface->Draw_Depth_Additive_Line(
						TacticalRect,
						start_pixel + _glow_offsets[2 * facing + 1],
						end_pixel + _glow_offsets[2 * facing + 1],
						outer_color,
						current_intensity, start_z, end_z
					);

					LogicalSurface->Draw_Depth_Additive_Line(TacticalRect, start_pixel, end_pixel, InnerColor, current_intensity, start_z, end_z);
				} else {
					LogicalSurface->Draw_Depth_Antialiased_Line(TacticalRect, start_pixel, end_pixel, InnerColor, start_z, end_z, false, has_r, has_g, has_b, current_intensity);
				}
			} else {
				LogicalSurface->Draw_Depth_Shaded_Line(TacticalRect, start_pixel, end_pixel,
					DSurface::Build_Hicolor_Pixel(InnerColor.Get_Red(), InnerColor.Get_Green(), InnerColor.Get_Blue()),
					start_z, end_z, false
				);

				if (has_outer_glow) {
					LogicalSurface->Draw_Depth_Shaded_Line(
						TacticalRect,
						start_pixel + _glow_offsets[2 * facing],
						end_pixel + _glow_offsets[2 * facing],
						outer_hicolor,
						start_z, end_z, false
					);

					LogicalSurface->Draw_Depth_Shaded_Line(
						TacticalRect,
						start_pixel + _glow_offsets[2 * facing + 1],
						end_pixel + _glow_offsets[2 * facing + 1],
						outer_hicolor,
						start_z, end_z, false
					);
				}
			}
		}
	}
}


/// <summary>
/// Draws the beam of a house colored weapon, which is thicker than an ordinary one.
/// This routine draws up to Thickness pairs of parallel lines on either side of the beam's path, each pair
/// one step further out than the last and half as bright. The pairs march outward by a step that depends
/// on which of the eight compass facings the beam lies nearest, and the beam has no line of its own along
/// its center. A beam that carries IsSupported starts bright: its first pair is drawn at twice the house
/// color, the second at the house color itself, and the pairs after that halve from there. Otherwise the
/// first pair is drawn at half the house color. The pairs stop early once all three guns fall below 8,
/// or below 64 when the frame rate is under the detail minimum.
/// </summary>
/// <remarks>This is gamemd's routine at 0x5509F0. It takes the software path of that routine; the
/// textured quads it draws when a Direct3D device is in use are not part of this engine.</remarks>
void LaserDrawClass::Draw_House_Color(void)
{
	/*
	**	How far each pair of lines moves out per step, by facing. Each facing lists the
	**	step of the first line (x, y), then the step of the second line (x, y). A facing
	**	that is even moves one axis of each line on alternate steps; an odd one moves both.
	*/
	static int const steps[8][4] = {
		{ -1, -1, 1, 1 },
		{ 0, -1, 0, 1 },
		{ -1, 1, 1, -1 },
		{ -1, 0, 1, 0 },
		{ -1, -1, 1, 1 },
		{ 0, -1, 0, 1 },
		{ -1, 1, 1, -1 },
		{ -1, 0, 1, 0 }
	};

	DirType dir = Direction(Start, End);
	int const facing = dir.As_Facing();

	Point2D start_pixel;
	TacticalMap->Coord_To_Pixel(Start, start_pixel);
	Point2D end_pixel;
	TacticalMap->Coord_To_Pixel(End, end_pixel);
	if (Emissive_Is_Enabled() && EmissiveStrength != 0) {
		Emissive_Stamp_Line_On(*LogicalSurface, TacticalRect, start_pixel, end_pixel, EmissiveStrength);
	}

	int const start_z = ZAdjust - TacticalMap->Z_Lepton_To_Pixel(Start.Z) - 2;
	int const end_z = -TacticalMap->Z_Lepton_To_Pixel(End.Z) - 2;

	bool const detailed = Detail_Min_Frame_Rate() <= LastFramesPerSecond && Options.DetailLevel != 0;

	float intensity = 1.0f;
	if (Fades) {
		intensity = (StartIntensity - EndIntensity) * (Duration - Fetch_Stage()) / Duration + EndIntensity;
	}
	int const intensity_level = (int)((double)intensity * 255.0);

	RGBClass color;
	if (IsSupported) {
		color = RGBClass((unsigned char)std::min(2 * InnerColor.Get_Red(), 255), (unsigned char)std::min(2 * InnerColor.Get_Green(), 255), (unsigned char)std::min(2 * InnerColor.Get_Blue(), 255));
	} else {
		color = RGBClass((unsigned char)(InnerColor.Get_Red() >> 1), (unsigned char)(InnerColor.Get_Green() >> 1), (unsigned char)(InnerColor.Get_Blue() >> 1));
	}

	/*
	**	The first line runs between start1 and end1, the second between start2 and end2.
	*/
	Point2D start1 = start_pixel;
	Point2D end1 = end_pixel;
	Point2D start2 = start_pixel;
	Point2D end2 = end_pixel;

	int const * const step = steps[facing];
	int const dim_limit = detailed ? 8 : 64;

	for (int i = 1; i <= Thickness; i++) {
		if ((facing & 1) != 0) {
			start1 += Point2D(step[0], step[1]);
			end1 += Point2D(step[0], step[1]);
			start2 += Point2D(step[2], step[3]);
			end2 += Point2D(step[2], step[3]);
		} else if ((i & 1) != 0) {
			start1.X += step[0];
			end1.X += step[0];
			start2.Y += step[3];
			end2.Y += step[3];
		} else {
			start1.Y += step[1];
			end1.Y += step[1];
			start2.X += step[2];
			end2.X += step[2];
		}

		if (detailed) {
			LogicalSurface->Draw_Depth_Additive_Line(TacticalRect, start1, end1, color, intensity, start_z, end_z);
			LogicalSurface->Draw_Depth_Additive_Line(TacticalRect, start2, end2, color, intensity, start_z, end_z);
		} else {
			/// The reduced path draws plain shaded lines in the color pulled toward white by the intensity.
			RGBClass shown = color;
			shown.Adjust(intensity_level, RGBClass(255, 255, 255));
			unsigned const hicolor = DSurface::Build_Hicolor_Pixel(shown);
			LogicalSurface->Draw_Depth_Shaded_Line(TacticalRect, start1, end1, hicolor, start_z, end_z, false);
			LogicalSurface->Draw_Depth_Shaded_Line(TacticalRect, start2, end2, hicolor, start_z, end_z, false);
		}

		if (IsSupported && i == 1) {
			color = InnerColor;
		} else {
			int const red = color.Get_Red() >> 1;
			int const green = color.Get_Green() >> 1;
			int const blue = color.Get_Blue() >> 1;
			if (red < dim_limit && green < dim_limit && blue < dim_limit) {
				break;
			}
			color = RGBClass((unsigned char)red, (unsigned char)green, (unsigned char)blue);
		}
	}
}
