/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

#include "always.h"

#include "score.h"

#include "_keyboar.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_surface.h"
#include "_timer.h"
#include "audio/audioengine.h"
#include "autotest.h"
#include "conquer.h"
#include "csf.h"
#include "data.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "globals.h"
#include "houstype.h"
#include "keyboard.h"
#include "language/language.h"
#include "mixfile.h"
#include "msgloop.h"
#include "scenario.h"
#include "session.h"
#include "surface.h"
#include "theme.h"
#include "winstub.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>


#define NUMFAMENAMES				9
#define MAX_FAMENAME_LENGTH	104

/*
 * The campaign results dialog (dialog 3015) lists two places of the hall of fame, and each one
 * gets a name field of this many dialog units.
 */
#define FAME_PLACES					2

void const * Beepy6;
int ControlQ;	// cheat key to skip past score/mapsel screens
bool StillUpdating;

struct Fame {
	char	name[MAX_FAMENAME_LENGTH];
	int	score;
	int	level;
	int	side;
};

namespace {

/*
 * The results screen's layout, in dialog units of gamemd's Campaign Results dialog (dialog 3015)
 * on the 800 by 600 stage.
 */
Rect const PICTURE_BOX = ScoreStageClass::Dialog_Rect(70, 84, 282, 141);
Rect const TABLE_RECT = ScoreStageClass::Dialog_Rect(66, 28, 280, 36);
Rect const BUTTON_RECT = ScoreStageClass::Dialog_Rect(425, 130, 108, 23);
Rect const PROMPT_RECT = ScoreStageClass::Dialog_Rect(10, 356, 303, 12);

constexpr int HEADING_Y = 31;
constexpr int ROW_Y[2] = { 40, 51 };
constexpr int NAME_X = 83;
constexpr int KILLS_RIGHT = 186;
constexpr int LOSSES_RIGHT = 236;
constexpr int BUILT_RIGHT = 286;
constexpr int SCORE_RIGHT = 334;

constexpr int PLACE_X[FAME_PLACES] = { 123, 240 };
constexpr int PLACE_LABEL_X[FAME_PLACES] = { 113, 233 };

/*
 * The picture's animation is 44 frames of 424 by 220 pixels, one pass of it runs while the screen
 * comes up, and then it loops until the player continues.
 */
constexpr int ANIMATION_FRAMES = 44;

/*
 * The side letter names the art, as on the multiplayer screen: A for the first side in the rules'
 * [Sides] list, S for the second, and Y for any other side.
 */
char const * Art_Prefix(void)
{
	char letter = ScoreStageClass::Side_Letter();
	if (letter == 'A') return("ASCR");
	if (letter == 'S') return("SSCR");
	return("SYCR");
}

char const * Palette_Name(void)
{
	char letter = ScoreStageClass::Side_Letter();
	if (letter == 'A') return("ASCORE.PAL");
	if (letter == 'S') return("SSCORE.PAL");
	return("YSCORE.PAL");
}

/*
 * Waits for the given number of ticks while the window keeps its messages.
 */
void Score_Delay(int ticks)
{
	for (int i = 0; i < ticks; i++) {
		Call_Back();
		Windows_Message_Handler();
		Sleep(16);
	}
}

/*
 * Formats an elapsed time the way the score screens print it.
 */
void Format_Elapsed(char * buffer, unsigned time)
{
	if (time / 3600 > 99) {
		time = (100 * 3600) - 1;
	}
	unsigned hours = time / 3600;
	time -= hours * 3600;
	unsigned minutes = time / 60;
	time -= minutes * 60;
	std::string format = Fetch_String_UTF8("TXT_TIME_FORMAT_HOURS");
	sprintf(buffer, format.c_str(), hours, minutes, time);
}

/*
 * The local player's name, the profile handle the menu uses, or the no-name text when it is empty.
 */
char const * Player_Name(void)
{
	return(Session.Handle[0] != 0 ? Session.Handle : Fetch_String(TXT_NONAME));
}

}

