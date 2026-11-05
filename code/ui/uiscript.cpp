/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/uiscript.h"

#include "_ui.h"
#include "bgfxbackend.h"
#include "ui/uishell.h"
#include "dbgprint.h"
#include "gamedirs.h"
#include "movie.h"
#include "strutil.h"
#include "video.h"
#include "windowevent.hh"

#include <RmlUi/Core.h>

#include "globals.h"
#include "goptions.h"
#include "hangwatch.h"
#include "hud.h"
#include "init.h"
#include "monotonic.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct UIStepType {
	std::string Command;
	std::string Argument;
};

std::vector<UIStepType> Steps;
std::size_t Next = 0;
long long WaitUntil = 0;
long long WaitForSince = 0;

// The capture request keeps the path's address until the next frame is shown.
std::string ShotPath;


// Moves the pointer to the element's centre, then presses and releases the button there through the shell's window
// events, which is the path a player's mouse takes.
void Window_Click(Rml::Element * element, WindowMouseButton button)
{
	Rml::Vector2f const origin = element->GetAbsoluteOffset(Rml::BoxArea::Border);
	Rml::Vector2f const size = element->GetBox().GetSize(Rml::BoxArea::Border);
	WindowEvent event;
	event.X = (int)(origin.x + size.x * 0.5f);
	event.Y = (int)(origin.y + size.y * 0.5f);
	event.Button = button;
	event.Clicks = 1;
	event.Type = WINDOW_EVENT_MOUSE_MOVE;
	UIShell.Handle_Window_Event(event);
	event.Type = WINDOW_EVENT_MOUSE_DOWN;
	UIShell.Handle_Window_Event(event);
	event.Type = WINDOW_EVENT_MOUSE_UP;
	UIShell.Handle_Window_Event(event);
}


// Calls find for each visible document, the topmost first, and returns the first element it finds.
template <typename Find>
Rml::Element * First_Visible(Rml::Context * context, Find find)
{
	for (int index = context->GetNumDocuments() - 1; index >= 0; index--) {
		Rml::ElementDocument * document = context->GetDocument(index);
		if (document != nullptr && document->IsVisible()) {
			if (Rml::Element * found = find(document)) {
				return(found);
			}
		}
	}
	return(nullptr);
}


Rml::Element * Find_Element(Rml::Context * context, std::string const & id)
{
	return(First_Visible(context, [&id](Rml::ElementDocument * document) { return document->GetElementById(id); }));
}


std::string Plain_Text(Rml::Element * element)
{
	std::string text;
	bool tag = false;
	for (char ch : element->GetInnerRML()) {
		if (ch == '<') {
			tag = true;
		} else if (ch == '>') {
			tag = false;
		} else if (!tag) {
			text += ch;
		}
	}
	return(Rml::StringUtilities::DecodeRml(Trimmed(text, TRIM_SPACE_TAB_EOL)));
}


Rml::Element * Find_Button(Rml::Element * element, std::string const & label)
{
	if (element->IsVisible() && element->GetTagName() == "button" && _stricmp(Plain_Text(element).c_str(), label.c_str()) == 0) {
		return(element);
	}
	for (int child = 0; child < element->GetNumChildren(); child++) {
		if (Rml::Element * found = Find_Button(element->GetChild(child), label)) {
			return(found);
		}
	}
	return(nullptr);
}


Rml::Element * Find_Button(Rml::Context * context, std::string const & label)
{
	return(First_Visible(context, [&label](Rml::ElementDocument * document) { return Find_Button(document, label); }));
}


// The innermost visible element whose text is the label, such as one row of a list.
Rml::Element * Find_Text(Rml::Element * element, std::string const & label)
{
	if (!element->IsVisible() || element->GetTagName() == "#text") {
		return(nullptr);
	}
	// A drop-down's open list is not an ordinary child, so those are counted too.
	for (int child = 0; child < element->GetNumChildren(true); child++) {
		if (Rml::Element * found = Find_Text(element->GetChild(child), label)) {
			return(found);
		}
	}
	if (_stricmp(Plain_Text(element).c_str(), label.c_str()) == 0) {
		return(element);
	}
	return(nullptr);
}


