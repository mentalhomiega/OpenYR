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

/* $Header: /CounterStrike/OPTIONS.CPP 1     3/03/97 10:25a Joe_bostic $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : OPTIONS.CPP                                                  *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : June 8, 1994                                                 *
 *                                                                                             *
 *                  Last Update : July 30, 1996 [JLB]                                          *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   OptionsClass::Adjust_Palette -- Adjusts the palette according to the settings specified.  *
 *   OptionsClass::Fixup_Palette -- Adjusts the real palette to match the palette sliders.     *
 *   OptionsClass::Get_Brightness -- Fetches the current brightness setting.                   *
 *   OptionsClass::Get_Contrast -- Gets the current contrast setting.                          *
 *   OptionsClass::Get_Game_Speed -- Fetches the current game speed setting.                   *
 *   OptionsClass::Get_Saturation -- Fetches the current color setting.                        *
 *   OptionsClass::Get_Scroll_Rate -- Fetches the current scroll rate setting.                 *
 *   OptionsClass::Get_Tint -- Fetches the current tint setting.                               *
 *   OptionsClass::Load_Settings -- reads options settings from the INI file                   *
 *   OptionsClass::Normalize_Delay -- Normalizes delay factor to keep rate constant.           *
 *   OptionsClass::Normalize_Volume -- Convert to a real volume value.                         *
 *   OptionsClass::One_Time -- This performs any one time initialization for the options class.*
 *   OptionsClass::OptionsClass -- The default constructor for the options class.              *
 *   OptionsClass::Process -- Handles all the options graphic interface.                       *
 *   OptionsClass::Save_Settings -- writes options settings to the INI file                    *
 *   OptionsClass::Set -- Sets options based on current settings                               *
 *   OptionsClass::Set_Brightness -- Sets the brightness level to that specified.              *
 *   OptionsClass::Set_Contrast -- Sets the contrast to the value specified.                   *
 *   OptionsClass::Set_Game_Speed -- Sets the game speed as specified.                         *
 *   OptionsClass::Set_Repeat -- Controls the score repeat option.                             *
 *   OptionsClass::Set_Saturation -- Sets the color to the value specified.                    *
 *   OptionsClass::Set_Score_Volume -- Sets the global score volume to that specified.         *
 *   OptionsClass::Set_Scroll_Rate -- Sets the scroll rate as specified.                       *
 *   OptionsClass::Set_Shuffle -- Controls the play shuffle setting.                           *
 *   OptionsClass::Set_Sound_Volume -- Sets the sound effects volume level.                    *
 *   OptionsClass::Set_Tint -- Sets the tint setting.                                          *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "options.h"

#include "_command.h"
#include "_deploymentconfig.h"
#include "_map.h"
#include "_rules.h"
#include "audio/audioengine.h"
#include "ccfile.h"
#include "ccrand.h"
#include "command.h"
#include "dbgprint.h"
#include "deploymentconfig.h"
#include "dsurface.h"
#include "globals.h"
#include "hud.h"
#include "init.h"
#include "ipxmgr.h"
#include "keyboard.h"
#include "language/language.h"
#include "mouse.h"
#include "msgbox.h"
#include "numparse.h"
#include "playerdisplay.h"
#include "rules.h"
#include "session.h"
#include "techno.h"
#include "theme.h"
#include "ui/screens/keyboard/uikeyboard.h"
#include "vector.h"
#include "video.h"
#include "vox.h"

#include "diff.hh"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "colourblind.h"
#include "emissive.h"
#include "goptions.h"


char const * const OptionsClass::HotkeyName = "WinHotkeys";


/// <summary>
/// Reads the movies the player has seen from the NetID entry of the settings file's Network
/// section, where Yuri's Revenge keeps them. The entry holds the text "soviet allied", such as
/// "1 2" for the first Soviet movie and the first two Allied ones, as one hex number per
/// character with every bit flipped: "ffce,ffdf,ffcd,". A missing or unreadable entry means none,
/// and so does a count outside 0 to 8.
/// </summary>
void OptionsClass::Load_Seen_Movies(void)
{
	int & soviet = LastSovietMovie;
	int & allied = LastAlliedMovie;
	soviet = -1;
	allied = -1;

	char list[256];
	ConfigINI.Get_String("Network", "NetID", "", list, sizeof(list));
	std::string text;
	for (char const * at = list; *at != '\0'; ) {
		unsigned value = 0;
		int used = 0;
		if (sscanf(at, "%x%n", &value, &used) != 1 || value == 0) {
			break;
		}
		unsigned short const character = (unsigned short)~value;
		text.push_back(character < 0x80 ? (char)character : '?');
		at += used;
		while (*at == ',') {
			at++;
		}
	}

	int * const slots[2] = {&soviet, &allied};
	char const * at = text.c_str();
	for (int * slot : slots) {
		at += std::strspn(at, " ");
		if (*at == '\0') {
			break;
		}
		char * end = nullptr;
		*slot = (int)std::strtol(at, &end, 10) - 1;
		at = end;
		at += std::strcspn(at, " ");
	}

	soviet = (soviet < -1 || soviet >= 8) ? -1 : soviet;
	allied = (allied < -1 || allied >= 8) ? -1 : allied;
}


/// <summary>
/// Writes the movies the player has seen in the form Load_Seen_Movies reads.
/// </summary>
void OptionsClass::Save_Seen_Movies(void) const
{
	char text[32];
	snprintf(text, sizeof(text), "%d %d", LastSovietMovie + 1, LastAlliedMovie + 1);

	std::string list;
	for (char const * at = text; *at != '\0'; at++) {
		char entry[16];
		snprintf(entry, sizeof(entry), "%x,", (unsigned)(unsigned short)~(unsigned short)*at);
		list += entry;
	}
	ConfigINI.Put_String("Network", "NetID", list.c_str());
}


/***********************************************************************************************
 * OptionsClass::OptionsClass -- The default constructor for the options class.                *
 *                                                                                             *
 *    This is the constructor for the options class. It handles setting up all the globals     *
 *    necessary for the options. This includes setting them to their default state.            *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/21/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
OptionsClass::OptionsClass(void) :
	Difficulty(DIFF_NORMAL),
	GameSpeed(3),
	ScrollRate(3),
	SoundVolume(.7f),
	VoiceVolume(1.0f),
	ScoreVolume(.5f),
	BitmapSystemFont(false),
	AutoScroll(true),
	IsScoreRepeat(false),
	IsScoreShuffle(false),
	IsSidebarOnRight(true),
	ModernInterface(true),
	IsModernInterfaceChosen(true),
	SidebarRows(HUD_DEFAULT_ROWS),
	DebugMode(false),
	PlayerColorView(0),
	SidebarCameoText(false),
	SidebarSorting(true),
	ActionLines(true),
	ShowHidden(false),
	ScreenShake(true),
	IsClassicMenus(false),
	MenuTheme(MENU_THEME_RANDOM),
	MenuBattle(true),
	ToolTips(true),
	AltToRally(false),
	AircraftBodyPick(true),
	SimulateWhileUnfocused(false),
	TextBackgroundColor(12),
	AutoSaveInterval(10800),
	ScreenWidth(-1),
	ScreenHeight(-1),
	ScrollMethod(0),
	DetailLevel(2),
	StretchMovies(0),
	Fullscreen(true),
	WindowWidth(-1),
	WindowHeight(-1),
	ScaleMode(VIDEO_SCALE_PIXELART),
	IntegerScaling(false),
	ShowFPS(false),
	RenderFrameRate(RENDER_FRAME_RATE_MATCH_DISPLAY),
	FrameRateLimit(-1),
	VSync(true),
	HDRHighlights(HDR_HIGHLIGHTS_ENHANCED),
	Renderer(0),
	HDROutput(VIDEO_HDR_AUTOMATIC),
	HDRPaperWhite(0),
	HDRPeakBrightness(0),
	ColorBlindMode(COLORBLIND_OFF),
	ColorBlindStrength(100),
	Brightness(100),
	Gamma(1.0f),
	CursorScale(0),
	InterfaceScale(0.0f),
	SystemCursor(true),
	SoundLatency(9),
	KeyForceMove1(KN_LALT),
	KeyForceMove2(KN_LALT),
	KeyForceAttack1(KN_LCTRL),
	KeyForceAttack2(KN_LCTRL),
	KeySelect1(KN_LSHIFT),
	KeySelect2(KN_LSHIFT),
	KeyQueueMove1(KN_Q),
	KeyQueueMove2(KN_Q),
	KeyAttackMove1(KN_NONE),
	KeyAttackMove2(KN_NONE),
	LastSovietMovie(-1),
	LastAlliedMovie(-1)
{
}


/***********************************************************************************************
 * OptionsClass::One_Time -- This performs any one time initialization for the options class.  *
 *                                                                                             *
 *    This routine should be called only once and it will perform any initializations for the  *
 *    options class that is needed. This may include things like file loading and memory       *
 *    allocation.                                                                              *
 *                                                                                             *
 * INPUT:   none                                                                               *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   Only call this routine once.                                                    *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/21/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void OptionsClass::One_Time(void)
{
}


/***********************************************************************************************
 * OptionsClass::Set_Shuffle -- Controls the play shuffle setting.                             *
 *                                                                                             *
 *    This routine will control the score shuffle flag. The setting to use is provided as      *
 *    a parameter. When shuffling is on, the score play order is scrambled.                    *
 *                                                                                             *
 * INPUT:   on -- Should the shuffle option be activated?                                      *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void OptionsClass::Set_Shuffle(bool on)
{
	IsScoreShuffle = on;
	Theme.Set_Shuffle(on);
	DebugString("ScoreShuffle is %s\n", IsScoreShuffle == true ? "ON" : "OFF");
}


/***********************************************************************************************
 * OptionsClass::Set_Repeat -- Controls the score repeat option.                               *
 *                                                                                             *
 *    This routine is used to control whether scores repeat or not. The setting to use for     *
 *    the repeat flag is provided as a parameter.                                              *
 *                                                                                             *
 * INPUT:   on -- Should the scores repeat?                                                    *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void OptionsClass::Set_Repeat(bool on)
{
	IsScoreRepeat = on;
	Theme.Set_Repeat(on);
	DebugString("ScoreRepeat is %s\n", IsScoreRepeat == true ? "ON" : "OFF");
}


/***********************************************************************************************
 * OptionsClass::Set_Score_Volume -- Sets the global score volume to that specified.           *
 *                                                                                             *
 *    This routine will set the global score volume to the value specified. The value ranges   *
 *    from zero to 255.                                                                        *
 *                                                                                             *
 * INPUT:   volume   -- The new volume setting to use for scores.                              *
 *                                                                                             *
 *          feedback -- Should a feedback sound effect be generated?                           *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void OptionsClass::Set_Score_Volume(float volume, bool feedback)
{
	ScoreVolume = std::min(volume, 1.0f);
	Theme.Set_Volume(ScoreVolume * 255.0);
	if (feedback && !Theme.Still_Playing()) {
		Sound_Effect(Rule->GenericBeep, ScoreVolume);
	}
	DebugString("ScoreVolume = %f\n", ScoreVolume);
}


/***********************************************************************************************
 * OptionsClass::Set_Sound_Volume -- Sets the sound effects volume level.                      *
 *                                                                                             *
 *    This routine will set the sound effect volume level as indicated. It can generate a      *
 *    sound effect for feedback purposes if desired. The volume setting can range from zero    *
 *    to 255. The value of 255 is the loudest.                                                 *
 *                                                                                             *
 * INPUT:   volume   -- The volume setting to use for the new value. 0 to 255.                 *
 *                                                                                             *
 *          feedback -- Should a feedback sound effect be generated?                           *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   01/19/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
void OptionsClass::Set_Sound_Volume(float volume, bool feedback)
{
	SoundVolume = std::min(volume, 1.0f);
	AudioEngine.Set_Group_Gain(AUDIO_GROUP_SFX, SoundVolume);
	AudioEngine.Set_Group_Gain(AUDIO_GROUP_MOVIE, SoundVolume);
	if (feedback) {
		Sound_Effect(Rule->GenericBeep);
	}
	DebugString("SoundVolume = %f\n", SoundVolume);
}


/// <summary>
/// Sets the speech volume level.
/// This routine will set the volume that the EVA and taunt speech plays back at. It can
/// generate a feedback sound if desired -- a taunt while a game is in progress, and a plain
/// beep otherwise.
/// </summary>
/// <param name="volume">The volume setting to use for the new value. Zero to one.</param>
/// <param name="feedback">Should a feedback sound be generated?</param>
void OptionsClass::Set_Voice_Volume(float volume, bool feedback)
{
	VoiceVolume = std::min(volume, 1.0f);
	Set_Speech_Volume(VoiceVolume * 255.0);
	if (feedback) {
		if (GameActive == true) {
			if (!Is_Speaking()) {
				Speak(Sim_Random_Pick(VOX_GDI_TAUNT_01, VOX_NOD_TAUNT_10), true);
			}
		} else {
			Voice_Sound_Effect(Rule->GenericBeep, VoiceVolume);
		}
	}
	DebugString("VoiceVolume = %f\n", VoiceVolume);
}


/// <summary>
/// Turns the name of a frame filter into the filter itself.
/// </summary>
/// <param name="name">The name as it appears in the settings file.</param>
/// <param name="fallback">What to return when the name is not one of them.</param>
/// <returns>int; One of the VideoScaleMode values.</returns>
static int Scale_Mode_From_Name(char const * name, int fallback)
{
	if (stricmp(name, "Nearest") == 0) return(VIDEO_SCALE_NEAREST);
	if (stricmp(name, "Linear") == 0) return(VIDEO_SCALE_LINEAR);
	if (stricmp(name, "PixelArt") == 0) return(VIDEO_SCALE_PIXELART);

	return(fallback);
}


static int HDR_Output_From_Name(char const * name)
{
	if (stricmp(name, "Off") == 0) return(VIDEO_HDR_OFF);
	if (stricmp(name, "On") == 0) return(VIDEO_HDR_ON);
	return(VIDEO_HDR_AUTOMATIC);
}


static int ColorBlind_Mode_From_Name(char const * name)
{
	if (stricmp(name, "Protanopia") == 0) return(COLORBLIND_PROTANOPIA);
	if (stricmp(name, "Deuteranopia") == 0) return(COLORBLIND_DEUTERANOPIA);
	if (stricmp(name, "Tritanopia") == 0) return(COLORBLIND_TRITANOPIA);
	return(COLORBLIND_OFF);
}


static char const * ColorBlind_Mode_Name(int mode)
{
	switch (mode) {
		case COLORBLIND_PROTANOPIA:
			return("Protanopia");

		case COLORBLIND_DEUTERANOPIA:
			return("Deuteranopia");

		case COLORBLIND_TRITANOPIA:
			return("Tritanopia");

		default:
			return("Off");
	}
}


/// <summary>
/// Reads a gamma exponent. Text that is not a positive number gives 1.0, and any other value is limited
/// to the range of the setting.
/// </summary>
static float Gamma_From_Text(char const * text)
{
	double const value = Parse_Nearest_Double(text, nullptr);
	if (!(value > 0.0)) {
		return(1.0f);
	}
	return((float)std::clamp(value, VIDEO_GAMMA_MIN / 100.0, VIDEO_GAMMA_MAX / 100.0));
}


static char const * HDR_Output_Name(int mode)
{
	switch (mode) {
		case VIDEO_HDR_OFF:
			return("Off");

		case VIDEO_HDR_ON:
			return("On");

		default:
			return("Automatic");
	}
}


/// <summary>
/// Reads a nit setting. "Auto", or anything that is not a positive number, gives zero.
/// </summary>
static int HDR_Nits_From_Text(char const * text)
{
	double const value = Parse_Nearest_Double(text, nullptr);
	return((value > 0.0) ? (int)std::min(value, 10000.0) : 0);
}


/// <summary>
/// Writes a nit setting as a number, or as "Auto" when it is zero.
/// </summary>
static void Save_HDR_Nits(char const * key, int nits)
{
	if (nits > 0) {
		ConfigINI.Put_Int("Video", key, nits);
	} else {
		ConfigINI.Put_String("Video", key, "Auto");
	}
}


/// <summary>
/// Names the frame filter setting for writing back.
/// </summary>
/// <param name="mode">One of the VideoScaleMode values.</param>
/// <returns>The name the settings file uses for that filter.</returns>
static char const * Scale_Mode_Name(int mode)
{
	switch (mode) {
		case VIDEO_SCALE_NEAREST:
			return("Nearest");

		case VIDEO_SCALE_LINEAR:
			return("Linear");

		default:
			return("PixelArt");
	}
}


/***********************************************************************************************
 * OptionsClass::Load_Settings -- reads options settings from the INI file                     *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   02/14/1995 BR : Created.                                                                  *
 *   07/03/1996 JLB : Reworked to use new INI handler.                                         *
 *   07/30/1996 JLB : Handles hotkeys.                                                         *
 *=============================================================================================*/
