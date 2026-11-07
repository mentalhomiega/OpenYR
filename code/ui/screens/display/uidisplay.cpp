/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/display/uidisplay.h"

#include "hdrcolour.h"
#include "ui/rml/rmlsettingpreview.h"
#include "ui/rml/rmlview.h"
#include "ui/uiclamp.h"
#include "ui/uisettingpreview.h"
#include "video.h"
#include "colourblind.h"
#include "hud.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <utility>


UIDisplayPresenterClass::UIDisplayPresenterClass(UIDisplayServiceClass & service, UIDisplayState state) :
	State(std::move(state)),
	Service(service),
	Start(State)
{
	ShownRowIndex = State.RowCount;
	PreviewedMode = Start.ColorBlindMode;
	PreviewedStrength = Start.ColorBlindStrength;
	PreviewedBrightness = Start.Brightness;
	PreviewedGamma = Start.Gamma;
	Refresh_Preview();
}


void UIDisplayPresenterClass::Chosen_Frame(int & width, int & height)
{
	int screen_width = State.ScreenWidth;
	int screen_height = State.ScreenHeight;
	if (Index_In_Range(State.Selected, State.Modes.size())) {
		screen_width = State.Modes[State.Selected].Width;
		screen_height = State.Modes[State.Selected].Height;
	}
	float const scale = Index_In_Range(State.Scale, State.Scales.size()) ? State.Scales[State.Scale].Value : 0.0f;

	width = 0;
	height = 0;
	if (screen_width > 0 && screen_height > 0) {
		Service.Frame_Size(screen_width, screen_height, scale, width, height);
	}
}


int UIDisplayPresenterClass::Chosen_Rows(void) const
{
	return(Index_In_Range(ShownRowIndex, State.RowCounts.size()) ? State.RowCounts[ShownRowIndex].Value : -1);
}


void UIDisplayPresenterClass::Refresh_Rows(void)
{
	int const previous = Chosen_Rows();
	int width = 0;
	int height = 0;
	Chosen_Frame(width, height);

	// The rows the new frame allows. The row count stays the largest one that does not exceed the rows before.
	std::vector<UIChoice> const before = State.RowCounts;
	State.RowCounts.clear();
	State.RowCount = -1;
	if (height > 0) {
		State.RowCount = Fill_Count_Choices(State.RowCounts, Service.Sidebar_Row_Counts(height), previous);
		if (State.RowCount < 0 && !State.RowCounts.empty()) {
			State.RowCount = 0;
		}
	}
	ShownRowIndex = State.RowCount;
	bool const same = before.size() == State.RowCounts.size() && std::equal(before.begin(), before.end(), State.RowCounts.begin(),
		[](UIChoice const & a, UIChoice const & b) { return(a.Value == b.Value); });
	RowCountsChanged = RowCountsChanged || !same;
	Refresh_Preview();
}


void UIDisplayPresenterClass::Refresh_Preview(void)
{
	int width = 0;
	int height = 0;
	Chosen_Frame(width, height);
	int const rows = Chosen_Rows();

	std::size_t const bound = State.Preview.Shapes.size();
	State.Preview = UISettingPreviewScreen();
	if (rows > 0 && width > 0 && height > 0) {
		std::optional<UISettingPreviewScreen> screen = UI_Setting_Preview_Screen("sidebar-rows", rows, width, height);
		if (screen.has_value()) {
			State.Preview = *screen;

			std::string label;
			if (Index_In_Range(State.Selected, State.Modes.size())) {
				label = State.Modes[State.Selected].Label;
			} else {
				char buffer[64];
				std::snprintf(buffer, sizeof(buffer), "%d x %d", State.ScreenWidth, State.ScreenHeight);
				label = buffer;
			}
			State.Preview.Caption = label + ", " + std::to_string(rows) + (rows == 1 ? " row" : " rows");
		}
	}
	UI_Setting_Preview_Pad(State.Preview, bound);
	PreviewChanged = true;
}


static int Pick_Choice(std::vector<UIChoice> const & choices, int index, int fallback)
{
	return(Index_In_Range(index, choices.size()) ? index : fallback);
}


static bool Choice_Changed(std::vector<UIChoice> const & choices, int index, int initial)
{
	return(index != initial && Index_In_Range(index, choices.size()));
}


