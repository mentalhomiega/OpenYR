---
key: Light.Enabled
scope: superweapontype
summary: "Chooses whether a nuclear missile, lightning storm or psychic dominator lights the map."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

A `Type=MultiMissile` weapon lights the map with a flash when its payload goes off, a `Type=LightningStorm` weapon while its storm rages, and a `Type=PsychicDominator` weapon while its blast shows. With `no`, the weapon leaves the map's lighting alone: a missile raises no flash, and a storm or blast leaves the scenario's lighting in place. Other behaviors ignore the key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Light.Enabled=no
```

See [Lighting](/systems/superweapons/#lighting).
