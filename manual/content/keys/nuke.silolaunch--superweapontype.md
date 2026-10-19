---
key: Nuke.SiloLaunch
scope: superweapontype
summary: "Chooses whether a silo launches the missile."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, the missile enters from the map edge nearest the target and needs no structure with `NukeSilo=yes`.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Nuke.SiloLaunch=no
```

See [Multi missile and chem missile](/systems/superweapons/#multi-missile-and-chem-missile).
