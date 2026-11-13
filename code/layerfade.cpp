/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "layerfade.h"

#include "autotest.h"
#include "cell.h"
#include "dbgprint.h"
#include "gpudraw.h"
#include "gscreen.h"
#include "_map.h"
#include "_rect.h"
#include "_tactica.h"
#include "globals.h"
#include "scenario.h"
#include "tactical.h"

#include <algorithm>
#include <cstdlib>


// Game frames the light must stay at its target before the exact redraw.
static constexpr int SETTLE_FRAMES = 30;

// The frozen picture is redrawn when the flat ground's scale leaves this range: brightening a dark picture
// far shows its coarse steps, darkening a bright one does not.
static constexpr int MOST_BRIGHTENING_PERCENT = 150;
static constexpr int MOST_DARKENING_PERCENT = 30;

static bool _Active = false;
static int _FrozenAmbient = 0;
static int _Settled = 0;


static bool Fade_Allowed(void)
{
	static int const _allowed = std::getenv("OPENYR_NO_FADE_DEFER") == NULL ? 1 : 0;
	return(_allowed != 0);
}


bool LayerFade_Active(void)
{
	return(_Active);
}


int LayerFade_Frozen_Ambient(void)
{
	return(_FrozenAmbient);
}


bool LayerFade_Defer(int ambient_before)
{
	if (!Fade_Allowed() || !GpuDraw_Steady()) {
		_Active = false;
		return(false);
	}
	if (!_Active) {
		_Active = true;
		_FrozenAmbient = ambient_before;
		AUTOTEST_LOG("layer fade frame %d: the tile layer keeps its picture of ambient %d\n", Frame, _FrozenAmbient);
	}
	_Settled = 0;
	if (Scen->CurrentAmbientLight == _FrozenAmbient) {
		_Active = false;
		return(true);
	}
	int const percent = _FrozenAmbient > 0 ? 100 * Scen->CurrentAmbientLight / _FrozenAmbient : MOST_BRIGHTENING_PERCENT + 1;
	if (percent > MOST_BRIGHTENING_PERCENT || percent < MOST_DARKENING_PERCENT) {
		// The redraw the caller now asks for draws the layer at the current light, and the next step freezes that.
		AUTOTEST_LOG("layer fade frame %d: redraw at ambient %d, too far from %d\n", Frame, Scen->CurrentAmbientLight, _FrozenAmbient);
		_Active = false;
		return(false);
	}
	return(true);
}


void LayerFade_AI(void)
{
	if (!_Active) {
		return;
	}
	if (_FrozenAmbient == Scen->CurrentAmbientLight) {
		// The picture is already exact: the light came back to the frozen level, or the whole layer was redrawn.
		if (Scen->DesiredAmbientLight == Scen->CurrentAmbientLight) {
			_Active = false;
		}
		return;
	}
	if (!GpuDraw_Steady()) {
		LayerFade_Resolve();
		return;
	}
	if (Scen->DesiredAmbientLight == Scen->CurrentAmbientLight && ++_Settled >= SETTLE_FRAMES) {
		LayerFade_Resolve();
	}
}


void LayerFade_Resolve(void)
{
	if (_Active) {
		_Active = false;
		AUTOTEST_LOG("layer fade frame %d: redraw at ambient %d, the fade is over\n", Frame, Scen->CurrentAmbientLight);
		Map.Flag_To_Redraw(GS_REDRAW_TACTICAL);
	}
}


void LayerFade_Clear(void)
{
	_Active = false;
	_Settled = 0;
}


bool LayerFade_Fill(std::vector<unsigned short> & plane, int & width, int & height, Rect const & area)
{
	if (!_Active || TacticalMap == NULL || area.Width <= 0 || area.Height <= 0) {
		return(false);
	}
	width = (area.Width + LAYER_FADE_DIVISOR - 1) / LAYER_FADE_DIVISOR;
	height = (area.Height + LAYER_FADE_DIVISOR - 1) / LAYER_FADE_DIVISOR;
	plane.assign((std::size_t)width * height, (unsigned short)LAYER_FADE_UNIT);

	// A cell covers a block a little larger than its diamond, so that neighbours leave no gaps.
	int const half_width = ISO_TILE_PIXEL_W / 2 / LAYER_FADE_DIVISOR + 1;
	int const half_height = ISO_TILE_PIXEL_H / 2 / LAYER_FADE_DIVISOR + 1;

	Map.Reset_Iterator();
	for (CellClass * cell = Map.Iterate(); cell != NULL; cell = Map.Iterate()) {
		Point2D pixel;
		if (!TacticalMap->Coord_To_Pixel(cell->Cell_Coord(), pixel)) {
			continue;
		}
		int const x = (pixel.X + TacticalRect.X - area.X) / LAYER_FADE_DIVISOR;
		int const y = (pixel.Y + TacticalRect.Y - area.Y) / LAYER_FADE_DIVISOR;
		int const left = std::max(x - half_width, 0);
		int const right = std::min(x + half_width, width - 1);
		int const top = std::max(y - half_height, 0);
		int const bottom = std::min(y + half_height, height - 1);
		if (left > right || top > bottom) {
			continue;
		}
		int const frozen = cell->Tile_Brightness_At(_FrozenAmbient);
		int const now = cell->TileBrightness;
		int scale = LAYER_FADE_UNIT;
		if (frozen > 0) {
			scale = std::min(now * LAYER_FADE_UNIT / frozen, 65535);
		}
		for (int row = top; row <= bottom; row++) {
			std::fill(plane.begin() + ((std::size_t)row * width + left), plane.begin() + ((std::size_t)row * width + right + 1), (unsigned short)scale);
		}
	}
	return(true);
}


LayerFadeDrawScope::LayerFadeDrawScope(bool whole_layer) :
	Saved(Scen->CurrentAmbientLight),
	Swapped(_Active && !whole_layer)
{
	// OPENYR_FADE_KEEP=1 draws even a whole layer at the frozen light, so that a screenshot, which redraws the
	// layer, shows the scaled picture.
	static bool const _keep = std::getenv("OPENYR_FADE_KEEP") != NULL;
	if (_keep && _Active) {
		whole_layer = false;
		Swapped = true;
	}
	if (_Active && whole_layer && _FrozenAmbient != Scen->CurrentAmbientLight) {
		AUTOTEST_LOG("layer fade frame %d: the whole layer is redrawn at ambient %d\n", Frame, Scen->CurrentAmbientLight);
		_FrozenAmbient = Scen->CurrentAmbientLight;
	}
	if (Swapped) {
		Scen->CurrentAmbientLight = _FrozenAmbient;
		Map.Update_Cell_Colors();
	}
}


LayerFadeDrawScope::~LayerFadeDrawScope(void)
{
	if (Swapped) {
		Scen->CurrentAmbientLight = Saved;
		Map.Update_Cell_Colors();
	}
}
