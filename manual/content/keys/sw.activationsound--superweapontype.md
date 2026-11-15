---
key: SW.ActivationSound
scope: superweapontype
summary: "Plays a second sound at the target cell when the effect starts."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A sound from the sound list, played at the target cell with [`SW.Sound=`](/keys/sw.sound/).

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.ActivationSound=NukeSiren
```

See [Presentation](/systems/superweapons/#presentation).
