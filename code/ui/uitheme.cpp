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

#include "globals.h"
#include "goptions.h"

#include <filesystem>

namespace {

std::string Directory;

}	// namespace


void UI_Theme_Set_Directory(std::string const & directory)
{
	Directory = directory;
}


std::string UI_Theme_Directory(void)
{
	char const separator = (char)std::filesystem::path::preferred_separator;
	return(Directory + "themes" + separator + (Options.IsClassicMenus ? "classic" : "modern") + separator);
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
