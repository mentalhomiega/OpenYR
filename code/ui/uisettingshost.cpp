/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/uisettingshost.h"


UISettingsHostPresenterClass::UISettingsHostPresenterClass(std::vector<UISettingsPageClass> pages, std::vector<UIPresenterClass *> presenters, unsigned offered, UISettingsTab first) :
	Pages(std::move(pages)),
	Presenters(std::move(presenters)),
	Offered(offered)
{
	Offer_Tabs(offered);
	for (UIPresenterClass * presenter : Presenters) {
		presenter->Offer_Tabs(offered);
	}

	for (int index = 0; index < (int)Pages.size(); index++) {
		if (Pages[index].Tab == first) {
			Current = index;
		}
	}
	for (UISettingsPageClass const & page : Pages) {
		page.View->Set_Tab((int)Pages[Current].Tab, Offered);
	}
}


void UISettingsHostPresenterClass::Execute(UIIntent const & intent)
{
	(void)intent;
}


void UISettingsHostPresenterClass::Refresh(void)
{
	for (UIPresenterClass * presenter : Presenters) {
		presenter->Refresh();
	}
}


// Each page drains its own queue, so an intent typed on a page reaches that page's presenter. A tab request
// moves to the page at once; a page that ends the screen ends it for all of them.
void UISettingsHostPresenterClass::Drain(void)
{
	UIPresenterClass::Drain();
	for (UIPresenterClass * presenter : Presenters) {
		if (Result.has_value()) {
			break;
		}
		presenter->Drain();

		if (presenter->Tab_Request != UI_TAB_NONE) {
			UISettingsTab const tab = presenter->Tab_Request;
			presenter->Tab_Request = UI_TAB_NONE;
			Show_Tab(tab);
		}
		if (presenter->Result.has_value() && !Result.has_value()) {
			Finish(presenter);
		}
	}
}


void UISettingsHostPresenterClass::Enter_Current(void)
{
	if (Current >= 0 && Current < (int)Pages.size() && Pages[Current].Enter) {
		Pages[Current].Enter();
	}
}


void UISettingsHostPresenterClass::Show_Tab(UISettingsTab tab)
{
	int next = -1;
	for (int index = 0; index < (int)Pages.size(); index++) {
		if (Pages[index].Tab == tab) {
			next = index;
		}
	}
	if (next < 0 || next == Current) {
		return;
	}

	Pages[Current].View->Hide();
	Current = next;
	for (UISettingsPageClass const & page : Pages) {
		page.View->Set_Tab((int)tab, Offered);
	}
	Enter_Current();
	Pages[Current].View->Sync();
	Pages[Current].View->Show(true);
}


// The page that ended the screen has applied or dropped its own values already, so the others do the same.
void UISettingsHostPresenterClass::Finish(UIPresenterClass * source)
{
	bool const accepted = (*source->Result == UI_RESULT_ACCEPTED);
	UIIntent intent;
	intent.Name = accepted ? "ok" : "cancel";
	for (UIPresenterClass * presenter : Presenters) {
		if (presenter != source && !presenter->Result.has_value()) {
			presenter->Execute(intent);
		}
	}
	Result = source->Result;
}


UISettingsHostViewClass::UISettingsHostViewClass(UISettingsHostPresenterClass & presenter) :
	Host(presenter)
{
}


bool UISettingsHostViewClass::Prepare(UIShellClass & shell)
{
	for (UISettingsPageClass const & page : Host.Pages_Of_Host()) {
		if (!page.View->Prepare(shell)) {
			return(false);
		}
	}
	return(true);
}


void UISettingsHostViewClass::Show(bool modal)
{
	Host.Enter_Current();
	Shown_View().Show(modal);
}


void UISettingsHostViewClass::Hide(void)
{
	Shown_View().Hide();
}


void UISettingsHostViewClass::Release(void)
{
	for (UISettingsPageClass const & page : Host.Pages_Of_Host()) {
		page.View->Release();
	}
}


void UISettingsHostViewClass::Sync(void)
{
	Shown_View().Sync();
}


bool UISettingsHostViewClass::Is_Shown(void) const
{
	return(Shown_View().Is_Shown());
}


void UISettingsHostViewClass::Placed(void)
{
	Shown_View().Placed();
}


float UISettingsHostViewClass::Reveal_Width(void) const
{
	return(Shown_View().Reveal_Width());
}


void UISettingsHostViewClass::Reveal_To(float width)
{
	Shown_View().Reveal_To(width);
}


void UISettingsHostViewClass::Reveal_Done(void)
{
	Shown_View().Reveal_Done();
}


UIViewClass & UISettingsHostViewClass::Shown_View(void) const
{
	return(*Host.Pages_Of_Host()[Host.Current_Index()].View);
}
