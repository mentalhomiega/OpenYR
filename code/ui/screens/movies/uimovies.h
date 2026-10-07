/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "ui/uiscreen.h"

#include <memory>
#include <string>
#include <vector>

class UIViewClass;


struct UIMovieRow
{
	// What the list shows.
	std::string Name;

	// The movie file, extension included.
	std::string File;
};


struct UIMoviesState
{
	std::string Title;
	std::string PlayCaption;
	std::string BackCaption;
	std::vector<UIMovieRow> Rows;
	int Selected = 0;
};


class UIMoviesPresenterClass : public UIPresenterClass
{
	public:
		explicit UIMoviesPresenterClass(UIMoviesState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		UIMoviesState State;
};


std::unique_ptr<UIViewClass> UI_Movies_View(UIMoviesPresenterClass & presenter);

// Lists the campaign movies and plays the one picked, again and again, until the player goes back.
void UI_Movies_Dialog(void);
