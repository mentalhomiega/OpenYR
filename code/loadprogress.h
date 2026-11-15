/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

/*
 * The loading notice: the modern menus' wait box with a line of text and a bar, shown over whatever
 * the screen holds while a long load runs. With the classic menus nothing is shown and every call
 * does nothing. Calls never run game logic or pump messages.
 *
 * Begin and End nest: the outermost pair shows and hides the notice, and an inner pair, such as a
 * saved game loaded from a dialog that began its own notice, shares it. The notice leaves the screen
 * under it in view, such as a loading picture, unless the outermost Begin asks for the menu backdrop.
 */
void Load_Progress_Begin(char const * text, bool backdrop = false);
void Load_Progress_End(void);

// Names the part of the load now starting and moves the bar to fraction, from 0 to 1.
void Load_Progress_Step(char const * text, double fraction);

// While one of these lives, steps leave the notice as it is, for a load nested in another
// whose steps would mislead, such as the menu battle loading under the start-up notice.
class LoadProgressMuteClass
{
	public:
		explicit LoadProgressMuteClass(bool mute = true);
		~LoadProgressMuteClass(void);

		LoadProgressMuteClass(LoadProgressMuteClass const &) = delete;
		LoadProgressMuteClass & operator=(LoadProgressMuteClass const &) = delete;

	private:
		bool Muting;
};

// Shows the notice for the life of the object, as a Begin and End pair.
class LoadProgressScopeClass
{
	public:
		explicit LoadProgressScopeClass(char const * text);
		~LoadProgressScopeClass(void);

		LoadProgressScopeClass(LoadProgressScopeClass const &) = delete;
		LoadProgressScopeClass & operator=(LoadProgressScopeClass const &) = delete;
};
