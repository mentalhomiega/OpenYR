/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "hotkeytiming.h"

#include <utility>


HotkeyTimingClass::HotkeyTimingClass(void) :
	DoublePressMs(HOTKEY_DOUBLE_PRESS_MS),
	HoldMs(HOTKEY_HOLD_MS),
	RepeatMs(HOTKEY_HELD_REPEAT_MS)
{
}


void HotkeyTimingClass::Set_Timing(unsigned int double_press_ms, unsigned int hold_ms, unsigned int repeat_ms)
{
	DoublePressMs = double_press_ms;
	HoldMs = hold_ms;
	RepeatMs = repeat_ms;
}


void HotkeyTimingClass::Set_Triggers(int key, bool press, bool doubled, bool hold, bool held)
{
	if (!press && !doubled && !hold && !held) {
		Bindings.erase(key);
		return;
	}
	Triggers & triggers = Bindings[key];
	triggers.Press = press;
	triggers.Doubled = doubled;
	triggers.Hold = hold;
	triggers.Held = held;
}


void HotkeyTimingClass::Clear(void)
{
	Bindings.clear();
	Downs.clear();
	Taps.clear();
}


void HotkeyTimingClass::Reset(void)
{
	Downs.clear();
	Taps.clear();
}


HotkeyTimingClass::Triggers HotkeyTimingClass::Triggers_Of(int key) const
{
	std::map<int, Triggers>::const_iterator found = Bindings.find(key);
	return((found != Bindings.end()) ? found->second : Triggers());
}


int HotkeyTimingClass::Base_Of(int key)
{
	return(key & 0xFF);
}


std::vector<HotkeyTimingClass::Fire> HotkeyTimingClass::Press(int key, unsigned int now)
{
	std::vector<Fire> fired;
	Poll_Taps_Into(fired, now);

	Triggers const triggers = Triggers_Of(key);
	Down down;
	down.Key = key;
	down.PressedAt = now;
	down.LastRepeat = now;

	if (triggers.Held) {
		fired.push_back({ key, HOTKEY_HELD });
		down.Held = true;
	}

	bool matched = false;
	if (triggers.Doubled) {
		for (std::vector<Pending>::iterator tap = Taps.begin(); tap != Taps.end(); ++tap) {
			if (tap->Key == key) {
				Taps.erase(tap);
				fired.push_back({ key, HOTKEY_DOUBLE });
				matched = true;
				break;
			}
		}
	}

	if (!matched) {
		if (triggers.Hold) {
			down.HoldPending = true;
		} else if (triggers.Doubled) {
			Taps.push_back({ key, now });
		} else if (triggers.Press) {
			fired.push_back({ key, HOTKEY_PRESS });
		}
	}

	Downs[Base_Of(key)] = down;
	return(fired);
}


std::vector<HotkeyTimingClass::Fire> HotkeyTimingClass::Release(int key)
{
	std::vector<Fire> fired;
	std::map<int, Down>::iterator found = Downs.find(Base_Of(key));
	if (found == Downs.end()) {
		return(fired);
	}

	Down const down = found->second;
	Downs.erase(found);

	// A press that never reached its hold time was a tap. It counts toward a double press when the key has one, and fires its press otherwise.
	if (down.HoldPending) {
		Triggers const triggers = Triggers_Of(down.Key);
		if (triggers.Doubled) {
			Taps.push_back({ down.Key, down.PressedAt });
		} else if (triggers.Press) {
			fired.push_back({ down.Key, HOTKEY_PRESS });
		}
	}

	return(fired);
}


bool HotkeyTimingClass::Is_Held(int key) const
{
	std::map<int, Down>::const_iterator found = Downs.find(Base_Of(key));
	return(found != Downs.end() && found->second.Key == key && found->second.Held);
}


std::vector<HotkeyTimingClass::Fire> HotkeyTimingClass::Poll(unsigned int now)
{
	std::vector<Fire> fired;
	Poll_Taps_Into(fired, now);

	for (std::map<int, Down>::iterator entry = Downs.begin(); entry != Downs.end(); ++entry) {
		Down & down = entry->second;

		if (down.HoldPending && now - down.PressedAt >= HoldMs) {
			down.HoldPending = false;
			fired.push_back({ down.Key, HOTKEY_HOLD });
		}

		if (down.Held && now - down.LastRepeat >= RepeatMs) {
			down.LastRepeat = now;
			fired.push_back({ down.Key, HOTKEY_HELD });
		}
	}
	return(fired);
}


/// <summary>
/// Fires the press of each waiting tap whose double-press window has ended, and drops those taps.
/// </summary>
void HotkeyTimingClass::Poll_Taps_Into(std::vector<Fire> & fired, unsigned int now)
{
	for (std::vector<Pending>::iterator tap = Taps.begin(); tap != Taps.end();) {
		if (now - tap->At < DoublePressMs) {
			++tap;
			continue;
		}
		if (Triggers_Of(tap->Key).Press) {
			fired.push_back({ tap->Key, HOTKEY_PRESS });
		}
		tap = Taps.erase(tap);
	}
}
