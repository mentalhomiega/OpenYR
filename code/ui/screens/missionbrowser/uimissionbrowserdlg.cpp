/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "campaign.h"
#include "campaignname.hh"
#include "ccfile.h"
#include "ccini.h"
#include "csf.h"
#include "data.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "gamedlg.h"
#include "globals.h"
#include "gscreen.h"
#include "init.h"
#include "loadscreen.h"
#include "options.h"
#include "palette.h"
#include "pcx.h"
#include "scenario.h"
#include "ui/screens/missionbrowser/uimissionbrowser.h"
#include "ui/uienginehost.h"
#include "ui/uipreview.h"
#include "ui/uiview.h"
#include "winstub.h"

#include <cstring>
#include <memory>
#include <optional>
#include <string>


namespace
{

// A label the string table lacks reads as MISSING:'<label>', as the objectives screen shows it.
std::string Label_Text(char const * label)
{
	if (StringTable.Find(label) == nullptr) {
		return(std::string("MISSING:'") + label + "'");
	}
	return(StringTable.Find_UTF8(label));
}


/// <summary>
/// The briefing a mission's objectives screen gives, read from the files before the map loads. The sources and their
/// order follow Briefing_Text in restate.cpp: the text MISSIONMD.INI names, then the map's own briefing, then the
/// mission INI's text, and at last Brief:Error when MISSIONMD.INI exists.
/// </summary>
std::string Mission_Briefing(char const * map)
{
	CCFileClass database("MISSIONMD.INI");
	bool const has_database = database.Is_Available();
	if (has_database) {
		CCINIClass missions;
		missions.Load(database, false);
		std::string const label = missions.Get_String(map, "Briefing");
		if (!label.empty()) {
			return(Label_Text(label.c_str()));
		}
	}

	char text[1024] = "";
	CCFileClass scenario(map);
	if (scenario.Is_Available()) {
		CCINIClass ini;
		ini.Load(scenario, false);
		ini.Get_TextBlock("Briefing", text, sizeof(text));
	}

	if (text[0] == '\0') {
		CCFileClass mission_file("MISSION.INI");
		if (mission_file.Is_Available()) {
			CCINIClass ini;
			ini.Load(mission_file, false);
			// Red Alert 2 names a string table label here, and Yuri's Revenge's text blocks name a section.
			std::string const entry = ini.Get_String(map, "Briefing");
			if (!entry.empty() && StringTable.Find(entry.c_str()) != nullptr) {
				return(Label_Text(entry.c_str()));
			}
			if (!entry.empty()) {
				ini.Get_TextBlock(entry.c_str(), text, sizeof(text));
			}
		}
	}

	if (text[0] == '\0' && has_database) {
		return(Label_Text("Brief:Error"));
	}
	return(std::string(text));
}


/// <summary>
/// Groups a campaign by its archive and its map's side. A map's name starts with ALL for the Allied missions and SOV for the
/// Soviet ones; a campaign of any other name, such as a tutorial, is listed under other campaigns.
/// </summary>
UIBrowserGroupType Group_Of(CampaignClass const & campaign)
{
	if (campaign.DebugOnly) {
		return(UI_BROWSER_TESTING);
	}

	bool const ra2 = campaign.ScenarioArchive == "RA2";
	std::string const map = campaign.ScenarioName;
	if (map.compare(0, 3, "ALL") == 0) {
		return(ra2 ? UI_BROWSER_RA2_ALLIED : UI_BROWSER_YR_ALLIED);
	}
	if (map.compare(0, 3, "SOV") == 0) {
		return(ra2 ? UI_BROWSER_RA2_SOVIET : UI_BROWSER_YR_SOVIET);
	}
	return(UI_BROWSER_OTHER);
}


/// <summary>
/// Gives a mission's title. A Red Alert 2 mission takes the name MISSION.INI gives its map under UIName=, resolved
/// through the string table, as the game names it. Any other mission, or a Red Alert 2 map that names no resolvable
/// label, keeps the campaign's description.
/// </summary>
std::string Mission_Title(CampaignClass const & campaign, CCINIClass const & ra2_missions)
{
	if (campaign.ScenarioArchive == "RA2") {
		std::string const label = ra2_missions.Get_String(campaign.ScenarioName, "UIName");
		if (!label.empty() && StringTable.Find(label.c_str()) != nullptr) {
			return(StringTable.Find_UTF8(label.c_str()));
		}
	}

	std::string title = StringTable.Find_UTF8(campaign.Description);
	return(title.empty() ? std::string(campaign.Description) : title);
}


/// <summary>
/// Lists the campaigns that can be offered and the mission each starts. A campaign whose map is one list already holds is
/// left out, so a campaign listed by two battle files appears once; a testing campaign gives way to a regular one.
/// </summary>
UIMissionBrowserState Mission_Browser_State(void)
{
	UIMissionBrowserState state;

	CCINIClass ra2_missions;
	CCFileClass ra2_file("MISSION.INI");
	if (ra2_file.Is_Available()) {
		ra2_missions.Load(ra2_file, false);
	}

	for (int level = 0; level < OptionsClass::MAX_DIFFICULTY_SETTING; level++) {
		UIBrowserOption option;
		option.Label = Fetch_String(GameDifficultyNames[level]);
		option.Value = level;
		state.Difficulties.push_back(option);
	}
	state.Difficulty = Options.Difficulty;

	for (int index = 0; index < Campaigns.Count(); index++) {
		CampaignClass * campaign = Campaigns[index];
		if (campaign == nullptr || !Campaign_Available(campaign) || !Campaign_Is_Mission_File(campaign->ScenarioName)) {
			continue;
		}

		UIBrowserMission mission;
		mission.Key = campaign->Name();
		mission.Map = campaign->ScenarioName;
		mission.Campaign = index;
		mission.Group = Group_Of(*campaign);

		mission.Title = Mission_Title(*campaign, ra2_missions);

		if (!CCFileClass(mission.Map.c_str()).Is_Available()) {
			mission.Playable = false;
			mission.Reason = "This mission cannot start: its map " + mission.Map + " is not in the game files.";
		}

		int existing = -1;
		for (int mine = 0; mine < (int)state.Missions.size(); mine++) {
			if (_stricmp(state.Missions[mine].Map.c_str(), mission.Map.c_str()) == 0) {
				existing = mine;
			}
		}
		if (existing < 0) {
			state.Missions.push_back(mission);
		} else if (state.Missions[existing].Group == UI_BROWSER_TESTING && mission.Group != UI_BROWSER_TESTING) {
			state.Missions[existing] = mission;
		}
	}

	// The mission chosen last time is selected again; otherwise the first mission that can start is.
	for (int index = 0; index < (int)state.Missions.size(); index++) {
		if (_stricmp(state.Missions[index].Key.c_str(), Options.LastMission.c_str()) == 0) {
			state.Selected = index;
			break;
		}
	}
	if (state.Selected < 0) {
		state.Selected = UI_Mission_Browser_First_Playable(state);
	}

	UI_Mission_Browser_Build_Groups(state);
	return(state);
}


/// <summary>
/// Reads a mission's loading picture entry from one of the mission files. The palette is the entry's LS800BkgdPal=, or
/// else the picture's own name with a .PAL extension, which the Yuri's Revenge entries use too.
/// </summary>
/// <param name="file">The mission file, MISSIONMD.INI or MISSION.INI.</param>
/// <param name="map">The mission's map file name, which names the file's section.</param>
/// <param name="small">True to read the 640 by 480 picture's name, false for the 800 by 600 one.</param>
/// <param name="picture">Receives the picture's SHP file name.</param>
/// <param name="palette">Receives the palette file name for the 800 by 600 picture.</param>
/// <returns>Returns with false when the file is missing or the mission names no picture in it.</returns>
bool Read_Art_Entry(char const * file, char const * map, bool small, std::string & picture, std::string & palette)
{
	CCFileClass database(file);
	if (!database.Is_Available()) {
		return(false);
	}
	CCINIClass ini;
	ini.Load(database, false);

	picture = ini.Get_String(map, small ? "LS640BkgdName" : "LS800BkgdName");
	if (picture.empty()) {
		return(false);
	}

	palette = ini.Get_String(map, "LS800BkgdPal");
	if (palette.empty()) {
		palette = picture.substr(0, picture.find('.')) + ".PAL";
	}
	return(true);
}


/// <summary>
/// Makes the picture a campaign mission shows, for the browser's picture box. The picture is the one the mission's
/// entry names: MISSIONMD.INI for Yuri's Revenge, then MISSION.INI for Red Alert 2, which the game's own loading screen
/// does not read. It is drawn at this screen's layout. A mission with no loadable picture shows its side's loading
/// backdrop, which the game picks from two at random; the browser shows the first of the two. The image is set only
/// when a picture loads; the caller then falls back to the map's preview.
/// </summary>
/// <returns>Returns with true when a picture is in the image.</returns>
bool Mission_Loading_Picture(CampaignClass const & campaign, char const * map, UIMapPreviewImage & image)
{
	int const generation = image.Generation + 1;
	image = UIMapPreviewImage();
	image.Generation = generation;

	// The loading art lives in archives that are mounted only while a loading screen runs, so they are mounted for this
	// picture and released when the function returns.
	LoadScreenClass art;
	art.Mount_Art();

	// The same test the loading screen makes of the screen's width.
	bool const small = (HiddenSurface->Get_Width() == 640);

	CCINIClass scenario;
	CCFileClass scenario_file(map);
	bool const has_scenario = scenario_file.Is_Available();
	if (has_scenario) {
		scenario.Load(scenario_file, false);
	}

	Point2D const size = LoadScreenClass::Campaign_Picture_Size(small);
	DSurface picture_surface(size.X, size.Y);
	picture_surface.Fill(0);
	char const * const files[] = {"MISSIONMD.INI", "MISSION.INI"};
	for (char const * file : files) {
		std::string picture;
		std::string palette;
		if (Read_Art_Entry(file, map, small, picture, palette)
			&& LoadScreenClass::Draw_Campaign_Picture(picture, palette, small, has_scenario ? &scenario : NULL, picture_surface, Point2D(0, 0))) {
			return(UI_Surface_Preview_Image(&picture_surface, image));
		}
	}

	// The backdrop the game shows for this side, at this resolution. The choice is the side's first picture, so no random number is drawn.
	int const side = Campaign_Load_Side(campaign);
	Point2D pos;
	char const * backdrop = Load_Background_Name(side, side << 1, pos);
	CCFileClass backdrop_file(backdrop);
	int width = 0;
	int height = 0;
	if (!backdrop_file.Is_Available() || !Read_PCX_Size(backdrop_file, width, height)) {
		return(false);
	}

	DSurface backdrop_surface(width, height);
	backdrop_surface.Fill(0);
	PaletteClass palette;
	Load_Title_Screen(backdrop, &backdrop_surface, &palette);
	return(UI_Surface_Preview_Image(&backdrop_surface, image));
}


class UIMissionBrowserEngineServiceClass : public UIMissionBrowserServiceClass
{
	public:
		virtual void Show_Mission(UIMissionBrowserState & state, int index) override
		{
			UIBrowserMission const & mission = state.Missions[index];
			state.Briefing = Mission_Briefing(mission.Map.c_str());
			if (!Mission_Loading_Picture(*Campaigns[mission.Campaign], mission.Map.c_str(), state.Preview)) {
				UI_Map_File_Preview_Image(mission.Map.c_str(), state.Preview);
			}
			state.HasPreview = state.Preview.Width > 0 && state.Preview.Height > 0;
		}
};

}


