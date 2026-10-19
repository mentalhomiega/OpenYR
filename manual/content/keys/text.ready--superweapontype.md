---
key: Text.Ready
scope: superweapontype
summary: "Names the cameo caption shown while the weapon is ready."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "Ready"
---

A string table label. For a charge-draining weapon it shows while the weapon is charged and off.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Text.Ready=NAME:ACTIVATE
```

See [Presentation](/systems/superweapons/#presentation).
