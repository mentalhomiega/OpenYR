---
key: ShrapnelWeapon
summary: "The weapon a projectile of this type throws at nearby enemies where it hits."
see_also: [ShrapnelCount]
when_omitted:
  kind: value
  value: none
---

When a projectile of this type hits a cell that holds something other than a structure, it fires its `ShrapnelWeapon` from the point of impact at up to [`ShrapnelCount`](/keys/shrapnelcount/) nearby objects. We search the cells around the impact cell, ring by ring out to the shrapnel weapon's `Range`, in the order of the game's cell spread table. The impact cell itself is not searched. In each cell we take the first object that is neither the firer nor an ally of the firer's house. Each piece is a new projectile of the shrapnel weapon, credited to the original firer, and a shrapnel weapon with `IsElectricBolt=yes` or `IsLaser=yes` also draws its bolt or beam from the impact point. A `ShrapnelCount` of `0` or below throws nothing.

```ini title="rulesmd.ini"
[MyBouncingBolt] ; example Projectile
ShrapnelWeapon=MyFragment
ShrapnelCount=2
```