/// <summary>
/// Turns the emissive masks on for Enhanced highlights while HDR output is on, and for the debug view.
/// The masks follow the renderer, so this is called again once the renderer has started.
/// </summary>
void Apply_HDR_Highlights(void)
{
	bool const enhanced = Video_HDR_Active() && Options.HDRHighlights == HDR_HIGHLIGHTS_ENHANCED;
	Emissive_Set_Enabled(enhanced || Emissive_Debug_View());
}


void OptionsClass::Load_Settings(void)
{
	DebugString("--------- Loading %s settings ---------------\n", DeploymentConfig.SettingsFile.c_str());

	/*
	**	Read in the Options values
	*/
	GameSpeed = ConfigINI.Get_Int("Options", "GameSpeed", GameSpeed);
	DebugString("GameSpeed = %d\n", GameSpeed);

	Difficulty = ConfigINI.Get_Int("Options", "Difficulty", Difficulty);
	Difficulty = std::min(Difficulty, (int)DIFF_COUNT - 1);
	Difficulty = std::max(Difficulty, 0);
	DebugString("Difficulty = %d\n", Difficulty);

	char last_mission[128];
	ConfigINI.Get_String("Options", "LastMission", "", last_mission, sizeof(last_mission));
	LastMission = last_mission;

	ScrollMethod = ConfigINI.Get_Int("Options", "ScrollMethod", ScrollMethod);
	DebugString("ScrollMethod = %d\n", ScrollMethod);

	ScrollRate = ConfigINI.Get_Int("Options", "ScrollRate", ScrollRate);
	DebugString("ScrollRate = %d\n", ScrollRate);

	AutoScroll = ConfigINI.Get_Bool("Options", "AutoScroll", AutoScroll);
	DebugString("AutoScroll is %s\n", AutoScroll == true ? "ON" : "OFF");


	BitmapSystemFont = ConfigINI.Get_Bool("Options", "BitmapSystemFont", BitmapSystemFont);
	DebugString("BitmapSystemFont is %s\n", BitmapSystemFont == true ? "ON" : "OFF");

	DetailLevel = ConfigINI.Get_Int("Options", "DetailLevel", DetailLevel);
	DetailLevel = std::min(DetailLevel, 2);
	DetailLevel = std::max(DetailLevel, 0);
	DebugString("DetailLevel = %d\n", DetailLevel);

	IsSidebarOnRight = true;
	DebugString("SideBar on %s\n", IsSidebarOnRight == true ? "RIGHT" : "LEFT");

	SidebarCameoText = ConfigINI.Get_Bool("Options", "SidebarCameoText", SidebarCameoText);
	DebugString("Sidebar Text is %s\n", SidebarCameoText == true ? "ON" : "OFF");

	SidebarSorting = ConfigINI.Get_Bool("Options", "SidebarSorting", SidebarSorting);
	DebugString("Sidebar Sorting is %s\n", SidebarSorting == true ? "ON" : "OFF");

	ActionLines = ConfigINI.Get_Bool("Options", "UnitActionLines", ActionLines);
	DebugString("ActionLines are %s\n", ActionLines == true ? "ON" : "OFF");

	ShowHidden = ConfigINI.Get_Bool("Options", "ShowHidden", ShowHidden);
	ScreenShake = ConfigINI.Get_Bool("Options", "ScreenShake", ScreenShake);
	DebugString("ShowHidden is %s\n", ShowHidden == true ? "ON" : "OFF");

	Load_Menu_Style();

	ToolTips = ConfigINI.Get_Bool("Options", "ToolTips", ToolTips);
	DebugString("ToolTips are %s\n", ToolTips == true ? "ON" : "OFF");

	DebugMode = ConfigINI.Get_Bool("Options", "DebugMode", DebugMode);
	DebugString("DebugMode is %s\n", DebugMode == true ? "ON" : "OFF");

	char playercolors[32];
	ConfigINI.Get_String("Options", "PlayerColors", "Default", playercolors, sizeof(playercolors));
	PlayerColorView = (stricmp(playercolors, "ColorBlindSafe") == 0) ? PLAYER_VIEW_COLORBLIND_SAFE
		: (stricmp(playercolors, "Relationship") == 0) ? PLAYER_VIEW_RELATIONSHIP : PLAYER_VIEW_DEFAULT;

	// Turns on the per-frame check value log, as -SYNCLOG does; a false or absent value leaves the switch as it was.
	if (ConfigINI.Get_Bool("Options", "SyncLog", false)) {
		Debug_SyncLog = true;
	}
	DebugString("SyncLog is %s\n", Debug_SyncLog == true ? "ON" : "OFF");

	AltToRally = ConfigINI.Get_Bool("Options", "AltToRally", AltToRally);
	DebugString("AltToRally is %s\n", AltToRally == true ? "ON" : "OFF");

	AircraftBodyPick = ConfigINI.Get_Bool("Options", "AircraftBodyPick", AircraftBodyPick);
	DebugString("AircraftBodyPick is %s\n", AircraftBodyPick == true ? "ON" : "OFF");

	SimulateWhileUnfocused = ConfigINI.Get_Bool("Options", "SimulateWhileUnfocused", SimulateWhileUnfocused);
	DebugString("SimulateWhileUnfocused is %s\n", SimulateWhileUnfocused == true ? "ON" : "OFF");

	TextBackgroundColor = ConfigINI.Get_Int("Options", "TextBackgroundColor", TextBackgroundColor);
	DebugString("TextBackgroundColor = %d\n", TextBackgroundColor);

	AutoSaveInterval = ConfigINI.Get_Int("Options", "AutoSaveInterval", AutoSaveInterval);
	DebugString("AutoSaveInterval = %d\n", AutoSaveInterval);

	ScreenWidth = ConfigINI.Get_Int("Video", "ScreenWidth", ScreenWidth);
	ScreenHeight = ConfigINI.Get_Int("Video", "ScreenHeight", ScreenHeight);
	DebugString("Resolution = %d X %d\n", ScreenWidth, ScreenHeight);

	StretchMovies = ConfigINI.Get_Bool("Video", "StretchMovies", StretchMovies);
	DebugString("StretchMovies is %s\n", StretchMovies == true ? "ON" : "OFF");

	IntegerScaling = ConfigINI.Get_Bool("Video", "IntegerScaling", IntegerScaling);
	ShowFPS = ConfigINI.Get_Bool("Video", "ShowFPS", ShowFPS);
	RenderFrameRate = std::clamp(ConfigINI.Get_Int("Video", "RenderFrameRate", RenderFrameRate), -1, 1000);
	FrameRateLimit = ConfigINI.Get_Int("Video", "FrameRateLimit", FrameRateLimit);

	char highlightsname[16];
	ConfigINI.Get_String("Video", "HDRHighlights", "Enhanced", highlightsname, sizeof(highlightsname));
	HDRHighlights = stricmp(highlightsname, "Classic") == 0 ? HDR_HIGHLIGHTS_CLASSIC : HDR_HIGHLIGHTS_ENHANCED;
	Emissive_Set_Debug_View(ConfigINI.Get_Bool("Video", "EmissiveDebugView", false));
	Apply_HDR_Highlights();

	char colorblindname[16];
	ConfigINI.Get_String("Video", "ColorBlindMode", "Off", colorblindname, sizeof(colorblindname));
	ColorBlindMode = ColorBlind_Mode_From_Name(colorblindname);
	ColorBlindStrength = std::clamp(ConfigINI.Get_Int("Video", "ColorBlindStrength", ColorBlindStrength), COLORBLIND_STRENGTH_MIN, COLORBLIND_STRENGTH_MAX);
	DebugString("ColorBlindMode is %s, ColorBlindStrength is %d\n", ColorBlind_Mode_Name(ColorBlindMode), ColorBlindStrength);

	Brightness = std::clamp(ConfigINI.Get_Int("Video", "Brightness", Brightness), VIDEO_BRIGHTNESS_MIN, VIDEO_BRIGHTNESS_MAX);
	char gammatext[16];
	ConfigINI.Get_String("Video", "Gamma", "1.0", gammatext, sizeof(gammatext));
	Gamma = Gamma_From_Text(gammatext);
	DebugString("Brightness is %d, Gamma is %g\n", Brightness, Gamma);

	DebugString("RenderFrameRate is %d\n", RenderFrameRate);

	char scalename[32];
	ConfigINI.Get_String("Video", "ScaleMode", (char *)Scale_Mode_Name(ScaleMode), scalename, sizeof(scalename));
	ScaleMode = Scale_Mode_From_Name(scalename, ScaleMode);
	DebugString("ScaleMode is %d, IntegerScaling is %s\n", ScaleMode, IntegerScaling == true ? "ON" : "OFF");

	CursorScale = ConfigINI.Get_Int("Video", "CursorScale", CursorScale);
	SystemCursor = ConfigINI.Get_Bool("Video", "SystemCursor", SystemCursor);

	Set_Sound_Volume(ConfigINI.Get_Float("Audio", "SoundVolume", SoundVolume), false);
	Set_Voice_Volume(ConfigINI.Get_Float("Audio", "VoiceVolume", VoiceVolume), false);
	Set_Score_Volume(ConfigINI.Get_Float("Audio", "ScoreVolume", ScoreVolume), false);

	Set_Repeat(ConfigINI.Get_Bool("Audio", "IsScoreRepeat", IsScoreRepeat));
	Set_Shuffle(ConfigINI.Get_Bool("Audio", "IsScoreShuffle", IsScoreShuffle));

	SoundLatency = ConfigINI.Get_Int("Audio", "SoundLatency", SoundLatency);
	DebugString("Emulated sound card latency default = %d\n", SoundLatency);

	Load_Seen_Movies();

	DebugString("--------- Complete -------------------------------\n");

	Map.Toggle_Cameo_Text(SidebarCameoText);
	TechnoClass::Set_Action_Lines(ActionLines);
}


