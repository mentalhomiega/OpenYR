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
	SLIDER
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

	// Filled by UI_Setting_Rows_Refresh for the page's markup.
	std::string KindName;
	std::string Intent;
	std::string ElementId;
	std::string RowId;
	std::string Classes;
	std::string ValueText;
	bool On = false;
	bool Disabled = false;
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

// Returns whether any row differs from the value the page opened with.
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

}


// Builds the rows of one page: each group in the order it first appears, under a heading row. value_of(setting) gives a row's value.
template<typename Owner, typename ValueOf>
std::vector<UISettingRow> UI_Setting_Rows(std::span<Setting<Owner> const> settings, char const * page, ValueOf value_of)
{
	std::vector<UISettingRow> rows;
	std::vector<std::string> groups;
	for (Setting<Owner> const & setting : settings) {
		if (!uisettingrows_detail::On_Page(setting, page) || std::find(groups.begin(), groups.end(), setting.Group) != groups.end()) {
			continue;
		}
		groups.push_back(setting.Group);

		UISettingRow heading;
		heading.Kind = UISettingRowKind::HEADING;
		heading.Label = setting.Group;
		rows.push_back(heading);

		for (Setting<Owner> const & member : settings) {
			if (uisettingrows_detail::On_Page(member, page) && std::strcmp(member.Group, setting.Group) == 0) {
				rows.push_back(uisettingrows_detail::Row_Of(member, value_of(member)));
			}
		}
	}
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
