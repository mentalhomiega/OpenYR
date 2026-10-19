---
key: FirersPalette
summary: Draws a shape projectile in the color scheme of the house that fired it.
see_also: [AnimPalette, Image]
when_omitted:
  kind: value
  value: "no"
---

A `FirersPalette=yes` projectile that is drawn from a shape takes the color scheme of the house that owned the firing object at the moment of firing, so its remap colors match that house's units. A projectile created with no firing object, such as one a superweapon or a trigger action launches, takes the local player's color scheme.

The shape's shadow keeps the normal palette. A voxel projectile ignores the key and uses its [`Color`](/keys/color/#scope-bullettype) scheme. [`AnimPalette=yes`](/keys/animpalette/) takes precedence when both are set.

```ini title="rulesmd.ini"
[MYMISSILE] ; a BulletType
FirersPalette=yes
Image=MYMISSILEART
```
