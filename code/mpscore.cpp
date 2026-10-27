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
#include "_surface.h"
#include "autotest.h"
#include "ccrand.h"
#include "conquer.h"
#include "csf.h"
#include "dbgprint.h"
#include "dsurface.h"
#include "globals.h"
#include "houstype.h"
#include "incdec.h"
#include "keyboard.h"
#include "msengine.h"
#include "palette.h"
#include "playerdisplay.h"
#include "scheme.h"
#include "scorestage.h"
#include "session.h"
#include "stats.h"
#include "surface.h"
#include "theme.h"

#include "color.hh"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>


namespace {

/*
 * The table is laid out in dialog units of gamemd's MultiplayerScore dialog (dialog 264), on the
 * 800 by 600 stage. A band sits behind the Game and Time line, one behind the headings, and one
 * behind each player row, and each player row's text sits 4 units below its band.
 */
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

/*
 * The picture box. Its art is 632 by 568, and the 640 wide variant is stretched into the same box.
 */
Rect const PICTURE_RECT(0, 0, 632, 568);

/*
 * The table is every band and figure a player row uses, and the counting animation redraws it.
 */
Rect const TABLE_RECT = ScoreStageClass::Dialog_Rect(BAND_X, TOP_BAND_Y, BAND_WIDTH, PLAYER_BAND_Y + BAND_HEIGHT * MAX_ROWS - TOP_BAND_Y);

Rect const BUTTON_RECT = ScoreStageClass::Dialog_Rect(425, 326, 108, 23);
Rect const PROMPT_RECT = ScoreStageClass::Dialog_Rect(2, 353, 303, 14);

int Left_Edge(Column const & column)
{
	return(ScoreStageClass::Dialog_X(column.X));
}

int Right_Edge(Column const & column)
{
	return(ScoreStageClass::Dialog_X(column.X + column.Width));
}

int Row_Text_Y(int row)
{
	return(ScoreStageClass::Dialog_Y(PLAYER_TEXT_Y + BAND_HEIGHT * row));
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
		void Draw_Picture(void);
		void Draw_Static(void);
		void Set_Row_Color(int row);
		void Print_Player_Names(void);
		void Print_Row(int row, int kills, int losses, int built, int score);
		void Print_Figures(void);
		void Print_Prompt(void);

		bool Skirmish;
		ScoreStageClass Stage;
		MPlayerScoreType * Scores[MAX_PLAYERS];
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
	Skirmish(false)
{
	for (int i = 0; i < MAX_PLAYERS; i++) {
		Scores[i] = NULL;
	}
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
/// This routine runs the whole end of game presentation. It clears the whole screen, starts the
/// score theme, tallies and ranks the round's scores, draws the table, reveals each player's name,
/// counts the figures up, and then waits for the player before restoring the display and the mouse.
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

		Stage.Clear_Screen();
		Theme.Stop();
		Theme.Play_Song(Theme.From_Name("SCORE"));

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

		Stage.Clear_Screen();
		Theme.Stop(true);
		Theme.Queue_Song(THEME_NONE);

		Deinit();
		Keyboard->Clear();
		MouseCursor->Show_Mouse();
		MouseCursor->Capture_Mouse();
	}
	return(true);
}


/// <summary>
/// Prepares the score screen for display by opening its stage and recording the skirmish caption.
/// </summary>
/// <returns>bool; Was the screen prepared successfully?</returns>
bool MultiScore::Init(void)
{
	Deinit();

	if (!Stage.Open()) {
		return(false);
	}

	Skirmish = (Session.Type == GAME_SKIRMISH);

	Add_Sound_Effect("Graph", "TEXT1.AUD");

	return(true);
}


/// <summary>
/// Releases the stage and the neutral archives the score screen mounted.
/// This routine is called by Init before it builds the screen, and again when the
/// presentation is over. It is safe to call at any time.
/// </summary>
void MultiScore::Deinit(void)
{
	Stage.Close();
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
/// Draws the side's picture into its box on the stage.
/// </summary>
void MultiScore::Draw_Picture(void)
{
	char letter = ScoreStageClass::Side_Letter();
	bool const small = Stage.Is_Small();

	/*
	 * gamemd has no small picture for the third side, and uses the second side's instead.
	 */
	if (small && letter == 'Y') {
		letter = 'S';
	}

	char name[32];
	sprintf(name, "MP%cSCRN%s.SHP", letter, small ? "S" : "L");

	char palname[32];
	sprintf(palname, "MP%cSCRN.PAL", ScoreStageClass::Side_Letter());
	Stage.Load_Palette(palname);

	ShapeSet * shapes = Stage.Load_Shape(name);
	if (shapes == NULL) {
		DebugString("MultiScore: %s is missing\n", name);
		return;
	}
	Stage.Draw_Shape_Frame(shapes, 0, PICTURE_RECT);
	ScoreStageClass::Free_Shape(shapes);
}


/// <summary>
/// Draws the parts of the screen that stay put while the figures count up, and keeps them on the
/// base surface.
/// </summary>
void MultiScore::Draw_Static(void)
{
	Stage.Canvas()->Fill(TBLACK);
	Draw_Picture();

	char const side = ScoreStageClass::Side_Letter();
	char name[32];
	sprintf(name, "MP%cSCRNLBAR%02d.PCX", side, 1);
	Stage.Draw_Band(name, ScoreStageClass::Dialog_X(BAND_X), ScoreStageClass::Dialog_Y(TOP_BAND_Y));
	sprintf(name, "MP%cSCRNLBAR%02d.PCX", side, 2);
	Stage.Draw_Band(name, ScoreStageClass::Dialog_X(BAND_X), ScoreStageClass::Dialog_Y(HEADING_BAND_Y));
	for (int row = 0; row < MAX_ROWS; row++) {
		sprintf(name, "MP%cSCRNLBAR%02d.PCX", side, 3 + row);
		Stage.Draw_Band(name, ScoreStageClass::Dialog_X(BAND_X), ScoreStageClass::Dialog_Y(PLAYER_BAND_Y + BAND_HEIGHT * row));
	}

	std::string title = Fetch_String_UTF8(Skirmish ? "GUI:SkirmishScore" : "GUI:MultiplayerScore");
	Stage.Print_Centre(title.c_str(), ScoreStageClass::Dialog_X(424), ScoreStageClass::Dialog_X(108), ScoreStageClass::Dialog_Y(1));

	char buffer[256];
	std::string game = Fetch_String_UTF8("TXT_GAME");
	if (Session.Type == GAME_INTERNET) {
		sprintf(buffer, game.c_str(), WestwoodOnline_GameID);
	} else {
		sprintf(buffer, game.c_str(), Session.GamesPlayed);
	}
	Stage.Print_Left(buffer, Left_Edge(NAME_COLUMN), ScoreStageClass::Dialog_Y(LINE_Y));

	Format_Time(buffer, Scen->ElapsedTimer / 60);
	Stage.Print_Right(buffer, ScoreStageClass::Dialog_X(248 + 105), ScoreStageClass::Dialog_Y(LINE_Y));

	std::string heading_name = Fetch_String_UTF8("GUI:Player");
	std::string heading_kills = Fetch_String_UTF8("GUI:Kills");
	std::string heading_losses = Fetch_String_UTF8("GUI:Losses");
	std::string heading_built = Fetch_String_UTF8("GUI:Built");
	std::string heading_score = Fetch_String_UTF8("GUI:Score");
	Stage.Print_Left(heading_name.c_str(), Left_Edge(NAME_COLUMN), ScoreStageClass::Dialog_Y(HEADING_Y));
	Stage.Print_Right(heading_kills.c_str(), Right_Edge(KILLS_COLUMN), ScoreStageClass::Dialog_Y(HEADING_Y));
	Stage.Print_Right(heading_losses.c_str(), Right_Edge(LOSSES_COLUMN), ScoreStageClass::Dialog_Y(HEADING_Y));
	Stage.Print_Right(heading_built.c_str(), Right_Edge(BUILT_COLUMN), ScoreStageClass::Dialog_Y(HEADING_Y));
	Stage.Print_Right(heading_score.c_str(), Right_Edge(SCORE_COLUMN), ScoreStageClass::Dialog_Y(HEADING_Y));

	RGBClass const edge(150, 150, 150);
	Stage.Canvas()->Draw_Rect(BUTTON_RECT, DSurface::Build_Hicolor_Pixel(edge));
	std::string caption = Fetch_String_UTF8("GUI:Continue");
	Stage.Print_Centre(caption.c_str(), BUTTON_RECT.X, BUTTON_RECT.Width, BUTTON_RECT.Y + (BUTTON_RECT.Height - Stage.Text_Height()) / 2);

	Stage.Save_Base();
	Stage.Present(Rect(0, 0, 800, 600));
}


/*
 * Sets the text color to the house color of the player on the row. gamemd's score dialog colors every
 * cell of a row this way: its row loop (005c9b10) sends the scheme's HSV converted by 00517440, which
 * HSVClass's conversion matches.
 */
void MultiScore::Set_Row_Color(int row)
{
	RGBClass const color = ColorSchemes[Scores[row]->Scheme]->HSV;
	Stage.Set_Text_Color(color.Get_Red(), color.Get_Green(), color.Get_Blue());
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
		Set_Row_Color(i);
		Stage.Print_Left(Scores[i]->Name, left, y);

		Rect const row(left, y, right - left, Stage.Text_Height());
		Stage.Keep(row);
		Stage.Present(row);
		Wait_Delay(7);
	}

	Wait_Delay(30);
}


/// <summary>
/// Prints one player's figures on its row.
/// </summary>
void MultiScore::Print_Row(int row, int kills, int losses, int built, int score)
{
	char buffer[32];
	int const y = Row_Text_Y(row);

	Set_Row_Color(row);
	sprintf(buffer, "%d", kills);
	Stage.Print_Right(buffer, Right_Edge(KILLS_COLUMN), y);
	sprintf(buffer, "%d", losses);
	Stage.Print_Right(buffer, Right_Edge(LOSSES_COLUMN), y);
	sprintf(buffer, "%d", built);
	Stage.Print_Right(buffer, Right_Edge(BUILT_COLUMN), y);
	sprintf(buffer, "%d", score);
	Stage.Print_Right(buffer, Right_Edge(SCORE_COLUMN), y);
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
		Stage.Restore(TABLE_RECT);
		for (int i = 0; i < Session.NumScores && i < MAX_ROWS; i++) {
			Print_Row(i, Scores[i]->Kills[0] * step / steps, Scores[i]->Lost[0] * step / steps, Scores[i]->Built[0] * step / steps, 0);
		}
		Stage.Present(TABLE_RECT);
		Play_Sound_Effect("Graph", 100);
		Wait_Delay(1);
	}

