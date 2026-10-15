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

#include "mpscore.h"

#include "_keyboar.h"
#include "_mixfile.h"
#include "_palette.h"
#include "_pk.h"
#include "_surface.h"
#include "autotest.h"
#include "ccfile.h"
#include "ccrand.h"
#include "conquer.h"
#include "convert.h"
#include "csf.h"
#include "data.h"
#include "dbgprint.h"
#include "draw.h"
#include "dsurface.h"
#include "globals.h"
#include "houstype.h"
#include "incdec.h"
#include "keyboard.h"
#include "mixfile.h"
#include "msengine.h"
#include "msfont.h"
#include "palette.h"
#include "pcx.h"
#include "playerdisplay.h"
#include "scheme.h"
#include "session.h"
#include "shapeset.h"
#include "side.h"
#include "stats.h"
#include "surface.h"

#include "color.hh"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>


namespace {

/*
 * The screen is laid out on a stage of this size. The positions below are the MultiplayerScore
 * dialog's (dialog 264 in gamemd.exe's resources) in dialog units, converted with the dialog
 * font's base units, so the stage is the dialog's 533 by 369 units.
 */
constexpr int STAGE_WIDTH = 800;
constexpr int STAGE_HEIGHT = 600;

/*
 * The dialog font is 8 point MS Sans Serif, whose base units are 6 by 13 pixels, so one dialog
 * unit is 1.5 pixels across and 13/8 pixels down.
 */
constexpr int Dialog_X(int units) { return(units * 6 / 4); }
constexpr int Dialog_Y(int units) { return(units * 13 / 8); }

/*
 * The picture box. The side's art is 632 by 568 on the stage. The 640 wide variant is 472 by 448
 * and is stretched into the same box.
 */
Rect const PICTURE_RECT(0, 0, 632, 568);

constexpr int MAX_ROWS = 8;
constexpr int BAND_X = 63;
constexpr int BAND_WIDTH = 293;
constexpr int BAND_HEIGHT = 22;
constexpr int TOP_BAND_Y = 72;
constexpr int HEADING_BAND_Y = 94;
constexpr int PLAYER_BAND_Y = 116;
constexpr int LINE_Y = 76;
constexpr int HEADING_Y = 98;
constexpr int PLAYER_TEXT_Y = 120;

struct Column {
	int X;
	int Width;
};

constexpr Column NAME_COLUMN = { 66, 75 };
constexpr Column KILLS_COLUMN = { 147, 45 };
constexpr Column LOSSES_COLUMN = { 202, 45 };
constexpr Column BUILT_COLUMN = { 257, 45 };
constexpr Column SCORE_COLUMN = { 308, 45 };

Rect Dialog_Rect(int x, int y, int width, int height)
{
	return(Rect(Dialog_X(x), Dialog_Y(y), Dialog_X(x + width) - Dialog_X(x), Dialog_Y(y + height) - Dialog_Y(y)));
}

/*
 * The table is every band and figure a player row uses. The counting animation redraws it.
 */
Rect const TABLE_RECT = Dialog_Rect(BAND_X, TOP_BAND_Y, BAND_WIDTH, PLAYER_BAND_Y + BAND_HEIGHT * MAX_ROWS - TOP_BAND_Y);

Rect const BUTTON_RECT = Dialog_Rect(425, 326, 108, 23);
Rect const PROMPT_RECT = Dialog_Rect(2, 353, 303, 14);

int Left_Edge(Column const & column)
{
	return(Dialog_X(column.X));
}

int Right_Edge(Column const & column)
{
	return(Dialog_X(column.X + column.Width));
}

int Row_Text_Y(int row)
{
	return(Dialog_Y(PLAYER_TEXT_Y + BAND_HEIGHT * row));
}

/*
 * The side letter names the art: A for the first side in the rules' [Sides] list, S for the second,
 * and Y for any other side. gamemd chooses its art by the same side index.
 */
char Side_Letter(void)
{
	int index = -1;
	if (PlayerPtr != NULL && PlayerPtr->Class != NULL) {
		index = SideClass::Place_In_Sides(PlayerPtr->Class->Side);
	}
	if (index == 0) return('A');
	if (index == 1) return('S');
	return('Y');
}

/*
 * Loads a palette stored in 6 bit values into the game palette.
 */
void Load_Score_Palette(char const * name)
{
	CCFileClass file(name);
	PaletteClass * palette = (PaletteClass *)Load_Alloc_Data(file);
	file.Close();
	if (palette == NULL) return;

	for (int i = 0; i < 256; i++) {
		CCPalette[i] = RGBClass(
			((unsigned char *)*palette)[i*3] << 2,
			((unsigned char *)*palette)[i*3+1] << 2,
			((unsigned char *)*palette)[i*3+2] << 2);
	}
	delete palette;
}


/// <summary>
/// Formats an elapsed time for display.
/// Absurdly long games are capped rather than printed with a runaway hour count.
/// </summary>
/// <param name="buffer">The buffer to fill in with the formatted time. NULL is tolerated.</param>
/// <param name="time">The elapsed time, in seconds.</param>
/// <remarks>Be sure that the destination buffer is big enough to hold the formatted time.</remarks>
inline void Format_Time(char * buffer, unsigned time)
{
	if (buffer != NULL) {
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
}


/// <summary>
/// Determines which of two players should be listed first.
/// This routine is the sort callback used to rank the players for the score screen.
/// Games won decide the order, and the round's score settles a tie.
/// </summary>
/// <param name="p1">Pointer to the first player's score record pointer.</param>
/// <param name="p2">Pointer to the second player's score record pointer.</param>
/// <returns>Returns with a negative value if the first player ranks ahead of the second, a
/// positive value if it ranks behind, and zero if they are tied.</returns>
int _cdecl Sort_Scores(const void *p1, const void *p2)
{
	MPlayerScoreType * mpscore1 = *(MPlayerScoreType **)p1;
	MPlayerScoreType * mpscore2 = *(MPlayerScoreType **)p2;

	int wins1 = mpscore1->Wins;
	int wins2 = mpscore2->Wins;

	if (wins1 > wins2) return(-1);
	if (wins1 < wins2) return(1);

	int score1 = mpscore1->Score[0];
	int score2 = mpscore2->Score[0];

	if (score1 == score2) return(0);
	return(score1 > score2 ? -1 : 1);
}

}


class MultiScore : public MSEngine
{
	public:
		MultiScore(void);
		virtual ~MultiScore(void);

