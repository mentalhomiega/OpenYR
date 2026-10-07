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
#include "globals.h"
#include "goptions.h"
#include "options.h"
#include "ui/screens/display/uidisplay.h"
#include "ui/uienginehost.h"
#include "ui/uishell.h"
#include "ui/uiview.h"
#include "video.h"
#include "wwmouse.h"

#include <cstdio>


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

		virtual void Set_Interface_Scale(float scale) override
		{
			// The caller resets the display, which settles the frame size from this.
			Options.InterfaceScale = scale;
		}
};

UIDisplayEngineServiceClass _Service;

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
			std::snprintf(buffer, sizeof(buffer), "%g", step);
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
		std::snprintf(buffer, sizeof(buffer), "%g", current);
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

	UIDisplayPresenterClass presenter(UI_Display_Service(), state);
	std::unique_ptr<UIViewClass> view = UI_Display_View(presenter);

	if (UI_Run_Modal(*view) == UI_RESULT_FAILED_TO_OPEN) {
		return(std::nullopt);
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