	for (int step = 1; step <= steps; step++) {
		Stage.Restore(TABLE_RECT);
		for (int i = 0; i < Session.NumScores && i < MAX_ROWS; i++) {
			Print_Row(i, Scores[i]->Kills[0], Scores[i]->Lost[0], Scores[i]->Built[0], Scores[i]->Score[0] * step / steps);
		}
		Stage.Present(TABLE_RECT);
		Play_Sound_Effect("Graph", 100);
		Wait_Delay(1);
	}

	Stage.Restore(TABLE_RECT);
	for (int i = 0; i < Session.NumScores && i < MAX_ROWS; i++) {
		Print_Row(i, Scores[i]->Kills[0], Scores[i]->Lost[0], Scores[i]->Built[0], Scores[i]->Score[0]);
	}
	Stage.Present(TABLE_RECT);
}


/// <summary>
/// Shows the prompt along the bottom of the screen.
/// </summary>
void MultiScore::Print_Prompt(void)
{
	Stage.Set_Text_Color(255, 255, 0);
	std::string prompt = Fetch_String_UTF8("TXT_CLICK_CONTINUE");
	Stage.Print_Left(prompt.c_str(), PROMPT_RECT.X, PROMPT_RECT.Y);
	Stage.Present(PROMPT_RECT);
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
