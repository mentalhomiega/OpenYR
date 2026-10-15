/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "numparse.h"
#include "ui/rml/rmlview.h"

#include "ui/uishell.h"

// windowsx.h defines macros with the names of these RmlUi Element methods.
#undef GetFirstChild
#undef GetNextSibling

#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/ElementScroll.h>
#include <RmlUi/Core/ElementText.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/Factory.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/ID.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/Log.h>
#include <RmlUi/Core/Property.h>
#include <RmlUi/Core/StyleSheetContainer.h>
#include <RmlUi/Core/Variant.h>
#include <cmath>
#include <cstring>


UIRmlViewClass::UIRmlViewClass(UIPresenterClass & presenter, char const * document, char const * model) :
	Owner(presenter),
	DocumentName(document),
	ModelName(model)
{
}


UIRmlViewClass::~UIRmlViewClass(void)
{
	Release();
}


bool UIRmlViewClass::Prepare(Rml::Context & context)
{
	numparse_detail::Nearest_Rounding_Scope const nearest;
	Release();
	Host = &context;
	Types = Rml::MakeUnique<Rml::DataTypeRegister>();

	Rml::DataModelConstructor constructor = context.CreateDataModel(ModelName, Types.get());
	if (!constructor) {
		Rml::Log::Message(Rml::Log::LT_ERROR, "The data model %s for %s could not be created.", ModelName.c_str(), DocumentName.c_str());
		Release();
		return(false);
	}
	ModelCreated = true;

	bool bound = Bind(constructor);
	bound = constructor.BindEventCallback("queue", [this](Rml::DataModelHandle, Rml::Event &, Rml::VariantList const & arguments) {
		if (!arguments.empty()) {
			Rml::String const name = arguments[0].Get<Rml::String>();
			Rml::String text;
			int value = 0;
			if (arguments.size() > 1) {
				value = arguments[1].Get<int>();
				text = arguments[1].Get<Rml::String>();
			}

			if (arguments.size() > 2) {
				text = arguments[2].Get<Rml::String>();
			}

			Queue(name.c_str(), value, text.c_str());
		}
	}) && bound;
	Model = constructor.GetModelHandle();

	if (!bound) {
		Rml::Log::Message(Rml::Log::LT_ERROR, "The data model %s for %s could not be bound.", ModelName.c_str(), DocumentName.c_str());
		Release();
		return(false);
	}

	Doc = context.LoadDocument(DocumentName);
	if (Doc == nullptr) {
		Rml::Log::Message(Rml::Log::LT_ERROR, "%s did not load.", DocumentName.c_str());
		Release();
		return(false);
	}

	Doc->AddEventListener(Rml::EventId::Keydown, this, true);
	Doc->AddEventListener(Rml::EventId::Textinput, this, true);
	Doc->AddEventListener(Rml::EventId::Mousedown, this);
	Doc->AddEventListener(Rml::EventId::Mousemove, this, true);
	Doc->AddEventListener(Rml::EventId::Focus, this, true);
	Loaded();
	Apply_Tabs();
	return(true);
}


static char const * const UI_TAB_NAMES[UI_TAB_COUNT] = {"game", "display", "graphics", "audio", "keyboard", "accessibility", "mods"};

// A tab chosen from the keyboard keeps the keyboard on the tab bar for the next page.
static bool UI_Tab_Keyboard = false;


/// <summary>
/// Marks the document as a page of the tabbed Settings: it styles the tab bar the menu style's
/// frame holds, lights the tab this page stands for and hides the tabs the host does not offer.
/// </summary>
UISettingsTab UIRmlViewClass::Tab_Of_View(void) const
{
	return((ViewTab != UI_TAB_NONE) ? ViewTab : Owner.Settings_Tab());
}


unsigned UIRmlViewClass::Offered_Of_View(void) const
{
	return((ViewTab != UI_TAB_NONE) ? ViewOffered : Owner.Settings_Tabs_Offered());
}


/// <summary>
/// Makes this view a page of the tabbed Settings: the tab bar of the page lights the tab given and hides the tabs not offered.
/// The host calls this on every page at once, so the tab bars of pages that are not shown agree with the one on show.
/// </summary>
void UIRmlViewClass::Set_Tab(int tab, unsigned offered)
{
	ViewTab = (UISettingsTab)tab;
	ViewOffered = offered;
	Apply_Tabs();
}