Rml::Element * Find_Text(Rml::Context * context, std::string const & label)
{
	return(First_Visible(context, [&label](Rml::ElementDocument * document) { return Find_Text(document, label); }));
}


void Log_Buttons(Rml::Element * element)
{
	if (element->IsVisible() && element->GetTagName() == "button") {
		DebugString("UISCRIPT   button \"%s\"%s\n", Plain_Text(element).c_str(), element->IsClassSet("disabled") ? " (disabled)" : "");
	}
	for (int child = 0; child < element->GetNumChildren(); child++) {
		Log_Buttons(element->GetChild(child));
	}
}


void Log_Ids(Rml::Element * element, int depth)
{
	if (!element->GetId().empty()) {
		std::string text = element->GetInnerRML();
		if (text.find('<') != std::string::npos || text.size() > 40) {
			text.clear();
		}
		DebugString("UISCRIPT   %*s#%s <%s> %s\n", depth * 2, "", element->GetId().c_str(), element->GetTagName().c_str(), text.c_str());
	}
	for (int child = 0; child < element->GetNumChildren(); child++) {
		Log_Ids(element->GetChild(child), depth + 1);
	}
}


// The modifiers that keydown steps have pressed and keyup steps have not released.
int ScriptHeldModifiers = 0;


bool Key_From_Name(std::string const & name, Rml::Input::KeyIdentifier & key)
{
	static struct { char const * Name; Rml::Input::KeyIdentifier Key; } const keys[] = {
		{"escape", Rml::Input::KI_ESCAPE},
		{"return", Rml::Input::KI_RETURN},
		{"tab", Rml::Input::KI_TAB},
		{"space", Rml::Input::KI_SPACE},
		{"up", Rml::Input::KI_UP},
		{"down", Rml::Input::KI_DOWN},
		{"left", Rml::Input::KI_LEFT},
		{"right", Rml::Input::KI_RIGHT},
		{"pageup", Rml::Input::KI_PRIOR},
		{"pagedown", Rml::Input::KI_NEXT},
		{"shift", Rml::Input::KI_LSHIFT},
		{"ctrl", Rml::Input::KI_LCONTROL},
		{"alt", Rml::Input::KI_LMENU},
	};
	for (auto const & entry : keys) {
		if (name == entry.Name) {
			key = entry.Key;
			return(true);
		}
	}
	if (name.size() == 1 && std::isalpha((unsigned char)name[0])) {
		key = (Rml::Input::KeyIdentifier)(Rml::Input::KI_A + (std::tolower((unsigned char)name[0]) - 'a'));
		return(true);
	}
	if (name.size() == 1 && std::isdigit((unsigned char)name[0])) {
		key = (Rml::Input::KeyIdentifier)(Rml::Input::KI_0 + (name[0] - '0'));
		return(true);
	}
	return(false);
}

// A step handler returns true when the tick must end, because the step waits or opened a screen.
typedef bool (*StepHandler)(Rml::Context * context, UIStepType const & step, long long now);


bool Settle(long long now)
{
	WaitUntil = now + 300;
	return(true);
}


// Clicks the element, or logs that no visible one was found; what names the kind of element in the log.
bool Click_Visible(Rml::Element * element, UIStepType const & step, long long now, char const * what)
{
	if (element == nullptr) {
		DebugString("UISCRIPT   %s %s: no such visible %s\n", step.Command.c_str(), step.Argument.c_str(), what);
		return(false);
	}
	element->Click();
	return(Settle(now));
}


bool Step_Wait(Rml::Context *, UIStepType const & step, long long now)
{
	WaitUntil = now + std::atoi(step.Argument.c_str());
	return(true);
}


