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
#include "bgfxbackend.h"
#include "globals.h"
#include "goptions.h"
#include "hdrcolour.h"
#include "hdrdisplay.h"
#include "hud.h"
#include "misc.h"
#include "options.h"
#include "playerdisplay.h"
#include "ui/screens/display/uidisplay.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uisettings.h"
#include "ui/uisettingpreview.h"
#include "ui/uiview.h"
#include "sdl/sdlwindow.h"
#include "vidscale.h"
#include "video.h"
#include "colourblind.h"
#include "wwmouse.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <initializer_list>
#include <string>
#include <vector>


namespace
{

enum {
	MIN_WIDTH = 640,
	MIN_HEIGHT = 400,
	MAX_WIDTH = 4096,
	MAX_HEIGHT = 4096
};


class UIDisplayEngineServiceClass : public UIDisplayServiceClass
{
	public:
		virtual void Set_Stretch_Movies(bool on) override
		{
			Options.StretchMovies = on;
		}

		virtual void Set_System_Cursor(bool on) override
		{
			Options.SystemCursor = on;
			Refresh_Window_Arrow();
		}

		virtual void Set_Classic_Menus(bool on) override
		{
			if (Options.IsClassicMenus != on) {
				Options.IsClassicMenus = on;
				UIShell.On_Menu_Style_Change();
			}
		}

		virtual void Set_Modern_Interface(bool on) override
		{
			Options.IsModernInterfaceChosen = on;
		}

		virtual void Set_Sidebar_Rows(int rows) override
		{
			// The saved rows are the shown rows now, so a preview ends with the save.
			Options.SidebarRows = rows;
			Hud_Rows_Preview(0);
			Modern_Panel_Rows_Changed();
		}

		virtual void Set_Bitmap_System_Font(bool on) override
		{
			Options.BitmapSystemFont = on;
		}

		virtual void Set_Show_FPS(bool on) override
		{
			Options.ShowFPS = on;
		}

		virtual void Set_Frame_Limit(int limit) override
		{
			Options.FrameRateLimit = limit;
			Video_Set_Refresh_Rate(Video_Refresh_Rate());
		}

		virtual void Set_Window_Size(int width, int height) override
		{
			Options.WindowWidth = width;
			Options.WindowHeight = height;
			if (WindowedMode) {
				int const frame_width = (width > 0) ? width : Options.ScreenWidth;
				int const frame_height = (height > 0) ? height : Options.ScreenHeight;
				Main_Window_Resize(frame_width, frame_height);
			}
		}

		virtual void Set_Player_Colors(int view) override
		{
			Options.PlayerColorView = view;
			Player_View_Changed();
		}

		virtual void Set_Interface_Scale(float scale) override
		{
			// The caller resets the display, which settles the frame size from this.
			Options.InterfaceScale = scale;
		}

		virtual void Set_Fullscreen(bool on) override
		{
			if (WindowedMode == !on) {
				return;
			}

			Options.Fullscreen = on;
			WindowedMode = !on;
			int const width = (Options.WindowWidth > 0) ? Options.WindowWidth : Options.ScreenWidth;
			int const height = (Options.WindowHeight > 0) ? Options.WindowHeight : Options.ScreenHeight;
			Main_Window_Set_Fullscreen(on, width, height);
			Main_Window_Confine_Cursor(on);
		}

		virtual void Set_VSync(bool on) override
		{
			Options.VSync = on;
			Backend_Set_VSync(on);
		}

		virtual void Set_Integer_Scaling(bool on) override
		{
			Options.IntegerScaling = on;
			Video_Refresh_Scaling();
		}

		virtual void Set_Gpu_Effects(bool render, bool softshadows, bool shockwaves, bool bloom) override
		{
			Options.GpuRender = render;
			Options.SoftShadows = softshadows;
			Options.Shockwaves = shockwaves;
			Options.Bloom = bloom;
			Apply_HDR_Highlights();
		}

		virtual void Set_Filter(int mode) override
		{
			Options.ScaleMode = mode;
			Video_Refresh_Scaling();
		}

		virtual void Set_Frame_Rate(int rate) override
		{
			Options.RenderFrameRate = rate;
		}

		virtual void Set_Cursor_Scale(int scale) override
		{
			Options.CursorScale = scale;
			Video_Refresh_Scaling();
		}

		virtual void Set_Menu_Theme(int theme) override
		{
			Options.MenuTheme = (MenuThemeType)theme;
		}

