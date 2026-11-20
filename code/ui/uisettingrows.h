/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "settingregistry.h"

#include <cstring>
#include <functional>
#include <span>
#include <string>
#include <vector>


enum class UISettingRowKind
{
	HEADING,
	SWITCH,
	SLIDER,
	// A drop-down whose value is the position of the chosen item in Items.
	CHOICE,
	// A heading that opens and closes the rows under it: Value is 1 while they show. Nothing is stored.
	FOLD
};


// The rows a page draws by hand, which a preset may name by key as it names a declared row. Get returns false for a key
// the page does not have and otherwise reads the row's pending value; Set changes that value.
struct UISettingOutsideRows
{
	std::function<bool(std::string const & key, int & value)> Get;
	std::function<void(std::string const & key, int value)> Set;
};


// One row a settings page builds from a declaration. A page edits Value; nothing reaches the setting until OK.
struct UISettingRow
{
	UISettingRowKind Kind = UISettingRowKind::SWITCH;
	std::string Key;
	std::string Label;
	std::string Tip;
	std::string Needs;
	SettingCondition Condition = SettingCondition::NONE;
	int Value = 0;
	int Start = 0;
	int Default = 0;
	// The value that reads as off, and the one a switch picks when the page opened on off.
	int Off = 0;
	int SwitchOn = 1;
	int Min = 0;
	int Max = 0;
	std::string Unit;
	std::string ZeroText;
	std::vector<std::string> Items;
	// A locked row shows its value greyed and keeps it; Note says why, beside the control.
	bool Locked = false;
	std::string Note;
	// The label of the fold row this row sits under, or empty. The row is hidden while that fold is closed.
	std::string Fold;
	// Set on a preset row (a CHOICE): its Value is read from the rows it names, as UI_Setting_Rows_Sync_Presets does.
	SettingPresetChoice const * Preset = nullptr;
	// On a preset row, the page's hand-drawn rows that its presets may name.
	UISettingOutsideRows Outside;

	// Filled by UI_Setting_Rows_Refresh for the page's markup.
	std::string KindName;
	std::string Intent;
	std::string ElementId;
	std::string RowId;
	std::string Classes;
	std::string ValueText;
	bool On = false;
	bool Disabled = false;
	bool Hidden = false;
};


// Answers a SettingCondition for the page showing the rows.
using UISettingConditionCheck = std::function<bool(SettingCondition)>;

// The markup name of a key: GpuRender gives gpu-render.
std::string UI_Setting_Element_Id(std::string const & key);

// Fills a row's text, ids and greying from its value and the values of the rows it needs.
void UI_Setting_Rows_Refresh(std::vector<UISettingRow> & rows, UISettingConditionCheck const & met);

// Sets the row the intent names and returns true, or returns false when no row has that name. A greyed row keeps its value.
bool UI_Setting_Row_Execute(std::vector<UISettingRow> & rows, std::string const & intent, int value);

void UI_Setting_Rows_Reset(std::vector<UISettingRow> & rows);

// Sets each preset row to the first preset whose values every row holds, or to Custom (the last item) when none. With start,
// the value the page opened with is set too.
void UI_Setting_Rows_Sync_Presets(std::vector<UISettingRow> & rows, bool start = false);

// Gives every preset row the page's hand-drawn rows and reads each choice again, as the value the page opened with.
void UI_Setting_Rows_Set_Outside(std::vector<UISettingRow> & rows, UISettingOutsideRows const & outside);

// Returns whether any row differs from the value the page opened with. A fold and a preset row hold no setting and do not count.
bool UI_Setting_Rows_Changed(std::vector<UISettingRow> const & rows);


