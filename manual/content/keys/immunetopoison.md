---
key: ImmuneToPoison
summary: "Spares objects of this type from Poison warheads."
see_also: [Poison]
when_omitted:
  kind: value
  value: "no"
---

An object of this type [takes no damage](/systems/warheads/#what-the-target-loses) from a [`Poison=yes`](/keys/poison/) warhead.

```ini title="rulesmd.ini"
[MYGASSNIPER] ; example InfantryType
ImmuneToPoison=yes
```
