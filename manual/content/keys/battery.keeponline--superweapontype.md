---
key: Battery.KeepOnline
scope: superweapontype
summary: "Lists the structures a running battery keeps powered on."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

While a `Type=Battery` weapon of the house runs, structures of these types stay powered on when the house is short of power.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Battery.KeepOnline=GAPILL
```

See [Battery](/systems/superweapons/#battery).
