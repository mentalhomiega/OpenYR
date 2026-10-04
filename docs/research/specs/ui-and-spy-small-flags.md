# HealthBar.Hide and SpyEffect.Custom (Phobos)

Sources:
- Health bar: https://phobos.readthedocs.io/en/latest/User-Interface.html (section "Custom health bars display")
- Spy effects: https://phobos.readthedocs.io/en/latest/New-or-Enhanced-Logics.html (section "Spy effects")

## HealthBar.Hide

| Key | File and section | Type | Default |
|---|---|---|---|
| `HealthBar.Hide` | rules, TechnoType | boolean | `false` |
| `HealthBar.HidePips` | TechnoType | boolean | `false` |
| `HealthBar.Permanent` | TechnoType | boolean | `false` |
| `HealthBar.Permanent.PipScale` | TechnoType | boolean | `false` |

- `HealthBar.Hide=true` hides both the health bar box and the health pips of that type.
- `HealthBar.HidePips=true` hides only the pips; the rest of the bar stays. (The documentation text reads "only hides the health bar without affecting anything else"; the pairing with `Hide` suggests the pips-only reading, see open questions.)
- `HealthBar.Permanent=true` shows the health bar at all times. `HealthBar.Permanent.PipScale=true` always shows the extra pips (the transport pip scale) and group numbers.

Stock YR draws the bar and pips for selected objects and for all objects when the player holds the option key or sets the always show option.

Hooks: `TechnoClass::Draw_Health_Bar` (`code/techno.cpp`) draws the box and pips (the old variant is `Draw_Health_Bar_Old`), and `TechnoClass::Draw_Pips` draws transport and ammo pips from the same data. Wrap those calls with the type flags and read the keys in `TechnoTypeClass` (`code/techtype.cpp`). Small, pure data plumbing.

## SpyEffect.Custom

| Key | File and section | Type | Default |
|---|---|---|---|
| `SpyEffect.Custom` | rules, BuildingType | boolean | `false` |
| `SpyEffect.VictimSuperWeapon` | BuildingType | SuperWeaponType | none |
| `SpyEffect.InfiltratorSuperWeapon` | BuildingType | SuperWeaponType | none |

- With `SpyEffect.Custom=yes`, when a spy of another house infiltrates this building the listed super weapons fire immediately at the building's centre cell. `VictimSuperWeapon` is fired for the owner of the infiltrated building. `InfiltratorSuperWeapon` is fired for the spy's house.
- The launch does not use the super weapon's charge: it fires whether or not it is ready, and the super weapon's recharge timer is restored afterwards to the value it had, so the infiltration does not reset or spend the player's real super weapon of that type.
- Nothing happens when the infiltrating house is the same as the building's owner.
- Whether the stock infiltration effects (power blackout, radar loss, tech theft, cash theft and so on) still occur with `SpyEffect.Custom=yes` is not stated in the documentation (open question 1). The Phobos handler returns a flag that says "custom effect handled" when the key is set, which suggests that it takes the place of the stock effect for the parts that Phobos owns; confirm with a test before relying on either reading.

Stock YR: infiltration has hard coded effects chosen by building kind.

Hooks: `BuildingClass::Spied_By(HouseClass *)` in `code/building.cpp` is the whole infiltration effect. It chooses one effect in order: radar, power plant, build tech, super weapon (resets its timer), cash, war factory, barracks. The new keys go at the top. Super weapons are fired at a cell by `SuperClass::Place(Cell const &, bool player)` (`code/super.cpp`, the routine its comments call `Launch`); a launch for a house needs that house's `SuperClass` for the type (`House->SuperWeapon[type]`, as `Spied_By` already does for the reset). Save the old recharge timer, call `Place` with the building's cell and `player=false`, then restore the timer. Per type read: `BuildingTypeClass` (`code/builtype.cpp`). `Place` switches on the super weapon type, and some types need a firing house, a source cell or other state set beforehand (the Chronosphere uses `ChronoSource`), so check what each type reads when no player designated a target.

## Open questions

1. Whether `SpyEffect.Custom` replaces or adds to the stock `Spied_By` effect.
2. `HealthBar.HidePips` versus `HealthBar.Hide` exact visual difference.
3. Whether placing for the infiltrator requires the super weapon type to be owned or enabled for that house.
