---
key: ButtonList
scope: ui-controls
label: Command bar buttons
summary: The commands on the command bar along the bottom of the screen, left to right.
see_also: ["system:sidebar"]
when_omitted:
  kind: value
  value: ""
---

`ButtonList=` names the command bar's buttons, separated by commas, in the order they appear from the left. UIMD.INI reads it from two sections: `[AdvancedCommandBar]` serves campaign and skirmish games, and `[MultiplayerAdvancedCommandBar]` serves every other game. [The command bar](/systems/sidebar/#the-command-bar) lists the names and what each button does.

Names are matched exactly, including letter case. A name the engine does not know still takes up a slot and leaves a gap; an empty entry between two commas takes up none. An omitted section leaves the bar without buttons.

```ini title="UIMD.INI"
[AdvancedCommandBar]
ButtonList=Team01,Team02,Team03,TypeSelect,Deploy,Guard,Stop
```