void UIRmlViewClass::Apply_Tabs(void)
{
	UISettingsTab const tab = Tab_Of_View();
	if (Doc == nullptr || tab == UI_TAB_NONE) {
		return;
	}

	Doc->SetClass("settings", true);
	Doc->SetClass(Rml::String("tab-") + UI_TAB_NAMES[tab], true);

	// Pages replace each other in place, so the slide-open of a first screen would only flicker.
	Doc->SetAttribute("reveal", Rml::String("none"));

	for (int index = 0; index < UI_TAB_COUNT; index++) {
		Rml::Element * button = Doc->GetElementById(Rml::String("tab-") + UI_TAB_NAMES[index]);
		if (button != nullptr) {
			button->SetClass("active", index == tab);
			button->SetClass("gone", (Offered_Of_View() & (1u << index)) == 0);
		}
	}

}


/// <summary>
/// Moves between the Settings tabs from the keyboard: Ctrl+Tab and Ctrl+Page Up/Down anywhere
/// on the page, and the arrow keys while a tab has the focus.
/// </summary>
/// <returns>bool; Was the key a tab change?</returns>
bool UIRmlViewClass::Step_Tab(Rml::Event & event, int key)
{
	if (Tab_Of_View() == UI_TAB_NONE) {
		return(false);
	}

	Rml::Element * target = event.GetTargetElement();

	// A hot key box takes every combination for itself.
	bool const ctrl = event.GetParameter<bool>("ctrl_key", false) && !(target != nullptr && target->IsClassSet("hotkey"));
	bool const onbar = target != nullptr && target->IsClassSet("tab");
	int step = 0;

	if (ctrl && key == Rml::Input::KI_TAB) {
		step = event.GetParameter<bool>("shift_key", false) ? -1 : 1;
	} else if (ctrl && key == Rml::Input::KI_PRIOR) {
		step = -1;
	} else if (ctrl && key == Rml::Input::KI_NEXT) {
		step = 1;
	} else if (onbar && key == Rml::Input::KI_LEFT) {
		step = -1;
	} else if (onbar && key == Rml::Input::KI_RIGHT) {
		step = 1;
	}

	if (step == 0) {
		return(false);
	}

	int tab = Tab_Of_View();
	for (int tries = 0; tries < UI_TAB_COUNT; tries++) {
		tab = (tab + step + UI_TAB_COUNT) % UI_TAB_COUNT;
		if ((Offered_Of_View() & (1u << tab)) != 0) {
			UI_Tab_Keyboard = true;
			Mark_Keyboard();
			Queue("tab", tab);
			return(true);
		}
	}
	return(false);
}


bool UIRmlViewClass::Prepare(UIShellClass & shell)
{
	if (shell.Rml_Context() == nullptr || !Prepare(*shell.Rml_Context())) {
		return(false);
	}

	Shell = &shell;

	std::string const side = shell.Side_Sheet();
	if (!side.empty() && UI_Apply_Style_Sheet(*Doc, side)) {
		Rml::Log::Message(Rml::Log::LT_INFO, "UI: %s styles %s", side.c_str(), DocumentName.c_str());
	}
	return(true);
}


/// <summary>
/// Adds a style sheet after the document's own, so its rules win at equal specificity. A
/// missing file leaves the document unchanged.
/// </summary>
/// <returns>bool; Was the sheet found and added?</returns>
bool UI_Apply_Style_Sheet(Rml::ElementDocument & document, Rml::String const & name)
{
	Rml::FileInterface * files = Rml::GetFileInterface();
	Rml::FileHandle file = (files != nullptr) ? files->Open(name) : 0;
	if (file == 0) {
		return(false);
	}
	files->Close(file);

	Rml::SharedPtr<Rml::StyleSheetContainer> sheet = Rml::Factory::InstanceStyleSheetFile(name);
	if (!sheet) {
		return(false);
	}

	Rml::StyleSheetContainer const * own = document.GetStyleSheetContainer();
	document.SetStyleSheetContainer(own != nullptr ? own->CombineStyleSheetContainer(*sheet) : sheet);
	return(true);
}


