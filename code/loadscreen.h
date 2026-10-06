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

#include <memory>
#include <string>

class ConvertClass;
class ProgressScreenClass;
class ShapeSet;
class Surface;
class MixFileClass;

/*
**	The Yuri's Revenge loading screen. A campaign mission shows the picture MISSIONMD.INI names
**	for it between a title bar, which holds the mission's loading message, and a strip that holds
**	the progress bar, with the mission's loading briefing over the picture. The screen is laid
**	out at 800x600, or at 640x480 on a screen 640 pixels wide, and is drawn centred on the screen,
**	enlarged by the largest whole factor that fits.
*/
class LoadScreenClass
{
	public:
		LoadScreenClass(void);
		~LoadScreenClass(void);

		bool Begin(char const * scenario);
		void Draw_Progress(ProgressScreenClass const & progress);
		void End(void);

		bool Is_Active(void) const {return(Active);}
		std::string const & Picture_Name(void) const {return(PictureName);}

	private:
		class ShapeFile;

		bool Begin_Campaign(char const * scenario);
		void Present(Rect const & area);

		bool Active = false;
		std::string PictureName;

		// The layout's size, and where and how much it is enlarged on the hidden surface.
		int Width = 0;
		int Height = 0;
		int Scale = 1;
		Point2D Origin;

		// The finished screen without any progress, and the screen as it is shown.
		std::unique_ptr<Surface> Backdrop;
		std::unique_ptr<Surface> Screen;

		std::unique_ptr<ShapeFile> BarShape;
		std::unique_ptr<ConvertClass> BarDrawer;
		Point2D BarPos;

		std::unique_ptr<MixFileClass> LoadMix;
		std::unique_ptr<MixFileClass> BaseLoadMix;
};

extern LoadScreenClass LoadScreen;
