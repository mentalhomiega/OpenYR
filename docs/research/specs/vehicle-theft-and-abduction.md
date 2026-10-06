# Vehicle thieves and abduction (Ares)

Sources:
- Hijackers: https://ares-developers.github.io/Ares-docs/new/hijackers.html
- Abductor and chrono prisons: https://ares-developers.github.io/Ares-docs/new/chronoprisons.html

## Vehicle thief keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `VehicleThief` | rules, InfantryType | boolean (stock key) | `no` |
| `VehicleThief.Allowed` | rules, any VehicleType or AircraftType (the victim) | boolean | `yes` |
| `VehicleThief.EnterSound` | InfantryType | sound | none |
| `VehicleThief.LeaveSound` | InfantryType | sound | none |
| `VehicleThief.BreakMindControl` | InfantryType | boolean | `yes` |
| `VehicleThief.OneTime` | InfantryType | boolean | `no` |
| `VehicleThief.KillPilots` | InfantryType | integer; `-1` = all | `0` |

## Thief behaviour

- A thief infantry unit walks into an enemy vehicle or aircraft and takes it over. The vehicle changes owner. The thief stays inside it.
- `VehicleThief.Allowed=no` on the victim type stops theft. It replaces the older use of `NonVehicle` for this purpose.
- A captured vehicle owned by a human player enters Guard mode; for the computer it enters Hunt.
- The thief keeps its health and rank. When the stolen vehicle is destroyed, the thief comes out with a random health up to 50% of what it had and keeps its rank, unless `VehicleThief.OneTime=yes`, in which case it is consumed on capture and never comes out.
- `VehicleThief.KillPilots` kills that many crew when the thief enters (`-1` all). Killed crew cannot escape when the vehicle is destroyed later.
- `VehicleThief.BreakMindControl=no` makes mind controlled vehicles immune to theft. With `yes`, theft breaks the controller's link.
- Thieves cannot take friendly units or vehicles affected by a Kill Driver warhead (those have their own capture rule, see `kill-driver.md`).
- A mind controlled thief captures vehicles for its original owner, not for the mind controller.
- `EnterSound` plays on capture and `LeaveSound` when the thief leaves a destroyed vehicle.

## Abduction keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `Abductor` | rules, WeaponType | boolean | `no` |
| `Abductor.Temporal` | WeaponType | boolean | `no` |
| `Abductor.Anim` | WeaponType | AnimationType | none |
| `Abductor.ChangeOwner` | WeaponType | boolean | `no` |
| `Abductor.AbductBelowPercent` | WeaponType | percentage | `100%` |
| `Abductor.MaxHealth` | WeaponType | integer hit points; `0` = no check | `0` |
| `ImmuneToAbduction` | TechnoType | boolean | `no` |
| `SizeLimit` | TechnoType (transport) | integer (stock key) | `0` |
| `PassengerTurret` | TechnoType | boolean | `no` |

## Abduction behaviour

- A weapon with `Abductor=yes` that hits a target pulls it into the attacker's passenger hold instead of damaging it. If the hold is full, the target is too large (`SizeLimit` defaults to 0, so abduction fails unless it is set), or the target is `ImmuneToAbduction=yes`, normal damage applies instead.
- The attacker must be a transport with a hold; all transports need `PipScale`.
- `Abductor.Temporal=yes` abducts the target only after a temporal warhead has erased it, and falls back to normal temporal erasure if the abduction fails.
- `Abductor.ChangeOwner=yes` gives the target to the abductor's house, unless the target is immune to psionics.
- `Abductor.AbductBelowPercent` and `Abductor.MaxHealth` restrict targets by current health ratio or absolute hit points.
- `Abductor.Anim` plays where the target was taken from.
- When the abductor dies, the passengers come out. Slaves and spawned units go to the Special house.
- `PassengerTurret=yes` selects the turret voxel from the passenger count (for example `footur.vxl`, `footur1.vxl`, `footur5.vxl`).
- Abduction of buildings is known to misbehave and is not considered a bug.

## What stock YR does

Stock has `VehicleThief=yes`, which already lets the infantry steal vehicles, and `NonVehicle=yes` on a vehicle type which blocks that theft (and some other things). There is no abduction.

## Where it hooks in OpenYR

Thief: stock logic exists.
- `InfantryTypeClass::IsVehicleThief` (`code/infatype.h`); the mission and capture handling in `code/infantry.cpp` (the `IsVehicleThief` branches around target selection and the `Considered_Vehicle` check) and `code/foot.cpp` (`MISSION_CAPTURE` assignment); AI planning in `code/house.cpp`.
- `UnitClass::IsNonVehicle` is read in `code/unittype.cpp` and returned by `UnitClass::Considered_Vehicle` (`code/unit.cpp`). `VehicleThief.Allowed` is a new bool on `TechnoTypeClass` that the thief's target check must test in addition to `Considered_Vehicle`.
- New work: the kill pilots count, one-time consumption, health roll on exit, sounds, mind control break, and Hunt or Guard mission on capture. Find the capture routine by following `MISSION_CAPTURE` in `code/infantry.cpp` and `Captured` in `code/techno.cpp` (`TechnoClass::Captured(HouseClass *)`).

Abduction: new runtime system.
- Weapon read: `WeaponTypeClass` (`code/weapon.cpp`, `code/weapon.h`) for the `Abductor.*` keys.
- Hit handling: `BulletClass::Detonate` (`code/bullet.cpp`) is where per-warhead effects (locomotor, parasite, temporal) are special-cased. Abduction needs the weapon, which `BulletClass` keeps as `Weapon`.
- Passenger hold: `TechnoClass::Cargo` (`code/techno.h`, `CargoClass`) and the load and unload routines in `code/unit.cpp`; size limit checks use `SizeLimit` and `Size` of the target type (read in `code/techtype.cpp`).
- Temporal path: `code/temporal.cpp`.
- Release on death: the code that kills cargo and unloads passengers on destruction (`TechnoClass::Kill_Cargo`, `techno.cpp`).

## Open questions

1. What the Ares documentation means by "cursor" and AI behaviour for thieves; it only describes human control.
2. Exact random health on exit ("up to 50%"): uniform or fixed.
3. Whether `VehicleThief.Allowed` also blocks drivers retaking a vehicle (`CanDrive`); the page says "replaces NonVehicle for this purpose".
4. Which passenger sizes the abductor accepts when `SizeLimit` is set but the target's `Size` is above 1; assume the stock transport size test.
