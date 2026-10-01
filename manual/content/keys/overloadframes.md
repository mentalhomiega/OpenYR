---
key: OverloadFrames
summary: "How long an overloaded mind control firer waits at each overload step."
see_also: [OverloadCount, OverloadDamage, "system:mind-control"]
when_omitted:
  kind: value
  value: none
---

Each entry is how many frames an [`InfiniteMindControl=yes`](/keys/infinitemindcontrol/) firer waits before its next overload look when [`OverloadCount`](/keys/overloadcount/) picks that position. A position past the end of the list means the next look comes on the following frame. [Overload](/systems/mind-control/#overload) covers the cycle.

```ini title="rulesmd.ini"
[CombatDamage]
OverloadCount=3,6,10,50
OverloadFrames=30,60,60,60
```
