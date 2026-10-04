# Electric bolt and radiation beam colours (Ares and Phobos)

Sources:
- Ares electric bolts: https://ares-developers.github.io/Ares-docs/new/weapons/electricbolts.html
- Phobos laser trails (`Bolt.Color*`, `Beam.Color`, `Beam.Amplitude` on a `LaserTrailType`): https://phobos.readthedocs.io/en/latest/New-or-Enhanced-Logics.html (section "Laser Trails")
- Phobos bolt options on weapons: https://phobos.readthedocs.io/en/latest/Fixed-or-Improved-Logics.html (section "Electric bolt customizations")

The same key names appear in two places with different meanings, so a mod file can use them on a WeaponType (Ares) or on a LaserTrailType (Phobos). Check the section name when parsing.

## Keys on a WeaponType (Ares)

| Key | Type | Default |
|---|---|---|
| `Bolt.Color1`, `Bolt.Color2`, `Bolt.Color3` | `R,G,B` integers 0 to 255 | palette derived (below) |
| `Bolt.ParticleSystem` | ParticleSystemType or `none` | `[CombatDamage] DefaultSparkSystem` |
| `IsAlternateColor` | boolean (stock key) | `no` |

A weapon draws a bolt when it has `IsElectricBolt=yes`. A bolt is three overlaid strands. Without colour keys, strand 1 and 2 use palette index 10 (index 5 with `IsAlternateColor=yes`) and strand 3 uses index 15 of the theater palette. A set key replaces that colour. The keys also apply to the bolt a Prism Forwarding supporter draws.

## Phobos additions on a WeaponType

| Key | Type | Default |
|---|---|---|
| `Bolt.Disable1`, `Bolt.Disable2`, `Bolt.Disable3` | boolean | `false` |
| `Bolt.Arcs` | integer; `0` draws a straight line | `8` |
| `Bolt.Duration` | frames, clamped to 1..31 | `17` |
| `Bolt.FollowFLH` | boolean | `true` for vehicles, `false` for others |

These are visual only.

## Keys on a LaserTrailType (Phobos, art)

| Key | Type | Meaning |
|---|---|---|
| `DrawType` | `laser`, `ebolt` or `radbeam` | how the trail is drawn |
| `Bolt.Color1`-`3`, `Bolt.Disable1`-`3`, `Bolt.Arcs`, `Bolt.ZAdjust`, `IsAlternateColor` | as above | used when `DrawType=ebolt` |
| `Beam.Color` | `R,G,B` | used when `DrawType=radbeam` |
| `Beam.Amplitude` | float, default `40.0` | height of the radiation beam wave |

Laser trails also need a trail system (types, per frame segments, attachment to technos, projectiles and voxel animations) which is a large, separate feature. Only the weapon keys above are small.

## What stock YR does

The bolt colours come only from the palette indexes above, so every bolt on a theater has the same colours apart from the alternate index. Radiation beams draw with the global radiation beam colour.

## Where it hooks in OpenYR

- `EBoltClass` (`code/ebolt.h`, `code/ebolt.cpp`). `EBoltClass::Draw_It` computes `outer_color = NormalDrawer->Convert_Pixel(IsAlternateColor ? 5 : 10)` and `core_color = NormalDrawer->Convert_Pixel(15)` and then draws the three strands. Replace those two values with per weapon colours when set. `EBoltClass::Set_Owner(TechnoClass *, int weapon)` already remembers which weapon fired the bolt, so the colours can be read from that weapon at draw time (and the owner may be gone by then, so copy the colours into the bolt when it is fired, which also keeps saves simple). Note the current draw uses only two colours (`outer_color` and `core_color`), where Ares names three; confirm how the three strands map before choosing which colour goes with which.
- Weapon read: `WeaponTypeClass` (`code/weapon.cpp`, `code/weapon.h`) for `Bolt.Color1-3`, `Bolt.ParticleSystem`, and Phobos keys. The bolt is created in `TechnoClass::Electric_Zap(AbstractClass *, int which, WeaponTypeClass const *)` in `code/techno.cpp`, which already has the weapon.
- Radiation beam: `RadBeamClass` (`code/radbeam.cpp`) has a `Color` member and a constant `Amplitude = 40.0`. Per beam colour and amplitude need those two to become instance data. They would only be reachable from a laser trail system, which does not exist.
- Save: `EBoltClass` objects are saved with the game; colours added to the bolt must go through its `Serialize`.

## Open questions

1. How the Ares three colours map onto the strands (outer, middle, core). The documentation says colour 3 defaults to palette index 15 and colours 1 and 2 to index 10 or 5.
2. Whether `Bolt.Color*` keys on a weapon in a mod mean the Ares weapon keys or the Phobos laser trail keys, which only the section type tells apart.
3. The default `DefaultSparkSystem` behaviour when `Bolt.ParticleSystem` is unset.
