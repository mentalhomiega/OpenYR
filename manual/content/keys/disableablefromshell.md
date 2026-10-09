---
key: DisableableFromShell
summary: Whether the superweapons option can withhold this superweapon.
see_also: ["BuildTech", "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

`DisableableFromShell=yes` withholds this superweapon while the match's superweapons option is off. The option is `SuperWeapons=no` in the [launch file](/formats/spawn-ini/), or the Superweapons checkbox in the skirmish lobby. While it is off, the building that grants this superweapon cannot be built, and a house does not receive the superweapon from a building it already owns. A building listed in [`BuildTech`](/keys/buildtech/) is never withheld.

With the option on, the key has no effect. `DisableableFromShell=no` leaves the superweapon available whatever the option says.