		virtual void Set_Menu_Battle(bool on) override
		{
			Options.MenuBattle = on;
		}

		virtual void Set_HDR_Output(int mode) override
		{
			// The output is decided when the renderer starts.
			Options.HDROutput = mode;
		}

		virtual void Set_HDR_Paper_White(int nits) override
		{
			Options.HDRPaperWhite = nits;
			Video_Refresh_HDR_Levels();
		}

		virtual void Set_HDR_Peak_Brightness(int nits) override
		{
			Options.HDRPeakBrightness = nits;
			Video_Refresh_HDR_Levels();
		}

		virtual void Set_Renderer(int renderer) override
		{
			// The renderer is chosen when the game starts.
			Options.Renderer = renderer;
		}

		virtual void Set_Colour_Blind(int mode, int strength) override
		{
			Options.ColorBlindMode = mode;
			Options.ColorBlindStrength = strength;
			Video_Apply_Colour_Settings();
		}

		virtual void Preview_Colour(int mode, int strength, int brightness, int gamma) override
		{
			// The pending values go on the frame now; the page puts the saved ones back when it is left without OK.
			Options.ColorBlindMode = mode;
			Options.ColorBlindStrength = strength;
			Options.Brightness = brightness;
			Options.Gamma = gamma / 100.0f;
			Video_Apply_Colour_Settings();
		}

		virtual void Set_Screen_Shake(bool on) override
		{
			Options.ScreenShake = on;
		}

		virtual void Set_Scripted_Camera_Zoom(bool on) override
		{
			Options.ScriptedCameraZoom = on;
		}

		virtual void Set_Brightness_Gamma(int brightness, int gamma) override
		{
			Options.Brightness = brightness;
			Options.Gamma = gamma / 100.0f;
			Video_Apply_Colour_Settings();
		}

		virtual void Set_HDR_Highlights(int highlights) override
		{
			Options.HDRHighlights = (highlights == 1) ? HDR_HIGHLIGHTS_ENHANCED : HDR_HIGHLIGHTS_CLASSIC;
			Apply_HDR_Highlights();
		}

		virtual void Frame_Size(int width, int height, float scale, int & frame_width, int & frame_height) override
		{
			Interface_Frame_For(width, height, scale, frame_width, frame_height);
		}