void UIRmlViewClass::Show(bool modal)
{
	if (Doc != nullptr) {
		Doc->Show(modal ? Rml::ModalFlag::Modal : Rml::ModalFlag::None, Rml::FocusFlag::Document);

		// The keyboard flag belongs to the page on show, so only that page takes it.
		bool const keyboard_tab = UI_Tab_Keyboard;
		UI_Tab_Keyboard = false;
		if (keyboard_tab && Tab_Of_View() != UI_TAB_NONE) {
			Rml::Element * button = Doc->GetElementById(Rml::String("tab-") + UI_TAB_NAMES[Tab_Of_View()]);
			if (button != nullptr) {
				button->Focus();
				Mark_Keyboard();
			}
		}
	}
}


void UIRmlViewClass::Hide(void)
{
	if (Doc != nullptr) {
		Doc->Hide();
	}
}


void UIRmlViewClass::Release(void)
{
	if (Doc != nullptr) {
		Doc->Hide();
		Doc->RemoveEventListener(Rml::EventId::Keydown, this, true);
		Doc->RemoveEventListener(Rml::EventId::Textinput, this, true);
		Doc->RemoveEventListener(Rml::EventId::Mousedown, this);
		Doc->RemoveEventListener(Rml::EventId::Mousemove, this, true);
	}


	if (Host != nullptr) {
		if (ModelCreated) {
			Host->RemoveDataModel(ModelName);
		}
		if (Doc != nullptr) {
			Host->UnloadDocument(Doc);
		}
	}

	Followed.clear();
	Logged.clear();
	Tipped.reset();
	TipUp = false;

	Doc = nullptr;
	Host = nullptr;
	Shell = nullptr;
	ModelCreated = false;
	Wallpaper = 0x7FFFFFFF;
	Model = Rml::DataModelHandle();
	Types.reset();
}


bool UIRmlViewClass::Is_Shown(void) const
{
	return(Doc != nullptr && Doc->IsVisible());
}


static Rml::Element * Reveal_Element(Rml::ElementDocument * document)
{
	return(document != nullptr ? document->GetElementById("reveal") : nullptr);
}


float UIRmlViewClass::Reveal_Width(void) const
{
	if (Doc == nullptr || Doc->GetAttribute<Rml::String>("reveal", "") == "none") {
		return(0.0f);
	}

	Rml::Element * reveal = Reveal_Element(Doc);
	return(reveal != nullptr ? reveal->GetBox().GetSize().x : 0.0f);
}


void UIRmlViewClass::Reveal_To(float width)
{
	Rml::Element * reveal = Reveal_Element(Doc);
	if (reveal == nullptr) {
		return;
	}

	Rml::Element * chrome = Doc->GetElementById("chrome");
	if (chrome != nullptr && !Anchored) {
		float full = reveal->GetBox().GetSize().x;
		chrome->SetProperty(Rml::PropertyId::Position, Rml::Property(Rml::Style::Position::Relative));
		chrome->SetProperty(Rml::PropertyId::Width, Rml::Property(full, Rml::Unit::PX));
		chrome->SetProperty(Rml::PropertyId::Left, Rml::Property(50.0f, Rml::Unit::PERCENT));
		chrome->SetProperty(Rml::PropertyId::MarginLeft, Rml::Property(full * -0.5f, Rml::Unit::PX));
		Anchored = true;
	}

	Doc->SetClass("revealing", true);
	reveal->SetProperty(Rml::PropertyId::Width, Rml::Property(width < 0.0f ? 0.0f : width, Rml::Unit::PX));
}


void UIRmlViewClass::Placed(void)
{
	Place_Wallpaper();
	Size_Grips();
	Follow_Lists();
	Serve_Tip();
}


void UIRmlViewClass::Show_Row(Rml::Element * list, Rml::Element * row)
{
	float scrolled = list->GetScrollTop();
	float top = row->GetAbsoluteOffset(Rml::BoxArea::Border).y
		- (list->GetAbsoluteOffset(Rml::BoxArea::Border).y + list->GetClientTop()) + scrolled;
	float height = row->GetBox().GetSize(Rml::BoxArea::Border).y;
	float view = list->GetClientHeight();

	if (top < scrolled) {
		list->SetScrollTop(top);
	} else if (top + height > scrolled + view) {
		list->SetScrollTop(top + height - view);
	}
}


