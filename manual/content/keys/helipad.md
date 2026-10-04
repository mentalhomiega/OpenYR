---
key: Helipad
summary: Lets a BuildingType accept an aircraft as a docking target.
see_also: ["system:ai-base-building"]
when_omitted:
  kind: value
  value: "no"
---

An allied aircraft may dock with a `Helipad=yes` structure. The pad refuses any other kind of object. Like every docking structure, it refuses while it is being built or sold, switched off, or already serving as many objects as it has [docks](/keys/numberofdocks--buildingtype/). On a structure that also sets [`UnitRepair=yes`](/keys/unitrepair/), the repair bay's docking rules decide instead.

A plain pad neither repairs a docked aircraft nor restores its ammunition. A [`UnitReload=yes`](/keys/unitreload/) pad does both: it rearms a docked aircraft, then repairs it once its ammunition is full. The stock pads set `UnitReload=yes`.

A player's aircraft ordered onto a pad docks there only while the pad has a free dock and carries no cargo. An aircraft heading for a pad that is already taken looks for another free structure of a type its [`Dock=`](/keys/dock/) lists. If it finds none, it lands at a clear landing spot instead. An empty carryall ordered into a pad lands at ground level.

A [`FreeUnit=`](/keys/freeunit/) aircraft handed out by a pad stays docked on it. It stands at the pad's center, not on its dock's [`DockingOffsetN=`](/keys/numberofdocks--buildingtype/) spot.

While [the base plan is assembled](/systems/ai-base-building/#building-the-plan), a computer house adds a type with this flag one to three extra times, so its plan holds two to four of that pad. A house whose side has a [base defense count list](/systems/ai-base-building/#the-counted-plan) adds no extra copies.