/// <summary>
/// Reads the menu style from the settings file. Startup calls it before the menus are made,
/// since the style decides their scale.
/// </summary>
void OptionsClass::Load_Menu_Style(void)
{
	char menustyle[32];
	ConfigINI.Get_String("Options", "MenuStyle", "Modern", menustyle, sizeof(menustyle));
	IsClassicMenus = (stricmp(menustyle, "Classic") == 0);

	char theme[16];
	ConfigINI.Get_String("Options", "MenuTheme", "Random", theme, sizeof(theme));
	MenuTheme = (stricmp(theme, "Allied") == 0) ? MENU_THEME_ALLIED
		: (stricmp(theme, "Soviet") == 0) ? MENU_THEME_SOVIET
		: (stricmp(theme, "Yuri") == 0) ? MENU_THEME_YURI
		: MENU_THEME_RANDOM;
	MenuBattle = ConfigINI.Get_Bool("Options", "MenuBattle", MenuBattle);
}


/// <summary>
/// Reads the in-game interface style from the settings file. Startup calls it before the
/// surfaces are made, since the style decides the size of the map view.
/// </summary>
void OptionsClass::Load_Interface_Style(void)
{
	char style[16];
	ConfigINI.Get_String("Options", "InterfaceStyle", "Modern", style, sizeof(style));
	ModernInterface = (stricmp(style, "Modern") == 0);
	IsModernInterfaceChosen = ModernInterface;
	SidebarRows = ConfigINI.Get_Int("Options", "SidebarRows", HUD_DEFAULT_ROWS);
	DebugString("Interface style is %s\n", ModernInterface ? "Modern" : "Classic");
}