void UIRmlViewClass::Follow_Lists(void)
{
	if (Doc == nullptr) {
		return;
	}

	Rml::ElementList lists;
	Doc->GetElementsByClassName(lists, "list");

	for (Rml::Element * list : lists) {
		if (list->IsClassSet("log")) {
			int & seen = Logged[list];
			int count = Line_Count(list);
			if (count > seen) {
				list->SetScrollTop(list->GetScrollHeight());
			}
			seen = count;
			continue;
		}

		Rml::ElementList rows = Rows_Of(list);
		int selected = Selected_Row(rows);
		int & shown = Followed[list];
		if (selected >= 0 && selected != shown) {
			Show_Row(list, rows[selected]);
		}
		shown = selected;
	}
}


void UIRmlViewClass::Size_Grips(void)
{
	Rml::Context * context = Doc != nullptr ? Doc->GetContext() : nullptr;
	if (context == nullptr) {
		return;
	}

	float ratio = context->GetDensityIndependentPixelRatio();
	if (ratio <= 0.0f) {
		ratio = 1.0f;
	}

	Rml::ElementList lists;
	Doc->GetElementsByClassName(lists, "list");
	for (Rml::Element * list : lists) {
		Rml::Element * bar = list->GetElementScroll()->GetScrollbar(Rml::ElementScroll::VERTICAL);
		Rml::Element * grip = nullptr;
		for (int index = 0; bar != nullptr && index < bar->GetNumChildren(true); index++) {
			if (bar->GetChild(index)->GetTagName() == "sliderbar") {
				grip = bar->GetChild(index);
			}
		}
		if (bar == nullptr || !bar->IsVisible() || grip == nullptr) {
			continue;
		}

		float row = 14.0f * ratio;
		int visible = (int)(list->GetClientHeight() / row);
		int count = (int)(list->GetScrollHeight() / row + 0.5f);
		int range = count - visible;
		if (range < 1) {
			continue;
		}

		float travel = list->GetClientHeight() / ratio - 44.0f;
		int height = (int)(travel - std::log((double)(range + 1)) * travel * 0.2);
		if (height < 14) {
			height = 14;
		}

		if (std::fabs(grip->GetBox().GetSize().y - (float)height * ratio) > 0.5f) {
			grip->SetProperty("height", Rml::ToString(height) + "dp");
			list->GetElementScroll()->FormatScrollbars();
		}
	}
}


void UIRmlViewClass::Place_Wallpaper(void)
{
	Rml::Element * wallpaper = Doc != nullptr ? Doc->GetElementById("wallpaper-art") : nullptr;
	Rml::Element * dialog = Doc != nullptr ? Doc->GetElementById("reveal") : nullptr;
	Rml::Context * context = Doc != nullptr ? Doc->GetContext() : nullptr;
	if (wallpaper == nullptr || dialog == nullptr || context == nullptr) {
		return;
	}

	float ratio = context->GetDensityIndependentPixelRatio();
	if (ratio <= 0.0f) {
		ratio = 1.0f;
	}

	float middle = (dialog->GetAbsoluteOffset(Rml::BoxArea::Border).y + dialog->GetBox().GetSize().y * 0.5f) / ratio;
	float top = std::floor(((float)context->GetDimensions().y / ratio - 400.0f) * 0.5f);
	float offset = top - middle;
	if (offset == Wallpaper) {
		return;
	}

	Wallpaper = offset;
	wallpaper->SetProperty("margin-top", Rml::ToString(offset) + "dp");
}


void UIRmlViewClass::Reveal_Done(void)
{
	Rml::Element * reveal = Reveal_Element(Doc);
	if (reveal == nullptr) {
		return;
	}

	Rml::Element * chrome = Doc->GetElementById("chrome");
	if (chrome != nullptr && Anchored) {
		chrome->RemoveProperty(Rml::PropertyId::Position);
		chrome->RemoveProperty(Rml::PropertyId::Width);
		chrome->RemoveProperty(Rml::PropertyId::Left);
		chrome->RemoveProperty(Rml::PropertyId::MarginLeft);
		Anchored = false;
	}

	Doc->SetClass("revealing", false);
	reveal->RemoveProperty(Rml::PropertyId::Width);
}


bool UIRmlViewClass::Sounds_A_Click(Rml::Element const * element)
{
	if (element == nullptr) {
		return(false);
	}

	Rml::String const & tag = element->GetTagName();
	if (tag == "input") {
		Rml::String type = element->GetAttribute<Rml::String>("type", "text");
		return(type == "checkbox" || type == "radio" || type == "range" || type == "button" || type == "submit");
	}

	return(tag == "button" || tag == "select" || tag == "dataselect" || tag == "option" || element->IsClassSet("item"));
}


