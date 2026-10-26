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
#include "_rules.h"
#include "_surface.h"
#include "_timer.h"
#include "autotest.h"
#include "conquer.h"
#include "csf.h"
#include "dsurface.h"
#include "globals.h"
#include "hud.h"
#include "house.h"
#include "keyboard.h"
#include "language/language.h"
#include "msgloop.h"
#include "rules.h"
#include "scenario.h"
#include "shapeset.h"
#include "stimer.h"
#include "surface.h"
#include "theme.h"
#include "utracker.h"
#include "winstub.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>


void const * Beepy6;
int ControlQ;	// cheat key to skip past score/mapsel screens
bool StillUpdating;

namespace {

/*
 * The emblem animation is 424 by 220 pixels.
 */
constexpr int EMBLEM_WIDTH = 424;
constexpr int EMBLEM_HEIGHT = 220;

/*
 * The side art, indexed by side: Allied, Soviet and Yuri. These are the names in gamemd's table
 * (0x844c58); the narrow entries are used on a 640 wide screen. The Yuri narrow entries name Soviet
 * files in gamemd's table, so they are kept here.
 */
char const * const BACKGROUND_WIDE[3] = { "ASCRBKMD.SHP", "SSCRBKMD.SHP", "SYCRBKMD.SHP" };
char const * const EMBLEM_WIDE[3] = { "ASCRAMD.SHP", "SSCRAMD.SHP", "SYCRAMD.SHP" };
char const * const BACKGROUND_NARROW[3] = { "ASCRBKSM.SHP", "SSCRBKSM.SHP", "SSCRBKSM.SHP" };
char const * const EMBLEM_NARROW[3] = { "ASCRASM.SHP", "SSCRASM.SHP", "SSCRASM.SHP" };
char const * const PALETTE[3] = { "ASCORE.PAL", "SSCORE.PAL", "YSCORE.PAL" };

/*
 * The side index of the art: 0 for the first side in the rules' [Sides] list, 1 for the second and 2
 * for any other.
 */
int Side_Index(void)
{
	char letter = ScoreStageClass::Side_Letter();
	if (letter == 'A') return(0);
	if (letter == 'S') return(1);
	return(2);
}

/*
 * The text of one results screen, fetched from the mission and the string tables.
 */
struct Figures {
	std::string Title;
	std::string Rank;
	std::string Message;
	std::string EnemiesLabel;
	std::string EnemiesValue;
	std::string ParLabel;
	std::string ParValue;
	std::string YourLabel;
	std::string YourValue;
	std::string Caption;
	std::string Status;
};

/*
 * Formats timer ticks as hh:mm:ss, the way gamemd prints the par and elapsed times.
 */
void Format_Time(char * buffer, int ticks)
{
	int const seconds = ticks / TIMER_SECOND;
	sprintf(buffer, "%02d:%02d:%02d", seconds / 3600, (seconds / 60) % 60, seconds % 60);
}

/*
 * The enemies destroyed by the human houses: the kills of each house in the four categories it tracks.
 */
int Enemies_Destroyed(void)
{
	int total = 0;
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * house = Houses[i];
		if (house == NULL || !house->IsHuman) {
			continue;
		}
		UnitTrackerClass * const trackers[4] = {
			house->DestroyedAircraft,
			house->DestroyedInfantry,
			house->DestroyedUnits,
			house->DestroyedBuildings
		};
		for (UnitTrackerClass * tracker : trackers) {
			if (tracker == NULL) {
				continue;
			}
			int * totals = tracker->Get_All_Totals();
			for (int unit = 0; unit < tracker->Get_Unit_Count(); unit++) {
				total += totals[unit];
			}
		}
	}
	return(total);
}

/*
 * Reads the figures of the mission that was just won. The rank is under par when the elapsed time is
 * not more than the par time, compared in whole seconds as gamemd compares them.
 */