/// <summary>
/// Reads the interface scale from the settings file. Startup calls it before the window is made,
/// since it decides the size of the frame the interface is drawn in. "Auto", or nothing, leaves
/// the choice to the screen height.
/// </summary>
void OptionsClass::Load_Interface_Scale(void)
{
	char text[32];
	ConfigINI.Get_String("Video", "InterfaceScale", "Auto", text, sizeof(text));

	float value = (float)Parse_Nearest_Double(text, nullptr);
	InterfaceScale = (value > 0.0f) ? std::min(value, 8.0f) : 0.0f;
	DebugString("InterfaceScale is %s\n", InterfaceScale > 0.0f ? text : "automatic");
}


/// <summary>
/// Reads the HDR output settings from the settings file. Startup calls it before the window is
/// made, since the window's renderer is started with the output decided by them.
/// </summary>
void OptionsClass::Load_HDR_Output(void)
{
	char text[32];
	ConfigINI.Get_String("Video", "HDROutput", "Automatic", text, sizeof(text));
	HDROutput = HDR_Output_From_Name(text);

	ConfigINI.Get_String("Video", "HDRPaperWhite", "Auto", text, sizeof(text));
	HDRPaperWhite = HDR_Nits_From_Text(text);

	ConfigINI.Get_String("Video", "HDRPeakBrightness", "Auto", text, sizeof(text));
	HDRPeakBrightness = HDR_Nits_From_Text(text);
	DebugString("HDROutput is %s, HDRPaperWhite is %d, HDRPeakBrightness is %d\n", HDR_Output_Name(HDROutput), HDRPaperWhite, HDRPeakBrightness);
}