		virtual std::vector<int> Sidebar_Row_Counts(int frame_height) override
		{
			std::vector<int> counts;
			for (int rows = HUD_MIN_ROWS; rows <= Hud_Max_Rows_For_Frame(frame_height); rows++) {
				counts.push_back(rows);
			}
			return(counts);
		}
};


// Adds the choices to the list and returns the index of the one with the current value, adding
// the current value under its own label when the list does not have it.
int Fill_Choices(std::vector<UIDisplayChoice> & list, std::initializer_list<UIDisplayChoice> choices, int current, char const * format)
{
	int picked = -1;
	for (UIDisplayChoice const & choice : choices) {
		if (choice.Value == current) {
			picked = (int)list.size();
		}
		list.push_back(choice);
	}
	if (picked < 0) {
		UIDisplayChoice choice;
		choice.Value = current;
		char buffer[32];
		std::snprintf(buffer, sizeof(buffer), format, current);
		choice.Label = buffer;
		picked = (int)list.size();
		list.push_back(choice);
	}
	return(picked);
}

UIDisplayEngineServiceClass _Service;

// The mock screen beside the resolution list is this wide in dp. The panel in it is never narrower
// than PANEL_PREVIEW_MIN_WIDTH dp, so its cameo cells stay readable; on a large screen it is then drawn
// larger than its true size, and the caption gives the screen size.
float const PANEL_PREVIEW_WIDTH = 136.0f;
float const PANEL_PREVIEW_MIN_WIDTH = 36.0f;

// The caption above the mock takes this much height in dp.
float const PANEL_PREVIEW_CAPTION_HEIGHT = 18.0f;

// The smallest width or height in dp of a mock shape, which keeps the power bar and arrows visible.
float const PANEL_PREVIEW_MIN_SHAPE = 3.0f;

// The category tabs the modern panel stacks beside the cameo grid.
int const PANEL_PREVIEW_TABS = 4;

std::string Dp_Text(float value)
{
	char buffer[32];
	std::snprintf(buffer, sizeof(buffer), "%.2fdp", value);
	return(buffer);
}


// The mock of the modern build panel with the rows given, drawn on a frame of the size given. Its shapes
// follow Hud_Layout_For_Frame, scaled to the mock and set against its right edge below the caption.
UISettingPreviewScreen Panel_Preview(int rows, int frame_width, int frame_height)
{
	UISettingPreviewScreen screen;
	if (frame_width <= 0 || frame_height <= 0) {
		return(screen);
	}

	HudLayout const layout = Hud_Layout_For_Frame(rows, frame_width, frame_height);
	float const box_width = PANEL_PREVIEW_WIDTH;
	float const box_height = box_width * frame_height / frame_width;
	float const scale = std::min(std::max(box_width / frame_width, PANEL_PREVIEW_MIN_WIDTH / static_cast<float>(HUD_PANEL_WIDTH)), box_height / layout.Panel.Height);
	float const origin = box_width - static_cast<float>(HUD_PANEL_WIDTH) * scale;

	auto place = [&](char const * kind, float left, float top, float width, float height) {
		UISettingPreviewShape shape;
		shape.Classes = std::string("setting-preview-shape ") + kind;
		shape.Left = Dp_Text(left);
		shape.Top = Dp_Text(PANEL_PREVIEW_CAPTION_HEIGHT + top);
		shape.Width = Dp_Text(width);
		shape.Height = Dp_Text(height);
		screen.Shapes.push_back(shape);
	};

	// Places a rectangle given in the panel's own coordinates.
	auto at = [&](char const * kind, float x, float y, float width, float height) {
		place(kind, origin + x * scale, y * scale, width * scale, height * scale);
	};

	place("mock-screen", 0.0f, 0.0f, box_width, box_height);
	at("mock-panel", 0.0f, 0.0f, static_cast<float>(HUD_PANEL_WIDTH), static_cast<float>(layout.Panel.Height));
	at("mock-top", HUD_MARGIN, HUD_MARGIN, HUD_PANEL_WIDTH - HUD_MARGIN * 2, static_cast<float>(layout.Readouts.Y + layout.Readouts.Height - HUD_MARGIN * 2));
	// The power bar and the arrows keep a few dp each on a small mock, so they stay visible. The fill rises
	// from the bottom of the bar to 60 per cent of its height.
	float const well_left = origin + layout.Power.X * scale;
	float const well_top = layout.Power.Y * scale;
	float const well_width = std::max(PANEL_PREVIEW_MIN_SHAPE, layout.Power.Width * scale);
	float const well_height = layout.Power.Height * scale;
	place("mock-well", well_left, well_top, well_width, well_height);
	place("mock-fill", well_left + 1.0f, well_top + well_height * 0.4f, std::max(1.0f, well_width - 2.0f), well_height * 0.6f - 1.0f);

	// The tabs keep a gap of at least a dp between them, however small the mock is drawn.
	float const tab_gap = 1.0f;
	float const tab_height = (layout.Tabs.Height * scale - tab_gap * (PANEL_PREVIEW_TABS - 1)) / static_cast<float>(PANEL_PREVIEW_TABS);
	for (int tab = 0; tab < PANEL_PREVIEW_TABS; tab++) {
		place("mock-tab", origin + (layout.Tabs.X + 2) * scale, layout.Tabs.Y * scale + tab * (tab_height + tab_gap), (layout.Tabs.Width - 4) * scale, tab_height);
	}

	float const column_width = layout.Grid.Width / static_cast<float>(HUD_GRID_COLUMNS);
	float const row_height = layout.Grid.Height / static_cast<float>(layout.Rows);
	for (int row = 0; row < layout.Rows; row++) {
		for (int column = 0; column < HUD_GRID_COLUMNS; column++) {
			// Each cell is inset half a dp on every side, so neighbouring cells stay apart on a small mock.
			float const left = origin + (layout.Grid.X + column * column_width) * scale;
			float const top = (layout.Grid.Y + row * row_height) * scale;
			place("mock-cell", left + 0.5f, top + 0.5f, column_width * scale - 1.0f, row_height * scale - 1.0f);
		}
	}

	float const arrow_width = std::max(PANEL_PREVIEW_MIN_SHAPE, 10.0f * scale);
	float const arrow_height = std::max(PANEL_PREVIEW_MIN_SHAPE, 12.0f * scale);
	float const centre = origin + (layout.Arrows.X + layout.Arrows.Width / 2.0f) * scale;
	float const arrow_top = (layout.Arrows.Y + layout.Arrows.Height / 2.0f) * scale - arrow_height / 2.0f;
	place("mock-arrow", centre - 4.0f * scale - arrow_width, arrow_top, arrow_width, arrow_height);
	place("mock-arrow", centre + 4.0f * scale, arrow_top, arrow_width, arrow_height);

	return(screen);
}


// Shows a Sidebar rows value in the modern build panel while the Display page has it picked. Reverting
// shows the saved rows again. The mock beside the list draws the same rows on the frame that is picked.
void Register_Setting_Previews(void)
{
	UISettingPreview rows;
	rows.Preview = [](int value) {
		Hud_Rows_Preview(value);
		Modern_Panel_Rows_Changed();
	};
	rows.Revert = []() {
		Hud_Rows_Preview(0);
		Modern_Panel_Rows_Changed();
	};
	rows.Screen = [](int value, int frame_width, int frame_height) {
		return(Panel_Preview(value, frame_width, frame_height));
	};
	UI_Setting_Preview_Register("sidebar-rows", rows);
}

}


