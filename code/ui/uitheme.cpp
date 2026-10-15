/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/uitheme.h"

#include "_ui.h"
#include "ui/uishell.h"
#include "globals.h"
#include "goptions.h"

#include <filesystem>
#include <random>

namespace {

std::string Directory;

// The tint Random drew for this run, or -1 before the first draw.
int Random_Tint = -1;

// Whether a live battle runs behind the menus.
bool Live_Background = false;

}	// namespace


void UI_Theme_Set_Directory(std::string const & directory)
{
	Directory = directory;
}


std::string UI_Theme_Base_Directory(void)
{
	return(Directory);
}


std::string UI_Theme_Directory(void)
{
	char const separator = (char)std::filesystem::path::preferred_separator;
	return(Directory + "themes" + separator + (Options.IsClassicMenus ? "classic" : "modern") + separator);
}


UIMenuTintType UI_Theme_Tint(void)
{
	switch (Options.MenuTheme) {
		case MENU_THEME_ALLIED:
			return(UI_TINT_ALLIED);

		case MENU_THEME_SOVIET:
			return(UI_TINT_SOVIET);

		case MENU_THEME_YURI:
			return(UI_TINT_YURI);

		default:
			break;
	}

	if (Random_Tint < 0) {
		std::random_device device;
		Random_Tint = (int)(device() % (unsigned int)UI_TINT_COUNT);
	}
	return((UIMenuTintType)Random_Tint);
}


void UI_Theme_Set_Live_Background(bool live)
{
	if (Live_Background == live) {
		return;
	}

	Live_Background = live;
	UIShell.Refresh();
}


bool UI_Theme_Live_Background(void)
{
	return(Live_Background);
}


// The modern style is laid out for a 1280x720 screen, so its panels fill a 1080 or 2160-line one
// by the same share; the classic style is laid out for Yuri's Revenge's 800x600.
float UI_Theme_Reference_Width(void)
{
	return(Options.IsClassicMenus ? 800.0f : 1280.0f);
}


float UI_Theme_Reference_Height(void)
{
	return(Options.IsClassicMenus ? 600.0f : 720.0f);
}