/***********************************************************************************************
 * OptionsClass::Save_Settings -- writes options settings to the INI file                      *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   02/14/1995 BR : Created.                                                                  *
 *   07/03/1996 JLB : Revamped and tightened up.                                               *
 *   07/30/1996 JLB : Handles hotkeys.                                                         *
 *=============================================================================================*/
void OptionsClass::Save_Settings (void)
{
	CCFileClass file(DeploymentConfig.SettingsFile.c_str());

	DebugString("Saving game settings\n");

	/*
	**	Save Options settings
	*/
	ConfigINI.Put_Int("Options", "GameSpeed", GameSpeed);
	ConfigINI.Put_Int("Options", "Difficulty", Difficulty);
	ConfigINI.Put_String("Options", "LastMission", LastMission.c_str());
	ConfigINI.Put_Int("Options", "ScrollMethod", ScrollMethod);
	ConfigINI.Put_Int("Options", "ScrollRate", ScrollRate);
	ConfigINI.Put_Bool("Options", "AutoScroll", AutoScroll);
	ConfigINI.Put_Bool("Options", "BitmapSystemFont", BitmapSystemFont);
	ConfigINI.Put_Int("Options", "DetailLevel", DetailLevel);
	ConfigINI.Put_Bool("Options", "SidebarCameoText", SidebarCameoText);
	ConfigINI.Put_Bool("Options", "SidebarSorting", SidebarSorting);
	ConfigINI.Put_Bool("Options", "UnitActionLines", ActionLines);
	ConfigINI.Put_Bool("Options", "ShowHidden", ShowHidden);
	ConfigINI.Put_Bool("Options", "ScreenShake", ScreenShake);
	ConfigINI.Put_String("Options", "MenuStyle", IsClassicMenus ? "Classic" : "Modern");
	ConfigINI.Put_String("Options", "MenuTheme", MenuTheme == MENU_THEME_ALLIED ? "Allied"
		: MenuTheme == MENU_THEME_SOVIET ? "Soviet" : MenuTheme == MENU_THEME_YURI ? "Yuri" : "Random");
	ConfigINI.Put_Bool("Options", "MenuBattle", MenuBattle);
	ConfigINI.Put_String("Options", "InterfaceStyle", IsModernInterfaceChosen ? "Modern" : "Classic");
	ConfigINI.Put_Int("Options", "SidebarRows", SidebarRows);
	ConfigINI.Put_Bool("Options", "DebugMode", DebugMode);
	ConfigINI.Put_String("Options", "PlayerColors", PlayerColorView == PLAYER_VIEW_COLORBLIND_SAFE ? "ColorBlindSafe"
		: PlayerColorView == PLAYER_VIEW_RELATIONSHIP ? "Relationship" : "Default");
	ConfigINI.Put_Bool("Options", "ToolTips", ToolTips);
	ConfigINI.Put_Bool("Options", "AltToRally", AltToRally);
	ConfigINI.Put_Bool("Options", "AircraftBodyPick", AircraftBodyPick);
	ConfigINI.Put_Bool("Options", "SimulateWhileUnfocused", SimulateWhileUnfocused);
	ConfigINI.Put_Int("Options", "TextBackgroundColor", TextBackgroundColor);
	ConfigINI.Put_Int("Options", "AutoSaveInterval", AutoSaveInterval);
	ConfigINI.Put_Int("Video", "ScreenWidth", ScreenWidth);
	ConfigINI.Put_Int("Video", "ScreenHeight", ScreenHeight);
	ConfigINI.Put_Bool("Video", "StretchMovies", StretchMovies);
	ConfigINI.Put_Bool("Video", "Fullscreen", Fullscreen);
	ConfigINI.Put_Int("Video", "WindowWidth", WindowWidth);
	ConfigINI.Put_Int("Video", "WindowHeight", WindowHeight);
	ConfigINI.Put_String("Video", "ScaleMode", (char *)Scale_Mode_Name(ScaleMode));
	ConfigINI.Put_Bool("Video", "IntegerScaling", IntegerScaling);
	ConfigINI.Put_Bool("Video", "ShowFPS", ShowFPS);
	ConfigINI.Put_Int("Video", "RenderFrameRate", RenderFrameRate);
	ConfigINI.Put_Int("Video", "FrameRateLimit", FrameRateLimit);
	ConfigINI.Put_Bool("Video", "VSync", VSync);
	ConfigINI.Put_String("Video", "HDRHighlights", HDRHighlights == HDR_HIGHLIGHTS_CLASSIC ? "Classic" : "Enhanced");
	ConfigINI.Put_Int("Video", "Renderer", Renderer);
	ConfigINI.Put_String("Video", "HDROutput", (char *)HDR_Output_Name(HDROutput));
	Save_HDR_Nits("HDRPaperWhite", HDRPaperWhite);
	Save_HDR_Nits("HDRPeakBrightness", HDRPeakBrightness);
	ConfigINI.Put_String("Video", "ColorBlindMode", ColorBlind_Mode_Name(ColorBlindMode));
	ConfigINI.Put_Int("Video", "ColorBlindStrength", ColorBlindStrength);
	ConfigINI.Put_Int("Video", "Brightness", Brightness);
	char gammasave[32];
	Snprintf_Nearest(gammasave, sizeof(gammasave), "%.2f", Gamma);
	ConfigINI.Put_String("Video", "Gamma", gammasave);
	ConfigINI.Put_Int("Video", "CursorScale", CursorScale);
	if (InterfaceScale > 0.0f) {
		char scaletext[32];
		Snprintf_Nearest(scaletext, sizeof(scaletext), "%g", InterfaceScale);
		ConfigINI.Put_String("Video", "InterfaceScale", scaletext);
	} else {
		ConfigINI.Put_String("Video", "InterfaceScale", "Auto");
	}
	ConfigINI.Put_Bool("Video", "SystemCursor", SystemCursor);
	ConfigINI.Put_Float("Audio", "SoundVolume", SoundVolume);
	ConfigINI.Put_Float("Audio", "VoiceVolume", VoiceVolume);
	ConfigINI.Put_Float("Audio", "ScoreVolume", ScoreVolume);
	ConfigINI.Put_Bool("Audio", "IsScoreRepeat", IsScoreRepeat);
	ConfigINI.Put_Bool("Audio", "IsScoreShuffle", IsScoreShuffle);
	ConfigINI.Put_Int("Audio", "SoundLatency", SoundLatency);
	Save_Seen_Movies();

	/*
	**	Write the INI data out to a file.
	*/
	ConfigINI.Save(file, false);
}


