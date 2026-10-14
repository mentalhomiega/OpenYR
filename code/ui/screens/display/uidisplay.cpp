/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/display/uidisplay.h"

#include "ui/rml/rmlsettingpreview.h"
#include "ui/rml/rmlview.h"
#include "ui/uisettingpreview.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <utility>


UIDisplayPresenterClass::UIDisplayPresenterClass(UIDisplayServiceClass & service, UIDisplayState state) :
	State(std::move(state)),
	Service(service),
	Initial(State.Selected),
	InitialScale(State.Scale),
	InitialFullscreen(State.Fullscreen),
	InitialVSync(State.VSync),
	InitialIntegerScaling(State.IntegerScaling),
	InitialFilter(State.Filter),
	InitialFrameRate(State.FrameRate),
	InitialCursorScale(State.CursorScale),
	InitialRenderer(State.Renderer),
	InitialDebugMode(State.DebugMode)
{
	ShownRowIndex = State.RowCount;
	Refresh_Preview();
}


void UIDisplayPresenterClass::Chosen_Frame(int & width, int & height)
{
	int screen_width = State.ScreenWidth;
	int screen_height = State.ScreenHeight;
	if (State.Selected >= 0 && State.Selected < (int)State.Modes.size()) {
		screen_width = State.Modes[State.Selected].Width;
		screen_height = State.Modes[State.Selected].Height;
	}
	float const scale = (State.Scale >= 0 && State.Scale < (int)State.Scales.size()) ? State.Scales[State.Scale].Value : 0.0f;

	width = 0;
	height = 0;
	if (screen_width > 0 && screen_height > 0) {
		Service.Frame_Size(screen_width, screen_height, scale, width, height);
	}
}


int UIDisplayPresenterClass::Chosen_Rows(void) const
{
	return((ShownRowIndex >= 0 && ShownRowIndex < (int)State.RowCounts.size()) ? State.RowCounts[ShownRowIndex].Value : -1);
}


