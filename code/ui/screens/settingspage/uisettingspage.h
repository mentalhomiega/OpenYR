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
#include "ui/uisettingrows.h"

#include <memory>
#include <span>
#include <string>
#include <vector>

class UIViewClass;

/*
 * The settings pages made only of declared rows (Option_Setting_Pages): one presenter and one
 * document show all of them, one page at a time. The page on show follows the tab, and each
 * page keeps its pending values until OK applies every page or Cancel drops them.
 */

class UISettingsPageServiceClass
{
	public:
		virtual ~UISettingsPageServiceClass(void) = default;
		// Writes the rows to the settings they were built from.
		virtual void Set_Declared_Settings(std::vector<UISettingRow> const & rows) = 0;
		virtual bool Condition_Met(SettingCondition condition) = 0;
};


// One declared page and the rows it shows.
struct UIDeclaredPage
{
	std::string Id;
	std::string Label;
	std::vector<UISettingRow> Rows;
};


struct UISettingsPageState
{
	// The pages as they were built. The presenter edits Rows, not the Rows of these.
	std::vector<UIDeclaredPage> Pages;
	int Shown = 0;
	// Every page's rows, page after page, as the markup lists them, with the index of the page each belongs to.
	// The markup hides the rows of the pages not on show, so the list keeps its length when the page changes.
	std::vector<UISettingRow> Rows;
	std::vector<int> RowPages;
};


class UISettingsPagePresenterClass : public UIPresenterClass
{
	public:
		UISettingsPagePresenterClass(UISettingsPageServiceClass & service, UISettingsPageState state);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		// Shows the page at the index. Every page keeps the values pending on it.
		void Show_Page(int index);
		// The rows of the page at the index, with the values pending on it.
		std::vector<UISettingRow> Rows_Of(int index) const;

		UISettingsPageState State;

	private:
		void Refresh_Rows(void);

		UISettingsPageServiceClass & Service;
};


// The page as the declarations give it, with each row's value from the owner.
template<typename Owner>
UIDeclaredPage UI_Declared_Page(std::span<Setting<Owner> const> settings, SettingPage const & page, std::type_identity_t<Owner> const & owner)
{
	UIDeclaredPage declared;
	declared.Id = page.Id;
	declared.Label = page.Label;
	declared.Rows = UI_Setting_Rows(settings, page.Id, [&owner](Setting<Owner> const & setting) { return(Setting_Value(setting, owner)); });
	return(declared);
}


std::unique_ptr<UIViewClass> UI_Settings_Page_View(UISettingsPagePresenterClass & presenter);

UISettingsPageServiceClass & UI_Settings_Page_Service(void);
// The presenter of every declared page, with the settings as they stand now.
std::unique_ptr<UISettingsPagePresenterClass> UI_Settings_Page_New_Presenter(void);

// Shows the declared page with the id on its own, as the classic options menu opens it. OK applies its rows.
void UI_Settings_Page_Dialog(std::string const & id);
