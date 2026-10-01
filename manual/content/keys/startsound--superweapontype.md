---
key: StartSound
scope: superweapontype
label: Superweapon start sound
see_also: [SpecialSound, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

A force shield weapon plays this sound at its target when it is fired. Other superweapons ignore it.

```ini title="rulesmd.ini"
[MyShieldSpecial] ; example SuperWeaponType
StartSound=MyShieldStarting ; a sound ID registered in SOUNDMD.INI
```

A name that matches no sound ID is ignored.
