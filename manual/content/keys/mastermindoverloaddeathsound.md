---
key: MasterMindOverloadDeathSound
summary: "The sound an overloaded mind control firer plays when the overload starts hurting it."
see_also: [OverloadDamage, InfiniteMindControl, "system:mind-control"]
when_omitted:
  kind: value
  value: none
---

Plays at the firer on its first [overload](/systems/mind-control/#overload) hit. It plays again only after a look at the overload table finds no damage to deal.

```ini title="rulesmd.ini"
[AudioVisual]
MasterMindOverloadDeathSound=MyOverload ; a sound ID registered in SOUNDMD.INI
```
