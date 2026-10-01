---
key: Explodes
scope: aircrafttype
label: Violent death
see_also: [DeathWeapon, DeathWeaponDamageModifier, Explosion, Primary, MaxDebris]
when_omitted:
  kind: value
  value: "no"
---

```ini title="rules.ini"
[MYAMMOTRUCK] ; a UnitType registered in [VehicleTypes]
Explodes=yes
```

A destroyed object of this type sets off its death weapon where it dies, as a projectile detonating there. An object whose rank grants the `EXPLODES` ability through [`VeteranAbilities`](/keys/veteranabilities/) or [`EliteAbilities`](/keys/eliteabilities/) does the same without this key.

The death weapon is the type's [`DeathWeapon`](/keys/deathweapon/#scope-aircrafttype), or its [`Primary`](/keys/primary/) weapon when it sets none. Either does its [`Damage`](/keys/damage/#scope-weapontype) multiplied by [`DeathWeaponDamageModifier`](/keys/deathweapondamagemodifier/). A type with neither sets off the rules' [`[CombatDamage] DeathWeapon`](/keys/deathweapon/#scope-global-rules) for half its [`Strength`](/keys/strength/#scope-aircrafttype). The detonation has the weapon's warhead effects and explosion animation, and its damage is credited to the dying object.

The key changes three more parts of the death:

- **Spilled Tiberium.** A vehicle, aircraft or infantry soldier carrying Tiberium, such as a harvester, spills its load around it, even when it has no weapon and sets off no blast. A scenario with [`HarvesterImmune=yes`](/keys/harvesterimmune/) spills nothing. [Spilled harvester loads](/systems/destruction-and-debris/#spilled-harvester-loads) covers the amount and where it lands. The `EXPLODES` ability has the same effect.
- **Vehicle death animation.** A vehicle that still has ammunition plays the last entry of its [`Explosion`](/keys/explosion/) list instead of a random one. The `EXPLODES` ability has the same effect.
- **Structures.** A structure places a `FIRE3` fire animation on any [explosive overlay](/keys/explodes/#scope-overlaytype) in the four cells that share an edge with the top corner cell of its footprint. It also stays on the map until its next update, which [releases its survivors a second time](/systems/destruction-and-debris/#when-the-structure-leaves-the-map). The `EXPLODES` ability does neither.

A vehicle or infantry soldier that falls into water leaves only a splash. It falls when a bridge collapses under it, or when it is a hover vehicle that sinks too low over water. It sets off no blast and spills no Tiberium. An aircraft shot down in flight is not falling in this sense, so its blast goes off where it was destroyed, even over water.
