---
key: AffectsAllies
summary: "With no, the warhead does nothing to objects of its attacker's allies."
see_also: [Verses, "system:warheads"]
when_omitted:
  kind: value
  value: "yes"
---

An `AffectsAllies=no` warhead [does no damage](/systems/warheads/#what-the-target-loses) to a vehicle, infantryman, aircraft or structure whose owner is an ally of the credited attacker's house, including the attacker's own house. Forced damage, and damage with no credited attacker, are not affected.

```ini title="rulesmd.ini"
[MyPsiWave] ; example Warhead
AffectsAllies=no
```
