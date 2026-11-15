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

#include <memory>
#include <string>

class UIViewClass;


class UIWaitBoxPresenterClass : public UIPresenterClass
{
	public:
		UIWaitBoxPresenterClass(std::string text, bool bar);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;
		void Set_Fraction(double fraction);

		std::string Text;
		bool Bar;
		int Percent = 0;
		// The box stands alone over what the screen holds, with no menu backdrop or shade behind it.
		bool Clear = false;
};


std::unique_ptr<UIViewClass> UI_Wait_Box_View(UIWaitBoxPresenterClass & presenter);


class UIWaitBoxClass
{
	public:
		UIWaitBoxClass(void);
		~UIWaitBoxClass(void);

		bool Show(char const * text, bool bar = false, bool clear = false);
		void Set_Text(char const * text);
		void Set_Fraction(double fraction);
		void Set_Step(char const * text, double fraction);
		void Hide(void);
		bool Is_Shown(void) const;

	private:
		std::unique_ptr<UIWaitBoxPresenterClass> Presenter;
		std::unique_ptr<UIViewClass> View;
};
