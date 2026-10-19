---
key: SW.Sound
scope: superweapontype
summary: "Plays a sound at the target cell when the effect starts."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "none"
---

A sound from the sound list, played at the target cell. [`SW.ActivationSound=`](/keys/sw.activationsound/) plays at the same moment.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Sound=NukeSiren
```

See [Presentation](/systems/superweapons/#presentation).
