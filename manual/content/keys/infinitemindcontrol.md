---
key: InfiniteMindControl
summary: "Lets a mind control weapon take objects without limit, at the cost of overload damage to its firer."
see_also: [MindControl, OverloadCount, "system:mind-control"]
when_omitted:
  kind: value
  value: "no"
---

Without this key, a mind control weapon's firer holds at most the weapon's `Damage` in objects. With it, the firer keeps taking objects and suffers [overload](/systems/mind-control/#overload) damage set by how many it holds.

```ini title="rulesmd.ini"
[MultipleMindControlTank] ; example Weapon
Damage=3
Warhead=Controller ; a MindControl=yes warhead
InfiniteMindControl=yes
```
