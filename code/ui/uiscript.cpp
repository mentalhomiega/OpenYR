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

#include "bgfxbackend.h"
#include "dbgprint.h"
#include "gamedirs.h"
#include "video.h"

#include <RmlUi/Core.h>

#include <chrono>
#include <cstdlib>
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

long long Now_Milliseconds(void)
{
	return(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}


Rml::Element * Find_Element(Rml::Context * context, std::string const & id)
{
	for (int index = context->GetNumDocuments() - 1; index >= 0; index--) {
		Rml::ElementDocument * document = context->GetDocument(index);
		if (document == nullptr || !document->IsVisible()) {
			continue;
		}
		if (Rml::Element * element = document->GetElementById(id)) {
			return(element);
		}
	}
	return(nullptr);
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
	text.erase(0, text.find_first_not_of(" \t\r\n"));
	text.erase(text.find_last_not_of(" \t\r\n") + 1);
	return(Rml::StringUtilities::DecodeRml(text));
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
	for (int index = context->GetNumDocuments() - 1; index >= 0; index--) {
		Rml::ElementDocument * document = context->GetDocument(index);
		if (document != nullptr && document->IsVisible()) {
			if (Rml::Element * found = Find_Button(document, label)) {
				return(found);
			}
		}
	}
	return(nullptr);
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
	for (int index = context->GetNumDocuments() - 1; index >= 0; index--) {
		Rml::ElementDocument * document = context->GetDocument(index);
		if (document != nullptr && document->IsVisible()) {
			if (Rml::Element * found = Find_Text(document, label)) {
				return(found);
			}
		}
	}
	return(nullptr);
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
	};
	for (auto const & entry : keys) {
		if (name == entry.Name) {
			key = entry.Key;
			return(true);
		}
	}
	return(false);
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
/// id), set (a slider's id and the value to give it, separated by a space), press (a button's
/// label, ignoring case), choose (the text of a list row or drop-down entry, or "id|text" to look only inside the element with that id), key (escape, return, tab, space, page up or down or an arrow, with ctrl+ or shift+ in front to hold that key),
/// shot (a name for a .tga in the screenshots folder), ids (logs the ids and buttons of every
/// visible screen), box (logs where an element and each element around it lie) and quit.
/// </summary>
void UIScript_Tick(Rml::Context * context)
{
	if (context == nullptr) {
		return;
	}

	while (Next < Steps.size()) {
		long long const now = Now_Milliseconds();
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

		if (step.Command == "wait") {
			WaitUntil = now + std::atoi(step.Argument.c_str());
			return;
		} else if (step.Command == "shot") {
			ShotPath = Screenshot_Name((step.Argument + ".tga").c_str());
			Backend_Request_Window_Capture(ShotPath.c_str());
			// A still screen is not shown again until something changes, so one is asked for.
			Video_Mark_Overlay_Dirty();
			WaitUntil = now + 500;
			return;
		} else if (step.Command == "ids") {
			for (int index = 0; index < context->GetNumDocuments(); index++) {
				Rml::ElementDocument * document = context->GetDocument(index);
				if (document != nullptr && document->IsVisible()) {
					DebugString("UISCRIPT   document %s\n", document->GetSourceURL().c_str());
					Log_Ids(document, 1);
					Log_Buttons(document);
				}
			}
		} else if (step.Command == "box") {
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
		} else if (step.Command == "click") {
			Rml::Element * element = Find_Element(context, step.Argument);
			if (element == nullptr) {
				DebugString("UISCRIPT   click %s: no such visible element\n", step.Argument.c_str());
			} else {
				element->Click();
				WaitUntil = now + 300;
				return;
			}
		} else if (step.Command == "set") {
			// "id value" sets the value of a slider, as dragging it would.
			std::size_t const space = step.Argument.find(' ');
			Rml::Element * element = Find_Element(context, step.Argument.substr(0, space));
			if (element == nullptr || space == std::string::npos) {
				DebugString("UISCRIPT   set %s: no such visible element or no value\n", step.Argument.c_str());
			} else {
				element->SetAttribute("value", step.Argument.substr(space + 1));
				WaitUntil = now + 300;
				return;
			}
		} else if (step.Command == "press") {
			Rml::Element * button = Find_Button(context, step.Argument);
			if (button == nullptr) {
				DebugString("UISCRIPT   press %s: no such visible button\n", step.Argument.c_str());
			} else {
				button->Click();
				WaitUntil = now + 300;
				return;
			}
		} else if (step.Command == "choose") {
			// "id|label" looks only inside the element with that id, such as one drop-down.
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
			} else {
				element->ScrollIntoView();
				element->Click();
				WaitUntil = now + 300;
				return;
			}
		} else if (step.Command == "key") {
			// A "ctrl+" or "shift+" in front of the name holds that key down.
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
				}
			}

			Rml::Input::KeyIdentifier key;
			if (!Key_From_Name(name, key)) {
				DebugString("UISCRIPT   key %s: unknown key\n", step.Argument.c_str());
			} else {
				context->ProcessKeyDown(key, modifiers);
				context->ProcessKeyUp(key, modifiers);
				WaitUntil = now + 300;
				return;
			}
		} else if (step.Command == "quit") {
			DebugString("UISCRIPT quit\n");
			std::exit(0);
		} else {
			DebugString("UISCRIPT   unknown step %s\n", step.Command.c_str());
		}
	}
}


bool UIScript_Fullscreen_Tick(void)
{
	while (Next < Steps.size()) {
		long long const now = Now_Milliseconds();
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