/// <summary>
/// Shows the campaign results screen after a mission.
/// The screen is drawn on the side's art in the layout of gamemd's Campaign Results dialog. It lists
/// the player, and the first enemy house, with the figures of the mission and the efficiency total.
/// The two best places of the hall of fame are shown below the table. A score that makes one of them
/// is entered under the player's name, without any typing.
/// </summary>
void ScoreClass::Presentation(void)
{
	if (!Stage.Open()) {
		return;
	}

	Hide_Mouse();
	Keyboard->Clear();
	Stage.Clear_Screen();

	Theme.Stop();
	Theme.Play_Song(Theme.From_Name("SCORE"));

	ScoreSnds = {
		new SfxEntry("Wipe", "WIPE.AUD"),
		new SfxEntry("Emblem", "EMBLEM.AUD"),
		new SfxEntry("BestBox", "BESTBOX.AUD"),
		new SfxEntry("Efficiency", "EFFICIEN.AUD"),
		new SfxEntry("BarGraph", "BARGRAPH.AUD"),
		new SfxEntry("Type", "TYPE.AUD"),
		new SfxEntry("Back", "SCOLD8.AUD")
	};

	DoSound("Wipe", 256);

	/*
	 * The side's palette, background and emblem animation.
	 */
	Stage.Load_Palette(Palette_Name());

	char name[32];
	sprintf(name, "%sBK%s.SHP", Art_Prefix(), Stage.Is_Small() ? "SM" : "MD");
	ShapeSet * background = Stage.Load_Shape(name);
	if (background != NULL) {
		Stage.Draw_Shape_Frame(background, 0, Rect(0, 0, 632, 568));
		ScoreStageClass::Free_Shape(background);
	}

	sprintf(name, "%sAMD.SHP", Art_Prefix());
	ShapeSet * animation = Stage.Load_Shape(name);

	/*
	 * The figures of the two rows. The score is the efficiency total, and the built figure is the
	 * share of the biggest spender's credits.
	 */
	HouseClass * enemy = NULL;
	for (int i = 0; i < Houses.Count() && enemy == NULL; i++) {
		HouseClass * hptr = Houses[i];
		if (hptr != NULL && hptr != PlayerPtr && !hptr->Class->IsMultiplayPassive && !hptr->IsObserver && !PlayerPtr->Is_Ally(hptr)) {
			enemy = hptr;
		}
	}

	unsigned int most_spent = 0;
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * hptr = Houses[i];
		if (hptr != NULL && !hptr->Class->IsMultiplayPassive && !hptr->IsObserver) {
			most_spent = std::max(most_spent, hptr->CreditsSpent);
		}
	}

	struct Figures {
		std::string Name;
		int Kills;
		int Losses;
		int Built;
		int Score;
	} rows[2];

	HouseClass * houses[2] = { PlayerPtr, enemy };
	int const row_count = (enemy != NULL) ? 2 : 1;
	for (int row = 0; row < row_count; row++) {
		HouseClass * hptr = houses[row];
		rows[row].Name = Session.Shown_Name(hptr);
		rows[row].Kills = 0;
		rows[row].Losses = 0;
		for (unsigned int i = 0; i < sizeof(hptr->UnitsKilled) / sizeof(hptr->UnitsKilled[0]); i++) {
			rows[row].Kills += hptr->UnitsKilled[i];
		}
		for (unsigned int i = 0; i < sizeof(hptr->BuildingsKilled) / sizeof(hptr->BuildingsKilled[0]); i++) {
			rows[row].Kills += hptr->BuildingsKilled[i];
		}
		rows[row].Losses = hptr->UnitsLost + hptr->BuildingsLost;
		rows[row].Built = (most_spent > 0) ? (int)((double)hptr->CreditsSpent / (double)most_spent * 100.0) : 0;
		rows[row].Score = Do_Calc(hptr);
	}
	unsigned int total = (unsigned int)rows[0].Score;

	/*
	 * The static part of the screen: title, time, headings and the continue button.
	 */
	std::string title = Fetch_String_UTF8("GUI:CampaignResults");
	Stage.Print_Centre(title.c_str(), ScoreStageClass::Dialog_X(424), ScoreStageClass::Dialog_X(108), ScoreStageClass::Dialog_Y(1));

	char buffer[128];
	Format_Elapsed(buffer, Scen->ElapsedTimer / 60);
	Stage.Print_Centre(buffer, ScoreStageClass::Dialog_X(167), ScoreStageClass::Dialog_X(87), ScoreStageClass::Dialog_Y(19));

	std::string heading_player = Fetch_String_UTF8("GUI:Player");
	std::string heading_kills = Fetch_String_UTF8("GUI:Kills");
	std::string heading_losses = Fetch_String_UTF8("GUI:Losses");
	std::string heading_built = Fetch_String_UTF8("GUI:Built");
	std::string heading_score = Fetch_String_UTF8("GUI:Score");
	Stage.Print_Left(heading_player.c_str(), ScoreStageClass::Dialog_X(NAME_X), ScoreStageClass::Dialog_Y(HEADING_Y));
	Stage.Print_Right(heading_kills.c_str(), ScoreStageClass::Dialog_X(KILLS_RIGHT), ScoreStageClass::Dialog_Y(HEADING_Y));
	Stage.Print_Right(heading_losses.c_str(), ScoreStageClass::Dialog_X(LOSSES_RIGHT), ScoreStageClass::Dialog_Y(HEADING_Y));
	Stage.Print_Right(heading_built.c_str(), ScoreStageClass::Dialog_X(BUILT_RIGHT), ScoreStageClass::Dialog_Y(HEADING_Y));
	Stage.Print_Right(heading_score.c_str(), ScoreStageClass::Dialog_X(SCORE_RIGHT), ScoreStageClass::Dialog_Y(HEADING_Y));

	RGBClass const edge(150, 150, 150);
	Stage.Canvas()->Draw_Rect(BUTTON_RECT, DSurface::Build_Hicolor_Pixel(edge));
	std::string caption = Fetch_String_UTF8("GUI:Continue");
	Stage.Print_Centre(caption.c_str(), BUTTON_RECT.X, BUTTON_RECT.Width, BUTTON_RECT.Y + (BUTTON_RECT.Height - Stage.Text_Height()) / 2);

	Stage.Save_Base();
	Stage.Present(Rect(0, 0, 800, 600));

	/*
	 * The emblem animation plays once as the screen comes up.
	 */
	if (animation != NULL) {
		DoSound("Emblem", 256);
		Rect const frame_box(PICTURE_BOX.X, PICTURE_BOX.Y, 424, 220);
		for (int frame = 0; frame < ANIMATION_FRAMES; frame++) {
			Stage.Draw_Shape_Frame(animation, frame, frame_box);
			Stage.Present(frame_box);
			Score_Delay(2);
		}
	}

	/*
	 * Each row's name comes up, then the figures count up together, and the score follows.
	 */
	DoSound("BestBox", 256);
	int const left = ScoreStageClass::Dialog_X(NAME_X);
	int const right = ScoreStageClass::Dialog_X(SCORE_RIGHT);
	for (int row = 0; row < row_count; row++) {
		int const y = ScoreStageClass::Dialog_Y(ROW_Y[row]);
		Stage.Print_Left(rows[row].Name.c_str(), left, y);
		Rect const line(left, y, right - left, Stage.Text_Height());
		Stage.Keep(line);
		Stage.Present(line);
		Score_Delay(6);
	}

	DoSound("Efficiency", 128);
	int const steps = 100;
	for (int step = 1; step <= steps; step++) {
		Stage.Restore(TABLE_RECT);
		for (int row = 0; row < row_count; row++) {
			Print_Row_Figures(row, rows[row].Kills * step / steps, rows[row].Losses * step / steps, rows[row].Built * step / steps, 0);
		}
		Stage.Present(TABLE_RECT);
		DoSound("BarGraph", 128);
		Score_Delay(1);
	}
	for (int step = 1; step <= steps; step++) {
		Stage.Restore(TABLE_RECT);
		for (int row = 0; row < row_count; row++) {
			Print_Row_Figures(row, rows[row].Kills, rows[row].Losses, rows[row].Built, rows[row].Score * step / steps);
		}
		Stage.Present(TABLE_RECT);
		DoSound("BarGraph", 128);
		Score_Delay(1);
	}
	Stage.Restore(TABLE_RECT);
	for (int row = 0; row < row_count; row++) {
		Print_Row_Figures(row, rows[row].Kills, rows[row].Losses, rows[row].Built, rows[row].Score);
	}
	Stage.Present(TABLE_RECT);

	/*
	 * The hall of fame keeps the best nine scores, and its two best places are shown here. A score
	 * that enters the list takes its name from the player.
	 */
	Fame hallfame[NUMFAMENAMES];
	memset(hallfame, 0, sizeof(hallfame));
	CCFileClass file;
	file.Set_Name(FAME_FILE_NAME);
	if (file.Is_Available() == true) {
		file.Read(hallfame, sizeof(hallfame));
		file.Close();
	}

	if (hallfame[NUMFAMENAMES-1].score >= (int)total) {
		hallfame[NUMFAMENAMES-1].score = 0;
	}
	int index = 0;
	for (index = 0; index < NUMFAMENAMES; index++) {
		if ((int)total > hallfame[index].score) {
			if (index < (NUMFAMENAMES-1)) {
				for (int i = (NUMFAMENAMES-1); i > index; i--) {
					hallfame[i] = hallfame[i-1];
				}
			}
			std::snprintf(hallfame[index].name, sizeof(hallfame[index].name), "%s", Player_Name());
			hallfame[index].score = (int)total;
			hallfame[index].level = Scen->Scenario;
			hallfame[index].side = PlayerPtr->Class->Side;
			break;
		}
	}

	DoSound("Emblem", 256);
	for (int place = 0; place < FAME_PLACES; place++) {
		std::string label = Fetch_String_UTF8(place == 0 ? "GUI:FirstPlace" : "GUI:SecondPlace");
		Stage.Print_Left(label.c_str(), ScoreStageClass::Dialog_X(PLACE_LABEL_X[place]), ScoreStageClass::Dialog_Y(232));
		if (hallfame[place].score > 0) {
			sprintf(buffer, "%d%%", hallfame[place].score);
			Stage.Print_Left(buffer, ScoreStageClass::Dialog_X(PLACE_X[place]), ScoreStageClass::Dialog_Y(254));
		}
		Rect const field = ScoreStageClass::Dialog_Rect(PLACE_X[place], 242, 55, 10);
		Stage.Print_Left(hallfame[place].name, field.X, field.Y);
		Rect const place_area = ScoreStageClass::Dialog_Rect(PLACE_LABEL_X[place], 232, 80, 34);
		Stage.Keep(place_area);
		Stage.Present(place_area);
	}

	if (file.Open(FAME_FILE_NAME, FileClass::WRITE)) {
		file.Write(hallfame, sizeof(hallfame));
		file.Close();
	}

	/*
	 * The prompt, and the screen waits for the player with the emblem looping.
	 */
	std::string prompt = Fetch_String_UTF8("TXT_CLICK_CONTINUE");
	Stage.Print_Left(prompt.c_str(), PROMPT_RECT.X, PROMPT_RECT.Y);
	Stage.Present(PROMPT_RECT);

	AutoTest_Score_Shown();

	Show_Mouse();
	Wait_For_Click(animation);
	Hide_Mouse();

	Theme.Stop(true);
	Theme.Queue_Song(THEME_NONE);

	ScoreSnds.Clear();
	if (animation != NULL) {
		ScoreStageClass::Free_Shape(animation);
	}
	Stage.Close();

	Stage.Clear_Screen();
	Show_Mouse();
	Keyboard->Clear();
}


