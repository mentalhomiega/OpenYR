---
key: DebugMode
scope: client-settings
label: Debug mode
when_omitted:
  kind: value
  value: "no"
---

`DebugMode=yes` turns on the debug cheat commands for single-player games, which are campaign and skirmish games. The commands are in the Debug category of the keyboard options. Infinite Money, Unlock Everything, Instant Build, Reveal Map, Full Power and Invulnerable Units are toggles: each command switches its cheat on or off. Win Mission, Lose Mission and Charge Superweapons run once each.

The default keys are Ctrl+Shift+I for infinite money, Ctrl+Shift+U for unlock everything, Ctrl+Shift+B for instant build, Ctrl+Shift+V for reveal map, Ctrl+Shift+P for full power, Ctrl+Shift+G for invulnerable units, Ctrl+Shift+W for win mission, Ctrl+Shift+L for lose mission and Ctrl+Shift+K for charge superweapons. A command gets its default key only when [`KEYBOARDMD.INI`](/formats/keyboard-ini/) binds neither that key nor that command.

With `no`, or in a multiplayer, network or playback game, the commands do nothing. Turning the setting off turns every toggle off. A new game or a loaded save starts with every toggle off. The first use of each cheat in a game is written to the debug log as `Debug cheat used:` followed by the cheat's name.

Full power makes the player's power count as full, so production runs at full speed and structures stay online. The raw power output shown on the sidebar does not change, and a blackout or a drained power plant still takes that plant offline.

Invulnerable units blocks damage to the player's units and buildings. Healing still applies, and other houses take damage as usual.

The display options screen has the same choice as its "Debug mode (for testing)" switch. The settings are saved to `RA2MD.INI` when the options screens close.

```ini title="RA2MD.INI"
[Options]
DebugMode=yes
```
