/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/cncnet/uicncnet.h"

#include "ui/rml/rmlview.h"

#include <algorithm>
#include <cstdio>
#include <utility>


namespace
{

std::string Megabytes(std::uint64_t bytes)
{
	return(std::to_string(bytes / (1024ull * 1024ull)));
}


std::string Hash_Text(std::uint64_t hash)
{
	char text[24];
	std::snprintf(text, sizeof(text), "%016llx", static_cast<unsigned long long>(hash));
	return(std::string(text));
}

}


UICNCNetSessionClass::UICNCNetSessionClass(std::string const & build) :
	Build(build)
{
}


void UICNCNetSessionClass::Find_Install(void)
{
	std::vector<std::string> const installs = CNCNet_Find_Installs(Build);
	if (installs.empty()) {
		Install.clear();
		Message = "No CnCNet install was found in the registry, beside this build or in its parent folder. Choose its folder.";
		Problem = true;
	} else {
		Install = installs.front();
		Message = "Found the CnCNet install at " + Install + ".";
		Problem = false;
	}
	Refresh();
}


void UICNCNetSessionClass::Choose_Install(std::string const & folder)
{
	if (!CNCNet_Is_Install(folder)) {
		Message = "That folder is not a CnCNet Yuri's Revenge install. It needs Resources\\ClientDefinitions.ini and CnCNetYRLauncher.exe.";
		Problem = true;
		return;
	}
	Install = folder;
	Message = "Using the CnCNet install at " + Install + ".";
	Problem = false;
	Refresh();
}


void UICNCNetSessionClass::Refresh(void)
{
	Status = CNCNet_Status(Install, Build);
	Plan = Install.empty() ? CNCNetPlanType() : CNCNet_Plan(Install, Build);
}


bool UICNCNetSessionClass::Start_Copy(void)
{
	if (Busy()) {
		return(false);
	}
	if (Install.empty() || !Plan.Problem.empty()) {
		Message = Install.empty() ? std::string("Choose the CnCNet install first.") : Plan.Problem;
		Problem = true;
		return(false);
	}
	if (!Job.Start_Copy(Plan, Build)) {
		Message = "The copy could not start.";
		Problem = true;
		return(false);
	}

	Copying = true;
	Confirming = false;
	Message = Plan.Update ? "Updating the copy in place." : "Copying the install. Cancel stops the copy and removes what it made.";
	Problem = false;
	return(true);
}


void UICNCNetSessionClass::Cancel_Copy(void)
{
	if (Busy() && Copying) {
		Job.Cancel();
		Message = "Stopping the copy.";
	}
}


bool UICNCNetSessionClass::Start_Remove(void)
{
	if (Busy() || !Status.Copy_Present) {
		return(false);
	}
	if (!Job.Start_Remove(Status.Copy_Folder, Build)) {
		Message = "This copy cannot be removed from here. Its folder has no marker, or this build runs from it.";
		Problem = true;
		return(false);
	}

	Copying = false;
	Confirming = false;
	Message = "Removing the copy.";
	Problem = false;
	return(true);
}


void UICNCNetSessionClass::Poll(void)
{
	CNCNetJobStateType const now = Job.State();
	if (now == Seen) {
		return;
	}
	Seen = now;
	if (now == CNCNetJobStateType::Idle || now == CNCNetJobStateType::Running) {
		return;
	}

	Copying = false;
	Refresh();
	Message = Job.Message();
	Problem = (now != CNCNetJobStateType::Done);
}


bool UICNCNetSessionClass::Busy(void) const
{
	return(Job.State() == CNCNetJobStateType::Running);
}


UICNCNetPresenterClass::UICNCNetPresenterClass(UICNCNetSessionClass & session) :
	Session(session)
{
	Show();
}


void UICNCNetPresenterClass::Execute(UIIntent const & intent)
{
	bool const busy = Session.Busy();
	bool const copy = Session.Status.Copy_Present;

	if (intent.Name == "find") {
		if (!busy) {
			Session.Find_Install();
		}

	} else if (intent.Name == "choose") {
		if (!busy) {
			Choice = UI_CNCNET_CHOOSE;
			Result = UI_RESULT_ACCEPTED;
		}

	} else if (intent.Name == "create") {
		Session.Start_Copy();

	} else if (intent.Name == "cancelcopy") {
		Session.Cancel_Copy();

	} else if (intent.Name == "remove") {
		if (!busy && copy) {
			Session.Confirming = true;
		}

	} else if (intent.Name == "keep") {
		Session.Confirming = false;

	} else if (intent.Name == "removeyes") {
		Session.Start_Remove();

	} else if (intent.Name == "start") {
		if (!busy && copy && Session.Status.Edits_Intact && Session.Status.Copy_Game_Present) {
			Choice = UI_CNCNET_START;
			Result = UI_RESULT_ACCEPTED;
		}

	} else if (intent.Name == "ok") {
		if (!busy) {
			Choice = UI_CNCNET_LEAVE;
			Result = UI_RESULT_ACCEPTED;
		}

	} else if (intent.Name == "cancel") {
		if (busy) {
			Session.Cancel_Copy();
		} else {
			Choice = UI_CNCNET_LEAVE;
			Result = UI_RESULT_CANCELLED;
		}
	}

	Show();
}


void UICNCNetPresenterClass::Refresh(void)
{
	Session.Poll();
	Show();
}


