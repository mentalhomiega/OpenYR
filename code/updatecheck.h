/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// The beta update offer on the title menus. The check reads the same release feed as the launcher,
// on a worker thread, and the menus ask for the result every frame. docs/LAUNCHER.md describes the flow.

#pragma once

#include <string>


struct UpdateOfferType
{
	// Whether the title menus show the offer panel.
	bool Visible = false;

	// "Beta 34 is available" and "You have Beta 33".
	std::string Title;
	std::string Line;

	// Why the last press of Update did nothing; empty when none has failed.
	std::string Error;
};


// Starts the check once per run, from the install record beside Game.exe and [Update] in OPENTS.INI.
// Nothing happens without a record, with [Update] Check=no, or after the first call.
void Update_Check_Begin(void);

// Starts the check for the given installed build and feed, whatever the files say. A feed that starts with
// '{' is the release text itself. For the tests.
void Update_Check_Begin(std::string const & build, std::string const & feed);

// Finishes a check that has ended and returns what the title menus show. Call it from the main thread.
UpdateOfferType Update_Offer(void);

// Hides the offer until the game is started again.
void Update_Later(void);

// Starts the launcher, which updates once this game has closed. True when it started; the caller then
// ends the game. False leaves the reason in the offer's Error.
bool Update_Now(void);
