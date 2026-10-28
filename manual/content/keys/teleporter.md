---
key: Teleporter
summary: Lets an organic unit survive the chronosphere, and makes a vehicle drive wherever it is sent but use its own locomotor to reach the refinery it docks at.
see_also: ["Locomotor", Organic, "system:tiberium", "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

A `Teleporter=yes` type is moved by the [chronosphere](/systems/superweapons/#chronosphere) even when it is [`Organic=yes`](/keys/organic/), as infantry are by default. Any other organic unit the chronosphere picks up is destroyed instead.

A `Teleporter=yes` VehicleType also keeps the [`Locomotor`](/keys/locomotor/) its type names, but drives every ordinary move: when it is sent somewhere, it switches to driving and carries its own locomotor along. It switches back only when it is sent onto a free docking cell of the refinery it is in radio contact with, and then it moves there with its own locomotor and enters the refinery. A teleporter that a Magnetron has lifted, or that is otherwise immobilized, keeps the locomotor it has and does not switch.

```ini title="rulesmd.ini"
[MyMiner] ; example VehicleType
Locomotor={4A582747-9839-11D1-B709-00A024DDAFD1} ; Teleport
Teleporter=yes
Harvester=yes
```

With the Teleport locomotor, as Yuri's Revenge's Chrono Miner uses it, the harvester drives to the ore field and teleports home to unload.

A teleport jumps as soon as its order is processed: the object leaves its cell and appears at the destination in the same frame. It then holds still for the delay set by [`ChronoTrigger`](/keys/chronotrigger/), [`ChronoDistanceFactor`](/keys/chronodistancefactor/), [`ChronoMinimumDelay`](/keys/chronominimumdelay/) and [`ChronoRangeMinimum`](/keys/chronorangeminimum/). An order given during the hold is refused. A soldier killed during the hold starts its death sequence at once, and the sequence advances during the hold.

A teleport chooses its landing spot when the order is given, and the object does not jump if it has none. A vehicle lands in the middle of the cell it is sent to. When it cannot enter that cell, it lands in the middle of a nearby cell it can enter, chosen from the cells of the closest ring that has any by the game frame. A soldier lands on the free spot of the target cell that is nearest the order, and a soldier whose `Enter`, `Capture`, `Eaten` or `Patrol` order names a unit, aircraft or structure on that cell lands on the cell with it, so its arrival can board or enter it. A soldier that cannot enter the cell and has a unit or structure as its target lands near that target. Otherwise the order ends and the soldier stays where it is. A soldier or vehicle standing on the cell is never removed.