UIDisplayServiceClass & UI_Display_Service(void)
{
	return(_Service);
}


void UI_Display_State(UIDisplayState & state)
{
	state = UIDisplayState();
	state.StretchMovies = Options.StretchMovies;
	state.SystemCursor = Options.SystemCursor;
	state.ClassicMenus = Options.IsClassicMenus;
	state.ModernInterface = Options.IsModernInterfaceChosen;
	state.BitmapSystemFont = Options.BitmapSystemFont;
	state.ShowFPS = Options.ShowFPS;
	state.FrameLimit = Fill_Choices(state.FrameLimits, {
		{-1, "Display refresh"},
		{30, "30"},
		{60, "60"},
		{120, "120"},
		{144, "144"},
		{240, "240"},
		{0, "Unlimited"}
	}, (Options.FrameRateLimit < 0) ? -1 : Options.FrameRateLimit, "%d");

	// Match resolution follows ScreenWidth and ScreenHeight. The fixed sizes above the desktop are left out.
	UIDisplayMode match;
	match.Width = -1;
	match.Height = -1;
	match.Label = "Match resolution";
	state.WindowSizes.push_back(match);
	static int const window_sizes[][2] = {{800, 600}, {1024, 768}, {1280, 720}, {1280, 800}, {1600, 900}, {1920, 1080}, {2560, 1440}, {3840, 2160}};
	int desktop_width = 0;
	int desktop_height = 0;
	bool const known_desktop = Main_Window_Desktop_Size(desktop_width, desktop_height);
	for (auto const & size : window_sizes) {
		if (known_desktop && (size[0] > desktop_width || size[1] > desktop_height)) {
			continue;
		}
		UIDisplayMode mode;
		mode.Width = size[0];
		mode.Height = size[1];
		mode.Label = std::to_string(size[0]) + " x " + std::to_string(size[1]);
		state.WindowSizes.push_back(mode);
	}
	// Zero or below in either dimension follows the frame, so both are treated as Match resolution.
	state.WindowSize = -1;
	if (Options.WindowWidth <= 0 && Options.WindowHeight <= 0) {
		state.WindowSize = 0;
	}
	for (int index = 1; state.WindowSize < 0 && index < (int)state.WindowSizes.size(); index++) {
		if (state.WindowSizes[index].Width == Options.WindowWidth && state.WindowSizes[index].Height == Options.WindowHeight) {
			state.WindowSize = index;
		}
	}
	if (state.WindowSize < 0) {
		UIDisplayMode mode;
		mode.Width = Options.WindowWidth;
		mode.Height = Options.WindowHeight;
		int const shown_width = (Options.WindowWidth > 0) ? Options.WindowWidth : Options.ScreenWidth;
		int const shown_height = (Options.WindowHeight > 0) ? Options.WindowHeight : Options.ScreenHeight;
		mode.Label = std::to_string(shown_width) + " x " + std::to_string(shown_height);
		state.WindowSize = (int)state.WindowSizes.size();
		state.WindowSizes.push_back(mode);
	}
	state.PlayerColors = Fill_Choices(state.PlayerColorChoices, {
		{PLAYER_VIEW_DEFAULT, "Default"},
		{PLAYER_VIEW_COLORBLIND_SAFE, "Color-blind safe"},
		{PLAYER_VIEW_RELATIONSHIP, "Relationship"}
	}, Options.PlayerColorView, "%d");
	state.Fullscreen = !WindowedMode;
	state.VSync = Options.VSync;
	state.IntegerScaling = Options.IntegerScaling;
	state.GpuRender = Options.GpuRender;
	state.SoftShadows = Options.SoftShadows;
	state.Shockwaves = Options.Shockwaves;
	state.Bloom = Options.Bloom;
	state.Highlights = (Options.HDRHighlights == HDR_HIGHLIGHTS_ENHANCED) ? 1 : 0;
	state.HDRActive = Video_HDR_Active();
	state.ColorBlindMode = Fill_Choices(state.ColorBlindModes, {
		{COLORBLIND_OFF, "Off"},
		{COLORBLIND_PROTANOPIA, "Protanopia"},
		{COLORBLIND_DEUTERANOPIA, "Deuteranopia"},
		{COLORBLIND_TRITANOPIA, "Tritanopia"}
	}, Options.ColorBlindMode, "%d");
	state.ColorBlindStrength = Options.ColorBlindStrength;
	state.Brightness = Options.Brightness;
	state.Gamma = (int)std::lround(Options.Gamma * 100.0f);

	state.Filter = Fill_Choices(state.Filters, {
		{VIDEO_SCALE_PIXELART, "Pixel art"},
		{VIDEO_SCALE_NEAREST, "Nearest"},
		{VIDEO_SCALE_LINEAR, "Linear"}
	}, Options.ScaleMode, "%d");
	state.FrameRate = Fill_Choices(state.FrameRates, {
		{0, "Off"},
		{-1, "Match display"},
		{30, "30"},
		{60, "60"},
		{120, "120"},
		{144, "144"},
		{240, "240"}
	}, Options.RenderFrameRate, "%d");
	state.CursorScale = Fill_Choices(state.CursorScales, {
		{0, "Auto"},
		{1, "1x"},
		{2, "2x"},
		{3, "3x"},
		{4, "4x"}
	}, (Options.CursorScale < 0) ? 1 : Options.CursorScale, "%dx");
	state.MenuTheme = Fill_Choices(state.MenuThemes, {
		{MENU_THEME_RANDOM, "Random"},
		{MENU_THEME_ALLIED, "Allied"},
		{MENU_THEME_SOVIET, "Soviet"},
		{MENU_THEME_YURI, "Yuri"}
	}, (int)Options.MenuTheme, "%d");
	state.MenuBattle = Options.MenuBattle;
	state.Renderer = Fill_Choices(state.Renderers, {
		{BACKEND_RENDERER_AUTO, "Auto"},
		{BACKEND_RENDERER_D3D11, "Direct3D 11"},
		{BACKEND_RENDERER_D3D12, "Direct3D 12"},
		{BACKEND_RENDERER_VULKAN, "Vulkan (experimental)"},
		{BACKEND_RENDERER_OPENGL, "OpenGL"}
	}, Options.Renderer, "%d");

	// HDR output is a choice of mode; the brightness sliders show the levels in use, whether set or automatic.
	state.HDROutput = Fill_Choices(state.HDROutputs, {
		{VIDEO_HDR_AUTOMATIC, "Automatic"},
		{VIDEO_HDR_OFF, "Off"},
		{VIDEO_HDR_ON, "On"}
	}, Options.HDROutput, "%d");
	state.PaperWhite = Video_HDR_Paper_White();
	state.PeakBrightness = Video_HDR_Peak();
	// The levels automatic mode resolves to, which Reset to defaults puts back.
	state.DefaultPaperWhite = HDR_Resolve_Paper_White(0, HDR_Current_Display().SDRWhiteKnown, HDR_Current_Display().SDRWhiteNits);
	state.DefaultPeakBrightness = HDR_Resolve_Peak(0, HDR_Current_Display().PeakNits, state.DefaultPaperWhite);
	state.ScreenShake = Options.ScreenShake;
	state.ScriptedCameraZoom = Options.ScriptedCameraZoom;

	// The rows the current screen fits, from one up. The page starts on the saved count, or on the most
	// the frame allows when the saved count is more.
	state.ScreenWidth = Options.ScreenWidth;
	state.ScreenHeight = Options.ScreenHeight;
	int frame_width = 0;
	int frame_height = 0;
	_Service.Frame_Size(Options.ScreenWidth, Options.ScreenHeight, Options.InterfaceScale, frame_width, frame_height);
	// The fewest rows when the saved count is below every choice.
	state.RowCount = 0;
	for (int value : _Service.Sidebar_Row_Counts(frame_height)) {
		UIDisplayChoice choice;
		choice.Value = value;
		choice.Label = std::to_string(value);
		if (value <= Options.SidebarRows) {
			state.RowCount = (int)state.RowCounts.size();
		}
		state.RowCounts.push_back(choice);
	}

	// Automatic, the usual steps, and whatever else the settings file has asked for.
	static float const steps[] = {0.0f, 1.0f, 1.5f, 2.0f, 2.5f, 3.0f};
	float const current = (Options.InterfaceScale > 0.0f) ? Options.InterfaceScale : 0.0f;
	state.Scale = -1;
	for (float step : steps) {
		if (step == current) {
			state.Scale = (int)state.Scales.size();
		}
		UIDisplayScale scale;
		scale.Value = step;
		if (step > 0.0f) {
			char buffer[32];
			std::snprintf(buffer, sizeof(buffer), "%ld%%", std::lround(step * 100.0f));
			scale.Label = buffer;
		} else {
			scale.Label = "Auto";
		}
		state.Scales.push_back(scale);
	}
	if (state.Scale < 0) {
		UIDisplayScale scale;
		scale.Value = current;
		char buffer[32];
		std::snprintf(buffer, sizeof(buffer), "%ld%%", std::lround(current * 100.0f));
		scale.Label = buffer;
		state.Scale = (int)state.Scales.size();
		state.Scales.push_back(scale);
	}

	int * modes = EnumDisplayModes(MIN_WIDTH, MIN_HEIGHT, MAX_WIDTH, MAX_HEIGHT);
	if (modes == NULL) {
		return;
	}

	for (int * entry = modes; *entry != 0; entry += 2) {
		UIDisplayMode mode;
		mode.Width = entry[0];
		mode.Height = entry[1];

		char buffer[64];
		std::snprintf(buffer, sizeof(buffer), "%d x %d", mode.Width, mode.Height);
		mode.Label = buffer;

		if (mode.Width == Options.ScreenWidth && mode.Height == Options.ScreenHeight) {
			state.Selected = (int)state.Modes.size();
		}
		state.Modes.push_back(mode);
	}

	delete [] modes;
}


