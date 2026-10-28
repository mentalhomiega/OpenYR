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

#include <vector>

class ConvertClass;
class FontClass;
class ShapeSet;
class Surface;

/*
 * The stage that the end-of-game score screens are drawn on. By default the stage is 800 by 600, the
 * dialog layouts that gamemd's score screens use are given in dialog units on it, and it is fitted to
 * the screen and centered. A stage opened with fill_screen is the screen's size and is drawn 1 to 1.
 * Nothing outside the stage is drawn unless the screen first gets a backdrop.
 */
class ScoreStageClass
{
	public:
		ScoreStageClass(void);
		~ScoreStageClass(void);

		bool Open(bool fill_screen = false);
		void Close(void);

		Surface * Canvas(void) const;
		int Width(void) const;
		int Height(void) const;

		void Save_Base(void);
		void Keep(Rect const & area);
		void Restore(Rect const & area);

		void Present(Rect const & area);
		void Clear_Screen(void);
		void Fill_Backdrop(Surface & target, Rect const & source);
		void Show_Screen(void);
		bool Contains_Screen_Point(Rect const & area, int x, int y) const;

		void Print_Left(char const * text, int x, int y);
		void Print_Right(char const * text, int right, int y);
		void Print_Centre(char const * text, int left, int width, int y);
		int Text_Width(char const * text) const;
		int Text_Height(void) const;

		void Load_Palette(char const * name);
		ConvertClass * Load_Drawer(char const * name) const;
		static void Free_Drawer(ConvertClass * drawer);
		void Set_Text_Color(int red, int green, int blue);
		ShapeSet * Load_Shape(char const * name) const;
		static void Free_Shape(ShapeSet const * shapes);
		void Draw_Shape_Frame(ShapeSet const * shapes, int frame, Rect const & box);
		void Draw_Shape_At(ShapeSet const * shapes, int frame, int x, int y, ConvertClass * drawer);
		void Draw_Keyed_Frame(ShapeSet const * shapes, int frame, Rect const & box);
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
		FontClass const * Font;
		ConvertClass * TextConverter;
		ConvertClass * Drawer;
		bool Mounted;
		int StageWidth;
		int StageHeight;
		ShapeSet const * KeyedShape;
		int KeyedWidth;
		int KeyedHeight;
		std::vector<std::vector<float>> KeyedFrames;
};