bool UIRmlViewClass::Takes_Enter(Rml::Element const * element)
{
	if (element == nullptr) {
		return(false);
	}

	Rml::String const & tag = element->GetTagName();
	if (tag == "input") {
		Rml::String type = element->GetAttribute<Rml::String>("type", "text");
		return(type == "text" || type == "password");
	}

	return(tag == "textarea");
}


bool UIRmlViewClass::Is_Disabled(Rml::Element const * element)
{
	return(element != nullptr && (element->IsClassSet("disabled") || element->HasAttribute("disabled")));
}


bool UIRmlViewClass::Is_Pressable(Rml::Element const * element)
{
	if (element == nullptr) {
		return(false);
	}

	Rml::String const & tag = element->GetTagName();
	if (tag == "input") {
		Rml::String type = element->GetAttribute<Rml::String>("type", "text");
		return(type == "checkbox" || type == "radio");
	}

	return(tag == "button");
}


Rml::Element * UIRmlViewClass::List_Of(Rml::Element * element) const
{
	for (; element != nullptr && element != Doc; element = element->GetParentNode()) {
		if (element->IsClassSet("list")) {
			return(element);
		}
	}
	return(nullptr);
}


Rml::Element * UIRmlViewClass::Holder_Of(Rml::Element * list)
{
	if (list != nullptr && list->GetNumChildren() > 0 && list->GetChild(0)->IsClassSet("rows")) {
		return(list->GetChild(0));
	}
	return(list);
}


int UIRmlViewClass::Line_Count(Rml::Element * list)
{
	Rml::Element * holder = Holder_Of(list);
	int count = 0;
	for (int index = 0; holder != nullptr && index < holder->GetNumChildren(); index++) {
		if (holder->GetChild(index)->IsVisible()) {
			count++;
		}
	}
	return(count);
}


Rml::ElementList UIRmlViewClass::Rows_Of(Rml::Element * list)
{
	Rml::ElementList rows;
	if (list == nullptr) {
		return(rows);
	}

	Rml::Element * holder = Holder_Of(list);
	for (int index = 0; index < holder->GetNumChildren(); index++) {
		Rml::Element * row = holder->GetChild(index);
		if (row->IsClassSet("item") && row->IsVisible()) {
			rows.push_back(row);
		}
	}
	return(rows);
}


int UIRmlViewClass::Selected_Row(Rml::ElementList const & rows)
{
	for (int index = 0; index < (int)rows.size(); index++) {
		if (rows[index]->IsClassSet("selected")) {
			return(index);
		}
	}
	return(-1);
}


Rml::String UIRmlViewClass::Row_Text(Rml::Element * row)
{
	if (row == nullptr) {
		return(Rml::String());
	}

	if (Rml::ElementText * text = rmlui_dynamic_cast<Rml::ElementText *>(row)) {
		return(text->GetText());
	}

	for (int index = 0; index < row->GetNumChildren(); index++) {
		Rml::String found = Row_Text(row->GetChild(index));
		if (!found.empty()) {
			return(found);
		}
	}
	return(Rml::String());
}


bool UIRmlViewClass::Navigate_List(Rml::Element * list, int key)
{
	Rml::ElementList rows = Rows_Of(list);
	if (rows.empty()) {
		return(false);
	}

	float height = rows[0]->GetBox().GetSize(Rml::BoxArea::Border).y;
	int page = (height > 0.0f) ? (int)(list->GetClientHeight() / height) : 1;
	if (page < 1) {
		page = 1;
	}

	int current = Selected_Row(rows);
	int last = (int)rows.size() - 1;
	int wanted;

	switch (key) {
		case Rml::Input::KI_UP:    wanted = (current < 0) ? last : current - 1; break;
		case Rml::Input::KI_DOWN:  wanted = (current < 0) ? 0 : current + 1; break;
		case Rml::Input::KI_PRIOR: wanted = (current < 0) ? last : current - page; break;
		case Rml::Input::KI_NEXT:  wanted = (current < 0) ? 0 : current + page; break;
		case Rml::Input::KI_HOME:  wanted = 0; break;
		case Rml::Input::KI_END:   wanted = last; break;
		default: return(false);
	}

	wanted = (wanted < 0) ? 0 : ((wanted > last) ? last : wanted);
	if (wanted != current) {
		rows[wanted]->Click();
	}
	return(true);
}


