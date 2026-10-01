/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "radbeam.h"

#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "dsurface.h"
#include "tactical.h"

#include <algorithm>
#include <cmath>


DynamicVectorClass<RadBeamClass *> RadBeamClass::Beams;

namespace {

// A beam lasts this many frames while its wave swells to its full height.
int const Duration = 15;

// The wave's full height in leptons, reached in the beam's last frame.
double const Amplitude = 40.0;

// The beam is drawn in straight pieces this many leptons long.
int const Step = 10;

}


/// <summary>
/// Starts a beam from start to end in the color given (TechnoClass::CreateRadBeam, 0x6FD620).
/// </summary>
void RadBeamClass::Fire(Coord const & start, Coord const & end, RGBClass const & color)
{
	RadBeamClass * beam = new RadBeamClass;
	beam->Start = start;
	beam->End = end;
	beam->Color = color;
	beam->Age = 0;
	Beams.Add(beam);
}


/// <summary>
/// Ages every beam by one game frame and removes those that have run out (RadBeam::UpdateAll,
/// 0x6591B0).
/// </summary>
void RadBeamClass::Update_All(void)
{
	for (int index = Beams.Count() - 1; index >= 0; index--) {
		RadBeamClass * beam = Beams[index];
		if (++beam->Age >= Duration) {
			Beams.Delete_Index(index);
			delete beam;
		}
	}
}


void RadBeamClass::Draw_All(void)
{
	for (int index = 0; index < Beams.Count(); index++) {
		Beams[index]->Draw_It();
	}
}


/// <summary>
/// Removes every beam, as when a scenario ends.
/// </summary>
void RadBeamClass::All_Clear(void)
{
	for (int index = 0; index < Beams.Count(); index++) {
		delete Beams[index];
	}
	Beams.Clear();
}


/// <summary>
/// Draws the beam as a vertical wave along the line from start to end (gamemd's FUN_00659650
/// and FUN_00659AC0). The wave completes one cycle per 180 leptons of beam, or two cycles on a
/// beam shorter than 360 leptons, and its height grows with the beam's age. Each piece is
/// drawn brighter the further it rises or falls.
/// </summary>
void RadBeamClass::Draw_It(void) const
{
	double const dx = End.X - Start.X;
	double const dy = End.Y - Start.Y;
	double const dz = End.Z - Start.Z;
	double const length = std::sqrt(dx * dx + dy * dy + dz * dz);
	int const pieces = (int)(length / Step);
	if (pieces < 1) {
		return;
	}

	double const height = Amplitude * Age / Duration;
	double const turn = 3.141592653589793 / 180.0 * 2.0 * std::max(length, 360.0) / pieces;

	auto point = [&](int piece) {
		double const wave = std::sin(turn * piece) * height;
		return Coord((int)(Start.X + dx * piece / pieces), (int)(Start.Y + dy * piece / pieces), (int)(Start.Z + dz * piece / pieces + wave));
	};

	Coord from = point(0);
	for (int piece = 0; piece < pieces; piece++) {
		Coord const to = point(piece + 1);
		double const wave = std::fabs(std::sin(turn * piece) * height);
		double const shade = wave * 0.5 / Amplitude + 0.5;
		unsigned const color = DSurface::Build_Hicolor_Pixel((int)(Color.Get_Red() * shade), (int)(Color.Get_Green() * shade), (int)(Color.Get_Blue() * shade));

		Point2D a;
		Point2D b;
		TacticalMap->Coord_To_Pixel(from, a);
		TacticalMap->Coord_To_Pixel(to, b);
		int const za = -Tactical::Z_Lepton_To_Pixel(from.Z) - 2;
		int const zb = -Tactical::Z_Lepton_To_Pixel(to.Z) - 2;
		LogicalSurface->Draw_Depth_Shaded_Line(TacticalRect, a, b, color, za, zb);
		from = to;
	}
}