std::unique_ptr<UIDisplayPresenterClass> UI_Display_New_Presenter(void)
{
	UIDisplayState state;
	UI_Display_State(state);
	Register_Setting_Previews();
	return(std::make_unique<UIDisplayPresenterClass>(UI_Display_Service(), state));
}


std::optional<UIDisplayMode> UI_Display_Settled(UIDisplayPresenterClass & presenter)
{
	if (presenter.Result != UI_RESULT_ACCEPTED) {
		presenter.Restore_Preview();
	}
	if (!presenter.Picked.has_value() && presenter.ScaleChanged) {
		UIDisplayMode current;
		current.Width = Options.ScreenWidth;
		current.Height = Options.ScreenHeight;
		return(current);
	}
	return(presenter.Picked);
}


std::optional<UIDisplayMode> UI_Display_Dialog(UISettingsTab page)
{
	UIDisplayState state;
	UI_Display_State(state);
	state.Page = page;
	Register_Setting_Previews();

	UIDisplayPresenterClass presenter(UI_Display_Service(), state);
	UI_Settings_Join(presenter, page);
	std::unique_ptr<UIViewClass> view = UI_Display_View(presenter);

	if (UI_Run_Modal(*view) == UI_RESULT_FAILED_TO_OPEN) {
		return(std::nullopt);
	}
	UI_Settings_Leave(presenter);
	if (presenter.Result != UI_RESULT_ACCEPTED) {
		presenter.Restore_Preview();
		UI_Setting_Preview_Revert();
	}

	if (!presenter.Picked.has_value() && presenter.ScaleChanged) {
		UIDisplayMode current;
		current.Width = Options.ScreenWidth;
		current.Height = Options.ScreenHeight;
		return(current);
	}

	return(presenter.Picked);
}


bool UI_Confirm_Mode_Dialog(void)
{
	UIConfirmModePresenterClass presenter(UIShell.Clock());
	std::unique_ptr<UIViewClass> view = UI_Confirm_Mode_View(presenter);

	UIResult result = UI_Run_Modal(*view);

	if (result == UI_RESULT_FAILED_TO_OPEN) {
		return(true);
	}

	return(result == UI_RESULT_ACCEPTED);
}