// The index of the choice with the value given, or the fallback when no choice has it.
static int Index_Of(std::vector<UIChoice> const & choices, int value, int fallback)
{
	for (int index = 0; index < (int)choices.size(); index++) {
		if (choices[index].Value == value) {
			return index;
		}
	}
	return fallback;
}


// Sets the settings of the page to the values Yuri's Revenge starts from. Nothing is saved until OK.
void UIDisplayPresenterClass::Reset_Page(void)
{
	if (State.Page == UI_TAB_DISPLAY) {
		// Match resolution is the first window size, and automatic the first interface scale.
		State.WindowSize = 0;
		State.FrameLimit = Index_Of(State.FrameLimits, -1, State.FrameLimit);
		State.ShowFPS = false;
		for (int index = 0; index < (int)State.Scales.size(); index++) {
			if (State.Scales[index].Value == 0.0f) {
				State.Scale = index;
			}
		}
		State.CursorScale = Index_Of(State.CursorScales, 0, State.CursorScale);
		State.MenuTheme = Index_Of(State.MenuThemes, 0, State.MenuTheme);
		State.MenuBattle = true;
		State.SystemCursor = true;
		State.Fullscreen = true;
		State.VSync = true;
		Refresh_Rows();

	} else if (State.Page == UI_TAB_GRAPHICS) {
		// A frame rate of -1 is match the display, and highlights 1 is Enhanced.
		State.Filter = Index_Of(State.Filters, VIDEO_SCALE_PIXELART, State.Filter);
		State.IntegerScaling = false;
		State.GpuRender = false;
		State.SoftShadows = false;
		State.Shockwaves = false;
		State.Bloom = false;
		State.WaterRipples = false;
		State.CloudShadows = false;
		State.Weather = false;
		State.FrameRate = Index_Of(State.FrameRates, -1, State.FrameRate);
		State.Renderer = Index_Of(State.Renderers, 0, State.Renderer);
		State.HDROutput = Index_Of(State.HDROutputs, VIDEO_HDR_AUTOMATIC, State.HDROutput);
		State.Highlights = 1;
		State.PaperWhite = State.DefaultPaperWhite;
		State.PeakBrightness = State.DefaultPeakBrightness;
		HDRLevelsReset = true;
		State.ModernInterface = true;
		State.ClassicMenus = false;
		State.BitmapSystemFont = false;
		State.StretchMovies = false;
		State.Brightness = 100;
		State.Gamma = 100;
		State.SidebarOpacity = HUD_DEFAULT_OPACITY;

		int const row = Index_Of(State.RowCounts, HUD_DEFAULT_ROWS, ShownRowIndex);
		if (Index_In_Range(row, State.RowCounts.size())) {
			ShownRowIndex = row;
			State.RowCount = row;
			PickedRows = State.RowCounts[row].Value;
			UI_Setting_Preview_Show("sidebar-rows", PickedRows);
			Refresh_Preview();
		}
		Show_Colour_Preview();

	} else {
		State.ColorBlindMode = Index_Of(State.ColorBlindModes, COLORBLIND_OFF, State.ColorBlindMode);
		State.ColorBlindStrength = COLORBLIND_STRENGTH_MAX;
		State.PlayerColors = Index_Of(State.PlayerColorChoices, 0, State.PlayerColors);
		State.ScreenShake = true;
		State.ScriptedCameraZoom = true;
		Show_Colour_Preview();
	}
}


// Shows the colour values on the page, which the whole frame takes while a graphics or accessibility page is open.
void UIDisplayPresenterClass::Show_Colour_Preview(void)
{
	if (State.Page == UI_TAB_DISPLAY) {
		return;
	}
	// The drop-down and sliders report their values when they are first shown, which is not a change.
	if (State.ColorBlindMode == PreviewedMode && State.ColorBlindStrength == PreviewedStrength
		&& State.Brightness == PreviewedBrightness && State.Gamma == PreviewedGamma) {
		return;
	}
	PreviewedMode = State.ColorBlindMode;
	PreviewedStrength = State.ColorBlindStrength;
	PreviewedBrightness = State.Brightness;
	PreviewedGamma = State.Gamma;

	int const mode = Index_In_Range(State.ColorBlindMode, State.ColorBlindModes.size())
		? State.ColorBlindModes[State.ColorBlindMode].Value : COLORBLIND_OFF;
	Service.Preview_Colour(mode, State.ColorBlindStrength, State.Brightness, State.Gamma);
	PreviewShown = true;
}