		virtual void Do_Custom_Draw(Surface * surface);
		virtual void Process_Idle(void);

		bool Init(void);
		void Deinit(void);

		bool User_Input(void);
		void Tally_Score(void);
		void Process_Scores(void);

		void Callback(void);
		bool Single_Presentation(HouseClass *house);
		bool Multi_Presentation(void);

	private:
		void Mount_Neutral_Archives(void);
		void Release_Neutral_Archives(void);
		bool Stage_Is_Small(void) const;
		Rect Stage_To_Screen(Rect const & area) const;
		void Present(Rect const & area);
		void Print_Text(char const * text, int x, int y);
		void Print_Right_Text(char const * text, int right, int y);
		void Print_Centre_Text(char const * text, int left, int width, int y);
		void Draw_Band(int frame, int dialog_y);
		void Draw_Picture(void);
		void Draw_Static(void);
		void Print_Player_Names(void);
		void Print_Row(int row, int kills, int losses, int built, int score);
		void Restore_Table(void);
		void Print_Figures(void);
		void Print_Prompt(void);

		double Scale;
		Point2D FitOrigin;
		Surface * StageSurface;
		Surface * BaseSurface;
		MSFont * Font;
		bool Skirmish;
		MPlayerScoreType * Scores[MAX_PLAYERS];
		MFCD * NeutralArchives[2];
};


/// <summary>
/// Determines if the specified house finished the round in the lead.
/// Only houses that actually take part in the game are considered, so a neutral or
/// spectating house can never take the lead away from a real player.
/// </summary>
/// <param name="house">The house to test for the lead.</param>
/// <returns>bool; Did this house come out on top?</returns>
/// <remarks>This routine tallies the round's scores as a side effect.</remarks>
bool Single_Score_Presentation(HouseClass *house)
{
	return(MultiScore().Single_Presentation(house));
}


/// <summary>
/// Presents the multiplayer score screen.
/// This routine is called when a multiplayer or skirmish game has finished, in order to show the
/// players how the round went.
/// </summary>
void Multi_Score_Presentation(void)
{
	MultiScore().Multi_Presentation();
	Keyboard->Clear();
}


/// <summary>
/// Constructs an idle multiplayer score screen.
/// The screen holds no resources until Init is called on it.
/// </summary>
MultiScore::MultiScore(void) :
	MSEngine(),
	Scale(1.0),
	FitOrigin(0, 0),
	StageSurface(NULL),
	BaseSurface(NULL),
	Font(NULL),
	Skirmish(false)
{
	for (int i = 0; i < MAX_PLAYERS; i++) {
		Scores[i] = NULL;
	}
	NeutralArchives[0] = NULL;
	NeutralArchives[1] = NULL;
}


/// <summary>
/// Destroys the score screen, releasing everything it built.
/// </summary>
MultiScore::~MultiScore(void)
{
	Deinit();
}


/// <summary>
/// Determines if the specified house finished the round in the lead.
/// Only houses that actually take part in the game are considered, so a neutral or
/// spectating house can never take the lead away from a real player.
/// </summary>
/// <param name="house">The house to test for the lead.</param>
/// <returns>bool; Did this house come out on top?</returns>
/// <remarks>This routine tallies the round's scores as a side effect.</remarks>
bool MultiScore::Single_Presentation(HouseClass * house)
{
	Tally_Score();

	int bestScore = 0;
	int bestIndex = -1;

	for (int i = 0; i < Houses.Count(); i++) {
		if (Houses[i] != NULL && !Houses[i]->Class->IsMultiplayPassive && !Houses[i]->IsObserver) {
			if (bestIndex == -1 || bestScore < Session.Score[i].Score[0]) {
				bestScore = Session.Score[i].Score[0];
				bestIndex = i;
			}
		}
	}

	if (bestIndex != -1 && Houses[bestIndex] == house) {
		return(true);
	}
	return(false);
}


/// <summary>
/// Handles the multiplayer score screen.
/// This routine runs the whole end of game presentation. It tallies and ranks the round's
/// scores, draws the table, reveals each player's name, counts the figures up, and then waits
/// for the player before restoring the display and the mouse.
/// </summary>
/// <returns>bool; Was the presentation run? It is refused if there is no surface to
/// present on.</returns>
bool MultiScore::Multi_Presentation(void)
{
	if (AlternateSurface == NULL || HiddenSurface == NULL) {
		return(false);
	}

	if (Init() == true) {
		Keyboard->Clear();
		Callback();
		MouseCursor->Release_Mouse();
		MouseCursor->Hide_Mouse();

		HiddenSurface->Fill(TBLACK);

		Tally_Score();
		Process_Scores();
		Draw_Static();
		Print_Player_Names();
		Print_Figures();
		Print_Prompt();

		AutoTest_Score_Shown();

		MouseCursor->Show_Mouse();
		User_Input();
		MouseCursor->Hide_Mouse();

		HiddenSurface->Fill(TBLACK);
		Add_Update_Rect(HiddenSurface->Get_Rect());
		Blit_All(HiddenSurface);

		Deinit();
		Keyboard->Clear();
		MouseCursor->Show_Mouse();
		MouseCursor->Capture_Mouse();
	}
	return(true);
}


/// <summary>
/// Prepares the score screen for display.
/// This routine builds the stage the screen is drawn on and the font it is printed with. The
/// stage is stretched to the largest whole fit inside the screen, so the screen grows with the
/// display and stays centered.
/// </summary>
/// <returns>bool; Was the screen prepared successfully?</returns>
bool MultiScore::Init(void)
{
	Deinit();

	StageSurface = new DSurface(STAGE_WIDTH, STAGE_HEIGHT);
	BaseSurface = new DSurface(STAGE_WIDTH, STAGE_HEIGHT);
	if (StageSurface == NULL || BaseSurface == NULL) {
		DebugString("MultiScore: Failed to create surface!\n");
		Deinit();
		return(false);
	}

	Mount_Neutral_Archives();

	int const screen_width = HiddenSurface->Get_Width();
	int const screen_height = HiddenSurface->Get_Height();
	Scale = std::min(screen_width / (double)STAGE_WIDTH, screen_height / (double)STAGE_HEIGHT);
	int const fit_width = (int)(STAGE_WIDTH * Scale + 0.5);
	int const fit_height = (int)(STAGE_HEIGHT * Scale + 0.5);
	FitOrigin = Point2D((screen_width - fit_width) / 2, (screen_height - fit_height) / 2);

	Skirmish = (Session.Type == GAME_SKIRMISH);

	Font = new MSFont(false);
	if (Font == NULL) {
		DebugString("MultiScore: Unable to create font!\n");
		Deinit();
		return(false);
	}

	Add_Sound_Effect("Graph", "TEXT1.AUD");

	return(true);
}


/// <summary>
/// Releases the resources the score screen allocated.
/// This routine is called by Init before it builds the screen, and again when the
/// presentation is over. It is safe to call at any time.
/// </summary>
void MultiScore::Deinit(void)
{
	if (Font != NULL) {
		delete Font;
		Font = NULL;
	}
	Release_Neutral_Archives();
	if (BaseSurface != NULL) {
		delete BaseSurface;
		BaseSurface = NULL;
	}
	if (StageSurface != NULL) {
		delete StageSurface;
		StageSurface = NULL;
	}
}


/// <summary>
/// Handles the custom drawing pass of the presentation.
/// The engine offers every animation frame to the derived screen through this routine.
/// The score screen paints itself through its own routines instead, so there is
/// nothing to add here.
/// </summary>
void MultiScore::Do_Custom_Draw(Surface * surface)
{

}


/// <summary>
/// Waits for the player to dismiss the score screen.
/// This routine sits until the player clicks or presses escape or space.
/// </summary>
bool MultiScore::User_Input(void)
{
	int key = VK_NONE;
	bool running = true;

	Keyboard->Clear();
	if (AutoTest_Post_Win()) {
		return(true);
	}

	while (running == true) {
		Wait_For_Focus();
		if (Keyboard->Check()) {
			key = Keyboard->Get();
		}
		switch (key) {
			case VK_LBUTTON:
			case VK_ESCAPE:
			case VK_SPACE:
				running = false;
				break;
		}
		Wait_Delay(1);
	}

	Keyboard->Clear();
	return(true);
}


/// <summary>
/// Mounts the neutral archives that hold the score screen art for the sides.
/// A match mounts only its own side's archives, so these are added for the score screen and
/// released again by Deinit. They are searched after every archive already mounted.
/// </summary>
void MultiScore::Mount_Neutral_Archives(void)
{
	static char const * const names[2] = { "NTRLMD.MIX", "NEUTRAL.MIX" };

	for (int i = 0; i < 2; i++) {
		if (NeutralArchives[i] == NULL && CCFileClass(names[i]).Is_Available()) {
			NeutralArchives[i] = new MFCD(names[i], &FastKey);
		}
	}
}


/// <summary>
/// Releases the neutral archives that Mount_Neutral_Archives added.
/// </summary>
void MultiScore::Release_Neutral_Archives(void)
{
	for (int i = 0; i < 2; i++) {
		if (NeutralArchives[i] != NULL) {
			delete NeutralArchives[i];
			NeutralArchives[i] = NULL;
		}
	}
}


/// <summary>
/// Converts an area of the stage into the screen rectangle it is stretched to.
/// </summary>
/// <param name="area">The area of the stage, in stage pixels.</param>
/// <returns>Returns with the screen rectangle that covers the area.</returns>
Rect MultiScore::Stage_To_Screen(Rect const & area) const
{
	int const left = FitOrigin.X + (int)std::floor(area.X * Scale);
	int const top = FitOrigin.Y + (int)std::floor(area.Y * Scale);
	int const right = FitOrigin.X + (int)std::ceil((area.X + area.Width) * Scale);
	int const bottom = FitOrigin.Y + (int)std::ceil((area.Y + area.Height) * Scale);
	return(Rect(left, top, right - left, bottom - top));
}


/// <summary>
/// Shows an area of the stage on the screen.
/// </summary>
/// <param name="area">The area of the stage that changed, in stage pixels.</param>
void MultiScore::Present(Rect const & area)
{
	Rect const screen = Stage_To_Screen(area);
	HiddenSurface->Blit_From(screen, *StageSurface, area);
	Add_Update_Rect(screen);
	Blit_All(HiddenSurface);
}


/// <summary>
/// Draws a text string on the stage with its left edge at the given position.
/// </summary>
void MultiScore::Print_Text(char const * text, int x, int y)
{
	Font->Draw_String(StageSurface, text, x, y, 2);
}


/// <summary>
/// Draws a text string on the stage with its right edge at the given position.
/// </summary>
void MultiScore::Print_Right_Text(char const * text, int right, int y)
{
	Font->Draw_String(StageSurface, text, right - Font->Get_String_Width(text), y, 2);
}


/// <summary>
/// Draws a text string on the stage, centered across a span.
/// </summary>
void MultiScore::Print_Centre_Text(char const * text, int left, int width, int y)
{
	Font->Draw_String(StageSurface, text, left + (width - Font->Get_String_Width(text)) / 2, y, 2);
}


/// <summary>
/// Draws one of the side's bar pictures onto the stage at a dialog height.
/// The pictures are 440 by 36 and carry their own palette, so each one is converted through
/// that palette rather than the game's.
/// </summary>
/// <param name="frame">The picture's frame number in the side's bar art, from 1 to 10.</param>
/// <param name="dialog_y">The dialog unit height the band starts at.</param>
void MultiScore::Draw_Band(int frame, int dialog_y)
{
	char name[32];
	sprintf(name, "MP%cSCRNLBAR%02d.PCX", Side_Letter(), frame);

	CCFileClass file(name);
	PaletteClass palette;
	Surface * band = Read_PCX_File(file, &palette);
	if (band != NULL) {
		ConvertClass * drawer = new ConvertClass(palette, palette, *StageSurface);
		Blit_Block(*StageSurface, *drawer, *band, band->Get_Rect(), Point2D(Dialog_X(BAND_X), Dialog_Y(dialog_y)), StageSurface->Get_Rect());
		delete drawer;
		delete band;
	}
}


/// <summary>
/// Is the screen the 640 wide size, which gets the small picture?
/// gamemd tests the same render width.
/// </summary>
bool MultiScore::Stage_Is_Small(void) const
{
	return(HiddenSurface->Get_Width() == 640);
}


/// <summary>
/// Draws the picture for the player's side into its box on the stage.
/// The picture is a shape with the side's palette. The small variant is drawn at its own size
/// and then stretched into the box.
/// </summary>
void MultiScore::Draw_Picture(void)
{
	char letter = Side_Letter();
	bool const small = Stage_Is_Small();

	/*
	 * gamemd has no small picture for the third side, and uses the second side's instead.
	 */
	if (small && letter == 'Y') {
		letter = 'S';
	}

	char name[32];
	sprintf(name, "MP%cSCRN%s.SHP", letter, small ? "S" : "L");

	char palname[32];
	sprintf(palname, "MP%cSCRN.PAL", Side_Letter());
	Load_Score_Palette(palname);

	CCFileClass file(name);
	ShapeSet * shapes = (ShapeSet *)Load_Alloc_Data(file);
	file.Close();
	if (shapes == NULL) {
		DebugString("MultiScore: %s is missing\n", name);
		return;
	}

	DSurface picture(shapes->Get_Width(), shapes->Get_Height());
	picture.Fill(0);
	ConvertClass * drawer = new ConvertClass(CCPalette, CCPalette, picture);
	Draw_Shape(picture, *drawer, shapes, 0, Point2D(0, 0), picture.Get_Rect(), SHAPE_WIN_REL);
	delete drawer;

	StageSurface->Blit_From(PICTURE_RECT, picture, picture.Get_Rect());

	delete [] (char *)shapes;
}


/// <summary>
/// Draws the parts of the screen that stay put while the figures count up.
/// These are the picture, the bands, the title, the Game and Time line, the headings and the
/// continue button. The result is kept on the base surface, which the counting animation restores
/// the table from.
/// </summary>
void MultiScore::Draw_Static(void)
{
	StageSurface->Fill(TBLACK);
	Draw_Picture();

	Draw_Band(1, TOP_BAND_Y);
	Draw_Band(2, HEADING_BAND_Y);
	for (int row = 0; row < MAX_ROWS; row++) {
		Draw_Band(3 + row, PLAYER_BAND_Y + BAND_HEIGHT * row);
	}

	std::string title = Fetch_String_UTF8(Skirmish ? "GUI:SkirmishScore" : "GUI:MultiplayerScore");
	Print_Centre_Text(title.c_str(), Dialog_X(424), Dialog_X(108), Dialog_Y(1));

	char buffer[256];
	std::string game = Fetch_String_UTF8("TXT_GAME");
	if (Session.Type == GAME_INTERNET) {
		sprintf(buffer, game.c_str(), WestwoodOnline_GameID);
	} else {
		sprintf(buffer, game.c_str(), Session.GamesPlayed);
	}
	Print_Text(buffer, Left_Edge(NAME_COLUMN), Dialog_Y(LINE_Y));

	Format_Time(buffer, Scen->ElapsedTimer / 60);
	Print_Right_Text(buffer, Dialog_X(248 + 105), Dialog_Y(LINE_Y));

	std::string heading_name = Fetch_String_UTF8("GUI:Player");
	std::string heading_kills = Fetch_String_UTF8("GUI:Kills");
	std::string heading_losses = Fetch_String_UTF8("GUI:Losses");
	std::string heading_built = Fetch_String_UTF8("GUI:Built");
	std::string heading_score = Fetch_String_UTF8("GUI:Score");
	Print_Text(heading_name.c_str(), Left_Edge(NAME_COLUMN), Dialog_Y(HEADING_Y));
	Print_Right_Text(heading_kills.c_str(), Right_Edge(KILLS_COLUMN), Dialog_Y(HEADING_Y));
	Print_Right_Text(heading_losses.c_str(), Right_Edge(LOSSES_COLUMN), Dialog_Y(HEADING_Y));
	Print_Right_Text(heading_built.c_str(), Right_Edge(BUILT_COLUMN), Dialog_Y(HEADING_Y));
	Print_Right_Text(heading_score.c_str(), Right_Edge(SCORE_COLUMN), Dialog_Y(HEADING_Y));

	RGBClass const edge(150, 150, 150);
	StageSurface->Draw_Rect(BUTTON_RECT, DSurface::Build_Hicolor_Pixel(edge));
	std::string caption = Fetch_String_UTF8("GUI:Continue");
	Print_Centre_Text(caption.c_str(), BUTTON_RECT.X, BUTTON_RECT.Width, BUTTON_RECT.Y + (BUTTON_RECT.Height - Font->Get_Font_Height()) / 2);

	BaseSurface->Blit_From(BaseSurface->Get_Rect(), *StageSurface, StageSurface->Get_Rect());
	Present(StageSurface->Get_Rect());
}


/// <summary>
/// Reveals each player's name on its row, one row at a time.
/// Each name is kept on the base surface, so the counting animation keeps it on screen.
/// </summary>
void MultiScore::Print_Player_Names(void)
{
	int const left = Left_Edge(NAME_COLUMN);
	int const right = Right_Edge(SCORE_COLUMN);

	for (int i = 0; i < Session.NumScores && i < MAX_ROWS; i++) {
		int const y = Row_Text_Y(i);
		Print_Text(Scores[i]->Name, left, y);

		Rect const row(left, y, right - left, Font->Get_Font_Height());
		BaseSurface->Blit_From(row, *StageSurface, row);
		Present(row);
		Wait_Delay(7);
	}

	Wait_Delay(30);
}


/// <summary>
/// Restores the table from the base surface, clearing every figure printed on it.
/// </summary>
void MultiScore::Restore_Table(void)
{
	StageSurface->Blit_From(TABLE_RECT, *BaseSurface, TABLE_RECT);
}


/// <summary>
/// Prints one player's figures on its row.
/// </summary>
void MultiScore::Print_Row(int row, int kills, int losses, int built, int score)
{
	char buffer[32];
	int const y = Row_Text_Y(row);

	sprintf(buffer, "%d", kills);
	Print_Right_Text(buffer, Right_Edge(KILLS_COLUMN), y);
	sprintf(buffer, "%d", losses);
	Print_Right_Text(buffer, Right_Edge(LOSSES_COLUMN), y);
	sprintf(buffer, "%d", built);
	Print_Right_Text(buffer, Right_Edge(BUILT_COLUMN), y);
	sprintf(buffer, "%d", score);
	Print_Right_Text(buffer, Right_Edge(SCORE_COLUMN), y);
}


/// <summary>
/// Counts the figures up to their final values.
/// Kills, losses and what each player built count up together, and then the score counts up
/// from zero.
/// </summary>
void MultiScore::Print_Figures(void)
{
	int const steps = 100;

	for (int step = 1; step <= steps; step++) {
		Restore_Table();
		for (int i = 0; i < Session.NumScores && i < MAX_ROWS; i++) {
			Print_Row(i, Scores[i]->Kills[0] * step / steps, Scores[i]->Lost[0] * step / steps, Scores[i]->Built[0] * step / steps, 0);
		}
		Present(TABLE_RECT);
		Play_Sound_Effect("Graph", 100);
		Wait_Delay(1);
	}

	for (int step = 1; step <= steps; step++) {
		Restore_Table();
		for (int i = 0; i < Session.NumScores && i < MAX_ROWS; i++) {
			Print_Row(i, Scores[i]->Kills[0], Scores[i]->Lost[0], Scores[i]->Built[0], Scores[i]->Score[0] * step / steps);
		}
		Present(TABLE_RECT);
		Play_Sound_Effect("Graph", 100);
		Wait_Delay(1);
	}

	Restore_Table();
	for (int i = 0; i < Session.NumScores && i < MAX_ROWS; i++) {
		Print_Row(i, Scores[i]->Kills[0], Scores[i]->Lost[0], Scores[i]->Built[0], Scores[i]->Score[0]);
	}
	Present(TABLE_RECT);
}


/// <summary>
/// Shows the prompt along the bottom of the screen.
/// </summary>
void MultiScore::Print_Prompt(void)
{
	std::string prompt = Fetch_String_UTF8("TXT_CLICK_CONTINUE");
	Print_Text(prompt.c_str(), PROMPT_RECT.X, PROMPT_RECT.Y);
	Present(PROMPT_RECT);
}


/// <summary>
/// Tallies the round's scores and fills in each house's row.
/// </summary>
/// <remarks>The Built value is the percentage of credits each house spent, relative to the
/// biggest spender. It is not yet confirmed to be what gamemd shows under the Built heading.</remarks>
void MultiScore::Tally_Score(void)
{
	HousesType house;
	HouseClass * hptr;
	HouseClass * hptr2;
	int score_index;
	int i;

	Session.NumScores = 0;

	/*
	 * The economy column measures each house against whoever spent most, so that figure is
	 * wanted before any house is scored.
	 */
	unsigned most_spent = 0;

	for (house = HOUSE_FIRST; house < Houses.Count(); house++) {
		hptr = Houses[house];

		if (hptr != NULL && !hptr->Class->IsMultiplayPassive && !hptr->IsObserver) {
			most_spent = std::max(most_spent, hptr->CreditsSpent);
		}
	}

	/*
	**	Loop through all houses, tallying up each player's score
	*/
	for (house = HOUSE_FIRST; house < Houses.Count(); house++) {
		hptr = Houses[house];

		/*
		**	Skip this house if it's not human.
		*/
		if (!hptr || hptr->Class->IsMultiplayPassive == true || hptr->IsObserver) {
			continue;
		}

		/*
		**	Just add this player to the end of the array, if there's room
		*/
		score_index = Session.NumScores++;

		/*
		**	Initialize this new score entry
		*/
		Session.Score[score_index].Wins = 0;
		strncpy(Session.Score[score_index].Name, Session.Shown_Name(hptr).c_str(), MPLAYER_NAME_MAX);
		Session.Score[score_index].Name[MPLAYER_NAME_MAX - 1] = '\0';

		/*
		**	Init this player's Kills to 0 (-1 means he didn't play this round;
		**	0 means he played but got no kills).
		*/
		Session.Score[score_index].Lost[0] = 0;
		Session.Score[score_index].Kills[0] = 0;
		Session.Score[score_index].Built[0] = 0;
		Session.Score[score_index].Score[0] = 0;

		/*
		**	Init this player's color to his last-used color index
		*/
		Session.Score[score_index].Scheme = House_Display_Scheme(hptr);

		/*
		**	If this house was undefeated, it must have been the winner.
		** (If no human houses are undefeated, the computer won.)
		*/
		if (!hptr->IsDefeated) {
			Session.Score[score_index].Wins++;
			Session.Winner = score_index;

			int score = 0;
			int count = 0;

			for (int i = 0; i < Houses.Count(); i++) {
				hptr2 = Houses[i];
				if (hptr2 != NULL && !hptr2->Class->IsMultiplayPassive && !hptr2->IsObserver && hptr2 != hptr) {
					score += hptr2->PointTotal;
					count++;
				}
			}

			if (count) {
				score /= count;
			}

			if (score <= 200) {
				score = 200;
			}

			Session.Score[score_index].Score[0] += score / 2;
		}

		/*
		**	Tally up all kills for this player
		*/
		int kills = 0;
		int losses = 0;

		for (i = 0; i < ARRAY_SIZE(hptr->UnitsKilled); i++) {
			kills += hptr->UnitsKilled[i];
		}
		for (i = 0; i < ARRAY_SIZE(hptr->BuildingsKilled); i++) {
			kills += hptr->BuildingsKilled[i];
		}
		Session.Score[score_index].Kills[0] = kills;

		losses += hptr->UnitsLost;
		losses += hptr->BuildingsLost;

		Session.Score[score_index].Lost[0] = losses;

		float kill_ratio = 0;
		if (losses > 0) {
			kill_ratio = (float)kills / (float)losses;
		}

		// Measuring what was spent leaves a house that was beaten showing what it built.
		float build_economy = most_spent > 0 ? (float)hptr->CreditsSpent / (float)most_spent : 0.0f;

		Session.Score[score_index].Built[0] = (int)(build_economy * 100.0);

		if (hptr->PointTotal > 0) {
			Session.Score[score_index].Score[0] += hptr->PointTotal;
		}

		DebugString(
			"%s: %s\n Scheme: %d\n Lost = %d\n Kills = %d\n Built = %d\n Score = %d\n",
			Session.Score[score_index].Name,
			Session.Score[score_index].Wins > 0 ? "Winner" : "Loser",
			Session.Score[score_index].Scheme,
			Session.Score[score_index].Lost[0],
			Session.Score[score_index].Kills[0],
			Session.Score[score_index].Built[0],
			Session.Score[score_index].Score[0]);
		DebugString(" KillRatio = %f\n BuildEconomy = %f\n", kill_ratio, build_economy);
	}
}


/// <summary>
/// Prepares the round's scores for presentation.
/// This routine ranks the players and then pads any score that would otherwise
/// undercut the player below it, so that the score column always reads as a
/// descending ladder.
/// </summary>
/// <remarks>Call this routine before any of the drawing routines; they all present the
/// players in the order it establishes.</remarks>
void MultiScore::Process_Scores(void)
{
	int i, j;

	memset(Scores, 0, sizeof(Scores));

	for (i = 0; i < Session.NumScores; i++) {
		Scores[i] = &Session.Score[i];
	}

	if (Session.NumScores > 1) {
		qsort(Scores, Session.NumScores, sizeof(MPlayerScoreType *), Sort_Scores);
	}

	int totalPoints = 0;
	int playerCount = 0;

	for (i = 0; i < Houses.Count(); i++) {
		HouseClass *house = Houses[i];
		if (house != NULL && !house->Class->IsMultiplayPassive && !house->IsObserver) {
			totalPoints += house->PointTotal;
			playerCount++;
		}
	}

	if (playerCount != 0) {
		totalPoints /= playerCount;
	}

	if (totalPoints <= 100) {
		totalPoints = 100;
	}

	for (i = 0; i < Session.NumScores; i++) {
		for (j = 0; j < Session.NumScores - 1; j++) {
			if (Scores[j]->Score[0] < Scores[j + 1]->Score[0]) {
				Scores[j]->Score[0] = Random_Pick(1, totalPoints) + Scores[j + 1]->Score[0];
			}
		}
	}
}


/// <summary>
/// Handles idle time during the score presentation.
/// The engine calls this routine whenever it is waiting on an animation or a delay.
/// </summary>
void MultiScore::Process_Idle(void)
{
	Callback();
}


/// <summary>
/// Services the network link while the score screen is up.
/// This routine is called from the presentation's idle handler.
/// </summary>
void MultiScore::Callback(void)
{
	Call_Back();
}
