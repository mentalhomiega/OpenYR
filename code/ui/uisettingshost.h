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
#include "ui/uiview.h"

#include <functional>
#include <vector>

/*
 * The tabbed Settings screen is one view. Each tab is a page: a view with its own document and
 * presenter, loaded once when the screen opens. The host shows one page at a time and hides the
 * others, so a tab change swaps visibility inside one modal run instead of loading a document.
 * Pending values stay on their presenters across tab changes; OK and Cancel act on every page.
 */

// One tab of the Settings screen.
struct UISettingsPageClass
{
	UISettingsTab Tab = UI_TAB_NONE;
	UIViewClass * View = nullptr;
	UIPresenterClass * Presenter = nullptr;
	// Runs when the page is shown, before its view is, so the page can follow the tab (such as the display pages).
	std::function<void(void)> Enter;
};


class UISettingsHostViewClass;


class UISettingsHostPresenterClass : public UIPresenterClass
{
	public:
		// The presenters are the distinct presenters the pages use; a presenter shared by pages appears once.
		UISettingsHostPresenterClass(std::vector<UISettingsPageClass> pages, std::vector<UIPresenterClass *> presenters, unsigned offered, UISettingsTab first);

		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;
		virtual void Drain(void) override;

		UISettingsTab Shown_Tab(void) const { return(Pages[Current].Tab); }
		std::vector<UISettingsPageClass> const & Pages_Of_Host(void) const { return(Pages); }
		int Current_Index(void) const { return(Current); }

		// Moves to the page for the tab, if the screen has one for it. The page on show is hidden first, so only one is shown.
		void Show_Tab(UISettingsTab tab);
		// Runs the Enter of the page on show, before it is shown.
		void Enter_Current(void);

	private:
		// Ends the screen: every other presenter applies the accepted values, or drops the changes it holds.
		void Finish(UIPresenterClass * source);

		std::vector<UISettingsPageClass> Pages;
		std::vector<UIPresenterClass *> Presenters;
		unsigned Offered;
		int Current = 0;
};


class UISettingsHostViewClass : public UIViewClass
{
	public:
		explicit UISettingsHostViewClass(UISettingsHostPresenterClass & presenter);

		virtual bool Prepare(UIShellClass & shell) override;
		virtual void Show(bool modal) override;
		virtual void Hide(void) override;
		virtual void Release(void) override;
		virtual void Sync(void) override;
		virtual bool Is_Shown(void) const override;
		virtual void Placed(void) override;
		virtual float Reveal_Width(void) const override;
		virtual void Reveal_To(float width) override;
		virtual void Reveal_Done(void) override;
		virtual UIPresenterClass & Presenter(void) const override { return(Host); }
		virtual char const * Name(void) const override { return("settings"); }

	private:
		UIViewClass & Shown_View(void) const;

		UISettingsHostPresenterClass & Host;
};
