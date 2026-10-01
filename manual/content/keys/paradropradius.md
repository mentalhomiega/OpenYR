---
key: ParadropRadius
summary: "How close to its target a paradrop plane drops its paratroopers, in leptons."
see_also: [AllyParaDropInf, "system:superweapons"]
when_omitted:
  kind: value
  value: "1024"
---

A paradrop plane drops a paratrooper every 5 frames while it is within this distance of its target, measured along the ground. 256 leptons make one cell, so the default is four cells. [Paradrops](/systems/superweapons/#paradrops) covers the flight and the drop.

```ini title="rulesmd.ini"
[General]
ParadropRadius=1024
```
