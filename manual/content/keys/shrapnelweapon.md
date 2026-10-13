---
key: ShrapnelWeapon
summary: "The weapon a projectile of this type throws at nearby enemies where it hits."
see_also: [ShrapnelCount]
when_omitted:
  kind: value
  value: none
---

When a projectile of this type hits a cell that holds something other than a structure, it throws [`ShrapnelCount`](/keys/shrapnelcount/) pieces of its `ShrapnelWeapon` from the point of impact. We search the cells around the impact cell, ring by ring out to the shrapnel weapon's `Range`, in the order of the game's cell spread table. The impact cell itself is not searched. In each searched cell we take the first object that is neither the firer nor an ally of the firer's house, and each one takes a piece until the pieces run out. A piece the search does not place lands on a random cell within two cells of the impact cell; a random cell off the map gets none. Each piece is a new projectile of the shrapnel weapon, credited to the original firer, and a shrapnel weapon with `IsElectricBolt=yes` or `IsLaser=yes` also draws its bolt or beam from the impact point. A `ShrapnelCount` of `0` has no limit, so a piece goes to every such enemy in range and none lands at random. A negative `ShrapnelCount` is described on its page. A projectile whose warhead sets `NukeMaker`, `MindControl`, `MakesDisguise`, `IvanBomb`, `ElectricAssault`, `IsLocomotor`, `BombDisarm`, `Parasite`, `Temporal` or `Airstrike` throws none.

```ini title="rulesmd.ini"
[MyBouncingBolt] ; example Projectile
ShrapnelWeapon=MyFragment
ShrapnelCount=2
```
