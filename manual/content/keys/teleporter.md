---
key: Teleporter
summary: Makes a vehicle drive wherever it is sent and use its own locomotor only to reach the refinery it docks at.
see_also: ["Locomotor", "system:tiberium"]
when_omitted:
  kind: value
  value: "no"
---

A `Teleporter=yes` VehicleType keeps the [`Locomotor`](/keys/locomotor/) its type names, but drives every ordinary move: when it is sent somewhere, it switches to driving and carries its own locomotor along. It switches back only when it is sent onto a free docking cell of the refinery it is in radio contact with, and then it moves there with its own locomotor and enters the refinery.

```ini title="rulesmd.ini"
[MyMiner] ; example VehicleType
Locomotor={4A582747-9839-11D1-B709-00A024DDAFD1} ; Teleport
Teleporter=yes
Harvester=yes
```

With the Teleport locomotor, as Yuri's Revenge's Chrono Miner uses it, the harvester drives to the ore field and teleports home to unload.
