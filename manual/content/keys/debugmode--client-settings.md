---
key: DebugMode
scope: client-settings
label: Debug mode
when_omitted:
  kind: value
  value: "no"
---

`DebugMode=yes` turns on the debug cheat commands for single-player games, which are campaign and skirmish games. The commands are in the Debug category of the keyboard options, and each one switches its cheat on or off. Ctrl+Shift+I, Ctrl+Shift+U, Ctrl+Shift+B and Ctrl+Shift+V are the default keys for infinite money, unlock everything, instant build and reveal map, unless the keyboard file already binds those keys.

With `no`, or in a multiplayer, network or playback game, the commands do nothing. Turning the setting off turns every cheat off. A new game or a loaded save starts with every cheat off. The first use of each cheat in a game is written to the debug log as `Debug cheat used:` followed by the cheat's name.

The display options screen has the same choice as its "Debug mode (for testing)" switch. The settings are saved to `RA2MD.INI` when the options screens close.

```ini title="RA2MD.INI"
[Options]
DebugMode=yes
```
