/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/colorpick/uicolorpickview.h"

#include "ui/rml/rmlsurface.h"
#include "ui/screens/colorpick/uicolorpick.h"

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <string>


bool UI_Register_Color_Structs(Rml::DataModelConstructor & model)
{
	Rml::StructHandle<UIColorPresetOption> preset = model.RegisterStruct<UIColorPresetOption>();
	if (!preset) {
		return(false);
	}
	preset.RegisterMember("label", &UIColorPresetOption::Label);
	preset.RegisterMember("value", &UIColorPresetOption::Value);
	preset.RegisterMember("color", &UIColorPresetOption::Color);
	preset.RegisterMember("textcolor", &UIColorPresetOption::TextColor);
	preset.RegisterMember("taken", &UIColorPresetOption::Taken);
	preset.RegisterMember("caption", &UIColorPresetOption::Caption);
	Rml::StructHandle<UIColorRampStep> step = model.RegisterStruct<UIColorRampStep>();
	if (!step) {
		return(false);
	}
	step.RegisterMember("color", &UIColorRampStep::Color);
	return(true);
}


UIColorPickViewClass::UIColorPickViewClass(UIPresenterClass & presenter, char const * document, char const * model) :
	UIRmlViewClass(presenter, document, model)
{
}


void UIColorPickViewClass::Loaded(void)
{
	UIRmlViewClass::Loaded();
	if (Document() != nullptr) {
		Document()->AddEventListener(Rml::EventId::Mouseup, this);
	}
}


// Routes the color field's pointer input into the picker while a button is held on it.
void UIColorPickViewClass::ProcessEvent(Rml::Event & event)
{
	Rml::ElementDocument * document = Document();
	Rml::Element * field = document != nullptr ? document->GetElementById("colorfield") : nullptr;
	if (field != nullptr) {
		if (event.GetId() == Rml::EventId::Mouseup) {
			Dragging = false;
		} else if (event.GetId() == Rml::EventId::Mousedown && event.GetTargetElement() == field && event.GetParameter<int>("button", 0) == 0) {
			Dragging = true;
		}
		if (Dragging && (event.GetId() == Rml::EventId::Mousedown || event.GetId() == Rml::EventId::Mousemove)) {
			Queue_Field(field, event);
		}
	}
	UIRmlViewClass::ProcessEvent(event);
}


void UIColorPickViewClass::Queue_Field(Rml::Element * field, Rml::Event & event)
{
	Rml::Vector2f const origin = field->GetAbsoluteOffset(Rml::BoxArea::Border);
	Rml::Vector2f const size = field->GetBox().GetSize(Rml::BoxArea::Border);
	if (size.x <= 0.0f || size.y <= 0.0f) {
		return;
	}
	float const mouse_x = event.GetParameter<float>("mouse_x", 0.0f);
	float const mouse_y = event.GetParameter<float>("mouse_y", 0.0f);
	int const x = (int)((mouse_x - origin.x) * UI_COLOR_FIELD_SIZE / size.x);
	int const y = (int)((mouse_y - origin.y) * UI_COLOR_FIELD_SIZE / size.y);
	Queue("pickfield", x, std::to_string(y).c_str());
}


void UIColorPickViewClass::Show_Picker(UIColorPickState const & picker)
{
	Rml::Element * chip = Document() != nullptr ? Document()->GetElementById("picker-chip") : nullptr;
	if (chip != nullptr && picker.Swatch != ShownSwatch) {
		chip->SetProperty("background-color", picker.Swatch);
		ShownSwatch = picker.Swatch;
	}
	if (picker.Generation == PickerShown || Document() == nullptr) {
		return;
	}
	UIRmlSurfaceElementClass * surface = rmlui_dynamic_cast<UIRmlSurfaceElementClass *>(Document()->GetElementById("colorfield"));
	if (surface == nullptr) {
		return;
	}
	PickerShown = picker.Generation;
	surface->Set_Image(UI_COLOR_FIELD_SIZE, UI_COLOR_FIELD_SIZE, UI_Color_Pick_Pixels(picker));
}
