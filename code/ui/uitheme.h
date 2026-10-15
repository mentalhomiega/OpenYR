/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <string>

// Records the folder the UI files are read from, ending in a separator.
void UI_Theme_Set_Directory(std::string const & directory);

// The folder the UI files are read from, ending in a separator.
std::string UI_Theme_Base_Directory(void);

// The folder holding the files of the menu style in force, ending in a separator. A file there
// replaces the shipped UI file of the same name.
std::string UI_Theme_Directory(void);

// The screen size the menu style in force is laid out for; the menus scale with the window
// from it.
float UI_Theme_Reference_Width(void);
float UI_Theme_Reference_Height(void);

// The faction tints of the modern menus. The colours of each tint are set by its own sheet.
enum UIMenuTintType
{
	UI_TINT_ALLIED,
	UI_TINT_SOVIET,
	UI_TINT_YURI,
	UI_TINT_COUNT
};

// The tint the modern menus show: the one the MenuTheme setting names, or for Random the one
// drawn the first time it is asked for in this run.
UIMenuTintType UI_Theme_Tint(void);

// The body class that selects a tint's colours, such as theme-soviet.
inline char const * UI_Theme_Tint_Class(UIMenuTintType tint)
{
	static char const * const CLASSES[UI_TINT_COUNT] = {"theme-allied", "theme-soviet", "theme-yuri"};
	return(CLASSES[tint]);
}
