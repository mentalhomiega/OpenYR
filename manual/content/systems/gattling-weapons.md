---
title: Gattling weapons
summary: "How a type reads a numbered list of weapons, and how a gattling type moves through weapon stages as its spin rises and falls."
category: combat-targeting
keys:
  - TurretCount
  - WeaponCount
  - Weapon1
  - EliteWeapon1
  - Weapon1FLH
  - EliteWeapon1FLH
  - IsGattling
  - WeaponStages
  - Stage1
  - EliteStage1
  - RateUp
  - RateDown
  - GuardAreaTargetingDelay
related:
  - type: system
    id: firing-geometry
  - type: system
    id: veterancy
---

A type with [`TurretCount`](/keys/turretcount/) above `0` reads its weapons from a numbered list. A gattling type uses that list in pairs, one pair per stage, and moves up a stage as it keeps firing.

## Numbered weapon lists

A type with `TurretCount` above `0` ignores `Primary`, `Secondary`, `ElitePrimary`, `EliteSecondary` and their art offsets. It reads [`WeaponCount`](/keys/weaponcount/) positions instead, up to 18:

| Position | Weapon | Elite weapon | Firing offset in the art section | Elite firing offset |
| --- | --- | --- | --- | --- |
| 1 | [`Weapon1`](/keys/weapon1/) | [`EliteWeapon1`](/keys/eliteweapon1/) | [`Weapon1FLH`](/keys/weapon1flh/) | [`EliteWeapon1FLH`](/keys/eliteweapon1flh/) |
| 2 | [`Weapon2`](/keys/weapon2/) | [`EliteWeapon2`](/keys/eliteweapon2/) | [`Weapon2FLH`](/keys/weapon2flh/) | [`EliteWeapon2FLH`](/keys/eliteweapon2flh/) |
| ... | ... | ... | ... | ... |
| 18 | [`Weapon18`](/keys/weapon18/) | [`EliteWeapon18`](/keys/eliteweapon18/) | [`Weapon18FLH`](/keys/weapon18flh/) | [`EliteWeapon18FLH`](/keys/eliteweapon18flh/) |

An elite object fires the normal weapon in any position with no elite weapon. An elite firing offset defaults to the normal offset of the same position. The list's weapons fire with no barrel length or thickness.

Positions 1 and 2 stand in for the primary and secondary weapon wherever the game looks those up, such as in range and threat checks. A type that is not a gattling type always fires position 1.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=2
Weapon1=MyGun
Weapon2=MyFlakGun
EliteWeapon1=MyEliteGun
```

Yuri's Revenge also lets an IFV's passenger choose the position, and reads a barrel length, barrel thickness and turret lock for each position. None of these is read yet.

## Stages

An [`IsGattling=yes`](/keys/isgattling/) type fires the pair of weapons for its current stage: positions 1 and 2 at the first stage, 3 and 4 at the second, and so on. It fires the first weapon of the pair, and the second only at an aircraft in the air, and then only when `Weapon2` has an anti-aircraft projectile. A gattling type with no weapon in position 1 or 2 chooses between those two positions as an ordinary type does, at every stage.

The stage follows the weapon's spin, a number that starts at `0`:

- The weapon moves up from stage *K* once the spin reaches the threshold `Stage`*K*, and back down to stage *K* once the spin falls below it.
- The spin rises by [`RateUp`](/keys/rateup/) a frame, but only while it is below the threshold of the last stage.
- The spin falls by [`RateDown`](/keys/ratedown/) a frame, never below `0`. With `RateDown=0`, it drops to `0` at once.

[`WeaponStages`](/keys/weaponstages/) sets the number of stages, up to 6. The thresholds [`Stage1`](/keys/stage1/) to `Stage6` are read only for an `IsGattling=yes` type with `WeaponStages` of 2 or more, and a threshold left unset is `0`. An elite object uses [`EliteStage1`](/keys/elitestage1/) to `EliteStage6` instead.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
TurretCount=1
WeaponCount=6
Weapon1=MyGun1        ; first stage, against the ground
Weapon2=MyAAGun1      ; first stage, against aircraft
Weapon3=MyGun2
Weapon4=MyAAGun2
Weapon5=MyGun3
Weapon6=MyAAGun3
IsGattling=yes
WeaponStages=3
Stage1=200            ; second stage from a spin of 200
Stage2=400            ; third stage from 400
Stage3=600            ; the spin stops rising at 600
RateUp=1
RateDown=50
```

## When the spin rises and falls

A vehicle updates its spin every frame. The spin rises while the vehicle fires at its target, turns to face it, or reloads, and falls at any other time, including when it has no target.

A structure updates its spin when its attack or guard mission runs, for all the frames since the last update:

- In the attack mission, the spin rises while the structure fires, faces its target or reloads. It falls when the structure loses its target, is busy or is cloaked. While the turret is still turning, the frames pass without changing the spin.
- In the guard mission, the spin falls.

Out of the attack mission, a structure's spin also falls by `RateDown` every frame once [`GuardAreaTargetingDelay`](/keys/guardareatargetingdelay/) plus five frames have passed since its last shot.

Infantry and aircraft never change their spin, so a gattling type of either kind stays at its first stage.

## Sound and animation

A gattling weapon's [`Report`](/keys/report/) plays as a loop at the object while the spin rises, instead of once a shot. The loop belongs to the first weapon of the current stage and restarts when the stage changes. When the spin starts to fall, the loop plays out its ending. Leaving the map or being removed silences it.

A voxel turret and barrel step through their animation frames while a gattling weapon's spin is above `0`, one frame each game frame. A spinning structure out of its attack mission steps two. Any other vehicle's or structure's turret steps a frame each time it fires or reloads. A vehicle's turret plays its own frames only while the body is on its first frame; otherwise the turret shows the body's frame.
