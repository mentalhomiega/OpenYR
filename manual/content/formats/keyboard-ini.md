---
format_id: keyboard-ini
title: KEYBOARDMD.INI
summary: Binds registered command names to keys, with a secondary key, a trigger for each key and the control scheme.
kind: file
filenames:
  - KEYBOARDMD.INI
source_files:
  - code/init.cpp
  - code/keyboard.h
  - code/keybinddefaults.cpp
  - code/keybindings.cpp
  - code/hotkeytiming.cpp
  - code/options.cpp
---

The file is `KEYBOARDMD.INI` unless [`Keyboard=`](/formats/opents-ini/#the-files-it-reads) in `OPENTS.INI` names another. Each command can hold two keys, a primary key and a secondary key. Each key has a trigger that says when it fires: a press, a double press, a hold, or a press held down. The name before `=` is the command's exact [registered name](/commands/), including its case.

```ini title="KEYBOARDMD.INI"
[Defaults]
Version=5
Scheme=Modern
DoublePressMs=250
HoldMs=400

[Hotkey]
TeamSelect_1=49
TeamCenter_1=49
ScrollNorth=38
AttackMove=65

[HotkeyTrigger]
TeamCenter_1=Double
ScrollNorth=Held

[Hotkey2]
CenterBase=72
Options=121
```

## Sections

| Section | Holds |
| --- | --- |
| `[Defaults]` | The version, the control scheme and the two timings. |
| `[Hotkey]` | Each command's primary key. |
| `[Hotkey2]` | Each command's secondary key. |
| `[HotkeyTrigger]` | The trigger of each primary key. A command with no line here fires on a press. |
| `[HotkeyTrigger2]` | The trigger of each secondary key. |

A key is a keyboard identifier: the key's Windows virtual-key code, plus 256 for Shift, 512 for Control and 1024 for Alt. Zero is no key. A binding of modifier bits alone, such as 768 for Control with Shift, has no key. Only the commands that read a held key take one: `AttackMove`, `ForceFire`, `ForceMove`, `SelectModifier`, `QueueMove` and `GridBuildModifier`. The game reads such a binding as held while its modifiers are down. A key with modifier bits, such as 856 for Control, Shift and X, fires only when those modifiers are down with it. The mouse and wheel use these codes:

| Code | Input |
| --- | --- |
| 1 and 2 | Left and right mouse button. The game uses them for orders, so a binding to them is not advised. |
| 4 | Middle mouse button (mouse 3). |
| 5 and 6 | Side mouse buttons (mouse 4 and mouse 5). |
| 7 | Wheel up. |
| 14 | Wheel down. |

A wheel step that a binding takes replaces the built-in action of that step: over the map the wheel zooms and elsewhere it scrolls the sidebar.

### Triggers

| Trigger | Fires |
| --- | --- |
| `Press` | When the key goes down. This is the default. |
| `Double` | On the second press that comes within `DoublePressMs` of the first. |
| `Hold` | When the key has stayed down for `HoldMs`. A quick tap fires the press instead, if the key has one. |
| `Held` | When the key goes down, and again every 40 ms while it stays down. The modifier commands use it to read whether a key is held. |

A key with a `Double` or `Hold` binding and a `Press` binding waits before its press fires. The press waits for the double-press window, or for the release when the key has a hold. A key with neither binding fires its press at once.

### Scheme and timings

`Scheme=` is `Classic` or `Modern`. A file with no scheme is Classic. The scheme decides which bindings a command gets when the file does not name it, and which bindings the keyboard dialog's reset restores.

`DoublePressMs=` and `HoldMs=` are in milliseconds, from 50 to 2000. A value outside that range, or one that is not a number, gives the default: 250 and 400.

## The keyboard dialog

A key pressed with Shift, Ctrl or Alt is captured with those modifiers, such as Control, Alt and X. Shift, Ctrl or Alt pressed and released with no other key is captured alone, such as Control with Shift. The capture box shows the keys while they are held. A command that needs a key press refuses a modifier-only capture, and its notice says so. The dialog also refuses the left and right mouse buttons and a key it cannot name.

Reset to defaults and Reset category unbind each command that the scheme gives no key, and the notice names it. The restored defaults replace the bindings of the commands they cover. Classic gives [Attack move](/commands/attackmove/) Control with Shift held, so a Classic reset of Commands replaces a Mouse 5 binding to it. A scheme change replaces every binding.

## Rules for a line

- A key that is not a number, or is `0`, is ignored.
- A command named twice in the same section keeps its first line.
- A line whose key and trigger the file already gives another command is ignored, so no two bindings share a key and a trigger. A `Press` and a `Double` of one key do not share one, and can belong to two commands.
- A trigger name other than `Press`, `Double`, `Hold` or `Held` is read as `Press`.
- A command name that is not registered is ignored.

## Version

The `[Defaults]` section's `Version=` records the defaults the file was written against. The keyboard dialog writes version 5. A file with no `Version=` is version 1, which the earlier builds wrote; a file with `Version=2` is version 2.

- Version 5 is read as saved. A command the file does not name has no key.
- Version 4 is read as saved, except that a Classic file that does not name [Attack move](/commands/attackmove/) gains Control with Shift held, unless another binding holds that chord.
- Version 3 is read as saved, except that the grid build modifier of Modern and the Classic attack-move chord are filled in when the file does not name them and no other binding holds their key. A command the file does not name otherwise has no key.
- Version 2 is read as saved, except that the held modifiers version 3 added are filled in when the file does not name them: `ForceFire` on Control, `ForceMove` on Alt, `SelectModifier` on Shift and `QueueMove` on Q. The grid build modifier and the attack-move chord are filled in when the file does not name them and their key is free. Those keys stay in use, so force fire and force move work as before.
- Version 1 is upgraded to the current defaults of its scheme in memory, as each earlier build upgraded it. An entry that still has its old default takes the current default, or loses its key if the scheme gives that command none. A key the file binds to a command is kept, and a default that would take that key is dropped. A command the file does not name gets its current default, if that key is free.

The upgrade happens in memory. The file keeps its version until the keyboard dialog saves it.

The game binds Delete to [`DeleteWaypoint`](/commands/deletewaypoint/) and Escape to [`Options`](/commands/options/), whatever the file says. If another command holds one of those keys, it takes the key the built-in command had, or loses its key when the built-in command had none. A key the file gives either built-in command is kept, so that command can have two keys.

At startup the game also binds Enter to [`ChatToAll`](/commands/chattoall/) and Backspace to [`ChatToAllies`](/commands/chattoallies/), each only when the file binds neither that command nor that key.

:::caution[Bind each key once for each trigger]
Give each key and trigger to only one command. A file that binds the same key and trigger twice keeps the first binding and drops the second, so the result depends on the order of the lines.
:::
