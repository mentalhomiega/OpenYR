---
title: Spawned aircraft and missiles
summary: "How carriers launch, recall and replace their aircraft, and how V3 rockets and naval missiles fly."
category: combat-targeting
keys:
  - Spawns
  - SpawnsNumber
  - SpawnRegenRate
  - SpawnReloadRate
  - SecondSpawnOffset
  - Spawner
  - MissileSpawn
  - V3Warhead
  - DMislWarhead
  - CMislWarhead
related:
  - type: system
    id: target-selection
---

An object whose type names an AircraftType in [`Spawns`](/keys/spawns/#scope-aircrafttype) carries [`SpawnsNumber`](/keys/spawnsnumber/#scope-aircrafttype) of them. Firing a [`Spawner=yes`](/keys/spawner/#scope-weapontype) weapon gives the carried aircraft or missiles the target; no projectile is fired. The aircraft carrier's Hornets, the V3 launcher's rockets and the Dreadnought's missiles work this way.

```ini title="rulesmd.ini"
[V3]
Primary=V3Launcher
Spawns=V3ROCKET
SpawnsNumber=1
SpawnRegenRate=400

[V3Launcher]
Spawner=yes
Range=18
```

## Launching

The carried aircraft are checked every 10 frames. While the object has a target in range of its primary weapon, its docked aircraft leave one at a time: missiles 9 frames apart, and other aircraft 20 frames apart. A missile does not leave while its launcher is moving. Each leaves from the primary weapon's firing point, or every second one from [`SecondSpawnOffset`](/keys/secondspawnoffset/#scope-aircrafttype) when that is set. Aircraft wait beside the object until all of them are out, and then attack the target together.

A target that goes out of range before the launch is dropped. A new target given during an attack replaces the old one for the aircraft still attacking.

## Returning and rearming

An aircraft that runs out of ammunition, or whose target is gone, flies back. It docks once it is within one and a half cells of the object, whatever its height, and after [`SpawnReloadRate`](/keys/spawnreloadrate/#scope-aircrafttype) frames it has its full ammunition and strength back and can launch again.

An aircraft that is destroyed, and a missile once it has taken off, is replaced after [`SpawnRegenRate`](/keys/spawnregenrate/#scope-aircrafttype) frames. When the object is destroyed or removed, its docked aircraft, and any that are still taking off, are removed with it, and its aircraft in flight crash. A missile in flight that is still moving carries on to its target; any other missile is removed.

## Missiles

The AircraftTypes that `V3RocketType`, `DMislType` and `CMislType` in `[General]` name fly as missiles. Each flies to the target's position when it was launched and explodes when it reaches that height or the ground. The settings come from `[General]` keys that begin with the same prefix:

| Key (after `V3Rocket`, `DMisl` or `CMisl`) | Effect |
| --- | --- |
| `PauseFrames` | Frames the missile waits on the launcher. |
| `TiltFrames` | Frames it takes to tilt from `PitchInitial` to `PitchFinal`. |
| `PitchInitial`, `PitchFinal` | Nose angles as fractions of straight up. The missile climbs at `PitchFinal`. |
| `Acceleration` | Speed gained each frame after the tilt, up to the type's `Speed`. |
| `Altitude` | Height in leptons where the climb ends. |
| `LazyCurve` | With `yes`, the missile bends smoothly from the climb into a dive at the target. With `no`, it levels off by `TurnRate` each frame and dives once the target is closer than it is high. |
| `TurnRate` | Radians the nose turns each frame while leveling off or diving. |
| `Damage`, `EliteDamage` | Damage of the explosion, the elite value when the launcher was elite when the missile left. |
| `Type` | The AircraftType flown with these settings. |

The explosion uses [`V3Warhead`](/keys/v3warhead/#scope-global-rules), [`DMislWarhead`](/keys/dmislwarhead/#scope-global-rules) or [`CMislWarhead`](/keys/cmislwarhead/#scope-global-rules) in `[CombatDamage]`, or their elite counterparts, and the launcher is credited with the kills. A missile plays its type's `AuxSound1` and the `V3TAKOFF` animation when its climb starts, and leaves `V3TRAIL` puffs every 3 frames. `RaiseRate` and `BodyLength` are read but have no effect.
