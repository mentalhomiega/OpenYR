---
title: Gunner vehicles
summary: "How a Gunner=yes vehicle takes its weapon from its first passenger and shows the turret for that weapon."
category: combat-targeting
keys:
  - Gunner
  - IFVMode
  - NormalTurretIndex
  - RepairTurretIndex
  - MachineGunTurretIndex
  - FlakTurretIndex
  - PistolTurretIndex
  - SniperTurretIndex
  - ShockTurretIndex
  - ExplodeTurretIndex
  - BrainBlastTurretIndex
  - RadCannonTurretIndex
  - ChronoTurretIndex
  - TerroristExplodeTurretIndex
  - CowTurretIndex
  - InitiateTurretIndex
  - VirusTurretIndex
  - YuriPrimeTurretIndex
  - GuardianTurretIndex
  - NormalTurretWeapon
  - RepairTurretWeapon
  - MachineGunTurretWeapon
  - FlakTurretWeapon
  - PistolTurretWeapon
  - SniperTurretWeapon
  - ShockTurretWeapon
  - ExplodeTurretWeapon
  - BrainBlastTurretWeapon
  - RadCannonTurretWeapon
  - ChronoTurretWeapon
  - TerroristExplodeTurretWeapon
  - CowTurretWeapon
  - InitiateTurretWeapon
  - VirusTurretWeapon
  - YuriPrimeTurretWeapon
  - GuardianTurretWeapon
related:
  - type: system
    id: gattling-weapons
---

A [`Gunner=yes`](/keys/gunner/) vehicle with [`TurretCount`](/keys/turretcount/) above `0` that is not a gattling type fires the weapon its first passenger chooses, and shows a turret to match.

## The weapon

Weapons are counted from `0` here, so weapon `0` is [`Weapon1`](/keys/weapon1/) of the vehicle's [numbered list](/systems/gattling-weapons/#numbered-weapon-lists), weapon `2` is `Weapon3`, and so on.

- An empty gunner vehicle fires weapon `0`.
- When a passenger boards the empty vehicle, the vehicle fires the weapon that passenger's [`IFVMode`](/keys/ifvmode/) names. A value outside `0` to `17` selects weapon `0`.
- Passengers who board after the first, and passengers who leave while others stay aboard, change nothing.
- Once the last passenger leaves or dies, the vehicle returns to weapon `0`.

```ini title="rulesmd.ini"
[MYIFV] ; example VehicleType
Gunner=yes
Passengers=1
TurretCount=2
WeaponCount=3
Weapon1=MyMissiles     ; weapon 0, while empty
Weapon2=MyRepairTool   ; weapon 1
Weapon3=MyMachineGun   ; weapon 2

[MYSOLDIER] ; example InfantryType
IFVMode=2              ; MYIFV fires MyMachineGun while this soldier rides in it
```

## The turret

A vehicle type with `TurretCount` above `0` that is not a gattling type loads that many numbered turrets from its art: turret `0` is the `TUR` voxel, turret `1` is `TUR1`, and so on, each with the barrel of the same number, `BARL`, `BARL1` and on. The vehicle shows the turret its current weapon maps to, and its ordinary `TUR` turret when that numbered turret is missing.

Only the vehicle type named `FV` maps weapons to turrets. Each pair below sends the weapon its `TurretWeapon` key names to the turret its `TurretIndex` key names, applied in this order, so a later pair overrides an earlier one for the same weapon. A `TurretWeapon` key left unset maps nothing, and a weapon no pair maps shows turret `0`.

| Weapon key | Turret key | Turret when unset |
| --- | --- | --- |
| [`NormalTurretWeapon`](/keys/normalturretweapon/) | [`NormalTurretIndex`](/keys/normalturretindex/) | `0` |
| [`RepairTurretWeapon`](/keys/repairturretweapon/) | [`RepairTurretIndex`](/keys/repairturretindex/) | `1` |
| [`MachineGunTurretWeapon`](/keys/machinegunturretweapon/) | [`MachineGunTurretIndex`](/keys/machinegunturretindex/) | `2` |
| [`FlakTurretWeapon`](/keys/flakturretweapon/) | [`FlakTurretIndex`](/keys/flakturretindex/) | `3` |
| [`PistolTurretWeapon`](/keys/pistolturretweapon/) | [`PistolTurretIndex`](/keys/pistolturretindex/) | `0` |
| [`SniperTurretWeapon`](/keys/sniperturretweapon/) | [`SniperTurretIndex`](/keys/sniperturretindex/) | `0` |
| [`ShockTurretWeapon`](/keys/shockturretweapon/) | [`ShockTurretIndex`](/keys/shockturretindex/) | `0` |
| [`ExplodeTurretWeapon`](/keys/explodeturretweapon/) | [`ExplodeTurretIndex`](/keys/explodeturretindex/) | `0` |
| [`BrainBlastTurretWeapon`](/keys/brainblastturretweapon/) | [`BrainBlastTurretIndex`](/keys/brainblastturretindex/) | `0` |
| [`RadCannonTurretWeapon`](/keys/radcannonturretweapon/) | [`RadCannonTurretIndex`](/keys/radcannonturretindex/) | `0` |
| [`ChronoTurretWeapon`](/keys/chronoturretweapon/) | [`ChronoTurretIndex`](/keys/chronoturretindex/) | `0` |
| [`TerroristExplodeTurretWeapon`](/keys/terroristexplodeturretweapon/) | [`TerroristExplodeTurretIndex`](/keys/terroristexplodeturretindex/) | `0` |
| [`CowTurretWeapon`](/keys/cowturretweapon/) | [`CowTurretIndex`](/keys/cowturretindex/) | `0` |
| [`InitiateTurretWeapon`](/keys/initiateturretweapon/) | [`InitiateTurretIndex`](/keys/initiateturretindex/) | `0` |
| [`VirusTurretWeapon`](/keys/virusturretweapon/) | [`VirusTurretIndex`](/keys/virusturretindex/) | `0` |
| [`YuriPrimeTurretWeapon`](/keys/yuriprimeturretweapon/) | [`YuriPrimeTurretIndex`](/keys/yuriprimeturretindex/) | `0` |
| [`GuardianTurretWeapon`](/keys/guardianturretweapon/) | [`GuardianTurretIndex`](/keys/guardianturretindex/) | `0` |

```ini title="rulesmd.ini"
[FV]
MachineGunTurretWeapon=2 ; weapon 2 ...
MachineGunTurretIndex=1  ; ... shows turret 1, the art's TUR1 voxel
```

Yuri's Revenge also hands a passenger's chrono weapon to the vehicle, draws the gunner's pip, and shows turret tooltips. None of these is done yet.