void UIDisplayPresenterClass::Refresh_Rows(void)
{
	int const previous = Chosen_Rows();
	int width = 0;
	int height = 0;
	Chosen_Frame(width, height);

	// The rows the new frame allows. The row count stays the largest one that does not exceed the rows before.
	std::vector<UIDisplayChoice> const before = State.RowCounts;
	State.RowCounts.clear();
	State.RowCount = -1;
	if (height > 0) {
		for (int value : Service.Sidebar_Row_Counts(height)) {
			UIDisplayChoice choice;
			choice.Value = value;
			choice.Label = std::to_string(value);
			if (value <= previous) {
				State.RowCount = (int)State.RowCounts.size();
			}
			State.RowCounts.push_back(choice);
		}
		if (State.RowCount < 0 && !State.RowCounts.empty()) {
			State.RowCount = 0;
		}
	}
	ShownRowIndex = State.RowCount;
	bool const same = before.size() == State.RowCounts.size() && std::equal(before.begin(), before.end(), State.RowCounts.begin(),
		[](UIDisplayChoice const & a, UIDisplayChoice const & b) { return(a.Value == b.Value); });
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
			if (State.Selected >= 0 && State.Selected < (int)State.Modes.size()) {
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


static int Pick_Choice(std::vector<UIDisplayChoice> const & choices, int index, int fallback)
{
	return((index >= 0 && index < (int)choices.size()) ? index : fallback);
}


static bool Choice_Changed(std::vector<UIDisplayChoice> const & choices, int index, int initial)
{
	return(index != initial && index >= 0 && index < (int)choices.size());
}


void UIDisplayPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "select") {
		State.Selected = (intent.Value >= 0 && intent.Value < (int)State.Modes.size()) ? intent.Value : -1;
		Refresh_Rows();

	} else if (intent.Name == "stretch") {
		State.StretchMovies = (intent.Value != 0);

	} else if (intent.Name == "systemcursor") {
		State.SystemCursor = (intent.Value != 0);

	} else if (intent.Name == "classicmenus") {
		State.ClassicMenus = (intent.Value != 0);

	} else if (intent.Name == "moderninterface") {
		State.ModernInterface = (intent.Value != 0);

	} else if (intent.Name == "debugmode") {
		State.DebugMode = (intent.Value != 0);

	} else if (intent.Name == "scale") {
		State.Scale = (intent.Value >= 0 && intent.Value < (int)State.Scales.size()) ? intent.Value : InitialScale;
		Refresh_Rows();

	} else if (intent.Name == "fullscreen") {
		State.Fullscreen = (intent.Value != 0);

	} else if (intent.Name == "vsync") {
		State.VSync = (intent.Value != 0);

	} else if (intent.Name == "integerscaling") {
		State.IntegerScaling = (intent.Value != 0);

	} else if (intent.Name == "filter") {
		State.Filter = Pick_Choice(State.Filters, intent.Value, InitialFilter);

	} else if (intent.Name == "framerate") {
		State.FrameRate = Pick_Choice(State.FrameRates, intent.Value, InitialFrameRate);

	} else if (intent.Name == "cursorscale") {
		State.CursorScale = Pick_Choice(State.CursorScales, intent.Value, InitialCursorScale);

	} else if (intent.Name == "renderer") {
		State.Renderer = Pick_Choice(State.Renderers, intent.Value, InitialRenderer);

	} else if (intent.Name == "sidebarrows") {
		// The drop-down reports its value again whenever its list or value is set, so only a row other than the
		// one shown is a pick. The bound row count is not compared: the drop-down writes it before this runs.
		int const index = Pick_Choice(State.RowCounts, intent.Value, ShownRowIndex);
		if (index != ShownRowIndex && index >= 0 && index < (int)State.RowCounts.size()) {
			ShownRowIndex = index;
			State.RowCount = index;
			PickedRows = State.RowCounts[State.RowCount].Value;
			UI_Setting_Preview_Show("sidebar-rows", PickedRows);
			Refresh_Preview();
		} else {
			// Not a pick: the drop-down shows the row it showed before, so its bound value goes back to that row.
			State.RowCount = ShownRowIndex;
		}

	} else if (intent.Name == "ok") {
		Service.Set_Stretch_Movies(State.StretchMovies);
		Service.Set_System_Cursor(State.SystemCursor);
		Service.Set_Classic_Menus(State.ClassicMenus);
		Service.Set_Modern_Interface(State.ModernInterface);
		if (State.DebugMode != InitialDebugMode) {
			Service.Set_Debug_Mode(State.DebugMode);
		}
		if (State.Fullscreen != InitialFullscreen) {
			Service.Set_Fullscreen(State.Fullscreen);
		}
		if (State.VSync != InitialVSync) {
			Service.Set_VSync(State.VSync);
		}
		if (State.IntegerScaling != InitialIntegerScaling) {
			Service.Set_Integer_Scaling(State.IntegerScaling);
		}
		if (Choice_Changed(State.Filters, State.Filter, InitialFilter)) {
			Service.Set_Filter(State.Filters[State.Filter].Value);
		}
		if (Choice_Changed(State.FrameRates, State.FrameRate, InitialFrameRate)) {
			Service.Set_Frame_Rate(State.FrameRates[State.FrameRate].Value);
		}
		if (Choice_Changed(State.CursorScales, State.CursorScale, InitialCursorScale)) {
			Service.Set_Cursor_Scale(State.CursorScales[State.CursorScale].Value);
		}
		if (Choice_Changed(State.Renderers, State.Renderer, InitialRenderer)) {
			Service.Set_Renderer(State.Renderers[State.Renderer].Value);
		}
		if (PickedRows >= 0) {
			Service.Set_Sidebar_Rows(PickedRows);
		}
		UI_Setting_Preview_Commit();
		if (State.Scale != InitialScale && State.Scale >= 0 && State.Scale < (int)State.Scales.size()) {
			Service.Set_Interface_Scale(State.Scales[State.Scale].Value);
			ScaleChanged = true;
		}
		if (State.Selected >= 0 && State.Selected != Initial) {
			Picked = State.Modes[State.Selected];
		}
		Result = UI_RESULT_ACCEPTED;

	} else if (intent.Name == "cancel") {
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

class UIDisplayViewClass : public UIRmlViewClass
{
	public:
		explicit UIDisplayViewClass(UIDisplayPresenterClass & presenter) :
			UIRmlViewClass(presenter, "display.rml", "display"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Model.DirtyVariable("selected");
			Model.DirtyVariable("stretch");
			Model.DirtyVariable("systemcursor");
			Model.DirtyVariable("classicmenus");
			Model.DirtyVariable("moderninterface");
			Model.DirtyVariable("debugmode");
			Model.DirtyVariable("scale");
			Model.DirtyVariable("fullscreen");
			Model.DirtyVariable("vsync");
			Model.DirtyVariable("integerscaling");
			Model.DirtyVariable("filter");
			Model.DirtyVariable("framerate");
			Model.DirtyVariable("cursorscale");
			Model.DirtyVariable("renderer");
			Model.DirtyVariable("rowcount");
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

			Rml::StructHandle<UIDisplayChoice> choice = model.RegisterStruct<UIDisplayChoice>();
			if (!choice) {
				return(false);
			}
			choice.RegisterMember("label", &UIDisplayChoice::Label);

			UIDisplayState & state = Data.State;
			return(model.RegisterArray<std::vector<UIDisplayMode>>()
				&& model.RegisterArray<std::vector<UIDisplayScale>>()
				&& model.RegisterArray<std::vector<UIDisplayChoice>>()
				&& model.Bind("scales", &state.Scales)
				&& model.Bind("scale", &state.Scale)
				&& model.Bind("modes", &state.Modes)
				&& model.Bind("selected", &state.Selected)
				&& model.Bind("stretch", &state.StretchMovies)
				&& model.Bind("systemcursor", &state.SystemCursor)
				&& model.Bind("classicmenus", &state.ClassicMenus)
				&& model.Bind("moderninterface", &state.ModernInterface)
				&& model.Bind("debugmode", &state.DebugMode)
				&& model.Bind("fullscreen", &state.Fullscreen)
				&& model.Bind("vsync", &state.VSync)
				&& model.Bind("integerscaling", &state.IntegerScaling)
				&& model.Bind("filters", &state.Filters)
				&& model.Bind("filter", &state.Filter)
				&& model.Bind("framerates", &state.FrameRates)
				&& model.Bind("framerate", &state.FrameRate)
				&& model.Bind("cursorscales", &state.CursorScales)
				&& model.Bind("cursorscale", &state.CursorScale)
				&& model.Bind("renderers", &state.Renderers)
				&& model.Bind("renderer", &state.Renderer)
				&& model.Bind("rowcounts", &state.RowCounts)
				&& model.Bind("rowcount", &state.RowCount)
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
	return(std::make_unique<UIDisplayViewClass>(presenter));
}


std::unique_ptr<UIViewClass> UI_Confirm_Mode_View(UIConfirmModePresenterClass & presenter)
{
	return(std::make_unique<UIConfirmModeViewClass>(presenter));
}
