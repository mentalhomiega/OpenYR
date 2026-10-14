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
#include "debugmode.h"
#include "globals.h"
#include "goptions.h"
#include "hud.h"
#include "misc.h"
#include "numparse.h"
#include "options.h"
#include "ui/screens/display/uidisplay.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uisettings.h"
#include "ui/uisettingpreview.h"
#include "ui/uiview.h"
#include "sdl/sdlwindow.h"
#include "video.h"
#include "wwmouse.h"

#include <algorithm>
#include <cstdio>
#include <initializer_list>
#include <string>


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

		virtual void Set_Debug_Mode(bool on) override
		{
			Debug_Mode_Set(on);
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

		virtual void Set_Renderer(int renderer) override
		{
			// The renderer is chosen when the game starts.
			Options.Renderer = renderer;
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

// Shows a Sidebar rows value in the modern build panel while the Display page has it picked. Reverting
// shows the saved rows again.
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
	state.DebugMode = Options.DebugMode;
	state.Fullscreen = !WindowedMode;
	state.VSync = Options.VSync;
	state.IntegerScaling = Options.IntegerScaling;

	state.Filter = Fill_Choices(state.Filters, {
		{VIDEO_SCALE_PIXELART, "Pixel art"},
		{VIDEO_SCALE_NEAREST, "Sharp"},
		{VIDEO_SCALE_LINEAR, "Smooth"}
	}, Options.ScaleMode, "%d");
	state.FrameRate = Fill_Choices(state.FrameRates, {
		{0, "Off"},
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
	state.Renderer = Fill_Choices(state.Renderers, {
		{BACKEND_RENDERER_AUTO, "Auto"},
		{BACKEND_RENDERER_D3D11, "Direct3D 11"},
		{BACKEND_RENDERER_D3D12, "Direct3D 12"},
		{BACKEND_RENDERER_VULKAN, "Vulkan"},
		{BACKEND_RENDERER_OPENGL, "OpenGL"}
	}, Options.Renderer, "%d");

	// The rows the screen fits, from one up. The panel holds the saved count within them.
	int const most = Hud_Max_Rows();
	for (int rows = HUD_MIN_ROWS; rows <= most; rows++) {
		UIDisplayChoice choice;
		choice.Value = rows;
		choice.Label = std::to_string(rows);
		state.RowCounts.push_back(choice);
	}
	state.RowCount = std::clamp(Hud_Rows(), int(HUD_MIN_ROWS), most) - HUD_MIN_ROWS;

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
			Snprintf_Nearest(buffer, sizeof(buffer), "%g", step);
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
		Snprintf_Nearest(buffer, sizeof(buffer), "%g", current);
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


std::optional<UIDisplayMode> UI_Display_Dialog(void)
{
	UIDisplayState state;
	UI_Display_State(state);
	Register_Setting_Previews();

	UIDisplayPresenterClass presenter(UI_Display_Service(), state);
	UI_Settings_Join(presenter, UI_TAB_DISPLAY);
	std::unique_ptr<UIViewClass> view = UI_Display_View(presenter);

	if (UI_Run_Modal(*view) == UI_RESULT_FAILED_TO_OPEN) {
		return(std::nullopt);
	}
	UI_Settings_Leave(presenter);
	if (presenter.Result != UI_RESULT_ACCEPTED) {
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
