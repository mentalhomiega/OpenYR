# Airburst spread, scatter, projectile ZAdjust and jumpjet crash (Phobos)

Source: Phobos, https://phobos.readthedocs.io/en/latest/Fixed-or-Improved-Logics.html (sections "Airburst & Splits", "FlakScatter distance customization", "ZAdjust for Projectiles", "Customizing locomotor warhead"). The Phobos documentation was read; no Phobos code is copied.

## Keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `AirburstSpread` | rules, projectile (BulletType) with `Airburst=yes` | float, cells | not stated in the Phobos page; stock YR fixes it in code (open question 1) |
| `Airburst.UseCluster` | `[CombatDamage]` and projectile | boolean | `false` |
| `Airburst.RandomClusters` | projectile | boolean | `false` |
| `Airburst.TargetAsSource` | projectile | boolean | `false` |
| `AroundTarget` | projectile with `Airburst=yes` or `Splits=yes` | boolean | the value of `Splits` |
| `BallisticScatter.Min` | rules, projectile with `FlakScatter=yes` | float, cells | `0` |
| `BallisticScatter.Max` | projectile | float, cells | `[CombatDamage] BallisticScatter` |
| `ZAdjust` | art, projectile image section (shape images only) | integer | `0` |
| `JumpjetCrash` | rules, WarheadType with `IsLocomotor=yes` and `Locomotor=Jumpjet` | float | the target's TechnoType `JumpjetCrash` |

## Behaviour

Airburst and Splits:
- With `Airburst=yes`, the projectile explodes in the air and fires its `AirburstWeapon` at each cell within `AirburstSpread` of the burst point. `AirburstSpread` is that radius in cells.
- `Airburst.UseCluster` limits the number of affected cells to `Cluster`. With `Airburst.RandomClusters` the cells are chosen at random; otherwise they are spaced evenly from the centre outward.
- `Airburst.TargetAsSource` fires from the intended target's position rather than the burst point. `Airburst.TargetAsSource.SkipHeight` keeps the burst height.
- `AroundTarget` decides where the search for split or airburst targets is centred: the original projectile's intended target (true) or the point where it exploded (false). It defaults to the `Splits` value, so a Splits projectile looks around its target by default and an Airburst projectile around the burst point.

BallisticScatter:
- With `FlakScatter=yes` on an `Inviso` projectile, the stock scatter distance is chosen between `0` and `2 * [CombatDamage] BallisticScatter` cells. `BallisticScatter.Min` and `.Max` replace the minimum and maximum before the doubling, so the range is `2*Min` to `2*Max` cells. Unset keys keep the stock values.

ZAdjust (projectile):
- A manual depth correction for projectiles drawn from an SHP. Use it when an automatically computed depth puts a missile behind the building it launches from. Voxel projectiles ignore it.

JumpjetCrash:
- A locomotor warhead that lifts a vehicle by jumpjet can set the crash speed the lifted vehicle falls with. The other jumpjet keys on warheads (`JumpjetSpeed`, `JumpjetClimb`, `JumpjetHeight`, `JumpjetAccel`, `JumpjetTurnRate`, `JumpjetWobbles`, `JumpjetNoWobbles`, `JumpjetDeviation`) follow the same pattern: unset means the target type's own value.

## What stock YR does

- Airburst weapon: fires at an area fixed by the stock radius with no cluster control.
- `FlakScatter`: scatter range fixed by `[CombatDamage] BallisticScatter`.
- Projectile depth: calculated automatically from height and position.
- A locomotor warhead imbues the jumpjet locomotor with the target's own jumpjet values.

## Where it hooks in OpenYR

- Airburst: `BulletClass::Detonate` in `code/bullet.cpp` contains the airburst and splits code. It gathers technos within a fixed 5 cells of `split_coord`, tops up the target list with random cells in a +-3 square, and creates bullets for `Class->Cluster` entries from `Class->AirburstWeapon`. `AirburstSpread` replaces the fixed distance and square; `AroundTarget` chooses `split_coord` (today it is the target's position when `TarCom` is an object, else the bullet position); the cluster options change how many cells are targeted. Keys are read in `BulletTypeClass` (`code/bullettype.cpp`, near the `Airburst`, `Splits` and `AirburstWeapon` reads) and saved in its `Serialize` and CRC functions.
- Scatter: the scatter code is in `BulletClass::Unlimbo` (`code/bullet.cpp`, around the `Rule->BallisticScatter` use) and `BulletClass::Bullet_Explodes` (`Coord_Scatter` with `CELL_LEPTON`). `FlakScatter` is not read by the engine (no match in `code/`); the live scatter path is the `IsInaccurate` handling in `Unlimbo`, whose scatter block is disabled by `#if 0` today. Establish what stock YR does for `FlakScatter` and `Inviso` projectiles before wiring `BallisticScatter.Min/Max`.
- ZAdjust: `BulletClass::Draw_It` (`code/bullet.cpp`) calls `Draw_Shape` with the fixed extra depth `-30 - TacticalMap->Z_Lepton_To_Pixel(Height)`; the shadow call uses `-10`. A per type adjustment adds to the `-30`. Read the key in `BulletTypeClass` art load (`code/bullettype.cpp`). Animations already carry a `ZAdjust` member (`AnimClass::ZAdjust`, `code/anim.cpp`) but `AnimTypeClass` does not read the key from art (no `ZAdjust` in `code/animtype.cpp`); the same one line read would give anim art the stock key that mods use widely.
- JumpjetCrash: `BulletClass::Detonate` handles `warhead->IsLocomotor` by calling `target->Imbue_Locomotor(Payback, warhead->Locomotor)`. The jumpjet locomotor (`code/jumpjet.cpp`, `JumpjetLocomotionClass`) has no crash speed variable today; `TechnoTypeClass` reads `JumpjetClimb`, `JumpjetHeight` and `JumpjetAccel` but not `JumpjetCrash` (`code/techtype.cpp`). This key needs the stock `JumpjetCrash` read, the falling behaviour that uses it, and a warhead override. It is the largest item in this group.

## Open questions

1. The stock default for `AirburstSpread` and whether the scan uses a square or a circle. In OpenYR today the area is a 5 cell radius for technos and a +-3 square for fill cells.
2. Whether the Phobos `Cluster` default for Airburst equals the stock `Cluster` key.
3. How `ZAdjust` interacts with the `SHAPE_ZGRAD` depth mode used for bullets.
