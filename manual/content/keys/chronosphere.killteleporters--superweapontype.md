---
key: Chronosphere.KillTeleporters
scope: superweapontype
summary: "Chooses whether a chronoshift destroys units that can teleport."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, a [`Teleporter=yes`](/keys/teleporter/) unit that the chronosphere picks is destroyed instead of moved, and the firing house is credited with the kill. The key is read from the `Type=ChronoSphere` weapon that picked the units, not from the `Type=ChronoWarp` weapon.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Chronosphere.KillTeleporters=yes
```

See [Chronosphere](/systems/superweapons/#chronosphere).