/// <summary>
/// Determines the efficiency rating for the score screen.
/// The rating weighs how much of what the house and its allies built came through the
/// mission alive against how much of the money they had to spend went unspent.
/// </summary>
/// <param name="house">The house being rated -- normally the player.</param>
/// <returns>Returns with the efficiency rating, expressed as a percentage.</returns>
unsigned int ScoreClass::Do_Calc(HouseClass * house)
{
	int remain = 0;
	int total = 0;

	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * hptr = Houses[i];
		if (house->Is_Ally(hptr) && hptr->Is_Ally(house) || house == hptr) {
			remain += hptr->ABQuantity.Total();
			remain += hptr->AUQuantity.Total();
			remain += hptr->AIQuantity.Total();
			remain += hptr->AAQuantity.Total();
			total += hptr->BuildingsLost;
			total += hptr->UnitsLost;
		}
	}

	total += remain;
	DebugString("Remaining: %ld, Total: %ld\n", remain, total);

	float build_eco = total > 0 ? ((float)remain / total) : 0.0f;

	DebugString("BuildEconomy = %f\n", build_eco);

	DebugString("Money: %ld, Harvested: %ld, Initial: %ld\n", house->Available_Money(), house->HarvestedCredits, house->Control.InitialCredits);

	total = house->HarvestedCredits + house->Control.InitialCredits;
	float money_eco = total > 0 ? (float)house->Available_Money() / (float)total : 0.0f;

	DebugString("MoneyEconomy = %f\n", money_eco);

	float eco = money_eco + build_eco;

	unsigned int efficiency = (unsigned int)((eco / 2.0) * 100.0);

	DebugString("Efficiency: %ld\n", efficiency);

	return(efficiency);
}


