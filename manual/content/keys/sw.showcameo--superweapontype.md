---
key: SW.ShowCameo
scope: superweapontype
summary: "Hides this superweapon's sidebar cameo when set to no."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, the house's sidebar never shows the weapon, although the house holds, charges and fires it. Combine it with `SW.AutoFire=yes` for a weapon that works without the player's input.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.ShowCameo=no
```

See [Firing automatically](/systems/superweapons/#firing-automatically).
