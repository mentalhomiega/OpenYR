---
key: Battery.Overpower
scope: superweapontype
summary: "Lists the structures a running battery overpowers."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

While a `Type=Battery` weapon of the house runs, structures of these types count as overpowered, with or without `Overpowerable=yes`.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Battery.Overpower=GAPILL
```

See [Battery](/systems/superweapons/#battery).
