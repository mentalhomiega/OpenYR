---
key: Dominator.PermanentCapture
scope: superweapontype
summary: "Chooses whether a unit a psychic dominator takes over is held for good."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, a unit that the blast takes over changes owner but is not marked as permanently controlled, so a later weapon with [`Dominator.CapturePermaMindControlled=no`](/keys/dominator.capturepermamindcontrolled/) does not skip it.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Dominator.PermanentCapture=no
```

See [Psychic dominator](/systems/superweapons/#psychic-dominator).
