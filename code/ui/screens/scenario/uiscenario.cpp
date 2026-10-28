/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "ui/screens/scenario/uiscenario.h"

#include "ui/rml/rmlsurface.h"
#include "ui/rml/rmlview.h"

#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/Input.h>

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <sstream>
#include <utility>


namespace
{

std::string Lower(std::string text)
{
	for (char & c : text) {
		c = (char)std::tolower((unsigned char)c);
	}
	return(text);
}


/// Compares two names ignoring case, with each run of digits compared by its value, so "Map 2" comes before "Map 10".
int Natural_Compare(std::string const & left, std::string const & right)
{
	std::size_t a = 0;
	std::size_t b = 0;
	while (a < left.size() && b < right.size()) {
		unsigned char const x = (unsigned char)left[a];
		unsigned char const y = (unsigned char)right[b];
		if (std::isdigit(x) && std::isdigit(y)) {
			std::size_t end_a = a;
			std::size_t end_b = b;
			while (end_a < left.size() && std::isdigit((unsigned char)left[end_a])) {
				end_a++;
			}
			while (end_b < right.size() && std::isdigit((unsigned char)right[end_b])) {
				end_b++;
			}
			while (a + 1 < end_a && left[a] == '0') {
				a++;
			}
			while (b + 1 < end_b && right[b] == '0') {
				b++;
			}
			if (end_a - a != end_b - b) {
				return(end_a - a < end_b - b ? -1 : 1);
			}
			int const order = left.compare(a, end_a - a, right, b, end_b - b);
			if (order != 0) {
				return(order < 0 ? -1 : 1);
			}
			a = end_a;
			b = end_b;
			continue;
		}
		int const lx = std::tolower(x);
		int const ly = std::tolower(y);
		if (lx != ly) {
			return(lx < ly ? -1 : 1);
		}
		a++;
		b++;
	}
	if (a < left.size()) {
		return(1);
	}
	return(b < right.size() ? -1 : 0);
}


std::vector<std::string> Words_Of(std::string const & text)
{
	std::vector<std::string> words;
	std::istringstream fields(Lower(text));
	std::string word;
	while (fields >> word) {
		words.push_back(word);
	}
	return(words);
}


std::string Count_Text(int players)
{
	if (players <= 0) {
		return("?");
	}
	return(std::to_string(players) + (players == 1 ? " player" : " players"));
}

}


UIScenarioPresenterClass::UIScenarioPresenterClass(UIScenarioServiceClass & service, UIScenarioState state) :
	State(std::move(state)),
	Service(service)
{
	Rebuild();
}


