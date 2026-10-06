# Custom palettes and PCX cameos (Ares)

Sources:
- CustomPalette: https://ares-developers.github.io/Ares-docs/new/customanimationandprojectilepalettes.html
- PCX cameos: https://ares-developers.github.io/Ares-docs/new/pcxcameos.html

## Keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `CustomPalette` | art, an `[AnimationType]` section, or a projectile image section | file name with `.pal` | none (the normal palette) |
| `CameoPCX` | art, the unit's image section | file name with `.pcx` | none (use the SHP cameo) |
| `AltCameoPCX` | art, the unit's image section | file name with `.pcx` | none |
| `SidebarPCX` | rules, `[SuperWeaponType]` | file name with `.pcx` | none |

## CustomPalette

- The named palette file replaces the palette used to draw the animation or projectile.
- Three tildes in the name are replaced by the theater's three letter code. `lib~~~.pal` becomes `libtem.pal`, `libsno.pal`, and so on. A name without tildes is used in every theater.
- For a projectile, the projectile image must also be listed in `[Animations]` or the key has no effect (the documentation states this without saying why; it seems the key is only read for entries in that list).
- Introduced in Ares 0.2.

## PCX cameos

- With `CameoPCX=` set, the sidebar draws that PCX instead of the SHP named by `Cameo=`. `AltCameoPCX` does the same for the alternate cameo (the one shown for the second faction variant or promoted look, as the stock `AltCameo` is).
- `SidebarPCX` is the same for a super weapon button.
- The file must be a 256 colour PCX of exactly 60 by 48 pixels, and the extension must be in the key value.
- The documentation says nothing about fallback when the file is missing or wrong sized, the search path (mix archives or loose files), or the palette used.

## What stock YR does

Animations and projectiles use the theater or unit palette and cannot choose another. Cameos are SHP files (`Cameo=` in art), drawn with the sidebar palette.

## Where it hooks in OpenYR

Palette:
- `AnimTypeClass` art read (`code/animtype.cpp`, where `AltPalette` is already read) gets `CustomPalette`. Draw: `AnimClass::Draw_It` (`code/anim.cpp`) picks a `ConvertClass * convert` (player colour scheme converter or `cellptr->Drawer`). A custom palette needs a `ConvertClass` built from the `.pal` file (`code/_convert.h`, `code/_palette.cpp`, `code/palette.cpp`) cached on the type, with the theater suffix resolved at load.
- `BulletClass::Draw_It` (`code/bullet.cpp`) uses `NormalDrawer`/`drawer` for projectile shapes; it needs the same override. Parsing for projectile images is in `code/bullettype.cpp`.

PCX cameos:
- Stock cameo loading: `TechnoTypeClass` art read (`code/techtype.cpp`, the `Cameo` `Get_String` call that fills `CameoData` and falls back to `XXICON.SHP`), `SuperWeaponTypeClass` (`code/suprtype.cpp`, `CameoData`), and infantry via `InfantryTypeClass::Get_Cameo_Data` (`code/infatype.cpp`).
- Sidebar drawing: `SidebarClass::StripClass::Draw_It` (`code/sidebar.cpp`) calls `obj->Get_Cameo_Data()` and draws it as a `ShapeSet`. A PCX cameo needs a second pointer or a tagged variant on the type and a blit path for a PCX surface (`code/pcx.cpp`, `code/pcx.h` already exist).
- Other users of the cameo pointer must handle it too: `code/building.cpp` (the factory cameo preview over the building) and `code/dropship.cpp` (loadout dialog) call `Get_Cameo_Data()` and cast to `ShapeSet`. The simplest design is to convert the PCX to a one-frame `ShapeSet` at load, so every caller keeps working; that costs one conversion per type.
- Save games store the cameo pointer by name (`TechnoTypeClass` load fetches `CameoData` again), so keep the file name on the type.

## Open questions

1. PCX lookup order and what is drawn when the file is missing (assume fall back to the SHP cameo).
2. Whether non-60x48 PCX files are rejected or scaled.
3. Which palette indexes of the PCX are treated as transparent, if any.
4. Whether `CustomPalette` also applies to the shadow and to team colour remapping.
