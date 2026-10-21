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
#include "ui/uisettingpreview.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

class UIViewClass;


struct UIDisplayMode
{
	int Width = 0;
	int Height = 0;
	std::string Label;
};


struct UIDisplayScale
{
	float Value = 0.0f;			// Zero is automatic.
	std::string Label;
};


// One entry of a list of settings values, such as a frame rate or a filter.
struct UIDisplayChoice
{
	int Value = 0;
	std::string Label;
};


class UIDisplayServiceClass
{
	public:
		virtual ~UIDisplayServiceClass(void) = default;
		virtual void Set_Stretch_Movies(bool on) = 0;
		virtual void Set_System_Cursor(bool on) = 0;
		virtual void Set_Classic_Menus(bool on) = 0;
		virtual void Set_Modern_Interface(bool on) = 0;
		virtual void Set_Sidebar_Rows(int rows) = 0;
		virtual void Set_Bitmap_System_Font(bool on) = 0;
		virtual void Set_Show_FPS(bool on) = 0;
		virtual void Set_Frame_Limit(int limit) = 0;
		virtual void Set_Window_Size(int width, int height) = 0;
		virtual void Set_Interface_Scale(float scale) = 0;
		virtual void Set_Fullscreen(bool on) = 0;
		virtual void Set_VSync(bool on) = 0;
		virtual void Set_Integer_Scaling(bool on) = 0;
		virtual void Set_Filter(int mode) = 0;
		virtual void Set_Frame_Rate(int rate) = 0;
		virtual void Set_Cursor_Scale(int scale) = 0;
		virtual void Set_Menu_Theme(int theme) = 0;
		virtual void Set_Menu_Battle(bool on) = 0;
		virtual void Set_Renderer(int renderer) = 0;
		virtual void Set_HDR_Output(int mode) = 0;
		virtual void Set_HDR_Paper_White(int nits) = 0;
		virtual void Set_HDR_Peak_Brightness(int nits) = 0;
		virtual void Set_HDR_Highlights(int highlights) = 0;
		virtual void Set_Colour_Blind(int mode, int strength) = 0;
		virtual void Set_Brightness_Gamma(int brightness, int gamma) = 0;
		virtual void Set_Player_Colors(int view) = 0;
		virtual void Set_Screen_Shake(bool on) = 0;
		virtual void Set_Scripted_Camera_Zoom(bool on) = 0;

		// Shows the colour adjustment on the frame now, without saving it. Cancel puts the saved values back
		// through the same call.
		virtual void Preview_Colour(int mode, int strength, int brightness, int gamma) = 0;

		// The size of the frame the interface is drawn in on a screen of the size given, at the interface
		// scale given (zero is automatic).
		virtual void Frame_Size(int width, int height, float scale, int & frame_width, int & frame_height) = 0;

		// The Sidebar rows values a frame of this height can show, in ascending order.
		virtual std::vector<int> Sidebar_Row_Counts(int frame_height) = 0;
};


struct UIDisplayState
{
	// The tab this page stands for. The display, graphics and accessibility tabs share one presenter.
	UISettingsTab Page = UI_TAB_DISPLAY;
	std::vector<UIDisplayMode> Modes;
	int Selected = -1;
	bool StretchMovies = false;
	bool SystemCursor = false;
	bool ClassicMenus = false;
	bool ModernInterface = false;
	bool BitmapSystemFont = false;
	bool ShowFPS = false;
	std::vector<UIDisplayChoice> FrameLimits;
	int FrameLimit = 0;
	// The window sizes offered; the first is Match resolution, which leaves both dimensions at -1.
	std::vector<UIDisplayMode> WindowSizes;
	int WindowSize = 0;
	// The player color view, as Options.PlayerColorView numbers it, and the choices the drop-down lists.
	int PlayerColors = 0;
	std::vector<UIDisplayChoice> PlayerColorChoices;
	std::vector<UIDisplayScale> Scales;
	int Scale = 0;
	bool Fullscreen = false;
	bool VSync = false;
	bool IntegerScaling = false;
	std::vector<UIDisplayChoice> Filters;
	int Filter = 0;
	std::vector<UIDisplayChoice> FrameRates;
	int FrameRate = 0;
	std::vector<UIDisplayChoice> CursorScales;
	int CursorScale = 0;
	std::vector<UIDisplayChoice> MenuThemes;
	int MenuTheme = 0;
	// The battle drawn behind the modern menus, from the MenuBattle setting.
	bool MenuBattle = true;
	std::vector<UIDisplayChoice> Renderers;
	int Renderer = 0;
	std::vector<UIDisplayChoice> HDROutputs;
	int HDROutput = 0;
	// The paper white and peak brightness on the sliders, in nits.
	int PaperWhite = 0;
	int PeakBrightness = 0;
	// Whether HDR output is on now. The Highlights list needs it, and its tooltip says so while it is off.
	bool HDRActive = false;
	// The tooltips of the HDR output and Highlights rows, which describe the choice on show. Empty means none.
	std::string HDROutputTip;
	std::string HighlightsTip;
	// Whether the emissive highlights are on, as 0 or 1.
	int Highlights = 0;
	// The colour-blind mode, as an index into ColorBlindModes, and how strongly it applies, in percent.
	std::vector<UIDisplayChoice> ColorBlindModes;
	int ColorBlindMode = 0;
	int ColorBlindStrength = 100;
	// The brightness in percent, and the gamma in hundredths, with the gamma text the page shows.
	int Brightness = 100;
	int Gamma = 100;
	std::string GammaLabel;
	std::vector<UIDisplayChoice> RowCounts;
	int RowCount = 0;
	// The screen the page starts on, used for the preview when no resolution is picked.
	int ScreenWidth = 0;
	int ScreenHeight = 0;
	UISettingPreviewScreen Preview;
	// Whether the screen-shake effect is on.
	bool ScreenShake = true;
	// Whether scripted scenes zoom the map view to one height, whatever zoom the player picked.
	bool ScriptedCameraZoom = true;
	// The automatic paper white and peak brightness, which Reset to defaults puts back, in nits.
	int DefaultPaperWhite = 0;
	int DefaultPeakBrightness = 0;
};