void UIScenarioPresenterClass::Rebuild(void)
{
	int const before = State.Selected;

	std::set<int> found;
	for (UIScenarioEntry const & entry : State.Entries) {
		if (entry.Players > 0) {
			found.insert(entry.Players);
		}
	}
	State.Counts.clear();
	State.Counts.push_back(UIScenarioCount{"Any number", 0});
	for (int players : found) {
		State.Counts.push_back(UIScenarioCount{Count_Text(players), players});
	}
	if (State.Players != 0 && found.count(State.Players) == 0) {
		State.Players = 0;
	}

	// The map lists can name one file twice, such as an official map in two packets. A file is listed once, at its
	// first place. Two different files with one name are told apart by their file names.
	std::map<std::string, int> first_of_file;
	std::map<std::string, int> named;
	std::vector<int> listed(State.Entries.size(), -1);
	for (int index = 0; index < (int)State.Entries.size(); index++) {
		UIScenarioEntry const & entry = State.Entries[index];
		std::string const file = Lower(entry.File);
		std::map<std::string, int>::iterator const known = file.empty() ? first_of_file.end() : first_of_file.find(file);
		if (known == first_of_file.end()) {
			first_of_file[file.empty() ? std::to_string(index) + "#" : file] = index;
			named[Lower(entry.Label)]++;
			listed[index] = index;
		} else {
			listed[index] = known->second;
		}
	}
	if (State.Selected >= 0 && State.Selected < (int)State.Entries.size()) {
		State.Selected = listed[State.Selected];
	}

	// Every word of the search must appear in the map's name or in its file name.
	std::vector<std::string> const words = Words_Of(State.Filter);
	State.Rows.clear();
	for (int index = 0; index < (int)State.Entries.size(); index++) {
		UIScenarioEntry const & entry = State.Entries[index];
		if (listed[index] != index) {
			continue;
		}
		if (State.Players != 0 && entry.Players != State.Players) {
			continue;
		}
		std::string const label = Lower(entry.Label);
		std::string const file = Lower(entry.File);
		bool all = true;
		for (std::string const & word : words) {
			if (label.find(word) == std::string::npos && file.find(word) == std::string::npos) {
				all = false;
				break;
			}
		}
		if (all) {
			std::string text = entry.Label;
			if (named[Lower(entry.Label)] > 1) {
				std::size_t const dot = entry.File.rfind('.');
				text += " (" + entry.File.substr(0, dot == std::string::npos ? std::string::npos : dot) + ")";
			}
			State.Rows.push_back(UIScenarioRow{text, Count_Text(entry.Players), index});
		}
	}

	// Maps of unknown size come last when the list is ordered by players.
	bool const by_players = State.Order == UI_SCENARIO_ORDER_PLAYERS;
	std::stable_sort(State.Rows.begin(), State.Rows.end(), [&](UIScenarioRow const & left, UIScenarioRow const & right) {
		if (by_players) {
			int const a = State.Entries[left.Index].Players;
			int const b = State.Entries[right.Index].Players;
			if (a != b) {
				return(a <= 0 ? false : (b <= 0 ? true : a < b));
			}
		}
		int const order = Natural_Compare(left.Label, right.Label);
		if (order != 0) {
			return(order < 0);
		}
		return(Natural_Compare(State.Entries[left.Index].File, State.Entries[right.Index].File) < 0);
	});

	bool shown = false;
	for (UIScenarioRow const & row : State.Rows) {
		shown = shown || row.Index == State.Selected;
	}
	if (!shown && !State.Rows.empty()) {
		State.Selected = State.Rows.front().Index;
	}
	if (State.Selected != before) {
		Service.Preview(State.Selected, State.Preview);
	}

	std::size_t const maps = first_of_file.size();
	if (State.Rows.size() == maps) {
		State.Summary = std::to_string(maps) + (maps == 1 ? " map" : " maps");
	} else {
		State.Summary = "Showing " + std::to_string(State.Rows.size()) + " of " + std::to_string(maps) + " maps";
	}

	State.Detail.clear();
	State.File.clear();
	if (State.Selected >= 0 && State.Selected < (int)State.Entries.size()) {
		UIScenarioEntry const & entry = State.Entries[State.Selected];
		State.Detail = entry.Players > 0 ? "Players: " + std::to_string(entry.Players) : "Players: not known";
		State.File = entry.File;
	}

	State.Generation++;
}


void UIScenarioPresenterClass::Select(int index)
{
	if (index >= 0 && index < (int)State.Entries.size() && index != State.Selected) {
		State.Selected = index;
		Service.Preview(State.Selected, State.Preview);
		Rebuild();
	}
}


/// Moves the highlight by that many rows of the list on show. With no highlighted row on show, a step down takes
/// the first row and a step up the last.
void UIScenarioPresenterClass::Step(int rows)
{
	if (State.Rows.empty()) {
		return;
	}

	int at = -1;
	for (int row = 0; row < (int)State.Rows.size(); row++) {
		if (State.Rows[row].Index == State.Selected) {
			at = row;
		}
	}
	int const last = (int)State.Rows.size() - 1;
	int wanted = at < 0 ? (rows > 0 ? 0 : last) : at + rows;
	wanted = std::max(0, std::min(last, wanted));
	Select(State.Rows[wanted].Index);
}