bool Step_Shot(Rml::Context *, UIStepType const & step, long long now)
{
	ShotPath = Screenshot_Name((step.Argument + ".tga").c_str());
	Backend_Request_Window_Capture(ShotPath.c_str());
	// A still screen is not shown again until something changes, so one is asked for.
	Video_Mark_Overlay_Dirty();
	WaitUntil = now + 500;
	return(true);
}


// Logs the game frame count and the clock, so two lines give the frame rate of the menu battle.
bool Step_Frames(Rml::Context *, UIStepType const &, long long now)
{
	DebugString("UISCRIPT frames %d at %lld ms\n", Frame, now);
	return(false);
}


bool Step_Ids(Rml::Context * context, UIStepType const &, long long)
{
	for (int index = 0; index < context->GetNumDocuments(); index++) {
		Rml::ElementDocument * document = context->GetDocument(index);
		if (document != nullptr && document->IsVisible()) {
			DebugString("UISCRIPT   document %s\n", document->GetSourceURL().c_str());
			Log_Ids(document, 1);
			Log_Buttons(document);
		}
	}
	return(false);
}


bool Step_Box(Rml::Context * context, UIStepType const & step, long long)
{
	Rml::Element * element = Find_Element(context, step.Argument);
	if (element == nullptr) {
		DebugString("UISCRIPT   box %s: no such visible element\n", step.Argument.c_str());
	}
	for (Rml::Element * at = element; at != nullptr; at = at->GetParentNode()) {
		Rml::Vector2f const offset = at->GetAbsoluteOffset(Rml::BoxArea::Border);
		Rml::Vector2f const size = at->GetBox().GetSize(Rml::BoxArea::Border);
		DebugString("UISCRIPT   box #%s <%s> at %.1f,%.1f size %.1fx%.1f position %d\n", at->GetId().c_str(), at->GetTagName().c_str(),
			offset.x, offset.y, size.x, size.y, (int)at->GetComputedValues().position());
	}
	return(false);
}


bool Step_Click(Rml::Context * context, UIStepType const & step, long long now)
{
	return(Click_Visible(Find_Element(context, step.Argument), step, now, "element"));
}


bool Step_Press(Rml::Context * context, UIStepType const & step, long long now)
{
	return(Click_Visible(Find_Button(context, step.Argument), step, now, "button"));
}


// "id value" sets the value of a slider, as dragging it would, or the text of a text
// box, as typing it and leaving the box would.
bool Step_Set(Rml::Context * context, UIStepType const & step, long long now)
{
	std::size_t const space = step.Argument.find(' ');
	Rml::Element * element = Find_Element(context, step.Argument.substr(0, space));
	bool const text_box = element != nullptr && element->GetAttribute<Rml::String>("type", "") == "text";
	if (element == nullptr || (space == std::string::npos && !text_box)) {
		DebugString("UISCRIPT   set %s: no such visible element or no value\n", step.Argument.c_str());
		return(false);
	}

	// A text box given no value is emptied.
	std::string const value = space == std::string::npos ? std::string() : step.Argument.substr(space + 1);
	element->SetAttribute("value", value);
	if (element->GetAttribute<Rml::String>("type", "") == "text") {
		Rml::Dictionary parameters;
		parameters["value"] = Rml::Variant(value);
		element->DispatchEvent(Rml::EventId::Change, parameters);
	}
	return(Settle(now));
}


// "id fx fy" presses and releases the mouse at a point given as fractions of the element's box.
bool Step_Pointer(Rml::Context * context, UIStepType const & step, long long now)
{
	std::istringstream fields(step.Argument);
	std::string id;
	float fx = 0.0f;
	float fy = 0.0f;
	fields >> id >> fx >> fy;
	Rml::Element * element = Find_Element(context, id);
	if (element == nullptr || fields.fail()) {
		DebugString("UISCRIPT   pointer %s: no such visible element or no point\n", step.Argument.c_str());
		return(false);
	}

	Rml::Vector2f const origin = element->GetAbsoluteOffset(Rml::BoxArea::Border);
	Rml::Vector2f const size = element->GetBox().GetSize(Rml::BoxArea::Border);
	context->ProcessMouseMove((int)(origin.x + fx * size.x), (int)(origin.y + fy * size.y), 0);
	context->ProcessMouseButtonDown(0, 0);
	context->ProcessMouseButtonUp(0, 0);
	return(Settle(now));
}