bool UIRmlViewClass::Step_Range(Rml::Element * element, int key)
{
	if (element == nullptr || element->GetTagName() != "input"
		|| element->GetAttribute<Rml::String>("type", "") != "range" || Is_Disabled(element)) {
		return(false);
	}

	float lowest = element->GetAttribute<float>("min", 0.0f);
	float highest = element->GetAttribute<float>("max", 100.0f);
	float step = element->GetAttribute<float>("step", 1.0f);
	if (step <= 0.0f) {
		step = 1.0f;
	}

	float page = (highest - lowest) / 5.0f;
	if (page < step) {
		page = step;
	}

	float value = element->GetAttribute<float>("value", lowest);
	switch (key) {
		case Rml::Input::KI_UP:    value -= step; break;
		case Rml::Input::KI_DOWN:  value += step; break;
		case Rml::Input::KI_PRIOR: value -= page; break;
		case Rml::Input::KI_NEXT:  value += page; break;
		case Rml::Input::KI_HOME:  value = lowest; break;
		case Rml::Input::KI_END:   value = highest; break;
		default: return(false);
	}

	value = (value < lowest) ? lowest : ((value > highest) ? highest : value);
	element->SetAttribute("value", value);
	return(true);
}


/// <summary>
/// Moves the focus to the nearest button of a grid, an element with class grid, in the arrow key's
/// direction. A button in the same row is the neighbour for Left and Right, and the nearest button
/// in the row above or below, and then in its column, for Up and Down.
/// </summary>
/// <returns>bool; Did the focus move?</returns>
bool UIRmlViewClass::Navigate_Grid(Rml::Element * element, int key)
{
	Rml::Element * grid = (element != nullptr) ? element->GetParentNode() : nullptr;
	if (grid == nullptr || element->GetTagName() != "button" || !grid->IsClassSet("grid")) {
		return(false);
	}

	bool const across = key == Rml::Input::KI_LEFT || key == Rml::Input::KI_RIGHT;
	if (!across && key != Rml::Input::KI_UP && key != Rml::Input::KI_DOWN) {
		return(false);
	}
	float const sign = (key == Rml::Input::KI_LEFT || key == Rml::Input::KI_UP) ? -1.0f : 1.0f;

	Rml::Vector2f const from = element->GetAbsoluteOffset(Rml::BoxArea::Border);
	float const half_height = element->GetBox().GetSize(Rml::BoxArea::Border).y * 0.5f;
	Rml::Element * best = nullptr;
	float best_rank = 0.0f;
	for (int index = 0; index < grid->GetNumChildren(); index++) {
		Rml::Element * other = grid->GetChild(index);
		if (other == element || other->GetTagName() != "button" || Is_Disabled(other) || !other->IsVisible()) {
			continue;
		}

		Rml::Vector2f const to = other->GetAbsoluteOffset(Rml::BoxArea::Border);
		float const along = (across ? to.x - from.x : to.y - from.y) * sign;
		float const side = across ? to.y - from.y : to.x - from.x;
		if (along <= 0.0f) {
			continue;
		}

		// Across a row, the candidate must sit in the same row; up or down, it must sit in a row of its own.
		float const rank = across
			? (std::fabs(side) < half_height ? along : -1.0f)
			: (along >= half_height ? along * 1000.0f + std::fabs(side) : -1.0f);
		if (rank < 0.0f) {
			continue;
		}
		if (best == nullptr || rank < best_rank) {
			best = other;
			best_rank = rank;
		}
	}

	if (best == nullptr) {
		return(false);
	}
	best->Focus();
	return(true);
}


bool UIRmlViewClass::Type_Ahead(Rml::Element * list, Rml::String const & text)
{
	if (text.empty() || (unsigned char)text[0] <= ' ') {
		return(false);
	}

	Rml::ElementList rows = Rows_Of(list);
	if (rows.empty()) {
		return(false);
	}

	int current = Selected_Row(rows);
	std::string wanted(1, text[0]);

	for (int step = 1; step <= (int)rows.size(); step++) {
		int index = (current + step) % (int)rows.size();
		std::string label = Row_Text(rows[index]);
		if (strnicmp(label.c_str(), wanted.c_str(), wanted.size()) == 0) {
			if (index != current) {
				rows[index]->Click();
			}
			return(true);
		}
	}
	return(false);
}


