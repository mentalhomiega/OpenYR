/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_ui.h"
#include "ui/screens/waitbox/uiwaitbox.h"
#include "ui/uishell.h"
#include "ui/uiview.h"


UIWaitBoxClass::UIWaitBoxClass(void) = default;


UIWaitBoxClass::~UIWaitBoxClass(void)
{
	Hide();
}


bool UIWaitBoxClass::Show(char const * text, bool bar, bool clear)
{
	Hide();

	Presenter = std::make_unique<UIWaitBoxPresenterClass>((text != NULL) ? text : "", bar);
	Presenter->Clear = clear;
	View = UI_Wait_Box_View(*Presenter);

	if (!UIShell.Show_Modeless(*View)) {
		View.reset();
		Presenter.reset();
		return(false);
	}
	return(true);
}


void UIWaitBoxClass::Set_Text(char const * text)
{
	if (View != nullptr) {
		Presenter->Text = (text != NULL) ? text : "";
		View->Sync();
		UIShell.Refresh();
	}
}


void UIWaitBoxClass::Set_Fraction(double fraction)
{
	if (View != nullptr) {
		Presenter->Set_Fraction(fraction);
		View->Sync();
		UIShell.Refresh();
	}
}


void UIWaitBoxClass::Set_Step(char const * text, double fraction)
{
	if (View != nullptr) {
		Presenter->Text = (text != NULL) ? text : "";
		Presenter->Set_Fraction(fraction);
		View->Sync();
		UIShell.Refresh();
	}
}


void UIWaitBoxClass::Hide(void)
{
	if (View != nullptr) {
		UIShell.Hide_Modeless(*View);
		View.reset();
		Presenter.reset();
	}
}


bool UIWaitBoxClass::Is_Shown(void) const
{
	return(View != nullptr);
}
