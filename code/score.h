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

#pragma once

#include "rect.h"
#include "scorestage.h"

class ShapeSet;


/*
 * The mission score screen shown after a campaign mission is won. It follows gamemd's standalone
 * score dialog (dialog 259): the rank and commendation come from the mission's [Ranking] section, and
 * it shows the par time, the elapsed time and the enemies destroyed. Only its Continue button closes it.
 */
class ScoreClass {
	public:
		ScoreClass(void) {};

		void Presentation(void);

	private:
		void Wait_For_Click(ShapeSet const * animation, Rect const & frame, Rect const & button);

		ScoreStageClass Stage;
};


void Multi_Score_Presentation(void);
