---
key: ChronoErasedSuperWeapons
scope: global-rules
label: Superweapons of chrono-erased structures
summary: Whether a superweapon that a structure grants stays usable while a Chrono Legionnaire erases that structure.
see_also: ["system:superweapons", "system:power", "IsPowered", "ChronoDelay"]
when_omitted:
  kind: value
  value: "no"
  note: gamemd suspends such a weapon, so `no` matches the original engine.
---

```ini title="rules.ini"
[General]
ChronoErasedSuperWeapons=no
```

With `no`, a structure that a temporal beam is erasing does not enable the superweapons it grants. A weapon whose type sets [`IsPowered=yes`](/keys/ispowered/) is then suspended, as it is while its house is short of power: it cannot be fired, the sidebar shows the hold, and its charge follows the rules of that key. A weapon whose type sets `IsPowered=no` is not suspended, so it stays usable and keeps charging.

With `yes`, the weapon stays usable and keeps charging for as long as the beam holds the structure.

When the beam lets go of the structure, its weapons are enabled again and a suspended weapon resumes. If the erase finishes, the structure is removed. Its weapon is then lost, unless another structure of the house that is switched on and not being erased grants it.
