/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "cncnetcopy.h"
#include "ui/uiscreen.h"

#include <memory>
#include <string>
#include <vector>

class UIViewClass;


enum UICNCNetChoice
{
	UI_CNCNET_LEAVE,
	UI_CNCNET_CHOOSE,
	UI_CNCNET_START,
};


// The install, the copy and the copy's job, which live for as long as the page is open.
class UICNCNetSessionClass
{
	public:
		explicit UICNCNetSessionClass(std::string const & build);

		// Looks for an install in the registry, beside this build and in its parent folder.
		void Find_Install(void);

		// Uses a folder the player picked, when it is an install.
		void Choose_Install(std::string const & folder);

		// Reads the copy and the plan again. Do not call it while a job runs.
		void Refresh(void);

		// Starts the copy, or the update in place when a marked copy exists. False when it cannot start.
		bool Start_Copy(void);
		void Cancel_Copy(void);
		bool Start_Remove(void);

		// Turns a finished job into the status line and a fresh read of the copy.
		void Poll(void);

		bool Busy(void) const;

		std::string Build;
		std::string Install;
		CNCNetStatusType Status;
		CNCNetPlanType Plan;
		CNCNetJobClass Job;

		std::string Message;
		bool Problem = false;
		bool Confirming = false;
		bool Copying = false;

	private:
		CNCNetJobStateType Seen = CNCNetJobStateType::Idle;
};


struct UICNCNetRow
{
	std::string Text;
};


struct UICNCNetState
{
	std::string InstallText;
	std::string CopyText;
	std::string EditText;
	std::string VersionText;
	std::string Status;
	std::string ConfirmText;
	std::string ProgressText;
	std::string CreateCaption;
	std::vector<UICNCNetRow> Rows;

	bool Problem = false;
	bool Busy = false;
	bool ShowProgress = false;
	bool CanCreate = false;
	bool CanCancel = false;
	bool CanStart = false;
	bool CanRemove = false;
	bool Confirming = false;
	bool CopyPresent = false;
	int Percent = 0;
};


class UICNCNetPresenterClass : public UIPresenterClass
{
	public:
		explicit UICNCNetPresenterClass(UICNCNetSessionClass & session);
		virtual void Execute(UIIntent const & intent) override;
		virtual void Refresh(void) override;

		UICNCNetChoice Choice = UI_CNCNET_LEAVE;
		UICNCNetState State;

	private:
		void Show(void);

		UICNCNetSessionClass & Session;
};


std::unique_ptr<UIViewClass> UI_CNCNet_View(UICNCNetPresenterClass & presenter);

// Shows the CnCNet page until the player leaves it. True when the player started CnCNet, after which
// the game should end.
bool UI_CNCNet_Dialog(void);
