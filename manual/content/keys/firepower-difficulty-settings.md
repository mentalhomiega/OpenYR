---
key: FirePower
summary: A multiplier on a difficulty setting that this build reads but does not apply to damage.
no_effect: true
see_also: ["system:difficulty"]
when_omitted:
  kind: value
  value: "1"
  note: A later file that contains the section without this key resets it to 1.
---

`[Easy]`, `[Normal]` and `[Difficult]` each set a value, and this build reads it but does not apply it. No difficulty changes how much damage any object's weapons deal, for human and computer houses alike.

Yuri's Revenge does apply it: the damage of each weapon a house fires is multiplied by the figure for [its difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot). We left that out on purpose, so that the difficulty never changes what a unit can do. A weapon's damage is still scaled by [the country's `Firepower=`](/keys/firepower-housetype/) outside a campaign game, by a firepower crate and by the veteran firepower bonus ([`VeteranCombat`](/keys/veterancombat/)).
