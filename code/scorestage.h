/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "point.h"
#include "rect.h"

class ConvertClass;
class MSFont;
class ShapeSet;
class Surface;

/*
 * The stage that the end-of-game score screens are drawn on. The stage is 800 by 600 and the
 * dialog layouts that gamemd's score screens use are given in dialog units on it. The stage is
 * stretched to the largest whole fit inside the screen and centered, and the rest of the screen is
 * black, so no part of the game's own interface shows around it.
 */
class ScoreStageClass
{
	public:
		ScoreStageClass(void);
		~ScoreStageClass(void);

		bool Open(void);
		void Close(void);

		Surface * Canvas(void) const;

		void Save_Base(void);
		void Keep(Rect const & area);
		void Restore(Rect const & area);

		void Present(Rect const & area);
		void Clear_Screen(void);
		bool Contains_Screen_Point(Rect const & area, int x, int y) const;

		void Print_Left(char const * text, int x, int y);
		void Print_Right(char const * text, int right, int y);
		void Print_Centre(char const * text, int left, int width, int y);
		int Text_Width(char const * text) const;
		int Text_Height(void) const;

		void Load_Palette(char const * name);
		ShapeSet * Load_Shape(char const * name) const;
		static void Free_Shape(ShapeSet const * shapes);
		void Draw_Shape_Frame(ShapeSet const * shapes, int frame, Rect const & box);
		void Draw_Band(char const * name, int x, int y);

		bool Is_Small(void) const;
		static char Side_Letter(void);

		static int Dialog_X(int units);
		static int Dialog_Y(int units);
		static Rect Dialog_Rect(int x, int y, int width, int height);

	private:
		Rect Stage_To_Screen(Rect const & area) const;

		double Scale;
		Point2D FitOrigin;
		Surface * StageSurface;
		Surface * BaseSurface;
		MSFont * Font;
		ConvertClass * Drawer;
		bool Mounted;
};
