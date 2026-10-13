---
key: DirectRocker
scope: warheadtype
label: Direct rocker
summary: A blast that strikes a vehicle rocks it instead of damaging it.
see_also: [Rocker, Weight, DirectRockingCoefficient]
when_omitted:
  kind: value
  value: "no"
---

A blast from this warhead that strikes a vehicle rocks that vehicle instead of blasting it. The warhead deals no blast damage to the vehicle or to anything else nearby, and it throws no [shrapnel](/keys/shrapnelweapon/). The impact animation still plays.

Only a vehicle is rocked. Against infantry, a structure, an aircraft or any other target, the warhead behaves as it would without this key. A warhead with `Airstrike=yes` ignores this key.

The rocking force is the blast's damage multiplied by [`DirectRockingCoefficient`](/keys/directrockingcoefficient/#scope-global-rules), divided by 100, and capped at 4. The damage is the figure that reaches the blast after firepower bonuses, not the weapon's base damage. Unlike [`Rocker`](/keys/rocker/), there is no damage floor, so a weak blast rocks only when its tilt clears the minimum that [`Weight`](/keys/weight/) describes.

Only a voxel-drawn vehicle rocks, as with [`Rocker`](/keys/rocker/). The vehicle tips away from the firer, as if the blast came from a point 10 leptons from the vehicle toward the firer. A [`Rocker`](/keys/rocker/) blast halves the forward tilt. A direct rocker blast keeps the full forward tilt.

A shot with no firer, or a vehicle under an Iron Curtain, is not rocked. Such a shot deals no damage to that vehicle either.

OpenTS does not hold the struck vehicle in place afterward. The vehicle keeps its movement and its orders.
