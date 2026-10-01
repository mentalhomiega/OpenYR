---
key: Poison
summary: "Marks a warhead as poison, so ImmuneToPoison types take no damage from it."
see_also: [ImmuneToPoison, "system:warheads"]
when_omitted:
  kind: value
  value: "no"
---

A `Poison=yes` warhead [does nothing](/systems/warheads/#what-the-target-loses) to a vehicle, infantryman, aircraft or structure whose type is [`ImmuneToPoison=yes`](/keys/immunetopoison/).

```ini title="rulesmd.ini"
[MyGas] ; example Warhead
Poison=yes
```
