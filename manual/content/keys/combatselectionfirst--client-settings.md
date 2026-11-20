---
key: CombatSelectionFirst
scope: client-settings
label: Select combat units first
summary: "Whether a selection box or Select View that holds combat units leaves out the unarmed ones."
see_also: [PrioritySelectionFiltering]
when_omitted:
  kind: value
  value: "yes"
---

With `CombatSelectionFirst=yes`, a selection box or the Select View command that catches at least one combat unit leaves out the unarmed units it caught, such as ore miners, engineers, an MCV, an unarmed transport or a spy. A combat unit is an object with a weapon that is not an ore or vein harvester or an engineer, so a harvester with a gun counts as a miner. A box that catches no combat unit selects what it caught, and a click, a select-by-type command and a group key are not changed. A box dragged with Shift held is judged the same way, on what it catches.

With `CombatSelectionFirst=no`, a box selects every selectable object it catches, as Yuri's Revenge does. The Game page of the settings sets the key in the `[Options]` section of `RA2MD.INI`.

The Select Combatants command is not affected. It selects the units whose type has `Trainable=yes`, as Yuri's Revenge does.
