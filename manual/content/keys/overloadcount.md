---
key: OverloadCount
summary: "The numbers of held objects that choose each overload step."
see_also: [OverloadDamage, OverloadFrames, InfiniteMindControl, "system:mind-control"]
when_omitted:
  kind: value
  value: none
---

An [`InfiniteMindControl=yes`](/keys/infinitemindcontrol/) firer uses the first entry at or above the number of objects it holds, or the last entry when none is that high. The entries at the same position in [`OverloadDamage`](/keys/overloaddamage/) and [`OverloadFrames`](/keys/overloadframes/) set the damage and the wait. [Overload](/systems/mind-control/#overload) covers the cycle.

```ini title="rulesmd.ini"
[CombatDamage]
OverloadCount=3,6,10,50
OverloadDamage=0,50,100,500
OverloadFrames=30,60,60,60
```

With the key unset, no firer takes overload damage.