/// <summary>
/// Prints one row's figures on the results table.
/// </summary>
void ScoreClass::Print_Row_Figures(int row, int kills, int losses, int built, int score)
{
	char buffer[32];
	int const y = ScoreStageClass::Dialog_Y(ROW_Y[row]);

	sprintf(buffer, "%d", kills);
	Stage.Print_Right(buffer, ScoreStageClass::Dialog_X(KILLS_RIGHT), y);
	sprintf(buffer, "%d", losses);
	Stage.Print_Right(buffer, ScoreStageClass::Dialog_X(LOSSES_RIGHT), y);
	sprintf(buffer, "%d%%", built);
	Stage.Print_Right(buffer, ScoreStageClass::Dialog_X(BUILT_RIGHT), y);
	sprintf(buffer, "%d%%", score);
	Stage.Print_Right(buffer, ScoreStageClass::Dialog_X(SCORE_RIGHT), y);
}


/// <summary>
/// Waits for the player to press Continue, with the emblem looping while the screen is up.
/// Only a left click on the Continue button ends the wait; keys and clicks elsewhere are ignored.
/// </summary>
/// <param name="animation">The emblem animation to loop, or NULL to wait with a still picture.</param>
void ScoreClass::Wait_For_Click(ShapeSet const * animation)
{
	Keyboard->Clear();
	if (AutoTest_Post_Win()) {
		return;
	}

	int frame = 0;
	int ticks = 0;
	bool running = true;
	Rect const frame_box(PICTURE_BOX.X, PICTURE_BOX.Y, 424, 220);

	while (running) {
		Call_Back();
		Windows_Message_Handler();
		if (Keyboard->Check()) {
			int key = Keyboard->Get();
			if (key == VK_LBUTTON && Stage.Contains_Screen_Point(BUTTON_RECT, Keyboard->Mouse_X(), Keyboard->Mouse_Y())) {
				running = false;
			}
		}
		if (animation != NULL && ++ticks % 3 == 0) {
			frame = (frame + 1) % ANIMATION_FRAMES;
			Stage.Draw_Shape_Frame(animation, frame, frame_box);
			Stage.Present(frame_box);
		}
		Sleep(16);
	}

	Keyboard->Clear();
}