void UIScenarioPresenterClass::Execute(UIIntent const & intent)
{
	if (intent.Name == "select") {
		Select(intent.Value);
	} else if (intent.Name == "step") {
		Step(intent.Value);
	} else if (intent.Name == "search") {
		// The text box has already written the bound text by the time this arrives.
		State.Filter = intent.Text;
		Rebuild();
	} else if (intent.Name == "players") {
		if (intent.Value != State.Players) {
			State.Players = intent.Value;
			Rebuild();
		}
	} else if (intent.Name == "order") {
		if (intent.Value != State.Order && (intent.Value == UI_SCENARIO_ORDER_NAME || intent.Value == UI_SCENARIO_ORDER_PLAYERS)) {
			State.Order = intent.Value;
			Rebuild();
		}
	} else if (intent.Name == "ok") {
		for (UIScenarioRow const & row : State.Rows) {
			if (row.Index == State.Selected) {
				Choice = UI_SCENARIO_ACCEPT;
				Result = UI_RESULT_ACCEPTED;
				break;
			}
		}
	} else if (intent.Name == "random") {
		int scenario = Service.Random();
		Service.Read(State);
		if (scenario >= 0 && scenario < (int)State.Entries.size()) {
			// The new map is shown even when the search or the player count would hide it.
			State.Filter.clear();
			State.Players = 0;
			State.Selected = scenario;
			Service.Preview(State.Selected, State.Preview);
		}
		Rebuild();
	} else if (intent.Name == "cancel") {
		Choice = UI_SCENARIO_CANCEL;
		Result = UI_RESULT_CANCELLED;
	}
}


void UIScenarioPresenterClass::Refresh(void)
{
}


namespace
{

class UIScenarioViewClass : public UIRmlViewClass
{
	public:
		explicit UIScenarioViewClass(UIScenarioPresenterClass & presenter) :
			UIRmlViewClass(presenter, "scenario.rml", "scenario"),
			Data(presenter),
			Shown(-1)
		{
		}

		virtual void Show(bool modal) override
		{
			UIRmlViewClass::Show(modal);
			Focus_Search();
		}

		virtual void Sync(void) override
		{
			if (Data.State.Generation != Seen) {
				Seen = Data.State.Generation;
				Model.DirtyAllVariables();
			}
			Show_Preview();
		}

	protected:
		virtual bool Bind(Rml::DataModelConstructor & model) override
		{
			Rml::StructHandle<UIScenarioRow> row = model.RegisterStruct<UIScenarioRow>();
			Rml::StructHandle<UIScenarioCount> count = model.RegisterStruct<UIScenarioCount>();
			if (!row || !count) {
				return(false);
			}
			row.RegisterMember("label", &UIScenarioRow::Label);
			row.RegisterMember("count", &UIScenarioRow::Count);
			row.RegisterMember("index", &UIScenarioRow::Index);
			count.RegisterMember("label", &UIScenarioCount::Label);
			count.RegisterMember("value", &UIScenarioCount::Value);

			UIScenarioState & state = Data.State;
			return(model.RegisterArray<std::vector<UIScenarioRow>>()
				&& model.RegisterArray<std::vector<UIScenarioCount>>()
				&& model.Bind("rows", &state.Rows)
				&& model.Bind("counts", &state.Counts)
				&& model.Bind("selected", &state.Selected)
				&& model.Bind("filter", &state.Filter)
				&& model.Bind("players", &state.Players)
				&& model.Bind("order", &state.Order)
				&& model.Bind("summary", &state.Summary)
				&& model.Bind("detail", &state.Detail)
				&& model.Bind("file", &state.File));
		}

