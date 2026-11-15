---
key: SW.ManualFire
scope: superweapontype
summary: "Whether clicking the cameo fires or aims the weapon."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no` on a weapon that has `SW.AutoFire=yes`, clicking the charged cameo does nothing. The key has no effect on a weapon without `SW.AutoFire=yes`.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.ManualFire=no
```

See [Firing automatically](/systems/superweapons/#firing-automatically).