/// <summary>
/// Plays one of the score screen's sound effects by name.
/// The sound is looked up in the list the score screen loaded at startup. A name that is
/// not in the list, or one whose sample failed to load, simply plays nothing.
/// </summary>
/// <param name="name">The name the sound was registered under.</param>
/// <param name="volume">The volume to play at, before the player's sound setting is
/// applied.</param>
void ScoreClass::DoSound(const char * name, int volume)
{
	if (name != NULL && AudioEngine.Is_Available()) {

		for (int i = 0; i < ScoreSnds.Count(); i++) {
			SfxEntry * snd = ScoreSnds[i];

			if (stricmp(name, snd->Get_Name()) == 0) {
				if (snd->Get_Sample() != NULL) {
					AudioEngine.Play_Sample(snd->Get_Sample(), AUDIO_GROUP_SFX, (float)volume / 255.0f, 255);
				}
				break;
			}
		}
	}
}


/// <summary>
/// Constructs a score screen sound effect entry.
/// The sample is fetched from the mixfiles when it lives there, and otherwise loaded from
/// disk with this entry taking ownership of the buffer. Nothing is loaded at all when
/// audio is unavailable.
/// </summary>
/// <param name="name">The name the score screen will ask for this sound by.</param>
/// <param name="filename">The file the sample is loaded from.</param>
SfxEntry::SfxEntry(char const * name, char const *filename) :
	Name(NULL),
	Sample(NULL),
	IsAllocated(false)
{
	if (AudioEngine.Is_Available()) {
		Name = strdup(name);
		Sample = (void *)MFCD::Retrieve(filename);
		if (Sample == NULL) {
			CCFileClass file(filename);
			Sample = Load_Alloc_Data(file);
			IsAllocated = true;
			file.Close();
			DebugString("ScoreScreen: Loaded %s\n", filename);
		}
	}
}


/// <summary>
/// Destroys a score screen sound effect entry.
/// Any playback still in progress is stopped before the sample is released, so a sound
/// can never outlive the buffer it is playing from. Samples that came out of a mixfile
/// are left alone.
/// </summary>
SfxEntry::~SfxEntry(void)
{
	if (Sample != NULL) {
		AudioEngine.Stop_Sample_Playing(Sample);
		AudioEngine.Release_Sample(Sample);
		if (IsAllocated == true) {
			delete Sample;
		}
	}
	if (Name != NULL) {
		free(Name);
	}
}
