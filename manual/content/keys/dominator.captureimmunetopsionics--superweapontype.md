---
key: Dominator.CaptureImmuneToPsionics
scope: superweapontype
summary: "Chooses whether a psychic dominator takes over units that are immune to psionics."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the blast ignores [`ImmuneToPsionics=`](/keys/immunetopsionics/) and takes such units over. Structures, aircraft, units under the Iron Curtain and [`BalloonHover=yes`](/keys/balloonhover/) types stay immune.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Dominator.CaptureImmuneToPsionics=yes
```

See [Psychic dominator](/systems/superweapons/#psychic-dominator).