// "id 1|2" presses a side button (1 is Mouse 4, 2 is Mouse 5) at the centre of the element.
bool Step_Xbutton(Rml::Context * context, UIStepType const & step, long long now)
{
	std::istringstream fields(step.Argument);
	std::string id;
	int side = 0;
	fields >> id >> side;
	Rml::Element * element = Find_Element(context, id);
	if (element == nullptr || fields.fail() || (side != 1 && side != 2)) {
		DebugString("UISCRIPT   xbutton %s: no such visible element or no button\n", step.Argument.c_str());
		return(false);
	}

	Window_Click(element, (side == 2) ? WINDOW_BUTTON_X2 : WINDOW_BUTTON_X1);
	return(Settle(now));
}


// "id" makes a left click at the centre of the element, as a player's click arrives.
bool Step_Wclick(Rml::Context * context, UIStepType const & step, long long now)
{
	Rml::Element * element = Find_Element(context, step.Argument);
	if (element == nullptr) {
		DebugString("UISCRIPT   wclick %s: no such visible element\n", step.Argument.c_str());
		return(false);
	}

	Window_Click(element, WINDOW_BUTTON_LEFT);
	return(Settle(now));
}


// "id|label" looks only inside the element with that id, such as one drop-down.
bool Step_Choose(Rml::Context * context, UIStepType const & step, long long now)
{
	std::string label = step.Argument;
	Rml::Element * within = nullptr;
	std::size_t const bar = label.find('|');
	if (bar != std::string::npos) {
		within = Find_Element(context, label.substr(0, bar));
		label.erase(0, bar + 1);
	}
	Rml::Element * element = bar != std::string::npos ? (within != nullptr ? Find_Text(within, label) : nullptr) : Find_Text(context, label);
	if (element == nullptr) {
		DebugString("UISCRIPT   choose %s: no such visible text\n", step.Argument.c_str());
		return(false);
	}

	element->ScrollIntoView();
	element->Click();
	return(Settle(now));
}


// A "ctrl+" or "shift+" in front of the name holds that key down.
bool Step_Key(Rml::Context * context, UIStepType const & step, long long now)
{
	std::string name = step.Argument;
	int modifiers = 0;
	for (bool more = true; more; ) {
		more = false;
		if (name.rfind("ctrl+", 0) == 0) {
			modifiers |= Rml::Input::KM_CTRL;
			name.erase(0, 5);
			more = true;
		} else if (name.rfind("shift+", 0) == 0) {
			modifiers |= Rml::Input::KM_SHIFT;
			name.erase(0, 6);
			more = true;
		} else if (name.rfind("alt+", 0) == 0) {
			modifiers |= Rml::Input::KM_ALT;
			name.erase(0, 4);
			more = true;
		}
	}

	Rml::Input::KeyIdentifier key;
	if (!Key_From_Name(name, key)) {
		DebugString("UISCRIPT   key %s: unknown key\n", step.Argument.c_str());
		return(false);
	}

	context->ProcessKeyDown(key, modifiers);
	context->ProcessKeyUp(key, modifiers);
	return(Settle(now));
}