void UICNCNetPresenterClass::Show(void)
{
	CNCNetStatusType const & status = Session.Status;
	CNCNetPlanType const & plan = Session.Plan;
	bool const busy = Session.Busy();

	if (Session.Install.empty()) {
		State.InstallText = "No CnCNet install is chosen. Find install looks in the registry, beside this build and in its parent folder.";
	} else {
		State.InstallText = "CnCNet install: " + Session.Install;
	}

	if (status.Copy_Present) {
		State.CopyText = "OpenYR copy: " + status.Copy_Folder;
	} else if (!Session.Install.empty()) {
		State.CopyText = "OpenYR copy: none yet. It will be made at " + CNCNet_Copy_Folder_For(Session.Install) + ".";
	} else {
		State.CopyText = "OpenYR copy: none found.";
	}

	State.EditText.clear();
	State.VersionText.clear();
	if (status.Copy_Present) {
		State.EditText = status.Edits_Intact
			? std::string("Client edits: intact.")
			: std::string("Client edits: missing or changed, for example after a client update. Repair puts them back.");
		if (!status.Copy_Game_Present) {
			State.VersionText = "Game.exe: the copy has none. Repair restores it.";
		} else if (status.Engine_Matches) {
			State.VersionText = "Game.exe: the copy's is this build's (" + std::to_string(status.Build_Game_Size) + " bytes, hash " + Hash_Text(status.Build_Game_Hash) + ").";
		} else {
			State.VersionText = "Game.exe: the copy's differs from this build's (" + std::to_string(status.Copy_Game_Size) + " bytes against " + std::to_string(status.Build_Game_Size) + "). Update the copy to match.";
		}
	}

	if (!status.Copy_Present) {
		State.CreateCaption = "Create copy";
	} else if (!status.Edits_Intact) {
		State.CreateCaption = "Repair copy";
	} else {
		State.CreateCaption = "Update copy";
	}

	State.Rows.clear();
	if (Session.Install.empty()) {
		State.Rows.push_back(UICNCNetRow{"Choose the install folder to see what the copy will change."});
	} else if (!plan.Problem.empty()) {
		State.Rows.push_back(UICNCNetRow{plan.Problem});
	} else {
		for (std::string const & line : plan.Lines) {
			State.Rows.push_back(UICNCNetRow{line});
		}
	}

	State.Status = Session.Message;
	State.Problem = Session.Problem;
	State.ConfirmText = "Remove the OpenYR copy at " + status.Copy_Folder + "? Only that folder is deleted. The install is not changed.";

	std::uint64_t const total = Session.Job.Total();
	std::uint64_t const done = Session.Job.Done();
	State.Percent = (total > 0) ? static_cast<int>(std::min<std::uint64_t>(100, done * 100 / total)) : 0;
	State.ProgressText = Session.Copying ? "Copying " + Megabytes(done) + " of " + Megabytes(total) + " MB" : std::string("Working.");

	State.Busy = busy;
	State.ShowProgress = busy;
	State.CanCancel = busy && Session.Copying;
	State.CopyPresent = status.Copy_Present;
	State.CanCreate = !busy && !Session.Install.empty() && plan.Problem.empty();
	State.CanStart = !busy && status.Copy_Present && status.Edits_Intact && status.Copy_Game_Present;
	State.CanRemove = !busy && status.Copy_Present;
	State.Confirming = Session.Confirming && !busy && status.Copy_Present;
}


namespace
{

class UICNCNetViewClass : public UIRmlViewClass
{
	public:
		explicit UICNCNetViewClass(UICNCNetPresenterClass & presenter) :
			UIRmlViewClass(presenter, "cncnet.rml", "cncnet"),
			Data(presenter)
		{
		}

		virtual void Sync(void) override
		{
			Model.DirtyAllVariables();
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UICNCNetRow> row = model.RegisterStruct<UICNCNetRow>();
			if (!row) {
				return(false);
			}
			row.RegisterMember("text", &UICNCNetRow::Text);

			UICNCNetState & state = Data.State;
			return(model.RegisterArray<std::vector<UICNCNetRow>>()
				&& model.Bind("installtext", &state.InstallText)
				&& model.Bind("copytext", &state.CopyText)
				&& model.Bind("edittext", &state.EditText)
				&& model.Bind("versiontext", &state.VersionText)
				&& model.Bind("status", &state.Status)
				&& model.Bind("confirmtext", &state.ConfirmText)
				&& model.Bind("progresstext", &state.ProgressText)
				&& model.Bind("createcaption", &state.CreateCaption)
				&& model.Bind("rows", &state.Rows)
				&& model.Bind("problem", &state.Problem)
				&& model.Bind("busy", &state.Busy)
				&& model.Bind("showprogress", &state.ShowProgress)
				&& model.Bind("cancreate", &state.CanCreate)
				&& model.Bind("cancancel", &state.CanCancel)
				&& model.Bind("canstart", &state.CanStart)
				&& model.Bind("canremove", &state.CanRemove)
				&& model.Bind("confirming", &state.Confirming)
				&& model.Bind("copypresent", &state.CopyPresent)
				&& model.Bind("percent", &state.Percent));
		}

	private:
		UICNCNetPresenterClass & Data;
};

}


std::unique_ptr<UIViewClass> UI_CNCNet_View(UICNCNetPresenterClass & presenter)
{
	return(std::make_unique<UICNCNetViewClass>(presenter));
}