void UIRmlViewClass::Mark_Keyboard(void)
{
	if (Doc != nullptr) {
		Doc->SetClass("keys", true);
	}
}


static char const * const UI_TIP_ATTRIBUTE = "tip";
static char const * const UI_TIP_ELEMENT = "tip";

// The owner-draw dialogs waited a second for the pointer to settle, and only a third of a
// second when another tip had just been taken down.
static const int UI_TIP_DELAY = 1000;
static const int UI_TIP_AGAIN = 300;
static const int UI_TIP_RECENT = 1000;
static const float UI_TIP_BELOW_POINTER = 16.0f;


Rml::Element * UIRmlViewClass::Tip_Of(Rml::Element * element) const
{
	for (Rml::Element * step = element; step != nullptr && step != Doc; step = step->GetParentNode()) {
		if (!step->GetAttribute<Rml::String>(UI_TIP_ATTRIBUTE, Rml::String()).empty()) {
			return(step);
		}
	}
	return(nullptr);
}


void UIRmlViewClass::Drop_Tip(void)
{
	Rml::Element * tip = (Doc != nullptr) ? Doc->GetElementById(UI_TIP_ELEMENT) : nullptr;
	if (tip != nullptr && TipUp) {
		tip->SetClass("up", false);
		TipDropped = (Shell != nullptr) ? Shell->Clock().Milliseconds() : 0;
	}
	TipUp = false;
	Tipped.reset();
}


void UIRmlViewClass::Serve_Tip(void)
{
	Rml::Element * tip = (Doc != nullptr) ? Doc->GetElementById(UI_TIP_ELEMENT) : nullptr;
	if (tip == nullptr) {
		return;
	}

	// Hover can end with no mouse move in the document, as when the pointer leaves the window.
	Rml::Element * tipped = Tipped.get();
	if (tipped != nullptr && !tipped->IsPseudoClassSet("hover")) {
		Drop_Tip();
		tipped = nullptr;
	}

	Rml::String text = (tipped != nullptr)
		? tipped->GetAttribute<Rml::String>(UI_TIP_ATTRIBUTE, Rml::String()) : Rml::String();
	if (text.empty()) {
		Drop_Tip();
		return;
	}

	if (tip->GetInnerRML() != text) {
		tip->SetInnerRML(text);
	}

	// Placed while it is still hidden, so the layout has settled before it is shown.
	Rml::Context * context = Doc->GetContext();
	float ratio = (context != nullptr) ? context->GetDensityIndependentPixelRatio() : 1.0f;
	if (ratio <= 0.0f) {
		ratio = 1.0f;
	}

	Rml::Vector2f size = tip->GetBox().GetSize(Rml::BoxArea::Border);
	Rml::Vector2f bounds = (context != nullptr) ? Rml::Vector2f(context->GetDimensions()) : size;

	float x = TipAt.x;
	float y = TipAt.y + UI_TIP_BELOW_POINTER * ratio;
	x = (x + size.x >= bounds.x) ? bounds.x - size.x : x;
	y = (y + size.y >= bounds.y) ? bounds.y - size.y : y;

	tip->SetProperty(Rml::PropertyId::Left, Rml::Property((x < 0.0f) ? 0.0f : x, Rml::Unit::PX));
	tip->SetProperty(Rml::PropertyId::Top, Rml::Property((y < 0.0f) ? 0.0f : y, Rml::Unit::PX));

	if (TipUp) {
		return;
	}

	int now = (Shell != nullptr) ? Shell->Clock().Milliseconds() : 0;
	int wait = (now - TipDropped <= UI_TIP_RECENT) ? UI_TIP_AGAIN : UI_TIP_DELAY;
	if (now - TipArmed >= wait) {
		tip->SetClass("up", true);
		TipUp = true;
	}
}

