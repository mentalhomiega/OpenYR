/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/cncnet/uicncnet.h"

#include "_deploymentconfig.h"
#include "_rules.h"
#include "ccfile.h"
#include "ccini.h"
#include "deploymentconfig.h"
#include "ui/rml/rmlview.h"

#include <algorithm>
#include <utility>


namespace
{

// The page's two choices are kept in their own section of the settings file.
char const * const CHOICE_SECTION = "CnCNet";


std::string Remembered(char const * key)
{
	return(ConfigINI.Get_String(CHOICE_SECTION, key, ""));
}


// Writes the install and the copy folder to the settings file. A failed write leaves the choice
// in effect for this open of the page only.
void Remember(std::string const & install, std::string const & destination)
{
	ConfigINI.Put_String(CHOICE_SECTION, "Install", install.c_str());
	ConfigINI.Put_String(CHOICE_SECTION, "Destination", destination.c_str());
	CCFileClass file(DeploymentConfig.SettingsFile.c_str());
	ConfigINI.Save(file, false);
}


std::string Megabytes(std::uint64_t bytes)
{
	return(std::to_string(bytes / (1024ull * 1024ull)));
}


}


UICNCNetSessionClass::UICNCNetSessionClass(std::string const & build, std::string const & documents) :
	Build(build),
	Documents(documents)
{
}


void UICNCNetSessionClass::Restore_Choices(void)
{
	std::string const destination = Remembered("Destination");
	Destination = destination.empty() ? CNCNet_Default_Destination(Build, Documents) : destination;

	std::string const install = Remembered("Install");
	if (CNCNet_Is_Install(install)) {
		Install = install;
		Message.clear();
		Problem = false;
		Refresh();
	} else {
		Find_Install();
	}
}


void UICNCNetSessionClass::Find_Install(void)
{
	std::vector<std::string> const installs = CNCNet_Find_Installs(Build);
	if (installs.empty()) {
		Install.clear();
		Message = "No CnCNet install was found in the registry, beside this build or in its parent folder.";
		Problem = true;
	} else {
		Install = installs.front();
		Message.clear();
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
	Message.clear();
	Problem = false;
	Remember_Choices();
	Refresh();
}


void UICNCNetSessionClass::Choose_Destination(std::string const & folder)
{
	Destination = folder;
	Message.clear();
	Problem = false;
	Remember_Choices();
	Refresh();
}


void UICNCNetSessionClass::Remember_Choices(void)
{
	Remember(Install, Destination);
}


void UICNCNetSessionClass::Refresh(void)
{
	Status = CNCNet_Status(Install, Destination, Build);
	Plan = Install.empty() ? CNCNetPlanType() : CNCNet_Plan(Install, Destination, Build);
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
	Remember_Choices();
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
			if (!Session.Install.empty()) {
				Session.Remember_Choices();
			}
		}

	} else if (intent.Name == "choose") {
		if (!busy) {
			Choice = UI_CNCNET_CHOOSE;
			Result = UI_RESULT_ACCEPTED;
		}

	} else if (intent.Name == "destination") {
		if (!busy) {
			Choice = UI_CNCNET_DESTINATION;
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
		State.InstallText = "CnCNet install: none chosen.";
	} else {
		State.InstallText = "CnCNet install: " + Session.Install;
	}

	if (status.Copy_Present) {
		State.CopyText = "OpenYR copy: " + status.Copy_Folder;
	} else if (Session.Destination.empty()) {
		State.CopyText = "OpenYR copy: none. Choose where it goes with Change destination.";
	} else {
		State.CopyText = "OpenYR copy: none yet. It will be made at " + Session.Destination + ".";
	}

	State.EditText.clear();
	State.VersionText.clear();
	if (status.Copy_Present) {
		State.EditText = status.Edits_Intact
			? std::string("Client edits: intact.")
			: std::string("Client edits: missing or changed, for example after a client update. Repair puts them back.");
		if (!status.Copy_Game_Present) {
			State.VersionText = "Game.exe: the copy has none. Repair copy restores it.";
		} else if (status.Engine_Matches) {
			State.VersionText = "Game.exe: the same as this build's.";
		} else {
			State.VersionText = "Game.exe: differs from this build's. Update copy replaces it.";
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
