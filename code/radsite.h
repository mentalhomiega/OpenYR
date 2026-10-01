/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/
#pragma once


#include "coord.h"
#include "vector.h"

class LightSourceClass;
class SaveStreamClass;


/*
 * A patch of radiation left where a weapon with RadLevel goes off. It raises the radiation of
 * the cells around its center, weaker towards its edge, lights them in RadColor, and fades
 * away over RadDurationMultiple frames for each point of its level.
 */
class RadSiteClass
{
	public:
		static void Irradiate(Coord const & where, int spread, int level);
		static void Update_All(void);
		static void Clear_All(void);
		static void Serialize_All(SaveStreamClass & stream);
		static void Detach_All(void const * target);

		void Serialize(SaveStreamClass & stream);

	private:
		void Activate(void);
		void Add(int level);
		bool Update(void);
		double Share(Cell const & cell) const;
		void Change_Cells(double factor);

		Cell BaseCell;
		int Spread;
		int SpreadInLeptons;
		int RadLevel;
		int LevelSteps;
		int Intensity;
		int IntensityDecrement;
		int Red;
		int Green;
		int Blue;
		int RadDuration;
		int RadTimeLeft;
		int LevelTimer;
		int LightTimer;
		LightSourceClass * Light;

		static DynamicVectorClass<RadSiteClass *> Sites;
};