// Puts back the colour values the page opened with, so a page left without OK changes nothing.
void UIDisplayPresenterClass::Restore_Preview(void)
{
	if (!PreviewShown || PreviewRestored) {
		return;
	}
	PreviewRestored = true;
	int const mode = Index_In_Range(Start.ColorBlindMode, State.ColorBlindModes.size())
		? State.ColorBlindModes[Start.ColorBlindMode].Value : COLORBLIND_OFF;
	Service.Preview_Colour(mode, Start.ColorBlindStrength, Start.Brightness, Start.Gamma);
}


void UIDisplayPresenterClass::Set_Gpu_Switch(bool & field, int value, bool needs_gpu)
{
	if (State.ModernInterface && (!needs_gpu || State.GpuRender)) {
		field = (value != 0);
	}
}


void UIDisplayPresenterClass::Apply_Settings(void)
{
	Service.Set_Stretch_Movies(State.StretchMovies);
	Service.Set_System_Cursor(State.SystemCursor);
	Service.Set_Classic_Menus(State.ClassicMenus);
	Service.Set_Modern_Interface(State.ModernInterface);
	if (State.BitmapSystemFont != Start.BitmapSystemFont) {
		Service.Set_Bitmap_System_Font(State.BitmapSystemFont);
	}
	if (State.ShowFPS != Start.ShowFPS) {
		Service.Set_Show_FPS(State.ShowFPS);
	}
	if (Choice_Changed(State.FrameLimits, State.FrameLimit, Start.FrameLimit)) {
		Service.Set_Frame_Limit(State.FrameLimits[State.FrameLimit].Value);
	}
	if (State.WindowSize != Start.WindowSize && Index_In_Range(State.WindowSize, State.WindowSizes.size())) {
		Service.Set_Window_Size(State.WindowSizes[State.WindowSize].Width, State.WindowSizes[State.WindowSize].Height);
	}
	if (Choice_Changed(State.PlayerColorChoices, State.PlayerColors, Start.PlayerColors)) {
		Service.Set_Player_Colors(State.PlayerColorChoices[State.PlayerColors].Value);
	}
	if (State.ScreenShake != Start.ScreenShake) {
		Service.Set_Screen_Shake(State.ScreenShake);
	}
	if (State.ScriptedCameraZoom != Start.ScriptedCameraZoom) {
		Service.Set_Scripted_Camera_Zoom(State.ScriptedCameraZoom);
	}
	if (State.Fullscreen != Start.Fullscreen) {
		Service.Set_Fullscreen(State.Fullscreen);
	}
	if (State.VSync != Start.VSync) {
		Service.Set_VSync(State.VSync);
	}
	if (State.IntegerScaling != Start.IntegerScaling) {
		Service.Set_Integer_Scaling(State.IntegerScaling);
	}
	if (State.GpuRender != Start.GpuRender || State.SoftShadows != Start.SoftShadows || State.Shockwaves != Start.Shockwaves || State.Bloom != Start.Bloom || State.WaterRipples != Start.WaterRipples
		|| State.CloudShadows != Start.CloudShadows
		|| State.Weather != Start.Weather) {
		Service.Set_Gpu_Effects(State.GpuRender, State.SoftShadows, State.Shockwaves, State.Bloom, State.WaterRipples, State.CloudShadows, State.Weather);
	}
	if (Choice_Changed(State.Filters, State.Filter, Start.Filter)) {
		Service.Set_Filter(State.Filters[State.Filter].Value);
	}
	if (Choice_Changed(State.FrameRates, State.FrameRate, Start.FrameRate)) {
		Service.Set_Frame_Rate(State.FrameRates[State.FrameRate].Value);
	}
	if (Choice_Changed(State.CursorScales, State.CursorScale, Start.CursorScale)) {
		Service.Set_Cursor_Scale(State.CursorScales[State.CursorScale].Value);
	}
	if (Choice_Changed(State.MenuThemes, State.MenuTheme, Start.MenuTheme)) {
		Service.Set_Menu_Theme(State.MenuThemes[State.MenuTheme].Value);
	}
	if (State.MenuBattle != Start.MenuBattle) {
		Service.Set_Menu_Battle(State.MenuBattle);
	}
	if (Choice_Changed(State.Renderers, State.Renderer, Start.Renderer)) {
		Service.Set_Renderer(State.Renderers[State.Renderer].Value);
	}
	if (Choice_Changed(State.HDROutputs, State.HDROutput, Start.HDROutput)) {
		Service.Set_HDR_Output(State.HDROutputs[State.HDROutput].Value);
	}
	if (HDRLevelsReset) {
		// Zero is the automatic level, which the engine follows from the display.
		Service.Set_HDR_Paper_White(0);
		Service.Set_HDR_Peak_Brightness(0);
	} else {
		if (State.PaperWhite != Start.PaperWhite) {
			Service.Set_HDR_Paper_White(State.PaperWhite);
		}
		if (State.PeakBrightness != Start.PeakBrightness) {
			Service.Set_HDR_Peak_Brightness(State.PeakBrightness);
		}
	}
	if (State.Highlights != Start.Highlights) {
		Service.Set_HDR_Highlights(State.Highlights);
	}
	if (Index_In_Range(State.ColorBlindMode, State.ColorBlindModes.size())
		&& (State.ColorBlindMode != Start.ColorBlindMode || State.ColorBlindStrength != Start.ColorBlindStrength)) {
		Service.Set_Colour_Blind(State.ColorBlindModes[State.ColorBlindMode].Value, State.ColorBlindStrength);
	}
	if (State.Brightness != Start.Brightness || State.Gamma != Start.Gamma) {
		Service.Set_Brightness_Gamma(State.Brightness, State.Gamma);
	}
	if (PickedRows >= 0) {
		Service.Set_Sidebar_Rows(PickedRows);
	}
	if (State.SidebarOpacity != Start.SidebarOpacity) {
		Service.Set_Sidebar_Opacity(State.SidebarOpacity);
	}
	UI_Setting_Preview_Commit();
	if (State.Scale != Start.Scale && Index_In_Range(State.Scale, State.Scales.size())) {
		Service.Set_Interface_Scale(State.Scales[State.Scale].Value);
		ScaleChanged = true;
	}
	if (State.Selected >= 0 && State.Selected != Start.Selected) {
		Picked = State.Modes[State.Selected];
	}
	Result = UI_RESULT_ACCEPTED;
}


