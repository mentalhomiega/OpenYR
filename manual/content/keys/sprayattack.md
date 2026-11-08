---
key: SprayAttack
summary: Makes the object's shots land in a ring around itself instead of at its target.
see_also: [Burst, AreaFire]
when_omitted:
  kind: value
  value: "no"
---

With `SprayAttack=yes`, every shot the object fires, from either weapon, strikes the ground about one cell away from the object instead of its target. The eight possible points form a ring around the object's position, so an object standing at the centre of a cell hits its eight neighbouring cells. The object still picks targets and attacks them normally; only the place the shot lands changes.

```ini title="rules.ini"
[MyFlameTank] ; example VehicleType
SprayAttack=yes ; with a Burst=4 weapon, each burst covers four sides
```

The first shot of each [`Burst`](/keys/burst/) lands at a random point of the ring. Each later shot moves `8 / Burst` points round the ring, rounded down, so a burst of 2 hits opposite sides, a burst of 4 hits four sides 90 degrees apart, and a burst of 8 hits every point once. With a burst above 8 the step is 0 and the whole burst hits one point. With `Burst=1` every shot picks a new random point.

The setting replaces [`AreaFire=yes`](/keys/areafire/#scope-weapontype) for this object: its shots go to the ring, not to its own cell. Stock Yuri's Revenge uses it on no object.
