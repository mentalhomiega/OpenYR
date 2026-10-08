---
title: Garrisons
summary: "How soldiers move into a structure, fire from inside it, change its owner, and come back out."
category: buildings-economy
keys:
  - OccupantAnim
  - CanBeOccupied
  - MaxNumberOccupants
  - CanOccupyFire
  - ShowOccupantPips
  - MuzzleFlash0
  - Occupier
  - OccupyWeapon
  - EliteOccupyWeapon
  - OccupyPip
  - OccupyDamageMultiplier
  - OccupyROFMultiplier
  - OccupyWeaponRange
---

A structure with [`CanBeOccupied=yes`](/keys/canbeoccupied/) holds up to [`MaxNumberOccupants`](/keys/maxnumberoccupants/) soldiers whose type sets [`Occupier=yes`](/keys/occupier/). With [`CanOccupyFire=yes`](/keys/canoccupyfire/), the structure fires its occupants' weapons for as long as one of them is inside.

```ini title="rulesmd.ini"
[MYOFFICES]             ; a BuildingType registered in [BuildingTypes]
CanBeOccupied=yes
MaxNumberOccupants=6
CanOccupyFire=yes

[MYRIFLEMAN]            ; an InfantryType registered in [InfantryTypes]
Occupier=yes
OccupyWeapon=MyGarrisonGun
EliteOccupyWeapon=MyEliteGarrisonGun
OccupyPip=PersonBlue
```

## Moving in

A structure can take a soldier when all of these hold:

- the soldier's type sets `Occupier=yes`;
- the soldier is not under mind control;
- fewer than `MaxNumberOccupants` soldiers are inside;
- the structure is above the [`ConditionRed`](/keys/conditionred/) health ratio;
- the structure is not being built up or sold;
- the structure belongs to the soldier's house, or to a house whose country sets [`MultiplayPassive=yes`](/keys/multiplaypassive/).

A player who points such a soldier at a structure that can take it gets the enter cursor. The soldier walks to the structure and goes inside when it reaches the structure. If the structure can no longer take it by then, the soldier steps aside. If the structure stops being able to take it while the soldier is still on the way, the soldier stops where it is.

An occupant is off the map while it is inside. It cannot be selected, attacked or healed.

## Owner

A garrisonable structure with `TechLevel=-1`, such as a civilian building, belongs to the first house whose country is on the `Civilian` side while it is empty. When the first soldier moves in, the structure passes to that soldier's house; when the last one leaves, it passes back. The change of owner scores nothing and springs no triggers.

The player hears [`BuildingGarrisonedSound`](/keys/buildinggarrisonedsound/) at the first soldier of theirs to move in, and [`BuildingAbandonedSound`](/keys/buildingabandonedsound/) when their garrison empties. With [EVAMD.INI](/formats/eva-ini/), the announcer adds `EVA_StructureGarrisoned` and `EVA_StructureAbandoned`. Units already heading for the structure keep it as their goal.

A structure that can be built, such as the Battle Bunker, keeps its owner whether or not anyone is inside, and so does every structure when no house is on the `Civilian` side.

## Firing

A `CanOccupyFire=yes` structure with at least one occupant searches for targets and fires like a defense. Its owner can also order it to attack a target within range.

Occupants take turns firing, in the order they moved in. Each shot uses the current occupant's [`OccupyWeapon`](/keys/occupyweapon/), or its [`EliteOccupyWeapon`](/keys/eliteoccupyweapon/) when that soldier is elite. An occupant whose type leaves that weapon unset fires its own primary weapon instead. The weapon's [`Range`](/keys/range/#scope-weapontype) limits how far the shot reaches.

Three `[CombatDamage]` settings change a garrison's fire:

- [`OccupyDamageMultiplier`](/keys/occupydamagemultiplier/) multiplies the damage of every shot.
- The delay between shots is the weapon's delay divided by the number of occupants, then divided by [`OccupyROFMultiplier`](/keys/occupyrofmultiplier/). More occupants fire faster.
- [`OccupyWeaponRange`](/keys/occupyweaponrange/) sets how far the structure searches for targets.

Shots leave from the art entry's [`MuzzleFlash0`](/keys/muzzleflash0/) through `MuzzleFlash9`, one point per occupant. The weapon's [`OccupantAnim`](/keys/occupantanim/#scope-weapontype) plays at that point for each shot.

## Pips

A garrisonable structure under the mouse, or selected, shows a row of figures with one figure per `MaxNumberOccupants` slot. Every player sees the row, whoever owns the structure. A slot with a soldier inside shows that soldier's [`OccupyPip`](/keys/occupypip/) figure; an empty slot shows an empty figure. [`ShowOccupantPips=no`](/keys/showoccupantpips/) hides the row.

## Moving out

The owner empties a garrison with the Deploy command while the structure is selected, or by clicking the structure while it is the only object selected.

Occupants also leave when the structure is destroyed. In a `TechLevel=-1` structure they also leave when it falls to the `ConditionRed` health ratio or below.

Each occupant leaving is placed on the nearest cell next to the structure that it could walk into, and stands guard there. An occupant with no such cell is removed from the game.

Selling a structure lets its occupants out the same way. An occupant with no such cell is placed on the structure's centre instead, and is removed only if it cannot be placed there either.
