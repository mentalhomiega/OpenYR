# AttackFriendlies, AttackCursorOnFriendlies and DefaultToGuardArea

Sources:
- ModEnc `AttackFriendlies` (stock key): https://modenc.renegadeprojects.com/AttackFriendlies
- ModEnc `DefaultToGuardArea`: https://modenc.renegadeprojects.com/DefaultToGuardArea
- Phobos weapon level overrides: https://phobos.readthedocs.io/en/latest/Fixed-or-Improved-Logics.html (sections "Can attack allies", "Customize the global default values of some vanilla/Ares flags", "Customize `DefaultToGuardArea` per gunner mode")

## Keys

| Key | File and section | Type | Default | Source |
|---|---|---|---|---|
| `AttackFriendlies` | rules, TechnoType | boolean | `no` | stock YR |
| `AttackCursorOnFriendlies` | TechnoType | boolean | `no` | stock YR |
| `AttackFriendlies` | WeaponType | boolean | unset (use the firer's) | Phobos |
| `AttackCursorOnFriendlies` | WeaponType | boolean | unset (use the firer's) | Phobos |
| `DefaultToGuardArea` | TechnoType | boolean | `no` | stock YR |
| `DefaultToGuardArea` | rules, `[General]` | boolean | `no` | Phobos (global default for the per type key) |
| `DefaultToGuardArea.Modes` | TechnoType with `Gunner=yes` | list of integers (IFV modes), `-1` = all | `-1` | Phobos |
| `DefaultToGuardArea.AIModes` | same | list of integers | `-1` | Phobos |

## Behaviour

- `AttackFriendlies=yes`: when scanning for targets, the object may also pick friendly objects. It is documented for infantry, vehicles, aircraft and buildings. In stock YR an AI building with this flag has its target reset every frame; Phobos fixes it, and an engine should not reproduce that fault.
- `AttackCursorOnFriendlies=yes`, or `AttackFriendlies=yes`: the attack cursor appears over friendly objects when hovering. Attacking an ally may still need the force-fire key (the documentation mentions Alt).
- Phobos lets a weapon override both flags for its firer. The weapon's value wins when set. When a unit has several weapons, the flag that matters is the one of the weapon that would be used on the target.
- `DefaultToGuardArea=yes`: when idle after combat, the object takes the area guard mission instead of plain guard. The guard radius is `1.1 * min(2 * GuardRange, 4096 leptons)`. The veteran ability `GUARD_AREA` gives the same effect by rank.
- Phobos `[General] DefaultToGuardArea` is the default for objects that do not set the key. `DefaultToGuardArea.Modes` restricts the effect to certain gunner modes for human players, and `.AIModes` for computer players.

## What stock YR does

The two unit flags exist and the engine honours them in targeting and the cursor. Per weapon overrides and the global default do not exist.

## Where it hooks in OpenYR

None of these keys is read today (no match for `AttackFriendlies`, `AttackCursorOnFriendlies` or `DefaultToGuardArea` in `code/`).

- Cursor: `TechnoClass::What_Action(ObjectClass const *, bool)` in `code/techno.cpp`. The attack branch is guarded by `ctrldown || !House->Is_Ally(object)`; add `|| attack-cursor-on-friendlies` for the selected weapon (from `What_Weapon_Should_I_Use`, which is called inside that branch). The cell version `What_Action(Cell const &, bool, bool)` needs the same check if cells can hold friendly objects.
- Targeting: `TechnoClass::Evaluate_Object` and `Greatest_Threat` (`code/techno.cpp`) reject allies. With `AttackFriendlies`, allied objects must pass. Related: `TechnoClass::Take_Damage` retaliation already refuses allies (`House->Is_Ally(source)`), which should stay.
- AI building target reset bug: when an assigned ally target is dropped each frame by the scan in `Greatest_Threat_Scan`/`TechnoClass::AI`, keep it. Check how `Assign_Target` and the validity test treat allied targets.
- Guard area: the default mission after idle is chosen in the mission code (`FootClass::Mission_Guard_Area` in `code/foot.cpp` is the area guard). The point where an idle object picks `MISSION_GUARD` versus the area guard mission needs the flag; find it by searching for `MISSION_GUARD` assignments in `code/unit.cpp`, `code/infantry.cpp` and `code/techno.cpp` (`Enter_Idle_Mode`). The `GUARD_AREA` ability already exists as `ABILITY_GUARD_AREA` in `code/ability.hh`, so the same test point can check both.
- Weapon read: `WeaponTypeClass` (`code/weapon.cpp`). Type read: `TechnoTypeClass` (`code/techtype.cpp`); global read: `RulesClass` general (`code/rules.cpp`).
- All three are small and need no save format change beyond the new type fields.

## Open questions

1. Whether a weapon level `AttackFriendlies=no` blocks a firer whose type has `AttackFriendlies=yes`. Phobos says the weapon value overrides.
2. Whether `AttackFriendlies` also lets the object auto fire at allies, or only lets it be ordered to. The stock page says "when scanning for targets", so automatic.
3. Which mission an area guard unit enters for a human player when the `Modes` list does not include the current gunner mode.