/***********************************************************************************************
 * OptionsClass::Set -- Sets options based on current settings                                 *
 *                                                                                             *
 * Use this routine to adjust the palette or sound settings after a fresh scenario load.       *
 * It assumes the values needed are already loaded into OptionsClass.                          *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/24/1995 BRR : Created.                                                                 *
 *=============================================================================================*/
void OptionsClass::Set(void)
{
	Set_Sound_Volume(SoundVolume, false);
	Set_Voice_Volume(VoiceVolume, false);
	Set_Score_Volume(ScoreVolume, false);
	Set_Repeat(IsScoreRepeat);
	Set_Shuffle(IsScoreShuffle);
	Map.Toggle_Cameo_Text(SidebarCameoText);
	TechnoClass::Set_Action_Lines(ActionLines);
}


/***********************************************************************************************
 * OptionsClass::Normalize_Delay -- Normalizes delay factor to keep rate constant.             *
 *                                                                                             *
 *    This routine is used to adjust delay factors that MUST be synchronized on all machines   *
 *    but should maintain a speed as close to constant as possible. Building animations are    *
 *    a good example of this.                                                                  *
 *                                                                                             *
 * INPUT:   delay -- The normal delay factor.                                                  *
 *                                                                                             *
 * OUTPUT:  Returns with the delay to use that has been modified so that a reasonably constant *
 *          rate will result.                                                                  *
 *                                                                                             *
 * WARNINGS:   This calculation is crude due to the coarse resolution that a 1/15 second timer *
 *             allows.                                                                         *
 *                                                                                             *
 *             Use of this routine ASSUMES that the GameSpeed is synchronized on all machines. *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   06/18/1995 JLB : Created.                                                                 *
 *   06/30/1995 JLB : Handles low values in a more consistent manner.                          *
 *=============================================================================================*/
