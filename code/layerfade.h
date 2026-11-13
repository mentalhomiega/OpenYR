/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "rect.h"

#include <vector>

class CellClass;


// While the ambient light fades and the GPU draws the map view, the tile layer keeps the picture it drew at the
// light level the fade started from, and the GPU map pass scales that picture to the current level cell by cell.
// One full redraw when the light has settled makes the picture exact again. Without this every step of a fade
// redraws the whole tile layer. OPENYR_NO_FADE_DEFER=1 in the environment turns it off.

// How finely the scale plane divides the map view: one value for each block of this many pixels a side.
inline constexpr int LAYER_FADE_DIVISOR = 8;

// The scale a plane value of 1 stands for is 1 / LAYER_FADE_UNIT.
inline constexpr int LAYER_FADE_UNIT = 4096;

// True while the tile layer holds a picture drawn at LayerFade_Frozen_Ambient instead of the current light level.
bool LayerFade_Active(void);
int LayerFade_Frozen_Ambient(void);

// Called when the ambient light has stepped away from ambient_before. True means the tile layer keeps its picture
// and the caller must not ask for a full redraw.
bool LayerFade_Defer(int ambient_before);

// Called once a game frame: ends the deferral with a full redraw when the light has settled or the GPU has stopped
// drawing the map view.
void LayerFade_AI(void);

// Ends the deferral now with a full redraw.
void LayerFade_Resolve(void);

// Forgets the deferral without a redraw, for a new or loaded scenario.
void LayerFade_Clear(void);

// Fills plane, width by height values in rows, with the scale each block of the map view under area takes from the
// frozen picture to the current light. False, with plane untouched, when nothing is deferred. version gets a
// number that changes whenever the plane is given other values than the last call left in it.
bool LayerFade_Fill(std::vector<unsigned short> & plane, int & width, int & height, Rect const & area, int & version);

// Gives the cells the lighting of the frozen light level for as long as it lives, so that a draw into the tile
// layer matches the picture already there. cells must hold every cell the draws read the lighting of, and stay
// unchanged meanwhile. It does nothing when nothing is deferred. A redraw of the whole layer needs no match: it
// is drawn at the current light, which becomes the frozen level.
class LayerFadeDrawScope
{
	public:
		LayerFadeDrawScope(bool whole_layer, std::vector<CellClass *> const & cells);
		~LayerFadeDrawScope(void);
		LayerFadeDrawScope(LayerFadeDrawScope const &) = delete;
		LayerFadeDrawScope & operator=(LayerFadeDrawScope const &) = delete;

	private:
		void Relight(int ambient);

		std::vector<CellClass *> const & Cells;
		int Saved;
		bool Swapped;
};
