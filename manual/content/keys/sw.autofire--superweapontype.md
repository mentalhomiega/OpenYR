---
key: SW.AutoFire
scope: superweapontype
summary: "Fires the weapon as soon as it is ready, at the cell the computer's rules pick."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

A house a person controls checks twice a second. A computer house checks in its superweapon pass. With [`SW.ManualFire=no`](/keys/sw.manualfire/) as well, clicking the cameo does nothing.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.AutoFire=yes
```

See [Firing automatically](/systems/superweapons/#firing-automatically).
