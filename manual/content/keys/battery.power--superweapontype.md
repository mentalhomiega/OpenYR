---
key: Battery.Power
scope: superweapontype
summary: "Sets the power a running battery adds."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "0"
---

The house's power output rises by this amount while a `Type=Battery` weapon runs, whatever damage its base has taken. The weapon runs from the click that switches it on until its [charge drain](/systems/superweapons/#charge-draining-weapons) ends.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Battery.Power=500
```

See [Battery](/systems/superweapons/#battery).
