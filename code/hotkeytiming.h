/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "keybinddefaults.h"

#include <map>
#include <vector>

// Decides when each binding fires from the key events and the time. The caller passes the time in
// milliseconds on any clock that only moves forward, so a test can pass its own times.
//
// Keys are the queue's identifiers: a virtual-key code with its modifier bits. A release is matched
// to its press by the virtual-key code alone, so a modifier that is let go first does not strand the
// press.
//
// A key with no Double or Hold binding fires its Press at once, as it did before triggers existed.
// A key with a Double binding waits out the double-press window before its Press fires, so the
// window is the only extra delay a single press can see. A key with a Hold binding decides its Press
// at the release.
class HotkeyTimingClass
{
	public:
		// One binding to run now: the key it is bound to and the trigger that fired.
		struct Fire
		{
			int Key;
			HotkeyTrigger Trigger;
		};

		HotkeyTimingClass(void);

		// Sets which triggers the key has. A key with no trigger set gets none of them.
		void Set_Triggers(int key, bool press, bool doubled, bool hold, bool held);

		// Forgets every trigger and every pending decision.
		void Clear(void);

		// Forgets the keys that are down and the presses that wait, but keeps the triggers. Use it when the
		// game stops reading the keyboard, so no key that was up in the meantime fires later.
		void Reset(void);

		// Gives the bindings the key press fires now. A press that waits returns nothing.
		std::vector<Fire> Press(int key, unsigned int now);

		// Gives the bindings the key release fires now.
		std::vector<Fire> Release(int key);

		// Whether the key is down and its press started a Held binding, as while a player holds a modifier.
		bool Is_Held(int key) const;

		// Gives the bindings that fall due by now: a waiting press, a hold and a repeat of a held key.
		std::vector<Fire> Poll(unsigned int now);

		// The times that set the windows, in milliseconds.
		void Set_Timing(unsigned int double_press_ms, unsigned int hold_ms, unsigned int repeat_ms);

	private:
		struct Triggers
		{
			bool Press = false;
			bool Doubled = false;
			bool Hold = false;
			bool Held = false;
		};

		// A press that is still waiting for its release or for the double-press window to end.
		struct Pending
		{
			int Key = 0;
			unsigned int At = 0;
		};

		// The state of a key that is down.
		struct Down
		{
			int Key = 0;
			unsigned int PressedAt = 0;
			bool HoldPending = false;
			bool Held = false;
			unsigned int LastRepeat = 0;
		};

		Triggers Triggers_Of(int key) const;
		static int Base_Of(int key);
		void Poll_Taps_Into(std::vector<Fire> & fired, unsigned int now);

		std::map<int, Triggers> Bindings;
		std::map<int, Down> Downs;
		std::vector<Pending> Taps;

		unsigned int DoublePressMs;
		unsigned int HoldMs;
		unsigned int RepeatMs;
};