bool UI_Bind_Display_Basics(Rml::DataModelConstructor & model, UIDisplayState & state)
{
	return(model.Bind("scales", &state.Scales)
		&& model.Bind("scale", &state.Scale)
		&& model.Bind("modes", &state.Modes)
		&& model.Bind("selected", &state.Selected)
		&& model.Bind("stretch", &state.StretchMovies)
		&& model.Bind("systemcursor", &state.SystemCursor)
		&& model.Bind("classicmenus", &state.ClassicMenus));
}


namespace
{

// An intent that sets one On/Off option to its value.
struct UIDisplayFlagIntentClass
{
	char const * Name;
	bool UIDisplayState::* Field;
};


const UIDisplayFlagIntentClass Flag_Intents[] = {
	{ "stretch", &UIDisplayState::StretchMovies },
	{ "systemcursor", &UIDisplayState::SystemCursor },
	{ "classicmenus", &UIDisplayState::ClassicMenus },
	{ "moderninterface", &UIDisplayState::ModernInterface },
	{ "bitmapfont", &UIDisplayState::BitmapSystemFont },
	{ "showfps", &UIDisplayState::ShowFPS },
	{ "screenshake", &UIDisplayState::ScreenShake },
	{ "scriptedzoom", &UIDisplayState::ScriptedCameraZoom },
	{ "fullscreen", &UIDisplayState::Fullscreen },
	{ "vsync", &UIDisplayState::VSync },
	{ "integerscaling", &UIDisplayState::IntegerScaling },
	{ "menubattle", &UIDisplayState::MenuBattle },
};

}


