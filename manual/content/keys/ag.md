---
key: AG
summary: Allows the projectile to be fired at targets that are on the ground.
see_also: ["system:target-selection"]
when_omitted:
  kind: value
  value: "yes"
---

A weapon whose projectile is not anti-ground cannot fire at a map cell. The test does not apply to an object. A weapon fires at an object on the ground or on water as [`NavalTargeting`](/keys/navaltargeting/) and [`LandTargeting`](/keys/landtargeting/) allow, whatever its projectile's `AG`. An anti-air weapon can therefore be ordered to fire at a landed aircraft.

An ordered attack on an object does not check `AG`. The weapon slot is chosen as for any other target, and a slot whose warhead does no damage to the target's armor is passed over.

A structure's attack cursor reads its first weapon slot. When that slot's projectile is `AG=no`, the player cannot order the structure to attack a map cell, or any object unless the object is airborne and the projectile is [`AA=yes`](/keys/aa/).

With `AA=no` beside it, which is the default, the weapon is ground-only. `AA=yes` with `AG=no` makes the air-only mirror.

Automatic target scans also read this setting. A weapon slot whose projectile is `AG=yes` adds infantry, vehicles and buildings to the kinds of target the object looks for, unless [`AV=yes`](/keys/av/) on the same projectile limits the slot to vehicles. [What each kind of object considers](/systems/target-selection/#what-each-kind-of-object-considers) says when an object's weapons fill that list.

:::caution[`AG=no` on the first weapon slot hides most ground targets from the automatic scan]
The automatic target scan checks the first slot's projectile, whatever weapon would actually fire. When that projectile is `AG=no`, the scan rejects every candidate standing on the map's lowest terrain level, not only a landed aircraft. An object whose first weapon is anti-air therefore picks no such target by itself, even when its second weapon could hit it. The check uses height above the lowest level, not above the local ground, so it does not reject a candidate standing on raised terrain.
:::

The setting also gates the computer's wall targeting: a wall is worth nothing to an object whose first weapon's projectile is `AG=no`.
