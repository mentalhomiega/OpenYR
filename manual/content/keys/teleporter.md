---
key: Teleporter
summary: Lets an organic unit survive the chronosphere, and makes a vehicle drive wherever it is sent but use its own locomotor to reach the refinery it docks at.
see_also: ["Locomotor", Organic, "system:tiberium", "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

A `Teleporter=yes` type is moved by the [chronosphere](/systems/superweapons/#chronosphere) even when it is [`Organic=yes`](/keys/organic/), as infantry are by default. Any other organic unit the chronosphere picks up is destroyed instead.

A `Teleporter=yes` VehicleType also keeps the [`Locomotor`](/keys/locomotor/) its type names, but drives every ordinary move: when it is sent somewhere, it switches to driving and carries its own locomotor along. It switches back only when it is sent onto a free docking cell of the refinery it is in radio contact with, and then it moves there with its own locomotor and enters the refinery.

```ini title="rulesmd.ini"
[MyMiner] ; example VehicleType
Locomotor={4A582747-9839-11D1-B709-00A024DDAFD1} ; Teleport
Teleporter=yes
Harvester=yes
```

With the Teleport locomotor, as Yuri's Revenge's Chrono Miner uses it, the harvester drives to the ore field and teleports home to unload.

A teleport waits before it jumps: the warp-out wait is set by [`ChronoTrigger`](/keys/chronotrigger/), [`ChronoDistanceFactor`](/keys/chronodistancefactor/), [`ChronoMinimumDelay`](/keys/chronominimumdelay/) and [`ChronoRangeMinimum`](/keys/chronorangeminimum/). The object then lands and holds still for [`ChronoDelay`](/keys/chronodelay/) frames, and its AI waits during both waits. An order given while it holds starts the next warp once the hold ends.