// Presses or releases one key, passing the shift, ctrl and alt keys that keydown steps hold, so a
// capture sees each modifier go down and come up as a player's would.
bool Step_Keydown_Or_Keyup(Rml::Context * context, UIStepType const & step, long long now)
{
	Rml::Input::KeyIdentifier key;
	if (!Key_From_Name(step.Argument, key)) {
		DebugString("UISCRIPT   %s %s: unknown key\n", step.Command.c_str(), step.Argument.c_str());
		return(false);
	}

	int const bit = (key == Rml::Input::KI_LSHIFT) ? Rml::Input::KM_SHIFT : (key == Rml::Input::KI_LCONTROL) ? Rml::Input::KM_CTRL
		: (key == Rml::Input::KI_LMENU) ? Rml::Input::KM_ALT : 0;
	if (step.Command == "keydown") {
		ScriptHeldModifiers |= bit;
		context->ProcessKeyDown(key, ScriptHeldModifiers);
	} else {
		context->ProcessKeyUp(key, ScriptHeldModifiers);
		ScriptHeldModifiers &= ~bit;
	}
	return(Settle(now));
}


// Sets the MenuTheme setting in memory, so the menus shown next take that tint.
bool Step_Theme(Rml::Context *, UIStepType const & step, long long now)
{
	std::string const name = Lower(step.Argument);
	if (name == "allied") {
		Options.MenuTheme = MENU_THEME_ALLIED;
	} else if (name == "soviet") {
		Options.MenuTheme = MENU_THEME_SOVIET;
	} else if (name == "yuri") {
		Options.MenuTheme = MENU_THEME_YURI;
	} else {
		Options.MenuTheme = MENU_THEME_RANDOM;
	}
	return(Settle(now));
}


// Holds the main thread without pumping messages, so the hang watchdog reports it.
bool Step_Stall(Rml::Context *, UIStepType const & step, long long)
{
	Hang_Watchdog_Test_Stall((unsigned int)std::max(0, std::atoi(step.Argument.c_str())));
	return(false);
}


bool Step_Command(Rml::Context *, UIStepType const & step, long long now)
{
	Execute_Command(step.Argument.c_str());
	return(Settle(now));
}


bool Step_Movie(Rml::Context *, UIStepType const & step, long long)
{
	Play_Movie(step.Argument.c_str());
	return(Settle(Monotonic_Milliseconds()));
}


bool Step_Checked(Rml::Context * context, UIStepType const & step, long long)
{
	Rml::Element * element = Find_Element(context, step.Argument);
	if (element == nullptr) {
		DebugString("UISCRIPT   checked %s: no such visible element\n", step.Argument.c_str());
	} else {
		DebugString("AUTOTEST checked %s %d\n", step.Argument.c_str(), element->HasAttribute("checked") ? 1 : 0);
	}
	return(false);
}


bool Step_Hudrows(Rml::Context *, UIStepType const &, long long)
{
	DebugString("AUTOTEST hud rows %d saved %d\n", Hud_Rows(), Options.SidebarRows);
	return(false);
}


bool Step_Quit(Rml::Context *, UIStepType const &, long long)
{
	DebugString("UISCRIPT quit\n");
	std::exit(0);
}


StepHandler Find_Step_Handler(std::string const & command)
{
	static struct { char const * Command; StepHandler Handler; } const handlers[] = {
		{"wait", Step_Wait},
		{"shot", Step_Shot},
		{"frames", Step_Frames},
		{"ids", Step_Ids},
		{"box", Step_Box},
		{"click", Step_Click},
		{"set", Step_Set},
		{"pointer", Step_Pointer},
		{"xbutton", Step_Xbutton},
		{"wclick", Step_Wclick},
		{"press", Step_Press},
		{"choose", Step_Choose},
		{"key", Step_Key},
		{"keydown", Step_Keydown_Or_Keyup},
		{"keyup", Step_Keydown_Or_Keyup},
		{"theme", Step_Theme},
		{"stall", Step_Stall},
		{"command", Step_Command},
		{"movie", Step_Movie},
		{"checked", Step_Checked},
		{"hudrows", Step_Hudrows},
		{"quit", Step_Quit},
	};
	for (auto const & entry : handlers) {
		if (command == entry.Command) {
			return(entry.Handler);
		}
	}
	return(nullptr);
}

}	// namespace


void UIScript_Add(std::string const & command, std::string const & argument)
{
	Steps.push_back(UIStepType{command, argument});
}


