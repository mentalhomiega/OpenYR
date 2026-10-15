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

class CCINIClass;
class ConvertClass;
class MapPreviewClass;
class MixFileClass;
class ProgressScreenClass;
class ShapeSet;
class Surface;

/*
**	The Yuri's Revenge loading screen. A campaign mission shows the picture MISSIONMD.INI names
**	for it between a title bar, which holds the mission's loading message, and a strip that holds
**	the progress bar, with the mission's loading briefing over the picture. A skirmish or
**	multiplayer match shows the local player's country with its name, special unit and
**	description, the map's preview, and a row for each player with a progress bar, a flag and a
**	name. The screen is laid out at 800x600, or at 640x480 on a screen 640 pixels wide, and is
**	drawn centred on the screen, smoothly enlarged to fill its height at any resolution.
*/
class LoadScreenClass
{
	public:
		LoadScreenClass(void);
		~LoadScreenClass(void);

		bool Begin(char const * scenario, CCINIClass const * ini, int players);
		void Draw_Progress(ProgressScreenClass const & progress);
		void Show_Start_Positions(void);
		void End(void);

		bool Is_Active(void) const {return(Active);}

		// True when the campaign loading screen drawn by the last Begin printed a mission briefing.
		bool Shows_Campaign_Briefing(void) const {return(CampaignBriefing);}
		void Clear_Campaign_Briefing(void) {CampaignBriefing = false;}
		std::string const & Picture_Name(void) const {return(PictureName);}

		// Mounts the archives the loading art is in. A loading screen mounts them in Begin; anything else that draws
		// the art mounts them here, and End or the destructor releases them.
		void Mount_Art(void);

		// Draws the picture MISSIONMD.INI names for a campaign mission, with its top left at the point given. Name receives
		// the picture's file name whenever the mission names one, even if the picture then fails to draw.
		static bool Draw_Campaign_Picture(char const * scenario, CCINIClass const * mission, bool small, Surface & surface, Point2D at, std::string & name);

		// The size of a campaign mission's picture, which fills the loading screen between the title bar and the strip.
		static Point2D Campaign_Picture_Size(bool small);

	private:
		class ShapeFile;

		bool Begin_Campaign(char const * scenario, CCINIClass const * mission);
		bool Begin_Match(CCINIClass const * ini, int players);
		void Draw_Preview(void);
		void Draw_Rows(int players);
		void Draw_Campaign_Progress(ProgressScreenClass const & progress);
		void Draw_Match_Progress(ProgressScreenClass const & progress);
		void Present(Rect const & area);

		bool Active = false;
		bool CampaignBriefing = false;
		bool Match = false;
		std::string PictureName;

		// The layout's size, and where and how large it is drawn on the hidden surface.
		int Width = 0;
		int Height = 0;
		Point2D Shown;
		Point2D Origin;

		// The finished screen without any progress, and the screen as it is shown.
		std::unique_ptr<Surface> Backdrop;
		std::unique_ptr<Surface> Screen;

		// The progress bar art, the drawer of a campaign's bar, and where the first bar's row starts.
		std::unique_ptr<ShapeFile> BarShape;
		std::unique_ptr<ConvertClass> BarDrawer;
		Point2D BarPos;

		// A match's player rows: how many, how tall each is, and how wide.
		int Rows = 0;
		int RowHeight = 0;
		int RowWidth = 0;

		// A match's map preview, kept until the start positions are marked, and where it is drawn.
		std::unique_ptr<MapPreviewClass> Preview;
		Rect PreviewBox;
		Rect PreviewArea;

		std::unique_ptr<MixFileClass> LoadMix;
		std::unique_ptr<MixFileClass> BaseLoadMix;
};

extern LoadScreenClass LoadScreen;