// Shows the desc attribute of the setting an element belongs to in the page's description element, if the page has one.
static void Describe_Setting(Rml::ElementDocument * doc, Rml::Element * element)
{
	if (doc == nullptr) {
		return;
	}
	for (Rml::Element * step = element; step != nullptr && step != doc; step = step->GetParentNode()) {
		Rml::String const text = step->GetAttribute<Rml::String>("desc", Rml::String());
		if (!text.empty()) {
			Rml::Element * panel = doc->GetElementById("description");
			if (panel != nullptr && panel->GetInnerRML() != text) {
				panel->SetInnerRML(text);
			}
			return;
		}
	}
}


void UIRmlViewClass::ProcessEvent(Rml::Event & event)
{
	if (event.GetId() == Rml::EventId::Focus) {
		Describe_Setting(Doc, event.GetTargetElement());
	}

	if (event.GetId() == Rml::EventId::Mousemove) {
		Describe_Setting(Doc, event.GetTargetElement());
		Rml::Element * tipped = Tip_Of(event.GetTargetElement());
		TipAt = Rml::Vector2f(event.GetParameter<float>("mouse_x", 0.0f),
			event.GetParameter<float>("mouse_y", 0.0f));
		if (tipped != Tipped.get()) {
			Drop_Tip();
			if (tipped != nullptr) {
				Tipped = tipped->GetObserverPtr();
			}
		}
		TipArmed = (Shell != nullptr) ? Shell->Clock().Milliseconds() : 0;
		return;
	}

	if (event.GetId() == Rml::EventId::Mousedown) {
		Drop_Tip();
		if (Doc != nullptr) {
			Doc->SetClass("keys", false);
		}
		if (Shell == nullptr || event.GetParameter<int>("button", 0) != 0) {
			return;
		}
		for (Rml::Element * element = event.GetTargetElement(); element != nullptr && element != Doc; element = element->GetParentNode()) {
			if (Is_Disabled(element)) {
				return;
			}
			if (Sounds_A_Click(element)) {
				Shell->Play_Click();
				return;
			}
		}
		return;
	}

	Rml::Element * target = event.GetTargetElement();

	if (event.GetId() == Rml::EventId::Textinput) {
		Rml::Element * list = List_Of(target);
		if (list != nullptr && !list->IsClassSet("log")
			&& Type_Ahead(list, event.GetParameter<Rml::String>("text", Rml::String()))) {
			Mark_Keyboard();
			event.StopPropagation();
		}
		return;
	}

	if (event.GetId() != Rml::EventId::Keydown) {
		return;
	}

	int key = event.GetParameter<int>("key_identifier", 0);

	if (Step_Tab(event, key)) {
		event.StopPropagation();
		return;
	}

	if (key == Rml::Input::KI_TAB) {
		Mark_Keyboard();
		return;
	}

	// RmlUi clicks a focused control on Enter or Space unless the event is stopped here.
	if (key == Rml::Input::KI_RETURN || key == Rml::Input::KI_NUMPADENTER) {
		if (Takes_Enter(target)) {
			return;
		}
		if (target != nullptr && target->GetTagName() == "button" && !Is_Disabled(target)) {
			if (Shell != nullptr) {
				Shell->Play_Click();
			}
			Mark_Keyboard();
			return;
		}
		Queue("ok");
		event.StopPropagation();
		return;
	}

	if (key == Rml::Input::KI_ESCAPE) {
		Queue("cancel");
		event.StopPropagation();
		return;
	}

	if (key == Rml::Input::KI_SPACE) {
		if (Is_Pressable(target)) {
			if (Is_Disabled(target)) {
				event.StopPropagation();
				return;
			}
			if (Shell != nullptr) {
				Shell->Play_Click();
			}
			Mark_Keyboard();
			return;
		}
		if (List_Of(target) != nullptr) {
			event.StopPropagation();
		}
		return;
	}

	if (Step_Range(target, key)) {
		Mark_Keyboard();
		event.StopPropagation();
		return;
	}

	if (Navigate_Grid(target, key)) {
		Mark_Keyboard();
		event.StopPropagation();
		return;
	}

	Rml::Element * list = List_Of(target);
	if (list != nullptr && !list->IsClassSet("log") && Navigate_List(list, key)) {
		Mark_Keyboard();
		event.StopPropagation();
	}
}


void UIRmlViewClass::Queue(char const * name, int value, char const * text)
{
	UIIntent intent;
	intent.Name = name;
	intent.Value = value;
	if (text != nullptr) {
		intent.Text = text;
	}
	Owner.Queue(intent);
}
