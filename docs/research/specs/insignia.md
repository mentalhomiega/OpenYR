# Veterancy insignia (Ares and Phobos)

Sources:
- Ares: https://ares-developers.github.io/Ares-docs/new/customizableinsignia.html
- Phobos: https://phobos.readthedocs.io/en/latest/Fixed-or-Improved-Logics.html (section "Customizable veterancy insignias")

Phobos extends the Ares keys and wins when both are installed. A mod that targets Phobos can use everything below.

## Keys

| Key | Section | Type | Default | Source |
|---|---|---|---|---|
| `Insignia.Rookie`, `Insignia.Veteran`, `Insignia.Elite` | rules, TechnoType | SHP name without extension | `pips.shp` | Ares, Phobos |
| `Insignia` | TechnoType | SHP name; the file for all three ranks | `pips.shp` | Phobos |
| `InsigniaFrame.Rookie`, `.Veteran`, `.Elite` | TechnoType | integer, zero based, `-1` = default | `-1` (rookie: none, veteran: 14, elite: 15) | Ares, Phobos |
| `InsigniaFrame` | TechnoType | integer | `-1` | Phobos |
| `InsigniaFrames` | TechnoType | three integers: rookie, veteran, elite | `-1,-1,-1` | Phobos |
| `Insignia.ShowEnemy` | TechnoType | boolean | `[General] EnemyInsignia` | Ares, Phobos |
| `EnemyInsignia` | `[General]` | boolean | `yes` | Ares |
| `Insignia.WeaponN` (+ `.Rookie/.Veteran/.Elite`), `InsigniaFrame(s).WeaponN` | TechnoType with `Gunner=yes` | as above | falls back to the plain keys | Phobos |
| `Insignia.PassengersN` (+ ranks), `InsigniaFrame(s).PassengersN` | transport TechnoType | as above, N = current passenger count | falls back | Phobos |
| `DrawInsignia.OnlyOnSelected` | `[AudioVisual]` | boolean | `no` | Phobos |
| `DrawInsignia.AdjustPos.Infantry`, `.Units`, `.Buildings` | `[AudioVisual]` | X,Y pixels | `5,2` / `10,6` / `10,6` | Phobos |
| `DrawInsignia.AdjustPos.BuildingsAnchor` | `[AudioVisual]` | `top`, `lefttop`, `leftbottom`, `bottom`, `rightbottom`, `righttop` | none | Phobos |
| `DrawInsignia.UsePixelSelectionBracketDelta` | `[AudioVisual]` | boolean | `no` | Phobos |
| `InsigniaType` | Phobos type system | see Miscellanous page | none | Phobos |

## Behaviour

- The insignia is a frame of an SHP drawn with `palette.pal` (the unit palette), like the stock `pips.shp`. The file name has no extension.
- Priority for what is drawn: `InsigniaType` settings (Phobos) over weapon mode keys over passenger count keys over the plain per rank keys over the shorthand `InsigniaFrames` over defaults.
- Frame `-1` means the stock frame for that rank. Rookie has no stock insignia, so `-1` draws nothing for rookie.
- Enemy players see an insignia only if `Insignia.ShowEnemy` (or the global) is true. Observers always see every insignia (Ares).
- Phobos `Insignia.WeaponN` applies only to `Gunner=yes` units (IFVs) and follows the current gunner mode (`N` is one based). `Insignia.PassengersN` follows the transport's current passenger size.

## What stock YR does

Every unit shows the stock chevron: frame 14 of `pips.shp` for veteran, frame 15 for elite, drawn at a fixed offset beside the health bar (infantry offset `+5,+2`, others `+10,+6`). The same graphic is used for every type. Rookie shows nothing. A below rookie ("dumbass") frame exists too.

## Where it hooks in OpenYR

- `TechnoClass::Draw_Insignia(Point2D const &, Point2D const &, Rect const &)` in `code/techno.cpp`. It already picks `PIP_VETERAN`, `PIP_ELITE` or `PIP_DUMBASS` from `Veterancy` and draws from `Class_Of()->PipShapes` at `center + (5,2)`, plus `(5,4)` for non-infantry. The caller is `TechnoClass::Draw_It`/`Draw_Pips` region (the call near the `Pip_Origin` use). Per type files and frames replace `PipShapes` and the frame index here.
- The visibility test (enemy rule) belongs to the caller: it must compare the techno's house against `PlayerPtr` and observer state before drawing.
- `TechnoTypeClass` rules read (`code/techtype.cpp`) for the key set. `PipShapes` is on `TechnoTypeClass`; custom insignia files need to load SHPs through the same mix lookup (`MFCD::Retrieve`) and keep the file name for save reload, as `CameoData` does.
- Gunner mode and passenger count are available on `UnitClass`/`TechnoClass` (`Cargo.How_Many()` seen in `TechnoClass::What_Action`); the gunner mode field is in `code/unit.cpp`/`techno.cpp` (search `Gunner`).
- Global keys: `RulesClass` general and audiovisual reads (`code/rules.cpp`).

## Open questions

1. Whether the Ares offsets differ from the stock ones (Phobos documents its own defaults; Ares says nothing).
2. Frame index for the below-rookie "dumbass" rank under custom files.
3. Behaviour of an insignia SHP with fewer than 16 frames when the default index is used.
