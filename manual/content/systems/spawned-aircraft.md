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

The carried aircraft are checked every 10 frames. While the object has a target in range of the weapon it selects for that target, its docked aircraft leave one at a time, 9 frames apart when the object's own type has `MissileSpawn=yes` and 20 frames apart otherwise. That is the primary weapon for most targets. Against an `Underwater=yes` target, a [`NavalTargeting=1`](/keys/navaltargeting/) object selects its secondary weapon, so the secondary's `Range` applies there. A missile does not leave while its launcher is moving. Each leaves from the firing point of its launch slot. The launch slot is the first slot when that slot's weapon has [`Spawner=yes`](/keys/spawner/#scope-weapontype), and the second slot otherwise. [`SecondSpawnOffset`](/keys/secondspawnoffset/#scope-aircrafttype) is added to that point as the key describes. A CMisl missile plays `V3TAKOFF` as it leaves. Aircraft wait beside the object until all of them are out, and then attack the target together.

A target that goes out of range before the launch is dropped. A new target given during an attack replaces the old one for the aircraft still attacking.

## Returning and rearming

An aircraft that runs out of ammunition, or whose target is gone, flies back. It docks once it is within one and a half cells of the object, whatever its height, and after [`SpawnReloadRate`](/keys/spawnreloadrate/#scope-aircrafttype) frames it has its full ammunition and strength back and can launch again.

An aircraft that is destroyed, and a missile once it has taken off, is replaced after [`SpawnRegenRate`](/keys/spawnregenrate/#scope-aircrafttype) frames. When the object is destroyed or removed, its docked and reloading aircraft, and any missile still taking off, are removed with it, and its aircraft in flight crash. Any other missile carries on to its target.

## Missiles

The AircraftTypes that `V3RocketType`, `DMislType` and `CMislType` in `[General]` name fly as missiles. Each flies to the target's position when it was launched and explodes when it reaches that height or the ground. The settings come from `[General]` keys that begin with the same prefix:

| Key (after `V3Rocket`, `DMisl` or `CMisl`) | Effect |
| --- | --- |
| `PauseFrames` | Frames the missile waits on the launcher. |
| `TiltFrames` | Frames it takes to tilt from `PitchInitial` to `PitchFinal`. |
| `PitchInitial`, `PitchFinal` | Nose angles as fractions of straight up. The missile climbs at `PitchFinal`. |
| `Acceleration` | Speed gained each frame after the tilt, up to the type's `Speed`. |
| `Altitude` | Height in leptons where the climb ends. |
| `LazyCurve` | With `yes`, the missile bends smoothly from the climb into a dive at the target. With `no`, it levels off by `TurnRate` each frame, then dives once its distance to the target across the ground is no more than its height above the target, so the dive starts about that far out. |
| `TurnRate` | Radians the nose turns each frame while leveling off or diving. |
| `Damage`, `EliteDamage` | Damage of the explosion, the elite value when the launcher was elite when the missile left. |
| `Type` | The AircraftType flown with these settings. |

The explosion uses [`V3Warhead`](/keys/v3warhead/#scope-global-rules), [`DMislWarhead`](/keys/dmislwarhead/#scope-global-rules) or [`CMislWarhead`](/keys/cmislwarhead/#scope-global-rules) in `[CombatDamage]`, or their elite counterparts, and the kill goes to the house that fired the missile, even after the launcher is destroyed. The launcher also gets the experience while it is alive. A missile plays its type's `AuxSound1` when its climb starts. The `V3TAKOFF` animation starts two frames later, with a Z adjust of -10, and the missile leaves `V3TRAIL` puffs every 3 frames. `RaiseRate` and `BodyLength` are read but have no effect.
