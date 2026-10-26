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
#include "vector.h"

class HouseClass;
class ShapeSet;

template<class T> class DynamicVectorClass;


/*
 * One of the campaign results screen's sounds, held by the name the screen asks for it by.
 */
class SfxEntry {
	public:
		SfxEntry(char const * name, char const *filename);
		~SfxEntry(void);

		const char * Get_Name(void) { return(Name);}
		void * Get_Sample(void) { return(Sample);}

	private:
		char * Name;
		void * Sample;
		bool IsAllocated;
};


/*
 * The campaign results screen. It shows the player and the enemy in the layout of gamemd's
 * Campaign Results dialog (dialog 3015), with the side's own art.
 */
class ScoreClass {
	public:
		ScoreClass(void) {};

		void Presentation(void);

	private:
		void DoSound(const char * name, int volume);
		void Print_Row_Figures(int row, int kills, int losses, int built, int score);
		void Wait_For_Click(ShapeSet const * animation);

		ScoreStageClass Stage;
		DynamicVectorClass<SfxEntry *> ScoreSnds;
};


void Multi_Score_Presentation(void);