/// <summary>
/// Shows the mission browser. Play returns the campaign of the selected mission and sets the difficulty to the one chosen;
/// Back, Escape, or a list with no mission that can be offered, returns nothing and leaves the difficulty as it was. The
/// selected mission is remembered either way.
/// </summary>
std::optional<int> UI_Mission_Browser_Dialog(void)
{
	UIMissionBrowserState state = Mission_Browser_State();
	if (state.Missions.empty()) {
		DebugString("Mission browser: no campaign can be offered\n");
		return(std::nullopt);
	}

	UIMissionBrowserEngineServiceClass service;
	UIMissionBrowserPresenterClass presenter(service, state);
	std::unique_ptr<UIViewClass> view = UI_Mission_Browser_View(presenter);
	bool const played = UI_Run_Modal(*view) == UI_RESULT_ACCEPTED && presenter.Picked.has_value();

	UIMissionBrowserState const & final_state = presenter.State;
	if (final_state.Selected >= 0 && final_state.Selected < (int)final_state.Missions.size()) {
		Options.LastMission = final_state.Missions[final_state.Selected].Key;
	}

	if (!played) {
		Options.Save_Settings();
		return(std::nullopt);
	}

	Options.Difficulty = final_state.Difficulty;
	Options.Save_Settings();
	DebugString("Mission browser: %s at difficulty %d\n", final_state.Missions[final_state.Selected].Map.c_str(), Options.Difficulty);
	return(presenter.Picked);
}