		/// The search box takes the arrow and page keys for the list and Enter for OK, and a printable key
		/// pressed anywhere else on the screen goes into the search box.
		virtual void ProcessEvent(Rml::Event & event) override
		{
			Rml::Element * target = event.GetTargetElement();
			bool const in_search = target != nullptr && target->GetId() == "search";
			bool const in_field = target != nullptr && target->GetTagName() == "input";

			if (event.GetId() == Rml::EventId::Textinput && !in_field) {
				Rml::String const text = event.GetParameter<Rml::String>("text", Rml::String());
				if (!text.empty() && (unsigned char)text[0] >= ' ') {
					Type_Into_Search(text, false);
					event.StopPropagation();
					return;
				}
			}

			if (event.GetId() == Rml::EventId::Keydown) {
				int const key = event.GetParameter<int>("key_identifier", 0);
				int step = 0;
				switch (key) {
					case Rml::Input::KI_UP: step = -1; break;
					case Rml::Input::KI_DOWN: step = 1; break;
					case Rml::Input::KI_PRIOR: step = -Page_Rows(); break;
					case Rml::Input::KI_NEXT: step = Page_Rows(); break;
					default: break;
				}
				if (in_search && step != 0) {
					Queue("step", step);
					event.StopPropagation();
					return;
				}
				if (in_search && (key == Rml::Input::KI_RETURN || key == Rml::Input::KI_NUMPADENTER)) {
					Queue("ok");
					event.StopPropagation();
					return;
				}
				if (key == Rml::Input::KI_BACK && !in_field && !Data.State.Filter.empty()) {
					Type_Into_Search(Rml::String(), true);
					event.StopPropagation();
					return;
				}
			}

			UIRmlViewClass::ProcessEvent(event);
		}

	private:
		void Focus_Search(void)
		{
			if (Document() != nullptr) {
				if (Rml::Element * search = Document()->GetElementById("search")) {
					search->Focus();
				}
			}
		}

		/// Adds text to the search box, or takes its last character off, as typing in the box would.
		void Type_Into_Search(Rml::String const & text, bool erase)
		{
			Rml::Element * search = Document() != nullptr ? Document()->GetElementById("search") : nullptr;
			if (search == nullptr) {
				return;
			}

			Rml::String value = Data.State.Filter;
			if (erase) {
				while (!value.empty() && ((unsigned char)value.back() & 0xC0) == 0x80) {
					value.pop_back();
				}
				if (!value.empty()) {
					value.pop_back();
				}
			} else if (value.size() + text.size() <= (std::size_t)search->GetAttribute<int>("maxlength", 40)) {
				value += text;
			}

			search->SetAttribute("value", value);
			Rml::Dictionary parameters;
			parameters["value"] = Rml::Variant(value);
			search->DispatchEvent(Rml::EventId::Change, parameters);
			search->Focus();
		}

		/// How many rows a page key moves: the rows the list shows at once, less one.
		int Page_Rows(void) const
		{
			Rml::Element * list = Document() != nullptr ? Document()->GetElementById("maps") : nullptr;
			if (list == nullptr || list->GetNumChildren() == 0 || list->GetChild(0)->GetNumChildren() == 0) {
				return(8);
			}
			float const height = list->GetChild(0)->GetChild(0)->GetBox().GetSize(Rml::BoxArea::Border).y;
			int const rows = height > 0.0f ? (int)(list->GetClientHeight() / height) - 1 : 8;
			return(rows < 1 ? 1 : rows);
		}

		void Show_Preview(void)
		{
			UIMapPreviewImage & preview = Data.State.Preview;
			if (preview.Generation == Shown || Document() == nullptr) {
				return;
			}

			UIRmlSurfaceElementClass * surface = rmlui_dynamic_cast<UIRmlSurfaceElementClass *>(Document()->GetElementById("preview"));
			if (surface == nullptr) {
				return;
			}

			Shown = preview.Generation;
			surface->Set_Image(preview.Width, preview.Height, preview.Pixels);
		}

		UIScenarioPresenterClass & Data;
		int Shown;
		unsigned Seen = ~0u;
};

}


std::unique_ptr<UIViewClass> UI_Scenario_View(UIScenarioPresenterClass & presenter)
{
	return(std::make_unique<UIScenarioViewClass>(presenter));
}
