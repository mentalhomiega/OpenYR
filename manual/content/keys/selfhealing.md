---
key: SelfHealing
summary: Mends the object a step at a time up to full strength.
see_also: ["SelfHealStep", "SelfHealRate", "SelfHealCap", "SelfHealingStep", "SelfHealingRate", "SelfHealingCap", "system:repair"]
when_omitted:
  kind: value
  value: "no"
---

`SelfHealing=yes` makes an object heal itself. It costs nothing, needs no building or order, and works on structures, vehicles, aircraft and infantry alike. The `SELF_HEAL` ability from [`VeteranAbilities`](/keys/veteranabilities/) or [`EliteAbilities`](/keys/eliteabilities/) has the same effect, so a promoted object heals exactly as a type with this flag does.

Each tick restores a set number of strength points, and ticks come at a set interval. At the engine defaults an object heals one point every 14 frames, the interval that [`RepairRate`](/keys/repairrate/) sets for structure repair. The type's [`SelfHealingStep`](/keys/selfhealingstep/) and [`SelfHealingRate`](/keys/selfhealingrate/) change the step and the interval for one type; [`SelfHealStep`](/keys/selfhealstep/) and [`SelfHealRate`](/keys/selfhealrate/) change them for every type. [Self-healing](/systems/repair/#self-healing) explains how the settings combine.

An object heals to full strength unless [`SelfHealingCap`](/keys/selfhealingcap/) or [`SelfHealCap`](/keys/selfhealcap/) sets a lower ceiling. With a ceiling, it stops healing once its strength passes it, which can leave it up to one step above it.
