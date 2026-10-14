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
		virtual void Set_Debug_Mode(bool on) = 0;
		virtual void Set_Interface_Scale(float scale) = 0;
		virtual void Set_Fullscreen(bool on) = 0;
		virtual void Set_VSync(bool on) = 0;
		virtual void Set_Integer_Scaling(bool on) = 0;
		virtual void Set_Filter(int mode) = 0;
		virtual void Set_Frame_Rate(int rate) = 0;
		virtual void Set_Cursor_Scale(int scale) = 0;
		virtual void Set_Renderer(int renderer) = 0;
};


struct UIDisplayState
{
	std::vector<UIDisplayMode> Modes;
	int Selected = -1;
	bool StretchMovies = false;
	bool SystemCursor = false;
	bool ClassicMenus = false;
	bool ModernInterface = false;
	bool DebugMode = false;
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
	std::vector<UIDisplayChoice> Renderers;
	int Renderer = 0;
	std::vector<UIDisplayChoice> RowCounts;
	int RowCount = 0;
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

	private:
		UIDisplayServiceClass & Service;
		int Initial;
		int InitialScale;
		bool InitialFullscreen;
		bool InitialVSync;
		bool InitialIntegerScaling;
		int InitialFilter;
		int InitialFrameRate;
		int InitialCursorScale;
		int InitialRenderer;
		int InitialRowCount;
		bool InitialDebugMode;
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
std::unique_ptr<UIViewClass> UI_Confirm_Mode_View(UIConfirmModePresenterClass & presenter);

UIDisplayServiceClass & UI_Display_Service(void);
void UI_Display_State(UIDisplayState & state);

// Returns the mode to switch to when the player changed the resolution or the interface scale
// (the current resolution, in the latter case), or nothing when the display need not be reset.
std::optional<UIDisplayMode> UI_Display_Dialog(void);

bool UI_Confirm_Mode_Dialog(void);