void UIDisplayPresenterClass::Execute(UIIntent const & intent)
{
	for (UIDisplayFlagIntentClass const & flag : Flag_Intents) {
		if (intent.Name == flag.Name) {
			State.*flag.Field = (intent.Value != 0);
			return;
		}
	}

	if (intent.Name == "select") {
		State.Selected = Index_In_Range(intent.Value, State.Modes.size()) ? intent.Value : -1;
		Refresh_Rows();

	} else if (intent.Name == "framelimit") {
		State.FrameLimit = Pick_Choice(State.FrameLimits, intent.Value, Start.FrameLimit);

	} else if (intent.Name == "windowsize") {
		State.WindowSize = Index_In_Range(intent.Value, State.WindowSizes.size()) ? intent.Value : Start.WindowSize;
	} else if (intent.Name == "playercolors") {
		State.PlayerColors = Pick_Choice(State.PlayerColorChoices, intent.Value, Start.PlayerColors);

	} else if (intent.Name == "reset") {
		Reset_Page();

	} else if (intent.Name == "scale") {
		State.Scale = Index_In_Range(intent.Value, State.Scales.size()) ? intent.Value : Start.Scale;
		Refresh_Rows();

	} else if (intent.Name == "gpurender") {
		Set_Gpu_Switch(State.GpuRender, intent.Value, false);

	} else if (intent.Name == "softshadows") {
		Set_Gpu_Switch(State.SoftShadows, intent.Value, true);

	} else if (intent.Name == "shockwaves") {
		Set_Gpu_Switch(State.Shockwaves, intent.Value, true);

	} else if (intent.Name == "bloom") {
		Set_Gpu_Switch(State.Bloom, intent.Value, true);

	} else if (intent.Name == "waterripples") {
		Set_Gpu_Switch(State.WaterRipples, intent.Value, true);

	} else if (intent.Name == "cloudshadows") {
		Set_Gpu_Switch(State.CloudShadows, intent.Value, true);

	} else if (intent.Name == "weather") {
		Set_Gpu_Switch(State.Weather, intent.Value, true);

	} else if (intent.Name == "filter") {
		State.Filter = Pick_Choice(State.Filters, intent.Value, Start.Filter);

	} else if (intent.Name == "framerate") {
		State.FrameRate = Pick_Choice(State.FrameRates, intent.Value, Start.FrameRate);

	} else if (intent.Name == "cursorscale") {
		State.CursorScale = Pick_Choice(State.CursorScales, intent.Value, Start.CursorScale);

	} else if (intent.Name == "menutheme") {
		State.MenuTheme = Pick_Choice(State.MenuThemes, intent.Value, Start.MenuTheme);

	} else if (intent.Name == "renderer") {
		State.Renderer = Pick_Choice(State.Renderers, intent.Value, Start.Renderer);

	} else if (intent.Name == "hdroutput") {
		State.HDROutput = Pick_Choice(State.HDROutputs, intent.Value, Start.HDROutput);

	} else if (intent.Name == "paperwhite") {
		State.PaperWhite = std::clamp(intent.Value, HDR_PAPER_WHITE_MIN, HDR_PAPER_WHITE_MAX);
		HDRLevelsReset = false;
		State.PeakBrightness = std::max(State.PeakBrightness, State.PaperWhite);

	} else if (intent.Name == "peak") {
		State.PeakBrightness = std::clamp(intent.Value, State.PaperWhite, (int)HDR_PQ_PEAK_NITS);
		HDRLevelsReset = false;

	} else if (intent.Name == "highlights") {
		State.Highlights = std::clamp(intent.Value, 0, 1);

	} else if (intent.Name == "colorblindmode") {
		State.ColorBlindMode = Pick_Choice(State.ColorBlindModes, intent.Value, Start.ColorBlindMode);
		Show_Colour_Preview();

	} else if (intent.Name == "colorblindstrength") {
		State.ColorBlindStrength = std::clamp(intent.Value, COLORBLIND_STRENGTH_MIN, COLORBLIND_STRENGTH_MAX);
		Show_Colour_Preview();

	} else if (intent.Name == "brightness") {
		State.Brightness = std::clamp(intent.Value, VIDEO_BRIGHTNESS_MIN, VIDEO_BRIGHTNESS_MAX);
		Show_Colour_Preview();

	} else if (intent.Name == "gamma") {
		State.Gamma = std::clamp(intent.Value, VIDEO_GAMMA_MIN, VIDEO_GAMMA_MAX);
		Show_Colour_Preview();

	} else if (intent.Name == "sidebarrows") {
		// The drop-down reports its value again whenever its list or value is set, so only a row other than the
		// one shown is a pick. The bound row count is not compared: the drop-down writes it before this runs.
		int const index = Pick_Choice(State.RowCounts, intent.Value, ShownRowIndex);
		if (index != ShownRowIndex && Index_In_Range(index, State.RowCounts.size())) {
			ShownRowIndex = index;
			State.RowCount = index;
			PickedRows = State.RowCounts[State.RowCount].Value;
			UI_Setting_Preview_Show("sidebar-rows", PickedRows);
			Refresh_Preview();
		} else {
			// Not a pick: the drop-down shows the row it showed before, so its bound value goes back to that row.
			State.RowCount = ShownRowIndex;
		}

	} else if (intent.Name == "sidebaropacity") {
		State.SidebarOpacity = std::clamp(intent.Value, 0, 100);

	} else if (intent.Name == "ok") {
		Apply_Settings();

	} else if (intent.Name == "cancel") {
		Restore_Preview();
		Result = UI_RESULT_CANCELLED;
	}
}


