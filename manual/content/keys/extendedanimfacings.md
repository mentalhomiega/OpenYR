---
key: ExtendedAnimFacings
summary: Lets a weapon's 16-, 32- or 64-entry Anim list pick its entry by the direction of the shot.
see_also: [Anim]
when_omitted:
  kind: value
  value: "no"
---

At `yes`, a weapon whose [`Anim`](/keys/anim/) list has `16`, `32` or `64` entries picks the entry for each shot by the direction the weapon fires in, as an eight-entry list does. At `no`, the default, only an eight-entry list picks by direction, as in Yuri's Revenge. Any other list plays its first entry on every shot, whatever this key is set to.

The list is divided into as many directions as it has entries. It starts at north-west and runs clockwise, as described on the [`Anim`](/keys/anim/) page.

The key applies to a weapon's shots only. A [parasite](/systems/parasites/) bite picks its entry by eight directions whatever the key is set to.

```ini title="rulesmd.ini"
[MyGatling] ; example WeaponType
Anim=GUNFIRE01,GUNFIRE02,GUNFIRE03,GUNFIRE04,GUNFIRE05,GUNFIRE06,GUNFIRE07,GUNFIRE08,GUNFIRE09,GUNFIRE10,GUNFIRE11,GUNFIRE12,GUNFIRE13,GUNFIRE14,GUNFIRE15,GUNFIRE16 ; sixteen AnimTypes
ExtendedAnimFacings=yes
```
