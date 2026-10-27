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
#include "winstub.h"

#include <algorithm>
#include <chrono>
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
 * gamemd's emblem control steps its animation on a 100 millisecond timer (0x6033f0 answers 100 for
 * the picture, control 1835 of dialog 259). When the animation wraps to frame 0 the dialog procedure
 * (0x6c9383) sets the frame to 29, so the first pass shows every frame and the later passes repeat
 * frame 29 to the last.
 */
constexpr int EMBLEM_FRAME_MILLISECONDS = 100;
constexpr int EMBLEM_LOOP_START = 29;

/*
 * The classic layout on the 800 by 600 stage. gamemd draws the side's background at the top left
 * (0x72e730) and the side's plate 80 pixels to the right of it (0x72e920). The emblem animation matches the
 * plate's art where it is drawn at 24 by 142 pixels inside the plate (found by comparing their frames).
 * The text controls of dialog 259 appear 50 pixels higher in the original's screenshot than the dialog
 * template puts them, and its status text starts at x 8; those offsets were measured from that screenshot.
 */
constexpr int PLATE_X = 80;
constexpr int PLATE_Y = 0;
constexpr int EMBLEM_X = PLATE_X + 24;
constexpr int EMBLEM_Y = PLATE_Y + 142;
constexpr int TEXT_RISE = 50;
// The rank, commendation and enemies text sit 3 pixels lower in the original's screenshot than the time boxes' text.
constexpr int PANEL_TEXT_RISE = 47;
constexpr int PARAGRAPH_LINE_HEIGHT = 17;
constexpr int STATUS_X = 10;
constexpr int STATUS_CENTRE_Y = 584;
constexpr int TITLE_TOP = 4;
constexpr int TITLE_HEIGHT = 30;

/*
 * The text color of gamemd's dialogs. OwnerDraw's primary text color is the color reference 0x00FFFF, yellow,
 * for every side (0xac18a4, set at 0x600588 and 0x60fa3f).
 */
constexpr int TEXT_RED = 255;
constexpr int TEXT_GREEN = 255;
constexpr int TEXT_BLUE = 0;

/*
 * The side art, indexed by side: Allied, Soviet and Yuri. These are the names in gamemd's table
 * (0x844c58) for the 800 wide screen. The small 640 wide entries are not used: the stage is always
 * 800 by 600 and is fitted to the screen.
 */
char const * const BACKGROUND[3] = { "ASCRBKMD.SHP", "SSCRBKMD.SHP", "SYCRBKMD.SHP" };
char const * const PLATE[3] = { "ASCRTMD.SHP", "SSCRTMD.SHP", "SYCRTMD.SHP" };
char const * const EMBLEM[3] = { "ASCRAMD.SHP", "SSCRAMD.SHP", "SYCRAMD.SHP" };
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
 * The shared menu art that frames the classic screen: the metal column with its monitor, the button
 * slots and the status bar. Each piece is drawn through its own palette, as gamemd does (0x72e450).
 */
struct ShellArt {
	ShellArt(void) : Top(NULL), Slot(NULL), Bottom(NULL), Status(NULL), Button(NULL), Shell(NULL), ShellSlot(NULL), ShellButton(NULL) {}

	~ShellArt(void)
	{
		ShapeSet const * const shapes[5] = { Top, Slot, Bottom, Status, Button };
		for (ShapeSet const * shape : shapes) {
			if (shape != NULL) {
				ScoreStageClass::Free_Shape(shape);
			}
		}
		ScoreStageClass::Free_Drawer(Shell);
		ScoreStageClass::Free_Drawer(ShellSlot);
		ScoreStageClass::Free_Drawer(ShellButton);
	}

	ShellArt(ShellArt const &) = delete;
	ShellArt & operator = (ShellArt const &) = delete;

