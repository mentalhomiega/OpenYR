---
key: AffectsEnemies
summary: "With no, the warhead does nothing to objects whose owner is not an ally of its attacker."
see_also: [AffectsAllies, Verses, "system:warheads"]
when_omitted:
  kind: value
  value: "yes"
---

An `AffectsEnemies=no` warhead [does no damage](/systems/warheads/#what-the-target-loses) to a vehicle, infantryman, aircraft or structure whose owner is not an ally of the attacker's house. An explosion with no credited attacker takes the house it is credited to instead; a hit with neither is not affected. Forced damage is affected too. The key does not change which targets a weapon chooses or accepts.

```ini title="rulesmd.ini"
[MyHealWH] ; example Warhead
Verses=-100%,-100%,-100%,0%,0%,0%,0%,0%,0%,0%,0%
AffectsEnemies=no
```

With `AffectsAllies=no` and `AffectsEnemies=no` together, the warhead damages nothing that has a credited attacker.
