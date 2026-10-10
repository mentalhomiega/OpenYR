---
key: ROF
scope: difficulty-settings
label: Difficulty reload multiplier
no_effect: true
see_also: ["system:difficulty"]
when_omitted:
  kind: value
  value: "1"
  note: Every file that contains the difficulty section resets the settings it leaves out, so a later file with the section but without this key restores 1.
---

`[Easy]`, `[Normal]` and `[Difficult]` each set their own multiplier, and this build reads it but does not apply it. No difficulty changes how fast any object fires, for human and computer houses alike.

Yuri's Revenge does apply it: a house multiplies the delay between its shots by the figure for [its difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot), so a value above 1 fires more slowly. We left that out on purpose, so that the difficulty never changes what a unit can do. A weapon's delay comes from [its `ROF`](/keys/rof/#scope-weapontype) and, outside a campaign game, the [country's multiplier](/keys/rof/#scope-housetype).
