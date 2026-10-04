# Killing drivers and protected drivers (Ares)

Source: Ares, https://ares-developers.github.io/Ares-docs/new/killingdrivers.html. Related ability list: https://ares-developers.github.io/Ares-docs/new/veteranabilities.html.

## Keys

Warhead side:

| Key | Type | Default |
|---|---|---|
| `KillDriver` | boolean | `no` |
| `KillDriver.Owner` | `civilian`, `special` or `neutral` | `special` |
| `KillDriver.KillBelowPercent` | percentage | `100%` |
| `KillDriver.Chance` | percentage | `100%` |
| `KillDriver.RemoveVeterancy` | boolean | `no` |

Target side:

| Key | Section | Type | Default |
|---|---|---|---|
| `ProtectedDriver` | TechnoType | boolean | `no` |
| `ProtectedDriver.MinHealth` | TechnoType | double ratio | none |
| `CanDrive` | InfantryType | boolean | `no` |
| `CanBeDriven` | TechnoType | boolean | `yes` |
| `CanBeDriven` | Country | boolean | the country's `MultiplayPassive` |
| `PROTECTED_DRIVER` | veteran or elite ability list | ability | none |

## Behaviour

- A warhead with `KillDriver=yes` does not damage the vehicle. It removes the driver and changes the vehicle's owner to the house named by `KillDriver.Owner`. The vehicle is left standing and can be taken by an infantry unit with `CanDrive=yes`.
- The driver is the first passenger whose type matches the vehicle's `Operator`. All other passengers are ejected.
- Normal warhead scope applies: `CellSpread`, `Verses` against the armor, immunities such as `ImmuneToPoison`, veteran abilities, and `AffectsAllies` and `AffectsEnemies`.
- `KillDriver.KillBelowPercent` makes the effect work only when the target's health ratio is at or below the number; above it the target takes ordinary damage. `KillDriver.Chance` is the probability per hit. `KillDriver.RemoveVeterancy` resets the target to rookie when ownership changes.
- Protection: `ProtectedDriver=yes` makes the driver immune. `ProtectedDriver.MinHealth` lets the driver be killed only when the vehicle's health ratio is below that value; if both the warhead threshold and this one are set, the lower one applies. Organic and Natural units are always immune. The veteran ability `PROTECTED_DRIVER` gives unconditional protection.
- Infantry with `CanDrive` can enter such vehicles and take them over. `CanBeDriven=no` on a type or country blocks that. Vehicle thieves cannot take neutralised vehicles by default, although a unit may have both `VehicleThief=yes` and `CanDrive=yes`.

## What stock YR does

Nothing. Stock Yuri's Revenge has no driver kill; vehicles only change owner by mind control, capture, or the thief.

## Where it hooks in OpenYR

This is a new feature that touches several systems.
- Warhead read: `WarheadTypeClass` (`code/warhead.cpp`) for the five keys.
- Application: the per-target damage path. `TechnoClass::Take_Damage` (`code/techno.cpp`) is where each target's `Modify_Damage` result is applied; the KillDriver branch must run before damage and short-circuit it. `BulletClass::Detonate` (`code/bullet.cpp`) shows how other non-damaging warheads (`IsMindControl`, `IsLocomotor`, `IsParasite`) are special-cased.
- Owner change: `TechnoClass::Captured(HouseClass *)` (`code/techno.cpp`); a neutral or special house is looked up from `HouseClass` (`code/house.cpp`).
- Passenger eject: `TechnoClass::Cargo` (`CargoClass`, `code/techno.h`) and the unload code in `code/unit.cpp`.
- Type read: `TechnoTypeClass` (`code/techtype.cpp`) for `ProtectedDriver`, `ProtectedDriver.MinHealth`, `CanBeDriven`; `InfantryTypeClass` (`code/infatype.cpp`) for `CanDrive`. Country read (`code/houstype.cpp`) for `CanBeDriven`.
- Ability: add `PROTECTED_DRIVER` to `AbilityType` in `code/ability.hh` and the `AbilityName` table in `code/veteran.cpp`; `TechnoClass::Has_Ability` is the test.
- The unit needs an "empty driver" flag so that a vehicle without a driver stops working until retaken; this does not exist yet, and the flag must be saved and in the CRC.

## Open questions

1. What an ownerless (driverless) vehicle does while waiting: whether it is stunned, whether it blocks being targeted by its old owner, and how it is drawn.
2. What `Operator` means when the vehicle has no passengers: the page implies the driver is virtual (not a passenger) when `Operator` is unset.
3. How `KillDriver.Chance` is rolled (per target per hit). Assume once per target.
4. Interaction with the Ares `CanBeDriven` Country default and map owners.