void UIDisplayPresenterClass::Refresh(void)
{
}


UIConfirmModePresenterClass::UIConfirmModePresenterClass(UIClockClass & clock, int timeout) :
	Seconds((timeout + 999) / 1000),
	Clock(clock),
	Timeout(timeout)
{
}


void UIConfirmModePresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "ok") {
		Result = UI_RESULT_ACCEPTED;
	} else if (intent.Name == "cancel") {
		Result = UI_RESULT_CANCELLED;
	}
}


void UIConfirmModePresenterClass::Refresh(void)
{
	int now = Clock.Milliseconds();
	if (!Deadline.has_value()) {
		Deadline = now + Timeout;
	}

	int left = *Deadline - now;
	Seconds = (left > 0) ? (left + 999) / 1000 : 0;

	if (left <= 0 && !Result.has_value()) {
		TimedOut = true;
		Result = UI_RESULT_CANCELLED;
	}
}


namespace
{

/// <summary>
/// Writes the gamma text the page shows, with two decimals, as the setting in hundredths reads.
/// </summary>
void Update_Gamma_Label(UIDisplayState & state)
{
	char text[16];
	std::snprintf(text, sizeof(text), "%.2f", state.Gamma / 100.0);
	state.GammaLabel = text;
}


/// <summary>
/// Sets the two HDR tooltips from the choice on show. The HDR output tooltip says what the choice does and
/// that it applies at the next start. The Highlights tooltip is empty while HDR output is on, so it shows
/// only while the list is grayed out.
/// </summary>
void Update_HDR_Tips(UIDisplayState & state)
{
	bool forced = Index_In_Range(state.HDROutput, state.HDROutputs.size()) && state.HDROutputs[state.HDROutput].Value == VIDEO_HDR_ON;
	bool off = Index_In_Range(state.HDROutput, state.HDROutputs.size()) && state.HDROutputs[state.HDROutput].Value == VIDEO_HDR_OFF;
	if (forced) {
		state.HDROutputTip = "On forces HDR10 output. Turn on HDR in Windows for this display first, or the picture will look wrong. Applies at the next start.";
	} else if (off) {
		state.HDROutputTip = "Off keeps the picture in SDR. Applies at the next start.";
	} else {
		state.HDROutputTip = "Automatic uses HDR10 output while Windows HDR is on for this display. Applies at the next start.";
	}
	state.HighlightsTip = state.HDRActive ? std::string() : std::string("Needs HDR output");
}


// The page document each tab of the display settings shows.
static char const * Page_Document(UISettingsTab page)
{
	switch (page) {
		case UI_TAB_GRAPHICS:
			return("graphics.rml");
		case UI_TAB_ACCESSIBILITY:
			return("accessibility.rml");
		default:
			return("display.rml");
	}
}


// The data model each tab's document binds to. Each tab has its own, as all of them stay loaded.
static char const * Page_Model(UISettingsTab page)
{
	switch (page) {
		case UI_TAB_GRAPHICS:
			return("graphics");
		case UI_TAB_ACCESSIBILITY:
			return("accessibility");
		default:
			return("display");
	}
}


class UIDisplayViewClass : public UIRmlViewClass
{
	public:
		UIDisplayViewClass(UIDisplayPresenterClass & presenter, UISettingsTab page) :
			UIRmlViewClass(presenter, Page_Document(page), Page_Model(page)),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Update_HDR_Tips(Data.State);
			Model.DirtyVariable("hdractive");
			Model.DirtyVariable("hdroutputtip");
			Model.DirtyVariable("highlightstip");
			Model.DirtyVariable("selected");
			Model.DirtyVariable("stretch");
			Model.DirtyVariable("systemcursor");
			Model.DirtyVariable("classicmenus");
			Model.DirtyVariable("moderninterface");
			Model.DirtyVariable("bitmapfont");
			Model.DirtyVariable("showfps");
			Model.DirtyVariable("framelimit");
			Model.DirtyVariable("windowsize");
			Model.DirtyVariable("playercolors");
			Model.DirtyVariable("scale");
			Model.DirtyVariable("fullscreen");
			Model.DirtyVariable("vsync");
			Model.DirtyVariable("integerscaling");
			Model.DirtyVariable("gpurender");
			Model.DirtyVariable("softshadows");
			Model.DirtyVariable("shockwaves");
			Model.DirtyVariable("bloom");
			Model.DirtyVariable("waterripples");
			Model.DirtyVariable("cloudshadows");
			Model.DirtyVariable("weather");
			Model.DirtyVariable("filter");
			Model.DirtyVariable("framerate");
			Model.DirtyVariable("cursorscale");
			Model.DirtyVariable("menutheme");
			Model.DirtyVariable("menubattle");
			Model.DirtyVariable("renderer");
			Model.DirtyVariable("hdroutput");
			Model.DirtyVariable("paperwhite");
			Model.DirtyVariable("peakbrightness");
			Model.DirtyVariable("highlights");
			Model.DirtyVariable("colorblindmode");
			Model.DirtyVariable("colorblindstrength");
			Model.DirtyVariable("screenshake");
			Model.DirtyVariable("scriptedzoom");
			Update_Gamma_Label(Data.State);
			Model.DirtyVariable("brightness");
			Model.DirtyVariable("gamma");
			Model.DirtyVariable("gammalabel");
			Model.DirtyVariable("rowcount");
			Model.DirtyVariable("sidebaropacity");
			if (Data.PreviewChanged) {
				Data.PreviewChanged = false;
				Model.DirtyVariable("preview");
				if (Data.RowCountsChanged) {
					Data.RowCountsChanged = false;
					Model.DirtyVariable("rowcounts");
				}
			}
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIDisplayMode> mode = model.RegisterStruct<UIDisplayMode>();
			if (!mode) {
				return(false);
			}
			mode.RegisterMember("label", &UIDisplayMode::Label);
			mode.RegisterMember("width", &UIDisplayMode::Width);
			mode.RegisterMember("height", &UIDisplayMode::Height);