Figures Collect_Figures(void)
{
	Figures figures;

	int const difficulty = std::clamp((int)Scen->Difficulty, 0, 2);
	int const par = Scen->ParTime[difficulty];
	int const elapsed = (int)Scen->ElapsedTimer;
	bool const under_par = (elapsed / TIMER_SECOND) <= (par / TIMER_SECOND);

	figures.Rank = Fetch_String_UTF8(under_par ? Scen->UnderParTitle : Scen->OverParTitle);
	figures.Message = Fetch_String_UTF8(under_par ? Scen->UnderParMessage : Scen->OverParMessage);

	char buffer[32];
	Format_Time(buffer, par);
	figures.ParValue = buffer;
	Format_Time(buffer, elapsed);
	figures.YourValue = buffer;
	sprintf(buffer, "%d", Enemies_Destroyed());
	figures.EnemiesValue = buffer;

	figures.Title = Fetch_String_UTF8("GUI:StandaloneScore");
	figures.EnemiesLabel = Fetch_String_UTF8("GUI:HeadsTaken");
	figures.ParLabel = Fetch_String_UTF8("GUI:ParTime");
	figures.YourLabel = Fetch_String_UTF8("GUI:YourTime");
	figures.Caption = Fetch_String_UTF8("GUI:Continue");
	figures.Status = Fetch_String_UTF8("STT:StandScoreButtonReturn");
	return(figures);
}

/*
 * Splits text into lines no wider than the width, breaking at spaces. A word wider than the width
 * keeps a line of its own.
 */
std::vector<std::string> Wrap_Text(ScoreStageClass const & stage, std::string const & text, int width)
{
	std::vector<std::string> lines;
	std::string line;
	std::size_t start = 0;
	while (start < text.size()) {
		std::size_t end = text.find(' ', start);
		if (end == std::string::npos) {
			end = text.size();
		}
		std::string const word = text.substr(start, end - start);
		start = end + 1;
		if (word.empty()) {
			continue;
		}
		std::string const joined = line.empty() ? word : line + " " + word;
		if (!line.empty() && stage.Text_Width(joined.c_str()) > width) {
			lines.push_back(line);
			line = word;
		} else {
			line = joined;
		}
	}
	if (!line.empty()) {
		lines.push_back(line);
	}
	return(lines);
}

void Print_Paragraph(ScoreStageClass & stage, std::string const & text, Rect const & area)
{
	int const line_height = stage.Text_Height();
	std::vector<std::string> const lines = Wrap_Text(stage, text, area.Width);
	for (std::size_t i = 0; i < lines.size(); i++) {
		stage.Print_Left(lines[i].c_str(), area.X, area.Y + (int)i * line_height);
	}
}

void Fill_Area(Surface * surface, Rect const & area, int color)
{
	for (int y = area.Y; y < area.Y + area.Height; y++) {
		surface->Draw_Rect(Rect(area.X, y, area.Width, 1), color);
	}
}

void Outline_Area(Surface * surface, Rect const & area, int color)
{
	Fill_Area(surface, Rect(area.X, area.Y, area.Width, 1), color);
	Fill_Area(surface, Rect(area.X, area.Y + area.Height - 1, area.Width, 1), color);
	Fill_Area(surface, Rect(area.X, area.Y, 1, area.Height), color);
	Fill_Area(surface, Rect(area.X + area.Width - 1, area.Y, 1, area.Height), color);
}

int Button_Color(void)
{
	return(DSurface::Build_Hicolor_Pixel(RGBClass(170, 22, 22)));
}

int Button_Edge_Color(void)
{
	return(DSurface::Build_Hicolor_Pixel(RGBClass(230, 190, 60)));
}

int Panel_Color(void)
{
	return(DSurface::Build_Hicolor_Pixel(RGBClass(10, 14, 24)));
}

int Panel_Edge_Color(void)
{
	return(DSurface::Build_Hicolor_Pixel(RGBClass(110, 130, 160)));
}

void Draw_Button(ScoreStageClass & stage, Rect const & button, std::string const & caption)
{
	Fill_Area(stage.Canvas(), button, Button_Color());
	Outline_Area(stage.Canvas(), button, Button_Edge_Color());
	stage.Print_Centre(caption.c_str(), button.X, button.Width, button.Y + (button.Height - stage.Text_Height()) / 2);
}

/*
 * Lays out the classic screen: gamemd's standalone score dialog on the 800 by 600 stage. The emblem
 * goes in the dialog's picture box, centred, and the button and the texts go where the dialog puts them.
 */