/// <summary>
/// Carries out the menu steps that are due. A step that opens a screen running its own loop
/// returns only when that screen closes, and the steps after it run from that loop's ticks.
/// Steps: wait (milliseconds), waitfor (an element id, up to 20 seconds), click (an element
/// id), pointer (an element id and two fractions of its box, where the mouse is pressed and released),
/// set (a slider's id and the value to give it, separated by a space; a text box's id alone empties it),
/// xbutton (an element id and 1 or 2: a press of Mouse 4 or Mouse 5 at its centre, through the window events),
/// wclick (an element id: a left click at its centre, through the window events),
/// press (a button's label, ignoring case), choose (the text of a list row or drop-down entry, or "id|text" to look only
/// inside the element with that id), key (escape, return, tab, space, page up or down, an arrow, a letter, a digit, shift
/// or ctrl, with ctrl+, shift+ or alt+ in front to hold that key),
/// frames (logs the game frame count and the clock), keydown and keyup (a key name, pressed or released, with the
/// shift, ctrl and alt keys that keydown holds), shot (a name for a .tga in the screenshots folder), ids (logs the ids and buttons of every
/// visible screen), box (logs where an element and each element around it lie), theme (sets the menu tint to allied, soviet, yuri or random, as the MenuTheme setting does), movie (plays the
/// movie of that file name, as a campaign does), stall (milliseconds to hold the main thread
/// without pumping messages, for testing the hang watchdog), command (runs the registered command of that
/// name, as its key does; the in-game script's command step cannot run while a replay is paused) and quit.
/// </summary>
void UIScript_Tick(Rml::Context * context)
{
	if (context == nullptr) {
		return;
	}

	while (Next < Steps.size()) {
		long long const now = Monotonic_Milliseconds();
		if (now < WaitUntil) {
			return;
		}

		UIStepType const step = Steps[Next];

		if (step.Command == "waitfor") {
			if (WaitForSince == 0) {
				WaitForSince = now;
			}
			if (Find_Element(context, step.Argument) == nullptr) {
				if (now - WaitForSince < 20000) {
					return;
				}
				DebugString("UISCRIPT waitfor %s: not found after 20 seconds\n", step.Argument.c_str());
			} else {
				DebugString("UISCRIPT waitfor %s: found\n", step.Argument.c_str());
			}
			WaitForSince = 0;
			Next++;
			continue;
		}

		// The step is consumed before it acts, so a screen it opens continues with the next one.
		Next++;
		DebugString("UISCRIPT %s %s\n", step.Command.c_str(), step.Argument.c_str());

		StepHandler const handler = Find_Step_Handler(step.Command);
		if (handler == nullptr) {
			DebugString("UISCRIPT   unknown step %s\n", step.Command.c_str());
		} else if (handler(context, step, now)) {
			return;
		}
	}
}


bool UIScript_Fullscreen_Tick(void)
{
	while (Next < Steps.size()) {
		long long const now = Monotonic_Milliseconds();
		if (now < WaitUntil) {
			return(false);
		}

		UIStepType const step = Steps[Next];
		if (step.Command == "wait") {
			Next++;
			WaitUntil = now + std::atoi(step.Argument.c_str());
			return(false);
		} else if (step.Command == "shot") {
			Next++;
			DebugString("UISCRIPT %s %s\n", step.Command.c_str(), step.Argument.c_str());
			ShotPath = Screenshot_Name((step.Argument + ".tga").c_str());
			Backend_Request_Window_Capture(ShotPath.c_str());
			Video_Mark_Overlay_Dirty();
			WaitUntil = now + 500;
			return(false);
		} else if (step.Command == "key" && step.Argument == "escape") {
			Next++;
			DebugString("UISCRIPT %s %s\n", step.Command.c_str(), step.Argument.c_str());
			return(true);
		} else if (step.Command == "quit") {
			DebugString("UISCRIPT quit\n");
			std::exit(0);
		}
		return(false);
	}
	return(false);
}