			Rml::StructHandle<UIDisplayScale> scale = model.RegisterStruct<UIDisplayScale>();
			if (!scale) {
				return(false);
			}
			scale.RegisterMember("label", &UIDisplayScale::Label);

			Rml::StructHandle<UIChoice> choice = model.RegisterStruct<UIChoice>();
			if (!choice) {
				return(false);
			}
			choice.RegisterMember("label", &UIChoice::Label);

			UIDisplayState & state = Data.State;
			return(model.RegisterArray<std::vector<UIDisplayMode>>()
				&& model.RegisterArray<std::vector<UIDisplayScale>>()
				&& model.RegisterArray<std::vector<UIChoice>>()
				&& UI_Bind_Display_Basics(model, state)
				&& model.Bind("moderninterface", &state.ModernInterface)
				&& model.Bind("moderninterfacerunning", &state.ModernInterfaceRunning)
				&& model.Bind("ingame", &state.InGame)
				&& model.Bind("bitmapfont", &state.BitmapSystemFont)
				&& model.Bind("showfps", &state.ShowFPS)
				&& model.Bind("framelimits", &state.FrameLimits)
				&& model.Bind("framelimit", &state.FrameLimit)
				&& model.Bind("windowsizes", &state.WindowSizes)
				&& model.Bind("windowsize", &state.WindowSize)
				&& model.Bind("playercolorchoices", &state.PlayerColorChoices)
				&& model.Bind("playercolors", &state.PlayerColors)
				&& model.Bind("fullscreen", &state.Fullscreen)
				&& model.Bind("vsync", &state.VSync)
				&& model.Bind("integerscaling", &state.IntegerScaling)
				&& model.Bind("gpurender", &state.GpuRender)
				&& model.Bind("softshadows", &state.SoftShadows)
				&& model.Bind("shockwaves", &state.Shockwaves)
				&& model.Bind("bloom", &state.Bloom)
				&& model.Bind("waterripples", &state.WaterRipples)
				&& model.Bind("cloudshadows", &state.CloudShadows)
				&& model.Bind("weather", &state.Weather)
				&& model.Bind("filters", &state.Filters)
				&& model.Bind("filter", &state.Filter)
				&& model.Bind("framerates", &state.FrameRates)
				&& model.Bind("framerate", &state.FrameRate)
				&& model.Bind("cursorscales", &state.CursorScales)
				&& model.Bind("cursorscale", &state.CursorScale)
				&& model.Bind("menuthemes", &state.MenuThemes)
				&& model.Bind("menutheme", &state.MenuTheme)
				&& model.Bind("menubattle", &state.MenuBattle)
				&& model.Bind("renderers", &state.Renderers)
				&& model.Bind("renderer", &state.Renderer)
				&& model.Bind("hdroutputs", &state.HDROutputs)
				&& model.Bind("hdroutput", &state.HDROutput)
				&& model.Bind("paperwhite", &state.PaperWhite)
				&& model.Bind("peakbrightness", &state.PeakBrightness)
				&& model.Bind("highlights", &state.Highlights)
				&& model.Bind("colorblindmodes", &state.ColorBlindModes)
				&& model.Bind("colorblindmode", &state.ColorBlindMode)
				&& model.Bind("colorblindstrength", &state.ColorBlindStrength)
				&& model.Bind("brightness", &state.Brightness)
				&& model.Bind("gamma", &state.Gamma)
				&& model.Bind("gammalabel", &state.GammaLabel)
				&& model.Bind("screenshake", &state.ScreenShake)
				&& model.Bind("scriptedzoom", &state.ScriptedCameraZoom)
				&& model.Bind("hdractive", &state.HDRActive)
				&& model.Bind("hdroutputtip", &state.HDROutputTip)
				&& model.Bind("highlightstip", &state.HighlightsTip)
				&& model.Bind("rowcounts", &state.RowCounts)
				&& model.Bind("rowcount", &state.RowCount)
				&& model.Bind("sidebaropacity", &state.SidebarOpacity)
				&& UI_Rml_Setting_Preview_Bind(model, "preview", state.Preview));
		}

	private:
		UIDisplayPresenterClass & Data;
};


class UIConfirmModeViewClass : public UIRmlViewClass
{
	public:
		explicit UIConfirmModeViewClass(UIConfirmModePresenterClass & presenter) :
			UIRmlViewClass(presenter, "confirm.rml", "confirm"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Model.DirtyVariable("seconds");
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			return(model.Bind("seconds", &Data.Seconds));
		}

	private:
		UIConfirmModePresenterClass & Data;
};

}


std::unique_ptr<UIViewClass> UI_Display_View(UIDisplayPresenterClass & presenter)
{
	return(std::make_unique<UIDisplayViewClass>(presenter, presenter.State.Page));
}


std::unique_ptr<UIViewClass> UI_Display_View(UIDisplayPresenterClass & presenter, UISettingsTab page)
{
	return(std::make_unique<UIDisplayViewClass>(presenter, page));
}


std::unique_ptr<UIViewClass> UI_Confirm_Mode_View(UIConfirmModePresenterClass & presenter)
{
	return(std::make_unique<UIConfirmModeViewClass>(presenter));
}
