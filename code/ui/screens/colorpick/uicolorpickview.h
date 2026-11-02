/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/
#pragma once

#include "ui/rml/rmlview.h"

#include <string>

namespace Rml
{
	class Element;
}

struct UIColorPickState;

// The view of a screen that offers the free color picker. It routes the color field's pointer input into the
// picker and shows the picker's swatch and field.
class UIColorPickViewClass : public UIRmlViewClass
{
	public:
		UIColorPickViewClass(UIPresenterClass & presenter, char const * document, char const * model);

	protected:
		virtual void Loaded(void) override;
		virtual void ProcessEvent(Rml::Event & event) override;

		// Shows the picker's swatch, and its color field when the picker has a new generation since the last call.
		void Show_Picker(UIColorPickState const & picker);

	private:
		void Queue_Field(Rml::Element * field, Rml::Event & event);

		int PickerShown = -1;
		std::string ShownSwatch;
		bool Dragging = false;
};
