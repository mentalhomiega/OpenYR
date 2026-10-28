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


struct UIReplayRow
{
	// What the list shows for the recording.
	std::string Map;
	std::string Players;
	std::string Date;
	std::string Length;
	std::string Result;

	// The file, as it is named in the Replays folder.
	std::string File;

	// Why this game cannot play the recording; empty when it can.
	std::string Fault;
};


struct UIReplaysState
{
	std::string Title;
	std::string PlayCaption;
	std::string DeleteCaption;
	std::string BackCaption;
	std::string EmptyText;
	std::vector<UIReplayRow> Rows;
	int Selected = 0;
};


enum UIReplaysChoice
{
	UI_REPLAYS_NONE,
	UI_REPLAYS_PLAY,
	UI_REPLAYS_DELETE,
};


class UIReplaysPresenterClass : public UIPresenterClass
{
	public:
		explicit UIReplaysPresenterClass(UIReplaysState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		// The text under the list: the reason the selected recording cannot play, or empty.
		std::string Status(void) const;

		UIReplaysState State;
		UIReplaysChoice Choice = UI_REPLAYS_NONE;
};


std::unique_ptr<UIViewClass> UI_Replays_View(UIReplaysPresenterClass & presenter);

// Lists the recordings in the Replays folder. Delete asks first and removes the file. Play answers true
// with the file name of the picked recording, which the caller starts; a recording this game cannot
// play is explained and the list stays open. Back or Escape answers false.
bool UI_Replays_Dialog(std::string & file);