namespace uisettingrows_detail
{

template<typename Owner>
UISettingRow Row_Of(Setting<Owner> const & setting, int value)
{
	UISettingRow row;
	row.Kind = (setting.Flag != nullptr || setting.SwitchOn != 0) ? UISettingRowKind::SWITCH : UISettingRowKind::SLIDER;
	row.Key = setting.Key;
	row.Label = setting.Label;
	row.Tip = (setting.Tip != nullptr) ? setting.Tip : "";
	if (setting.NeedsRestart) {
		row.Tip += " Applies at the next start.";
	}
	row.Needs = (setting.Needs != nullptr) ? setting.Needs : "";
	row.Condition = setting.Condition;
	row.Fold = (setting.Fold != nullptr) ? setting.Fold : "";
	row.Value = value;
	row.Start = value;
	row.Default = setting.Default;
	row.Off = (setting.Flag != nullptr) ? 0 : setting.Off;
	row.SwitchOn = (setting.Flag != nullptr) ? 1 : setting.SwitchOn;
	row.Min = setting.Min;
	row.Max = setting.Max;
	row.Unit = setting.Unit;
	row.ZeroText = setting.ZeroText;
	return(row);
}


template<typename Owner>
bool On_Page(Setting<Owner> const & setting, char const * page)
{
	return(setting.Label != nullptr && setting.Page != nullptr && std::strcmp(setting.Page, page) == 0);
}


// Adds a group's heading and its rows.
template<typename Owner, typename ValueOf>
void Add_Group(std::vector<UISettingRow> & rows, std::span<Setting<Owner> const> settings, char const * page, char const * group, ValueOf value_of)
{
	UISettingRow heading;
	heading.Kind = UISettingRowKind::HEADING;
	heading.Label = group;
	for (Setting<Owner> const & member : settings) {
		if (On_Page(member, page) && std::strcmp(member.Group, group) == 0) {
			heading.Fold = (member.Fold != nullptr) ? member.Fold : "";
			break;
		}
	}
	rows.push_back(heading);

	for (Setting<Owner> const & member : settings) {
		if (On_Page(member, page) && std::strcmp(member.Group, group) == 0) {
			rows.push_back(Row_Of(member, value_of(member)));
		}
	}
}


// Adds a preset choice under its group heading.
inline void Add_Preset(std::vector<UISettingRow> & rows, SettingPresetChoice const & choice)
{
	UISettingRow heading;
	heading.Kind = UISettingRowKind::HEADING;
	heading.Label = choice.Group;
	rows.push_back(heading);

	UISettingRow row;
	row.Kind = UISettingRowKind::CHOICE;
	row.Key = choice.Key;
	row.Label = choice.Label;
	row.Tip = choice.Tip;
	row.Condition = choice.Condition;
	row.Preset = &choice;
	for (SettingPreset const & preset : choice.Presets) {
		row.Items.push_back(preset.Label);
	}
	row.Items.push_back("Custom");
	rows.push_back(row);
}

}


// Builds the rows of one page: its preset choices first, then each group in the order it first appears, under a heading
// row. The groups of one fold follow a fold row, which opens and closes them, at the first of them. value_of(setting)
// gives a row's value.
template<typename Owner, typename ValueOf>
std::vector<UISettingRow> UI_Setting_Rows(std::span<Setting<Owner> const> settings, char const * page, ValueOf value_of, std::span<SettingPresetChoice const> presets = {})
{
	std::vector<UISettingRow> rows;
	for (SettingPresetChoice const & choice : presets) {
		if (choice.Page != nullptr && std::strcmp(choice.Page, page) == 0) {
			uisettingrows_detail::Add_Preset(rows, choice);
		}
	}

	std::vector<std::string> added;
	for (Setting<Owner> const & setting : settings) {
		if (!uisettingrows_detail::On_Page(setting, page)) {
			continue;
		}
		std::string const name = std::string((setting.Fold != nullptr) ? "fold " : "group ") + ((setting.Fold != nullptr) ? setting.Fold : setting.Group);
		if (std::find(added.begin(), added.end(), name) != added.end()) {
			continue;
		}
		added.push_back(name);

		if (setting.Fold == nullptr) {
			uisettingrows_detail::Add_Group(rows, settings, page, setting.Group, value_of);
			continue;
		}

		UISettingRow fold;
		fold.Kind = UISettingRowKind::FOLD;
		fold.Key = setting.Fold;
		fold.Label = setting.Fold;
		fold.Tip = "Shows or hides the settings in this part of the page. Closing it keeps every change.";
		rows.push_back(fold);

		std::vector<std::string> groups;
		for (Setting<Owner> const & member : settings) {
			if (uisettingrows_detail::On_Page(member, page) && member.Fold != nullptr && std::strcmp(member.Fold, setting.Fold) == 0
				&& std::find(groups.begin(), groups.end(), member.Group) == groups.end()) {
				groups.push_back(member.Group);
				uisettingrows_detail::Add_Group(rows, settings, page, member.Group, value_of);
			}
		}
	}
	UI_Setting_Rows_Sync_Presets(rows, true);
	return(rows);
}


// Writes each row's value to the setting it was built from.
template<typename Owner>
void UI_Setting_Rows_Apply(std::span<Setting<Owner> const> settings, std::vector<UISettingRow> const & rows, std::type_identity_t<Owner> & owner)
{
	for (UISettingRow const & row : rows) {
		for (Setting<Owner> const & setting : settings) {
			if (row.Kind != UISettingRowKind::HEADING && row.Key == setting.Key) {
				Setting_Set(setting, owner, row.Value);
			}
		}
	}
}