int OptionsClass::Normalize_Delay(int delay) const
{
	static int _adjust[][MAX_SPEED_SETTING + 1] = {
		{2,2,1,1,1,1,1,1},
		{3,3,3,2,2,2,1,1},
		{5,4,4,3,3,2,2,1},
		{7,6,5,4,4,4,3,2}
	};
	if (delay) {
		if (delay < 5) {
			delay = _adjust[delay-1][GameSpeed];
		} else {
			delay = ((delay * (MAX_SPEED_SETTING + 1)) / (GameSpeed+1));
		}
	}
	return(delay);
}


/***********************************************************************************************
 * OptionsClass::Normalize_Volume -- Convert to a real volume value.                           *
 *                                                                                             *
 *    This routine will take a relative volume value and convert it to the real volume value   *
 *    to use. This allows all the game volumes to be corrected to the correct global volume.   *
 *                                                                                             *
 * INPUT:   volume   -- Requested volume level.                                                *
 *                                                                                             *
 * OUTPUT:  Returns with the actual volume level to use.                                       *
 *                                                                                             *
 * WARNINGS:   none                                                                            *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   09/22/1995 JLB : Created.                                                                 *
 *=============================================================================================*/
int OptionsClass::Normalize_Volume(int volume) const
{
	return(volume * SoundVolume);
}


/// <summary>
/// Displays the keyboard configuration dialog.
/// This routine brings up the hotkey assignment screen and does not return until the player
/// dismisses it.
/// </summary>
void OptionsClass::Hotkey_Dialog(void)
{
	UI_Keyboard_Dialog();
}


/// <summary>
/// Fetches the tactical map scrolling method the player prefers.
/// </summary>
int OptionsClass::Get_Scroll_Method(void) const
{
	return(ScrollMethod);
}


/// <summary>
/// Returns how many extra pictures per second the game draws between game frames in this session.
/// </summary>
/// <returns>int; Zero for none. Match display gives the display's refresh rate in solo and skirmish games, and zero in network games.</returns>
int OptionsClass::Render_Frame_Rate(void) const
{
	if (RenderFrameRate >= 0) {
		return(RenderFrameRate);
	}
	if (Session.Type != GAME_NORMAL && Session.Type != GAME_SKIRMISH) {
		return(0);
	}
	return(Video_Refresh_Rate());
}
