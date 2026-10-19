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
#include "ccrand.h"
#include "cell.h"
#include "dsurface.h"
#include "logic.h"
#include "map.h"
#include "session.h"
#include "tactical.h"

#include <algorithm>
#include <cmath>


DynamicVectorClass<RadBeamClass *> RadBeamClass::Beams;

namespace {

// A beam lasts this many frames while its wave swells to its full height.
int const WaveDuration = 15;

// The wave's full height in leptons, reached in the beam's last frame.
double const WaveAmplitude = 40.0;

// The beam is drawn in straight pieces this many leptons long.
int const Step = 10;

// An eruption beam is drawn in straight pieces this many leptons long.
double const EruptionStep = 20.0;

}


/// <summary>
/// Starts a beam from start to end in the color given (TechnoClass::CreateRadBeam, 0x6FD620).
/// </summary>
void RadBeamClass::Fire(Coord const & start, Coord const & end, RGBClass const & color)
{
	RadBeamClass * beam = new RadBeamClass;
	beam->Kind = KIND_WAVE;
	beam->Start = start;
	beam->End = end;
	beam->Color = color;
	beam->Age = 0;
	beam->Duration = WaveDuration;
	beam->Amplitude = WaveAmplitude;
	beam->IsVisible = true;
	Beams.Add(beam);
}


/// <summary>
/// Starts an arching beam from origin to each of the eight cells around it, spread cells out
/// (TechnoClass::FireRadEruption, 0x6FD800). Each beam ends at a random point within half a cell of the
/// center of its cell, lasts 5 to 20 frames and arches 100 to 500 leptons high. The cells are taken
/// column by column, and a cell off the map still gets a beam.
/// </summary>
void RadBeamClass::Fire_Eruption(Coord const & origin, int spread, RGBClass const & color)
{
	short const column = (short)(origin.X / CELL_LEPTON_W);
	short const row = (short)(origin.Y / CELL_LEPTON_H);

	for (int across = -1; across <= 1; across++) {
		for (int down = -1; down <= 1; down++) {
			if (across == 0 && down == 0) {
				continue;
			}

			Cell const cell((short)((short)(across * spread) + column), (short)((short)(down * spread) + row));
			Coord end = Map[cell].Center_Coord();
			end.X += Sim_Random_Pick(-128, 128);
			end.Y += Sim_Random_Pick(-128, 128);

			RadBeamClass * beam = new RadBeamClass;
			beam->Kind = KIND_ERUPTION;
			beam->Start = origin;
			beam->End = end;
			beam->Color = color;
			beam->Age = 0;
			beam->Duration = Sim_Random_Pick(5, 20);
			beam->Amplitude = Sim_Random_Pick(100, 500);
			// gamemd also draws a random value from 0 to 20 here and stores it, and no later step uses it.
			Sim_Random_Pick(0, 20);
			beam->IsVisible = true;
			Beams.Add(beam);
		}
	}
}


/// <summary>
/// Ages every beam by one game frame and removes those that have run out (RadBeam::UpdateAll,
/// 0x6591B0). While the game runs slower than its desired frame rate, each visible eruption beam
/// has a chance to stop being drawn, and the chance grows as the frame rate falls.
/// </summary>
void RadBeamClass::Update_All(void)
{
	int eruptions = 0;
	int visible = 0;
	for (int index = Beams.Count() - 1; index >= 0; index--) {
		if (Beams[index]->Kind == KIND_ERUPTION) {
			eruptions++;
			if (Beams[index]->IsVisible) {
				visible++;
			}
		}
	}

	// The share of the visible eruption beams, in percent, that this frame's rate leaves too many.
	int hide_percent = 0;
	if (visible > 0 && Session.DesiredFrameRate > 0) {
		double const ratio = (double)LastFramesPerSecond / (double)Session.DesiredFrameRate;
		double const excess = (double)visible / eruptions - ratio * ratio * ratio;
		hide_percent = (int)((excess > 0.0 ? eruptions * excess : 0.0) / visible * 100.0);
	}

	for (int index = Beams.Count() - 1; index >= 0; index--) {
		RadBeamClass * beam = Beams[index];
		if (beam->Kind == KIND_ERUPTION && beam->IsVisible && Sim_Random_Pick(0, 99) < hide_percent) {
			beam->IsVisible = false;
		}
		if (++beam->Age >= beam->Duration) {
			Beams.Delete_Index(index);
			delete beam;
		}
	}
}