void Draw_Classic(ScoreStageClass & stage, Figures const & figures, Rect & emblem, Rect & button)
{
	Rect const picture = ScoreStageClass::Dialog_Rect(72, 115, 282, 141);
	emblem = Rect(picture.X + (picture.Width - EMBLEM_WIDTH) / 2, picture.Y + (picture.Height - EMBLEM_HEIGHT) / 2, EMBLEM_WIDTH, EMBLEM_HEIGHT);
	button = ScoreStageClass::Dialog_Rect(425, 346, 108, 23);

	Rect const title = ScoreStageClass::Dialog_Rect(425, 1, 108, 10);
	stage.Print_Centre(figures.Title.c_str(), title.X, title.Width, title.Y);

	Rect const rank = ScoreStageClass::Dialog_Rect(85, 35, 257, 10);
	stage.Print_Left(figures.Rank.c_str(), rank.X, rank.Y);
	Print_Paragraph(stage, figures.Message, ScoreStageClass::Dialog_Rect(85, 50, 257, 40));

	Rect const enemies_label = ScoreStageClass::Dialog_Rect(85, 100, 110, 10);
	Rect const enemies_value = ScoreStageClass::Dialog_Rect(200, 100, 142, 10);
	stage.Print_Left(figures.EnemiesLabel.c_str(), enemies_label.X, enemies_label.Y);
	stage.Print_Left(figures.EnemiesValue.c_str(), enemies_value.X, enemies_value.Y);

	Rect const par_label = ScoreStageClass::Dialog_Rect(125, 266, 55, 10);
	Rect const par_value = ScoreStageClass::Dialog_Rect(125, 277, 55, 10);
	stage.Print_Centre(figures.ParLabel.c_str(), par_label.X, par_label.Width, par_label.Y);
	stage.Print_Centre(figures.ParValue.c_str(), par_value.X, par_value.Width, par_value.Y);

	Rect const your_label = ScoreStageClass::Dialog_Rect(242, 266, 55, 10);
	Rect const your_value = ScoreStageClass::Dialog_Rect(242, 277, 55, 10);
	stage.Print_Centre(figures.YourLabel.c_str(), your_label.X, your_label.Width, your_label.Y);
	stage.Print_Centre(figures.YourValue.c_str(), your_value.X, your_value.Width, your_value.Y);

	Draw_Button(stage, button, figures.Caption);

	Rect const status = ScoreStageClass::Dialog_Rect(10, 357, 303, 12);
	stage.Print_Left(figures.Status.c_str(), status.X, status.Y);
}

/*
 * Lays out the modern screen: three panels across the screen. The left panel holds the rank and the
 * commendation, the centre panel the emblem, and the right panel the figures and the Continue button.
 */
void Draw_Modern(ScoreStageClass & stage, Figures const & figures, Rect & emblem, Rect & button)
{
	int const width = stage.Width();
	int const height = stage.Height();
	int const text_height = stage.Text_Height();
	int const pad = std::max(8, height / 40);
	int const top = height * 14 / 100;
	int const panel_height = height * 70 / 100;

	Rect const left(width * 4 / 100, top, width * 30 / 100, panel_height);
	Rect const centre(width * 36 / 100, top, width * 28 / 100, panel_height);
	Rect const right(width * 66 / 100, top, width * 30 / 100, panel_height);

	stage.Print_Centre(figures.Title.c_str(), 0, width, height * 4 / 100);

	Rect const panels[3] = { left, centre, right };
	for (Rect const & panel : panels) {
		Fill_Area(stage.Canvas(), panel, Panel_Color());
		Outline_Area(stage.Canvas(), panel, Panel_Edge_Color());
	}

	stage.Print_Left(figures.Rank.c_str(), left.X + pad, left.Y + pad);
	Print_Paragraph(stage, figures.Message, Rect(left.X + pad, left.Y + pad + 2 * text_height, left.Width - 2 * pad, left.Height));

	double const fit = std::min(1.5, std::min((centre.Width - 2 * pad) / (double)EMBLEM_WIDTH, (centre.Height - 2 * pad) / (double)EMBLEM_HEIGHT));
	int const emblem_width = (int)(EMBLEM_WIDTH * fit);
	int const emblem_height = (int)(EMBLEM_HEIGHT * fit);
	emblem = Rect(centre.X + (centre.Width - emblem_width) / 2, centre.Y + (centre.Height - emblem_height) / 2, emblem_width, emblem_height);

	int const figure_right = right.X + right.Width - pad;
	int y = right.Y + pad;
	stage.Print_Left(figures.EnemiesLabel.c_str(), right.X + pad, y);
	stage.Print_Right(figures.EnemiesValue.c_str(), figure_right, y + text_height + 2);
	y += 3 * text_height;
	stage.Print_Left(figures.ParLabel.c_str(), right.X + pad, y);
	stage.Print_Right(figures.ParValue.c_str(), figure_right, y + text_height + 2);
	y += 3 * text_height;
	stage.Print_Left(figures.YourLabel.c_str(), right.X + pad, y);
	stage.Print_Right(figures.YourValue.c_str(), figure_right, y + text_height + 2);

	int const button_height = std::max(text_height + 6, height * 6 / 100);
	button = Rect(right.X + pad, right.Y + right.Height - pad - button_height, right.Width - 2 * pad, button_height);
	Draw_Button(stage, button, figures.Caption);

	stage.Print_Left(figures.Status.c_str(), left.X, top + panel_height + pad);
}

}


