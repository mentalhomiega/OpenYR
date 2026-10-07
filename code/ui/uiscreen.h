/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <optional>
#include <string>
#include <vector>


enum UIResult
{
	UI_RESULT_ACCEPTED,
	UI_RESULT_CANCELLED,
	UI_RESULT_SESSION_ENDED,
	UI_RESULT_FAILED_TO_OPEN,
};


struct UIIntent
{
	std::string Name;
	int Value = 0;
	std::string Text;
};


/// <summary>
/// The pages of the tabbed Settings screen the modern menu style uses in place of separate
/// option screens.
/// </summary>
enum UISettingsTab
{
	UI_TAB_NONE = -1,
	UI_TAB_GAME,
	UI_TAB_DISPLAY,
	UI_TAB_AUDIO,
	UI_TAB_KEYBOARD,
	UI_TAB_MODS,
	UI_TAB_COUNT
};


class UIClockClass
{
	public:
		virtual ~UIClockClass(void) = default;
		virtual int Milliseconds(void) = 0;
};


class UIPresenterClass
{
	public:
		virtual ~UIPresenterClass(void) = default;
		void Queue(UIIntent const & intent);
		void Drain(void);
		void Discard(void);
		bool Has_Pending(void) const;
		virtual void Execute(UIIntent const & intent) = 0;
		virtual void Refresh(void) = 0;

		std::optional<UIResult> Result;

		/// <summary>
		/// Makes this screen one page of the tabbed Settings. Choosing another tab then applies
		/// the page as OK does, ends it, and leaves the chosen tab in Tab_Request for the host to
		/// open next.
		/// </summary>
		/// <param name="tab">The tab this screen stands for.</param>
		/// <param name="offered">A bit for every tab the tab bar offers, 1 << UISettingsTab.</param>
		void Set_Settings_Tab(UISettingsTab tab, unsigned offered);
		UISettingsTab Settings_Tab(void) const { return(OwnTab); }
		unsigned Settings_Tabs_Offered(void) const { return(Offered); }

		/// <summary>The tab the player picked to leave this page for, or UI_TAB_NONE.</summary>
		UISettingsTab Tab_Request = UI_TAB_NONE;

	private:
		void Switch_Tab(int tab);

		UISettingsTab OwnTab = UI_TAB_NONE;
		unsigned Offered = 0;
		std::vector<UIIntent> Pending;
};
