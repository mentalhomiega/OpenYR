---
key: TogglePower
summary: Whether the player may switch this structure on and off.
see_also: ["system:power"]
when_omitted:
  kind: value
  value: "yes"
---

`TogglePower=no` stops the player switching the structure on or off with the power cursor. [Switching a structure off](/systems/power/#switching-a-structure-off) lists the other tests the cursor applies.

It does not keep the structure switched on. The [Turn off building](/mapping/actions/taction-turn-off-attached/) trigger action and a scenario's structure record can switch it off regardless, and the player then cannot switch it back on.

`TogglePower=no` does not change what a power shortfall does to a [`Powered=yes`](/keys/powered/) structure with drain. Such a structure goes out of service in a shortfall, and its spotlight, laser fence and cloak field stop with it, unless two soldiers [charge](/systems/power/#overpowered-defenses) it.

[Defenses](/systems/power/#defenses) defines operational and sets these tests side by side.
