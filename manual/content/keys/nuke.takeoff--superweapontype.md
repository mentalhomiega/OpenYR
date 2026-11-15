---
key: Nuke.TakeOff
scope: superweapontype
label: Silo launch animation of a missile weapon
summary: "The animation a missile silo plays as this super weapon's missile takes off."
see_also: [NukeTakeOff, "system:superweapons"]
when_omitted:
  kind: inherited
  note: "[General] NukeTakeOff."
---

`Nuke.TakeOff` is an Ares key. When a silo launches this super weapon's missile, the named animation plays where the missile leaves the silo, in place of [`NukeTakeOff`](/keys/nuketakeoff/). `none`, or a name that is not an animation type, plays no animation. A super weapon without the key plays `NukeTakeOff`, so stock rules launch as before.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Nuke.TakeOff=V3TAKOFF
```

See [Multi missile and chem missile](/systems/superweapons/#multi-missile-and-chem-missile).
