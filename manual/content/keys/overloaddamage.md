---
key: OverloadDamage
summary: "The damage an overloaded mind control firer takes at each overload step."
see_also: [OverloadCount, OverloadFrames, MasterMindOverloadDeathSound, "system:mind-control"]
when_omitted:
  kind: value
  value: none
---

Each entry is the damage an [`InfiniteMindControl=yes`](/keys/infinitemindcontrol/) firer takes when [`OverloadCount`](/keys/overloadcount/) picks its position. The hit is a [`C4Warhead`](/keys/c4warhead/) hit, so the firer's armor applies to it and an active Iron Curtain stops it. A zero entry, or a position past the end of the list, deals nothing. [Overload](/systems/mind-control/#overload) covers the cycle.

```ini title="rulesmd.ini"
[CombatDamage]
OverloadCount=3,6,10,50
OverloadDamage=0,50,100,500
```