class UIDisplayPresenterClass : public UIPresenterClass
{
	public:
		UIDisplayPresenterClass(UIDisplayServiceClass & service, UIDisplayState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		UIDisplayState State;
		std::optional<UIDisplayMode> Picked;
		bool ScaleChanged = false;
		bool PreviewChanged = false;
		bool RowCountsChanged = false;

		// Puts back the colour values the page found, when the page is left without OK.
		void Restore_Preview(void);

	private:
		// Sets the page's settings to their defaults, as its Reset to defaults button does.
		void Reset_Page(void);
		// Shows the colour values now on the frame, on the graphics and accessibility pages.
		void Show_Colour_Preview(void);
		// The row choices follow the frame that the picked resolution and interface scale give.
		void Refresh_Rows(void);
		void Refresh_Preview(void);
		void Chosen_Frame(int & width, int & height);
		int Chosen_Rows(void) const;

		UIDisplayServiceClass & Service;
		int Initial;
		int InitialScale;
		bool InitialFullscreen;
		bool InitialVSync;
		bool InitialIntegerScaling;
		int InitialFilter;
		int InitialFrameRate;
		int InitialCursorScale;
		int InitialMenuTheme;
		bool InitialMenuBattle;
		int InitialRenderer;
		int InitialHighlights;
		// The rows the player last picked on this page, or -1 for none. Only a pick is saved on OK.
		int PickedRows = -1;
		// The index of the row the drop-down shows, as the presenter last set it.
		int ShownRowIndex = -1;
		bool InitialBitmapSystemFont;
		bool InitialShowFPS;
		int InitialFrameLimit;
		int InitialWindowSize;
		int InitialHDROutput;
		int InitialPaperWhite;
		int InitialPeakBrightness;
		int InitialColorBlindMode;
		int InitialColorBlindStrength;
		int InitialBrightness;
		int InitialGamma;
		int InitialPlayerColors;
		bool InitialScreenShake;
		bool InitialScriptedCameraZoom;
		// Whether a colour preview has been shown, and whether the page has put its colour back.
		bool PreviewShown = false;
		bool PreviewRestored = false;
		// The colour values last sent to the frame, as the page's indices and values, so an unchanged value is not sent again.
		int PreviewedMode = 0;
		int PreviewedStrength = 0;
		int PreviewedBrightness = 0;
		int PreviewedGamma = 0;
		// Set by Reset to defaults on the HDR levels, so OK writes them back to automatic.
		bool HDRLevelsReset = false;
};


class UIConfirmModePresenterClass : public UIPresenterClass
{
	public:
		enum {
			DEFAULT_TIMEOUT = 10000
		};

		UIConfirmModePresenterClass(UIClockClass & clock, int timeout = DEFAULT_TIMEOUT);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		int Seconds;
		bool TimedOut = false;

	private:
		UIClockClass & Clock;
		int Timeout;
		std::optional<int> Deadline;
};


std::unique_ptr<UIViewClass> UI_Display_View(UIDisplayPresenterClass & presenter);
// The view of one tab of the display settings. The tabs share the presenter; each keeps its own document.
std::unique_ptr<UIViewClass> UI_Display_View(UIDisplayPresenterClass & presenter, UISettingsTab page);
// The presenter the display, graphics and accessibility tabs share, with the settings as they stand now.
std::unique_ptr<UIDisplayPresenterClass> UI_Display_New_Presenter(void);
// What the display tabs settle to when they close: the mode to try, if any, as UI_Display_Dialog returns it.
std::optional<UIDisplayMode> UI_Display_Settled(UIDisplayPresenterClass & presenter);
std::unique_ptr<UIViewClass> UI_Confirm_Mode_View(UIConfirmModePresenterClass & presenter);

UIDisplayServiceClass & UI_Display_Service(void);
void UI_Display_State(UIDisplayState & state);

// Returns the mode to switch to when the player changed the resolution or the interface scale
// (the current resolution, in the latter case), or nothing when the display need not be reset.
std::optional<UIDisplayMode> UI_Display_Dialog(UISettingsTab page);

bool UI_Confirm_Mode_Dialog(void);
