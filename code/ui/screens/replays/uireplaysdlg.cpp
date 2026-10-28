/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_ui.h"
#include "csf.h"
#include "dbgprint.h"
#include "gamedirs.h"
#include "init.h"
#include "language/language.h"
#include "msgbox.h"
#include "nettiming.h"
#include "replay.h"
#include "replaylist.h"
#include "session.h"
#include "ui/screens/replays/uireplays.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uiview.h"

#include <filesystem>
#include <system_error>


namespace
{

std::string Text(char const * label, char const * fallback)
{
	std::string const text = StringTable.Find_UTF8(label);
	return(text.empty() ? std::string(fallback) : text);
}


/// <summary>The seconds a number of frames take at the game speed the recording was made at.</summary>
std::uint32_t Seconds_Of(std::uint32_t frames, int game_speed)
{
	unsigned int rate = NetTiming::Solo_Game_Speed_Frame_Rate(game_speed);
	return(frames / (rate > 0 ? rate : 15));
}


UIReplayRow Row_Of(ReplayList::EntryType const & entry)
{
	UIReplayRow row;
	row.File = entry.File;

	if (entry.Load != ReplayFile::LoadResult::OK) {
		row.Map = entry.File;
		row.Fault = ReplayFile::Load_Result_Text(entry.Load);
		return(row);
	}

	ReplayFile::HeaderType const & header = entry.Header;
	row.Map = header.MapTitle.empty() ? header.Map : header.MapTitle;
	row.Players = ReplayList::Describe_Players(header);
	row.Date = ReplayList::Format_Date(entry.When);
	row.Length = ReplayList::Format_Length(Seconds_Of(entry.Frames, header.GameSpeed));
	row.Result = entry.Result == ReplayFile::ResultType::WON ? "Won" : entry.Result == ReplayFile::ResultType::LOST ? "Lost" : "-";
	row.Fault = Replay_Playability(header);
	return(row);
}


void Fill_Rows(UIReplaysState & state)
{
	state.Rows.clear();
	for (ReplayList::EntryType const & entry : ReplayList::Scan(Replay_Name(""))) {
		state.Rows.push_back(Row_Of(entry));
	}
	if (state.Selected >= (int)state.Rows.size()) {
		state.Selected = (int)state.Rows.size() - 1;
	}
}

}


bool UI_Replays_Dialog(std::string & file)
{
	// A map in a map pack is found only once the packs are mounted.
	Mount_Map_Packs();

	UIReplaysState state;
	state.Title = Text("GUI:Replays", "Replays");
	state.PlayCaption = Text("GUI:PlayReplay", "Play");
	state.DeleteCaption = Text("GUI:DeleteReplay", "Delete");
	state.BackCaption = Text("GUI:Back", "Back");
	state.EmptyText = "No replays yet. Turn on Record replays in Options, play a skirmish, and it is listed here.";
	Fill_Rows(state);

	while (true) {
		UIReplaysPresenterClass presenter(state);
		std::unique_ptr<UIViewClass> view = UI_Replays_View(presenter);

		UIResult const result = UIShell.Run_Modal(*view, []() {
			bool ended = UI_Service_Game();
			Title_Screen_Restore();
			return(ended);
		});
		if (result != UI_RESULT_ACCEPTED || presenter.State.Selected < 0 || presenter.State.Selected >= (int)state.Rows.size()) {
			return(false);
		}

		state.Selected = presenter.State.Selected;
		UIReplayRow const & row = state.Rows[(std::size_t)state.Selected];

		if (presenter.Choice == UI_REPLAYS_DELETE) {
			std::string const question = Text("GUI:DeleteReplayQuery", "Delete this replay?") + "\n" + row.File;
			std::string const yes = Text("GUI:Yes", "Yes");
			std::string const no = Text("GUI:No", "No");
			if (WWMessageBox()._Process(question.c_str(), 1, yes.c_str(), no.c_str(), NULL) == 0) {
				std::error_code error;
				std::filesystem::remove(std::filesystem::path(Replay_Name(row.File.c_str())), error);
				if (error) {
					DebugString("Replays: %s could not be deleted: %s\n", row.File.c_str(), error.message().c_str());
				}
				Fill_Rows(state);
			}
			continue;
		}

		if (!row.Fault.empty()) {
			WWMessageBox().Process(row.Fault.c_str(), TXT_OK);
			continue;
		}

		file = row.File;
		return(true);
	}
}