void RadBeamClass::Draw_All(void)
{
	for (int index = 0; index < Beams.Count(); index++) {
		if (Beams[index]->Kind == KIND_ERUPTION) {
			Beams[index]->Draw_Eruption();
		} else {
			Beams[index]->Draw_It();
		}
	}
}


int RadBeamClass::Count(bool eruption)
{
	int count = 0;
	for (int index = 0; index < Beams.Count(); index++) {
		count += (Beams[index]->Kind == KIND_ERUPTION) == eruption ? 1 : 0;
	}
	return(count);
}


int RadBeamClass::Count_Visible_Eruptions(void)
{
	int count = 0;
	for (int index = 0; index < Beams.Count(); index++) {
		count += (Beams[index]->Kind == KIND_ERUPTION && Beams[index]->IsVisible) ? 1 : 0;
	}
	return(count);
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
		// The wave is truncated to whole units before the shade, as gamemd's FUN_00659AC0 does (0x659AC0).
		int const wave = (int)(std::sin(turn * piece) * height);
		double const shade = std::abs(wave) * 0.5 / Amplitude + 0.5;
		unsigned const color = DSurface::Build_Hicolor_Pixel((int)(Color.Get_Red() * shade), (int)(Color.Get_Green() * shade), (int)(Color.Get_Blue() * shade));

		Draw_Piece(from, to, color);
		from = to;
	}
}


/// <summary>
/// Draws one straight piece of a beam in the pixel given.
/// </summary>
void RadBeamClass::Draw_Piece(Coord const & from, Coord const & to, unsigned color) const
{
	Point2D a;
	Point2D b;
	TacticalMap->Coord_To_Pixel(from, a);
	TacticalMap->Coord_To_Pixel(to, b);
	int const za = -Tactical::Z_Lepton_To_Pixel(from.Z) - 2;
	int const zb = -Tactical::Z_Lepton_To_Pixel(to.Z) - 2;
	// The beam is drawn at the colour it was shaded to. gamemd's line takes that pixel as is (0x659650), so the
	// alpha buffer, which holds the terrain's brightness, must not darken it.
	LogicalSurface->Draw_Depth_Shaded_Line(TacticalRect, a, b, color, za, zb, false, false);
}


/// <summary>
/// Draws the beam as an arch over the line from start to end (gamemd's FUN_00659CA0 and
/// FUN_00659FF0). The line is cut into pieces about 20 leptons long, and the arch rises as a half
/// sine wave whose height grows to the beam's full height at half its duration and falls back to
/// nothing at the end. Each piece is drawn brighter the higher it ends.
/// </summary>
void RadBeamClass::Draw_Eruption(void) const
{
	if (!IsVisible) {
		return;
	}

	int const dx = End.X - Start.X;
	int const dy = End.Y - Start.Y;
	int const dz = End.Z - Start.Z;
	int const squares = dx * dx + dy * dy + dz * dz;
	int const pieces = (int)(std::sqrt((double)squares) / EruptionStep);
	if (pieces <= 0) {
		return;
	}
	int const stepx = dx / pieces;
	int const stepy = dy / pieces;
	int const stepz = dz / pieces;

	int const rise = Age < Duration / 2 ? 2 * Age : Duration - (2 * Age) % Duration;
	double const height = (double)rise / Duration * Amplitude;
	double const radians = 0.017453292519943295;

	for (int piece = 0; piece < pieces; piece++) {
		double const first = std::sin(radians * ((double)piece / pieces * 180.0)) * height;
		double const last = std::sin(radians * ((double)(piece + 1) / pieces * 180.0)) * height;
		Coord const from(Start.X + stepx * piece, Start.Y + stepy * piece, (int)((double)(Start.Z + stepz * piece) + first));
		Coord const to(Start.X + stepx * (piece + 1), Start.Y + stepy * (piece + 1), (int)((double)(Start.Z + stepz * (piece + 1)) + last));

		double const shade = std::abs((int)last) * 0.5 / Amplitude + 0.5;
		unsigned const color = DSurface::Build_Hicolor_Pixel((int)(Color.Get_Red() * shade), (int)(Color.Get_Green() * shade), (int)(Color.Get_Blue() * shade));
		Draw_Piece(from, to, color);
	}
}
