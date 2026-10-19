---
key: Nuke.Payload
scope: superweapontype
summary: "Names the weapon a missile weapon launches."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "The section's WeaponType"
---

A weapon section. A silo launch uses it in place of [`WeaponType=`](/keys/weapontype/). With `Nuke.SiloLaunch=no`, a missing key falls back to the `MultiLauncher` weapon, then to `NukePayload`.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Nuke.Payload=NukePayload
```

See [Multi missile and chem missile](/systems/superweapons/#multi-missile-and-chem-missile).