/// <summary>
/// Shows the campaign mission score after a won mission, in the layout of the interface style: the
/// classic 800 by 600 dialog or the modern three panels. The side's art is drawn behind it. Only the
/// Continue button closes the screen; keys and clicks elsewhere do nothing.
/// </summary>
void ScoreClass::Presentation(void)
{
	bool const modern = Modern_Interface();
	if (!Stage.Open(modern)) {
		return;
	}

	Hide_Mouse();
	Keyboard->Clear();
	Stage.Clear_Screen();

	Theme.Stop();
	Theme.Play_Song(Theme.From_Name("SCORE"));

	int const side = Side_Index();
	bool const narrow = Stage.Is_Small();
	Stage.Load_Palette(PALETTE[side]);

	ShapeSet * background = Stage.Load_Shape((narrow ? BACKGROUND_NARROW : BACKGROUND_WIDE)[side]);
	ShapeSet * emblem = Stage.Load_Shape((narrow ? EMBLEM_NARROW : EMBLEM_WIDE)[side]);

	Rect const whole(0, 0, Stage.Width(), Stage.Height());
	Rect const art = (background != NULL && !modern) ? Rect(0, 0, background->Get_Width(), background->Get_Height()) : whole;
	if (background != NULL) {
		Stage.Draw_Shape_Frame(background, 0, art);
	}

	/*
	 * The art is blurred behind the screen: over the whole modern stage, or into the bars beside the classic
	 * stage. The classic stage outside the art gets the same dimmed backdrop, since the art does not cover it.
	 */
	if (modern) {
		Stage.Fill_Backdrop(*Stage.Canvas(), whole);
	} else {
		Stage.Fill_Backdrop(*HiddenSurface, art);
		Stage.Show_Screen();
		Stage.Fill_Backdrop(*Stage.Canvas(), art);
		if (background != NULL) {
			Stage.Draw_Shape_Frame(background, 0, art);
		}
	}

	Figures const figures = Collect_Figures();
	Rect emblem_box;
	Rect button;
	if (modern) {
		Draw_Modern(Stage, figures, emblem_box, button);
	} else {
		Draw_Classic(Stage, figures, emblem_box, button);
	}

	if (emblem != NULL) {
		Stage.Draw_Shape_Frame(emblem, 0, emblem_box);
	}
	Stage.Present(whole);

	AutoTest_Score_Shown();

	Show_Mouse();
	Wait_For_Click(emblem, emblem_box, button);
	Hide_Mouse();

	Theme.Stop(true);
	Theme.Queue_Song(THEME_NONE);

	if (emblem != NULL) {
		ScoreStageClass::Free_Shape(emblem);
	}
	if (background != NULL) {
		ScoreStageClass::Free_Shape(background);
	}
	Stage.Close();

	Stage.Clear_Screen();
	Show_Mouse();
	Keyboard->Clear();
}


/// <summary>
/// Waits for the player to click the Continue button, with the emblem animation looping while the
/// screen is up. A left click outside the button, and any key, does nothing.
/// </summary>
/// <param name="animation">The emblem animation to loop, or NULL to wait with a still picture.</param>
/// <param name="frame">The stage area each animation frame is drawn into.</param>
/// <param name="button">The stage area of the Continue button.</param>
void ScoreClass::Wait_For_Click(ShapeSet const * animation, Rect const & frame, Rect const & button)
{
	Keyboard->Clear();
	if (AutoTest_Post_Win()) {
		return;
	}

	int const frame_count = (animation != NULL) ? std::max(1, animation->Get_Count()) : 1;
	int const loops_per_frame = std::max(1, Rule->ScoreEmblemTicks);
	int current = 0;
	int loops = 0;
	bool running = true;

	while (running) {
		Call_Back();
		Windows_Message_Handler();
		if (Keyboard->Check()) {
			int key = Keyboard->Get();
			if (key == VK_LBUTTON && Stage.Contains_Screen_Point(button, Keyboard->Mouse_X(), Keyboard->Mouse_Y())) {
				running = false;
			}
		}
		if (animation != NULL && ++loops % loops_per_frame == 0) {
			current = (current + 1) % frame_count;
			Stage.Draw_Shape_Frame(animation, current, frame);
			Stage.Present(frame);
		}
		Sleep(16);
	}

	Keyboard->Clear();
}