	ShapeSet * Top;
	ShapeSet * Slot;
	ShapeSet * Bottom;
	ShapeSet * Status;
	ShapeSet * Button;
	ConvertClass * Shell;
	ConvertClass * ShellSlot;
	ConvertClass * ShellButton;
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
 * The enemies destroyed by the player's houses. gamemd adds up the kill trackers of every house whose
 * player control flag is set (0x6c92b0, the flag at +0x1ED), and a tracker counts what that house killed,
 * except types with DontScore. TechnoClass::Count_The_Kill gives the same kills to the house's UnitsKilled
 * and BuildingsKilled arrays, by victim house, in every kind of game.
 */
int Enemies_Destroyed(void)
{
	int total = 0;
	for (int i = 0; i < Houses.Count(); i++) {
		HouseClass * house = Houses[i];
		if (house == NULL || !house->IsPlayerControl) {
			continue;
		}
		for (int victim = 0; victim < ARRAY_SIZE(house->UnitsKilled); victim++) {
			total += house->UnitsKilled[victim] + house->BuildingsKilled[victim];
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

void Print_Paragraph(ScoreStageClass & stage, std::string const & text, Rect const & area, int line_height)
{
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

/*
 * The top of a line of text whose middle is on the given line, for the stage's text height.
 */
int Centred_Text_Top(ScoreStageClass const & stage, int centre)
{
	return(centre - stage.Text_Height() / 2);
}

/*
 * Loads the shared menu art. A missing piece stays NULL and is left out of the screen.
 */
void Load_Shell(ScoreStageClass & stage, ShellArt & shell)
{
	shell.Top = stage.Load_Shape("SDTP.SHP");
	shell.Slot = stage.Load_Shape("SDBTNBKGD.SHP");
	shell.Bottom = stage.Load_Shape("SDBTM.SHP");
	shell.Status = stage.Load_Shape("LWSCRNL.SHP");
	shell.Button = stage.Load_Shape("SDBTNANM.SHP");
	shell.Shell = stage.Load_Drawer("SHELL.PAL");
	shell.ShellSlot = stage.Load_Drawer("SHELL2.PAL");
	shell.ShellButton = stage.Load_Drawer("SDBTNANM.PAL");
}

/*
 * Draws the art of the classic screen, without text, placed as gamemd places it (0x72e450 and 0x72ec70): the
 * side's background and plate, the metal column down the right with its button slots, and the status bar
 * along the bottom. The last full slot of the column holds the Continue button, which is returned with the
 * column's area.
 */
void Draw_Classic_Art(ScoreStageClass & stage, ShapeSet const * background, ShapeSet const * plate, ShellArt const & shell, Rect & button, Rect & column)
{
	int const width = stage.Width();
	int const height = stage.Height();

	if (background != NULL) {
		stage.Draw_Shape_Frame(background, 0, Rect(0, 0, background->Get_Width(), background->Get_Height()));
	}
	if (plate != NULL) {
		stage.Draw_Shape_Frame(plate, 0, Rect(PLATE_X, PLATE_Y, plate->Get_Width(), plate->Get_Height()));
	}

	int top_height = 0;
	int column_x = width;
	if (shell.Top != NULL) {
		top_height = shell.Top->Get_Height();
		column_x = width - shell.Top->Get_Width();
		stage.Draw_Shape_At(shell.Top, 0, column_x, 0, shell.Shell);
	}
	column = Rect(column_x, 0, width - column_x, height);

	int slots = 0;
	if (shell.Slot != NULL && shell.Slot->Get_Height() > 0) {
		int const slot_height = shell.Slot->Get_Height();
		slots = (height - top_height) / slot_height;
		for (int i = 0; i < slots; i++) {
			stage.Draw_Shape_At(shell.Slot, 0, width - shell.Slot->Get_Width(), top_height + i * slot_height, shell.ShellSlot);
		}
		if (shell.Bottom != NULL) {
			stage.Draw_Shape_At(shell.Bottom, 0, width - shell.Bottom->Get_Width(), top_height + slots * slot_height, shell.Shell);
		}
	}

	if (shell.Status != NULL) {
		stage.Draw_Shape_At(shell.Status, 0, 0, height - shell.Status->Get_Height(), shell.Shell);
	}

	if (shell.Button != NULL && slots > 0) {
		button = Rect(width - shell.Button->Get_Width(), top_height + (slots - 1) * shell.Slot->Get_Height(), shell.Button->Get_Width(), shell.Button->Get_Height());
		stage.Draw_Shape_At(shell.Button, 2, button.X, button.Y, shell.ShellButton);
	} else {
		button = ScoreStageClass::Dialog_Rect(425, 346, 108, 23);
	}
}

/*
 * Prints the texts of the classic screen at the positions of dialog 259's controls. The rank and the
 * commendation are left aligned, the two times are centred under their labels, and the number of enemies
 * destroyed is left aligned, as the dialog's static styles say.
 */
void Draw_Classic_Text(ScoreStageClass & stage, Figures const & figures, Rect const & column, Rect const & button)
{
	Rect const title(column.X, TITLE_TOP, column.Width, TITLE_HEIGHT);
	stage.Print_Centre(figures.Title.c_str(), title.X, title.Width, Centred_Text_Top(stage, title.Y + title.Height / 2));

	Rect const rank(128, 57 - PANEL_TEXT_RISE, 386, 16);
	stage.Print_Left(figures.Rank.c_str(), rank.X, Centred_Text_Top(stage, rank.Y + rank.Height / 2));
	Print_Paragraph(stage, figures.Message, Rect(128, 81 - PANEL_TEXT_RISE, 386, 65), PARAGRAPH_LINE_HEIGHT);

	Rect const enemies_label(128, 163 - PANEL_TEXT_RISE, 165, 16);
	Rect const enemies_value(300, 163 - PANEL_TEXT_RISE, 213, 16);
	stage.Print_Left(figures.EnemiesLabel.c_str(), enemies_label.X, Centred_Text_Top(stage, enemies_label.Y + enemies_label.Height / 2));
	stage.Print_Left(figures.EnemiesValue.c_str(), enemies_value.X, Centred_Text_Top(stage, enemies_value.Y + enemies_value.Height / 2));

	Rect const par_label(188, 432 - TEXT_RISE, 83, 16);
	Rect const par_value(188, 450 - TEXT_RISE, 83, 16);
	stage.Print_Centre(figures.ParLabel.c_str(), par_label.X, par_label.Width, Centred_Text_Top(stage, par_label.Y + par_label.Height / 2));
	stage.Print_Centre(figures.ParValue.c_str(), par_value.X, par_value.Width, Centred_Text_Top(stage, par_value.Y + par_value.Height / 2));

	Rect const your_label(363, 432 - TEXT_RISE, 83, 16);
	Rect const your_value(363, 450 - TEXT_RISE, 83, 16);
	stage.Print_Centre(figures.YourLabel.c_str(), your_label.X, your_label.Width, Centred_Text_Top(stage, your_label.Y + your_label.Height / 2));
	stage.Print_Centre(figures.YourValue.c_str(), your_value.X, your_value.Width, Centred_Text_Top(stage, your_value.Y + your_value.Height / 2));

	stage.Print_Centre(figures.Caption.c_str(), button.X, button.Width, Centred_Text_Top(stage, button.Y + button.Height / 2));
	stage.Print_Left(figures.Status.c_str(), STATUS_X, Centred_Text_Top(stage, STATUS_CENTRE_Y));
}

void Draw_Button(ScoreStageClass & stage, Rect const & button, std::string const & caption)
{
	Fill_Area(stage.Canvas(), button, Button_Color());
	Outline_Area(stage.Canvas(), button, Button_Edge_Color());
	stage.Print_Centre(caption.c_str(), button.X, button.Width, button.Y + (button.Height - stage.Text_Height()) / 2);
}

/*
 * Lays out the modern screen: three panels across the screen. The left panel holds the rank and the
 * commendation, the centre panel the emblem, and the right panel the figures and the Continue button.
 * The emblem fills the width of its panel.
 */
void Draw_Modern(ScoreStageClass & stage, Figures const & figures, Rect & emblem, Rect & button)
{
	int const width = stage.Width();
	int const height = stage.Height();
	int const text_height = stage.Text_Height();
	int const pad = std::max(8, height / 40);
	int const top = height * 14 / 100;
	int const panel_height = height * 70 / 100;

	Rect const left(width * 4 / 100, top, width * 26 / 100, panel_height);
	Rect const centre(width * 32 / 100, top, width * 36 / 100, panel_height);
	Rect const right(width * 70 / 100, top, width * 26 / 100, panel_height);

	stage.Print_Centre(figures.Title.c_str(), 0, width, height * 4 / 100);

	Rect const panels[3] = { left, centre, right };
	for (Rect const & panel : panels) {
		Fill_Area(stage.Canvas(), panel, Panel_Color());
		Outline_Area(stage.Canvas(), panel, Panel_Edge_Color());
	}

	int const text_width = left.Width - 2 * pad;
	int const rank_lines = (int)Wrap_Text(stage, figures.Rank, text_width).size();
	Print_Paragraph(stage, figures.Rank, Rect(left.X + pad, left.Y + pad, text_width, left.Height), text_height);
	Print_Paragraph(stage, figures.Message, Rect(left.X + pad, left.Y + pad + (rank_lines + 1) * text_height, text_width, left.Height), text_height);

	double const fit = std::min((centre.Width - 2 * pad) / (double)EMBLEM_WIDTH, (centre.Height - 2 * pad) / (double)EMBLEM_HEIGHT);
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
	Stage.Set_Text_Color(TEXT_RED, TEXT_GREEN, TEXT_BLUE);
	ShellArt shell;
	if (!modern) {
		Load_Shell(Stage, shell);
	}
	Stage.Load_Palette(PALETTE[side]);

	ShapeSet * background = Stage.Load_Shape(BACKGROUND[side]);
	ShapeSet * plate = Stage.Load_Shape(PLATE[side]);
	ShapeSet * emblem = Stage.Load_Shape(EMBLEM[side]);

	Rect const whole(0, 0, Stage.Width(), Stage.Height());
	Figures const figures = Collect_Figures();
	Rect emblem_box;
	Rect button;

	if (modern) {
		// The side art is blurred over the whole screen and the panels go over it.
		if (background != NULL) {
			Stage.Draw_Shape_Frame(background, 0, whole);
		}
		Stage.Fill_Backdrop(*Stage.Canvas(), whole);
		Draw_Modern(Stage, figures, emblem_box, button);
	} else {
		// A screen larger than the stage shows the same art around it, blurred and dimmed.
		Rect column;
		Draw_Classic_Art(Stage, background, plate, shell, button, column);
		Stage.Fill_Backdrop(*HiddenSurface, whole);
		Stage.Show_Screen();
		Draw_Classic_Text(Stage, figures, column, button);
		emblem_box = Rect(EMBLEM_X, EMBLEM_Y, EMBLEM_WIDTH, EMBLEM_HEIGHT);
	}

	Stage.Save_Base();
	int frame = 0;
	Draw_Emblem(emblem, frame, emblem_box, modern);

	// A test run captures the screen next, so it shows the emblem at the end of its first pass.
	if (AutoTest_Active() && emblem != NULL) {
		frame = std::max(1, emblem->Get_Count()) - 1;
		Draw_Emblem(emblem, frame, emblem_box, modern);
	}
	Stage.Present(whole);

	AutoTest_Score_Shown();

	Show_Mouse();
	Wait_For_Click(emblem, emblem_box, button, frame, modern);
	Hide_Mouse();

	Theme.Stop(true);
	Theme.Queue_Song(THEME_NONE);

	if (emblem != NULL) {
		ScoreStageClass::Free_Shape(emblem);
	}
	if (plate != NULL) {
		ScoreStageClass::Free_Shape(plate);
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
/// Draws one frame of the emblem animation into its box. The classic emblem goes over the plate it was
/// made for; the modern emblem is stretched to its panel with its dark background left out.
/// </summary>
/// <param name="animation">The emblem animation, or NULL to draw nothing.</param>
/// <param name="frame">The frame to draw.</param>
/// <param name="box">The stage area the frame fills.</param>
/// <param name="modern">Is the modern layout in use?</param>
void ScoreClass::Draw_Emblem(ShapeSet const * animation, int frame, Rect const & box, bool modern)
{
	if (animation == NULL) {
		return;
	}
	if (modern) {
		Stage.Restore(box);
		Stage.Draw_Keyed_Frame(animation, frame, box);
	} else {
		Stage.Draw_Shape_Frame(animation, frame, box);
	}
}


/// <summary>
/// Waits for the player to click the Continue button, with the emblem animation playing while the
/// screen is up. A left click outside the button, and any key, does nothing.
/// </summary>
/// <param name="animation">The emblem animation to play, or NULL to wait with a still picture.</param>
/// <param name="box">The stage area each animation frame is drawn into.</param>
/// <param name="button">The stage area of the Continue button.</param>
/// <param name="start">The frame the animation shows when the wait starts.</param>
/// <param name="modern">Is the modern layout in use?</param>
void ScoreClass::Wait_For_Click(ShapeSet const * animation, Rect const & box, Rect const & button, int start, bool modern)
{
	Keyboard->Clear();
	if (AutoTest_Post_Win()) {
		return;
	}

	using Clock = std::chrono::steady_clock;
	auto const step = std::chrono::milliseconds(EMBLEM_FRAME_MILLISECONDS);

	int const frame_count = (animation != NULL) ? std::max(1, animation->Get_Count()) : 1;
	int current = start;
	auto next = Clock::now() + step;
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

		auto const now = Clock::now();
		if (animation != NULL && now >= next) {
			next = std::max(next + step, now);
			current++;
			if (current >= frame_count) {
				current = (frame_count > EMBLEM_LOOP_START) ? EMBLEM_LOOP_START : 0;
			}
			Draw_Emblem(animation, current, box, modern);
			Stage.Present(box);
		}
		Sleep(16);
	}

	Keyboard->Clear();
}
