/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/uisettingrows.h"

#include "ui/uiclamp.h"

#include <algorithm>
#include <cctype>


namespace
{

UISettingRow const * Row_Named(std::vector<UISettingRow> const & rows, std::string const & key)
{
	for (UISettingRow const & row : rows) {
		if (row.Kind != UISettingRowKind::HEADING && row.Key == key) {
			return(&row);
		}
	}
	return(nullptr);
}


// A chain longer than the table can only be a declaration that needs itself, which reads as greyed.
bool Row_Enabled(std::vector<UISettingRow> const & rows, UISettingRow const & row, UISettingConditionCheck const & met, std::size_t depth = 0)
{
	if (depth > rows.size() || (row.Condition != SettingCondition::NONE && !met(row.Condition))) {
		return(false);
	}
	if (row.Needs.empty()) {
		return(true);
	}
	UISettingRow const * needed = Row_Named(rows, row.Needs);
	return(needed != nullptr && needed->Value != needed->Off && Row_Enabled(rows, *needed, met, depth + 1));
}


std::string Lower_Text(std::string text)
{
	std::transform(text.begin(), text.end(), text.begin(), [](unsigned char letter) { return((char)std::tolower(letter)); });
	return(text);
}


char const * Kind_Name(UISettingRowKind kind)
{
	switch (kind) {
		case UISettingRowKind::HEADING:
			return("heading");
		case UISettingRowKind::SLIDER:
			return("slider");
		default:
			return("switch");
	}
}


std::string Row_Classes(UISettingRow const & row)
{
	switch (row.Kind) {
		case UISettingRowKind::HEADING:
			return("section");
		case UISettingRowKind::SLIDER:
			return(row.Disabled ? "setting slider-row disabled" : "setting slider-row");
		default:
			return(row.Disabled ? "check setting disabled" : "check setting");
	}
}

}


std::string UI_Setting_Element_Id(std::string const & key)
{
	std::string id;
	for (std::size_t index = 0; index < key.size(); index++) {
		if (index > 0 && std::isupper((unsigned char)key[index])) {
			id += '-';
		}
		id += (char)std::tolower((unsigned char)key[index]);
	}
	return(id);
}


void UI_Setting_Rows_Refresh(std::vector<UISettingRow> & rows, UISettingConditionCheck const & met)
{
	for (UISettingRow & row : rows) {
		row.KindName = Kind_Name(row.Kind);
		row.On = (row.Value != row.Off);
		row.Disabled = (row.Kind != UISettingRowKind::HEADING) && !Row_Enabled(rows, row, met);
		row.Classes = Row_Classes(row);
		if (row.Kind == UISettingRowKind::HEADING) {
			continue;
		}
		row.Intent = Lower_Text(row.Key);
		row.ElementId = UI_Setting_Element_Id(row.Key);
		row.RowId = (row.Kind == UISettingRowKind::SWITCH) ? "switch-" + row.ElementId : row.ElementId + "-row";
		row.ValueText = (row.Value == 0 && !row.ZeroText.empty()) ? row.ZeroText : std::to_string(row.Value) + row.Unit;
	}
}


bool UI_Setting_Row_Execute(std::vector<UISettingRow> & rows, std::string const & intent, int value)
{
	for (UISettingRow & row : rows) {
		if (row.Kind == UISettingRowKind::HEADING || row.Intent != intent) {
			continue;
		}
		if (row.Disabled) {
			return(true);
		}
		if (row.Kind == UISettingRowKind::SLIDER) {
			row.Value = (row.Min < row.Max) ? std::clamp(value, row.Min, row.Max) : value;
		} else if (value == 0) {
			row.Value = row.Off;
		} else {
			row.Value = (row.Start != row.Off) ? row.Start : row.SwitchOn;
		}
		return(true);
	}
	return(false);
}


void UI_Setting_Rows_Reset(std::vector<UISettingRow> & rows)
{
	for (UISettingRow & row : rows) {
		row.Value = row.Default;
	}
}


bool UI_Setting_Rows_Changed(std::vector<UISettingRow> const & rows)
{
	return(std::any_of(rows.begin(), rows.end(), [](UISettingRow const & row) { return(row.Value != row.Start); }));
}
