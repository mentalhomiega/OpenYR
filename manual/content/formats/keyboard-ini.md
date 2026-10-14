---
format_id: keyboard-ini
title: KEYBOARDMD.INI
summary: Maps registered command names to integer keyboard identifiers.
kind: file
filenames:
  - KEYBOARDMD.INI
source_files:
  - code/init.cpp
  - code/keyboard.h
  - code/keybinddefaults.cpp
  - code/options.cpp
---

The file is `KEYBOARDMD.INI` unless [`Keyboard=`](/formats/opents-ini/#the-files-it-reads) in `OPENTS.INI` names another. Each assignment in `[Hotkey]` binds a key to a command. The name before `=` is the command's exact [registered name](/commands/), including its case. The value is a keyboard identifier: the key's Windows virtual-key code, plus 256 for Shift, 512 for Control and 1024 for Alt.

```ini title="KEYBOARDMD.INI"
[Defaults]
Version=2

[Hotkey]
SelectView=577      ; A with Control
ToggleRepair=338    ; R with Shift
ScatterObject=88    ; X
```

A command name can appear only once in the section, so the file gives each command at most one key. If a name is repeated, the later value is used.

The game reads the file at startup. A loose `KEYBOARDMD.INI` in the game directory replaces one inside an archive. An entry is ignored if its command name is not registered or its value is `0`.

Each command has a default key. With no file, or with a file that has no section header, every command has its default key. A file with a `[Hotkey]` section replaces the defaults, so a command the file does not name has no key, unless the file is version 1 (see [Version](#version)).

### Version

The `[Defaults]` section's `Version=` records the defaults the file was written against. The keyboard dialog writes version 2. A file with no `Version=` is version 1, which is what the earlier builds wrote. The game upgrades a version 1 file each time it reads it:

- An entry that still has its old default takes the current default, or loses its key if the current defaults give that command none.
- A key bound to a command in the file is kept. A default that would take that key is dropped.
- A command the file does not name gets its current default, if that key is free.

The upgrade happens in memory. The file keeps version 1 until the keyboard dialog saves it.

The game binds Delete to [`DeleteWaypoint`](/commands/deletewaypoint/) and Escape to [`Options`](/commands/options/). If the file binds either key, one binding for that key is removed, so a key the file binds once goes only to the built-in command. A key the file gives either command is kept, so that command can have two keys.

The keyboard dialog saves the bindings the player accepts to a loose `KEYBOARDMD.INI`, and canceling it discards the changes. Its reset control deletes the loose file at once, even if the dialog is then canceled, and reads the defaults again.

At startup the game also binds Enter to [`ChatToAll`](/commands/chattoall/) and Backspace to [`ChatToAllies`](/commands/chattoallies/), each only when the file binds neither that command nor that key. A file that binds the command to another key, or gives the key to another command, keeps its binding.

:::caution[Bind each key once]
Give each key to only one command. If two entries bind the same key, both are kept, and pressing the key runs only one of the two commands. Which one runs cannot be predicted from the file.
:::
