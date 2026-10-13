/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/display/uidisplay.h"

#include "ui/rml/rmlview.h"

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
	InitialRenderer(State.Renderer)
{
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

	} else if (intent.Name == "stretch") {
		State.StretchMovies = (intent.Value != 0);

	} else if (intent.Name == "systemcursor") {
		State.SystemCursor = (intent.Value != 0);

	} else if (intent.Name == "classicmenus") {
		State.ClassicMenus = (intent.Value != 0);

	} else if (intent.Name == "moderninterface") {
		State.ModernInterface = (intent.Value != 0);

	} else if (intent.Name == "scale") {
		State.Scale = (intent.Value >= 0 && intent.Value < (int)State.Scales.size()) ? intent.Value : InitialScale;

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

	} else if (intent.Name == "ok") {
		Service.Set_Stretch_Movies(State.StretchMovies);
		Service.Set_System_Cursor(State.SystemCursor);
		Service.Set_Classic_Menus(State.ClassicMenus);
		Service.Set_Modern_Interface(State.ModernInterface);
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
			Model.DirtyVariable("scale");
			Model.DirtyVariable("fullscreen");
			Model.DirtyVariable("vsync");
			Model.DirtyVariable("integerscaling");
			Model.DirtyVariable("filter");
			Model.DirtyVariable("framerate");
			Model.DirtyVariable("cursorscale");
			Model.DirtyVariable("renderer");
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
				&& model.Bind("renderer", &state.Renderer));
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
