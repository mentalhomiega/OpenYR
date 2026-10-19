/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "linetrail.h"

#include "_rect.h"
#include "_surface.h"
#include "_tactica.h"
#include "dsurface.h"
#include "ftimer.h"
#include "object.h"
#include "goptions.h"
#include "tactical.h"
#include "vector.h"

#include <algorithm>


DynamicVectorClass<LineTrailClass *> LineTrailClass::Lines;


LineTrailClass::LineTrailClass(ObjectClass * owner) :
	Color(128, 128, 128),
	Owner(owner),
	Decrement(16),
	ActiveSlot(0)
{
	for (PlaceType & place : Places) {
		place.Position = Coord(0, 0, 0);
		place.Value = 0;
	}
	Lines.Add(this);
}


LineTrailClass::~LineTrailClass(void)
{
	if (Owner != NULL) {
		Owner->LineTrailer = NULL;
		Owner = NULL;
	}
}


void LineTrailClass::Set_Decrement(int decrement)
{
	if (Options.DetailLevel == 0) {
		decrement <<= 1;
	}
	Decrement = decrement;
}


/// <summary>
/// Records the owner's position as the newest place when it differs from the last one (LineTrail
/// update, 0x556B70), then lowers every place's opacity by the decrement, to no less than 0.
/// </summary>
void LineTrailClass::Update(void)
{
	if (Owner != NULL) {
		Coord const here = Owner->PositionCoord;
		if (here != Places[ActiveSlot].Position) {
			ActiveSlot--;
			if (ActiveSlot < 0) {
				ActiveSlot = PLACES - 1;
			}
			Places[ActiveSlot].Value = 255;
			Places[ActiveSlot].Position = here;
		}
	}
	for (PlaceType & place : Places) {
		place.Value = std::max(place.Value - Decrement, 0);
	}
}


/// <summary>
/// Draws a segment from each place to the next older one, in this line's color at the opacity of
/// the newer place (LineTrail draw, 0x556C00). Drawing stops at a place with no opacity left or one
/// that was never filled. A segment is hidden where the depth buffer holds something nearer.
/// </summary>
void LineTrailClass::Draw(void) const
{
	Coord const none(0, 0, 0);
	PlaceType const * place = &Places[ActiveSlot];
	if (place->Position == none) {
		return;
	}
	unsigned const color = DSurface::Build_Hicolor_Pixel(Color);
	int index = ActiveSlot;
	while (place->Value != 0) {
		index = (index + 1) % PLACES;
		PlaceType const * next = &Places[index];
		if (next->Position == none) {
			return;
		}
		Point2D from;
		Point2D to;
		TacticalMap->Coord_To_Pixel(place->Position, from);
		TacticalMap->Coord_To_Pixel(next->Position, to);
		int const from_z = -2 - Tactical::Z_Lepton_To_Pixel(place->Position.Z);
		int const to_z = -2 - Tactical::Z_Lepton_To_Pixel(next->Position.Z);
		LogicalSurface->Draw_Depth_Shaded_Line(TacticalRect, from, to, color, from_z, to_z, false, true, place->Value);
		if ((index + 1) % PLACES == ActiveSlot) {
			return;
		}
		place = next;
	}
}


/// <summary>
/// Moves every line on a frame and draws it (0x556D40). The lines move once per game frame however
/// often the view is drawn. A line whose object is gone and whose newest place has faded is deleted.
/// </summary>
void LineTrailClass::Update_And_Draw_All(void)
{
	static int last_frame = -1;
	bool const advance = last_frame != Frame;
	last_frame = Frame;

	for (int index = Lines.Count() - 1; index >= 0; index--) {
		LineTrailClass * line = Lines[index];
		if (advance) {
			line->Update();
		}
		if (line->Owner == NULL && line->Places[line->ActiveSlot].Value == 0) {
			Lines.Delete(line);
			delete line;
		} else {
			line->Draw();
		}
	}
}


void LineTrailClass::All_Clear(void)
{
	for (int index = 0; index < Lines.Count(); index++) {
		LineTrailClass * line = Lines[index];
		if (line->Owner != NULL) {
			line->Owner->LineTrailer = NULL;
			line->Owner = NULL;
		}
		delete line;
	}
	Lines.Clear();
}


int LineTrailClass::Count(void)
{
	return(Lines.Count());
}


int LineTrailClass::Lit_Places(void)
{
	int lit = 0;
	for (int index = 0; index < Lines.Count(); index++) {
		for (PlaceType const & place : Lines[index]->Places) {
			if (place.Value > 0) {
				lit++;
			}
		}
	}
	return(lit);
}
