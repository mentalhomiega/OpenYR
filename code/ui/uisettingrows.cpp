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
#include <utility>


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


UISettingRow * Row_Named(std::vector<UISettingRow> & rows, std::string const & key)
{
	return(const_cast<UISettingRow *>(Row_Named(std::as_const(rows), key)));
}


// The fold row that has the label, or null.
UISettingRow const * Fold_Named(std::vector<UISettingRow> const & rows, std::string const & label)
{
	for (UISettingRow const & row : rows) {
		if (row.Kind == UISettingRowKind::FOLD && row.Label == label) {
			return(&row);
		}
	}
	return(nullptr);
}


// The position of the first preset whose values every row holds, or one past the last when none is.
int Preset_Index(std::vector<UISettingRow> const & rows, SettingPresetChoice const & choice)
{
	int index = 0;
	for (SettingPreset const & preset : choice.Presets) {
		bool all = true;
		for (SettingPresetValue const & value : preset.Values) {
			UISettingRow const * row = Row_Named(rows, value.Key);
			all = all && row != nullptr && row->Value == value.Value;
		}
		if (all) {
			return(index);
		}
		index++;
	}
	return(index);
}


// A chain longer than the table can only be a declaration that needs itself, which reads as greyed.
bool Row_Enabled(std::vector<UISettingRow> const & rows, UISettingRow const & row, UISettingConditionCheck const & met, std::size_t depth = 0)
{
	if (row.Locked || depth > rows.size() || (row.Condition != SettingCondition::NONE && !met(row.Condition))) {
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
		case UISettingRowKind::CHOICE:
			return("choice");
		case UISettingRowKind::FOLD:
			return("fold");
		default:
			return("switch");
	}
}


std::string Row_Classes(UISettingRow const & row)
{
	std::string classes;
	switch (row.Kind) {
		case UISettingRowKind::HEADING:
			classes = "section";
			break;
		case UISettingRowKind::SLIDER:
			classes = row.Disabled ? "setting slider-row disabled" : "setting slider-row";
			break;
		case UISettingRowKind::CHOICE:
			classes = row.Disabled ? "setting choice-row disabled" : "setting choice-row";
			break;
		case UISettingRowKind::FOLD:
			classes = "fold-row";
			break;
		default:
			classes = row.Disabled ? "check setting disabled" : "check setting";
			break;
	}
	return(row.Hidden ? classes + " folded" : classes);
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


void UI_Setting_Rows_Sync_Presets(std::vector<UISettingRow> & rows, bool start)
{
	for (UISettingRow & row : rows) {
		if (row.Preset != nullptr) {
			row.Value = Preset_Index(rows, *row.Preset);
			if (start) {
				row.Start = row.Value;
			}
		}
	}
}


void UI_Setting_Rows_Refresh(std::vector<UISettingRow> & rows, UISettingConditionCheck const & met)
{
	UI_Setting_Rows_Sync_Presets(rows);
	for (UISettingRow & row : rows) {
		row.KindName = Kind_Name(row.Kind);
		row.On = (row.Value != row.Off);
		row.Disabled = (row.Kind != UISettingRowKind::HEADING && row.Kind != UISettingRowKind::FOLD) && !Row_Enabled(rows, row, met);
		UISettingRow const * fold = row.Fold.empty() ? nullptr : Fold_Named(rows, row.Fold);
		row.Hidden = (fold != nullptr && fold->Value == 0);
		row.Classes = Row_Classes(row);
		if (row.Kind == UISettingRowKind::HEADING) {
			continue;
		}
		row.Intent = Lower_Text(row.Key);
		row.ElementId = UI_Setting_Element_Id(row.Key);
		row.RowId = (row.Kind == UISettingRowKind::SWITCH) ? "switch-" + row.ElementId : row.ElementId + "-row";
		if (row.Kind == UISettingRowKind::FOLD) {
			row.ValueText = std::string(row.On ? "[-] Hide " : "[+] Show ") + row.Label;
		} else if (row.Kind == UISettingRowKind::CHOICE) {
			row.ValueText = Index_In_Range(row.Value, row.Items.size()) ? row.Items[row.Value] : std::string();
		} else {
			row.ValueText = (row.Value == 0 && !row.ZeroText.empty()) ? row.ZeroText : std::to_string(row.Value) + row.Unit;
		}
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
		if (row.Kind == UISettingRowKind::FOLD) {
			row.Value = (value != 0) ? 1 : 0;
		} else if (row.Preset != nullptr) {
			// A position past the presets is Custom, which changes no row. The choice then reads from the rows.
			if (value >= 0 && value < (int)row.Preset->Presets.size()) {
				for (SettingPresetValue const & preset_value : row.Preset->Presets[value].Values) {
					UISettingRow * target = Row_Named(rows, preset_value.Key);
					if (target != nullptr) {
						target->Value = preset_value.Value;
					}
				}
			}
			UI_Setting_Rows_Sync_Presets(rows);
		} else if (row.Kind == UISettingRowKind::SLIDER) {
			row.Value = (row.Min < row.Max) ? std::clamp(value, row.Min, row.Max) : value;
		} else if (row.Kind == UISettingRowKind::CHOICE) {
			row.Value = row.Items.empty() ? 0 : std::clamp(value, 0, (int)row.Items.size() - 1);
		} else if (value == 0) {
			row.Value = row.Off;
		} else {
			row.Value = (row.Start != row.Off) ? row.Start : row.SwitchOn;
		}
		UI_Setting_Rows_Sync_Presets(rows);
		return(true);
	}
	return(false);
}


void UI_Setting_Rows_Reset(std::vector<UISettingRow> & rows)
{
	for (UISettingRow & row : rows) {
		if (row.Kind != UISettingRowKind::FOLD) {
			row.Value = row.Default;
		}
	}
	UI_Setting_Rows_Sync_Presets(rows);
}


bool UI_Setting_Rows_Changed(std::vector<UISettingRow> const & rows)
{
	return(std::any_of(rows.begin(), rows.end(), [](UISettingRow const & row) {
		return(row.Kind != UISettingRowKind::FOLD && row.Preset == nullptr && row.Value != row.Start);
	}));
}
