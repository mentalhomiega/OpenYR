---
key: SpecialSound
summary: "The sound the force shield plays as it starts to fade."
see_also: [ForceShieldPlayFadeSoundTime, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

A force shield weapon plays this sound at its target [`ForceShieldPlayFadeSoundTime`](/keys/forceshieldplayfadesoundtime/) frames before its protection ends. Other superweapons ignore it.

```ini title="rulesmd.ini"
[MyShieldSpecial] ; example SuperWeaponType
SpecialSound=MyShieldFading ; a sound ID registered in SOUNDMD.INI
```

A name that matches no sound ID is ignored.
