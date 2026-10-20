---
key: RelativeDamage.Buildings
scope: warheadtype
summary: "The percentage a RelativeDamage warhead takes from a structure."
see_also: [RelativeDamage]
when_omitted:
  kind: value
  value: "0, so a structure takes no damage"
---

A percentage from -100 to 100, cut to that range when read. A [`RelativeDamage=yes`](/keys/relativedamage/) warhead takes this share of the full `Strength=` of a structure when the value is above 0, and the same share of its current strength when the value is below 0. With `0`, the hit does no damage. The share is rounded down.

```ini title="rulesmd.ini"
[MadWH] ; example WarheadType
RelativeDamage=yes
RelativeDamage.Buildings=-25 ; a quarter of the current strength
```
