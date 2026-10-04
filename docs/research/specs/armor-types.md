# Custom armor types and Versus.* (Ares)

Source: Ares, https://ares-developers.github.io/Ares-docs/new/additionalarmortypesandverses.html. Phobos adds `EffectsRequireVerses`: https://phobos.readthedocs.io/en/latest/Fixed-or-Improved-Logics.html (section "Customizable Warhead trigger conditions").

## Keys

| Key | File and section | Type | Default |
|---|---|---|---|
| `<name>=<armor>` or `<name>=<N%>` | rules, `[ArmorTypes]` | new armor name = existing armor name, or a percentage | none |
| `Versus.<name>` | rules, each `[WarheadType]` | percentage (or float) | the value declared in `[ArmorTypes]` |
| `Versus.<name>.ForceFire` | warhead | boolean | derived from the Versus value, see below |
| `Versus.<name>.Retaliate` | warhead | boolean | derived from the Versus value |
| `Versus.<name>.PassiveAcquire` | warhead | boolean | derived from the Versus value |
| `EffectsRequireVerses` (Phobos) | warhead | boolean | `true` |

## Declaring armor types

The stock engine has 11 armor types (none, flak, plate, light, medium, heavy, wood, steel, concrete, special_1, special_2), set per object by `Armor=` and per warhead by the comma list `Verses=`. Ares lets a mod add more names in `[ArmorTypes]`. There are two forms of an entry:

- `paper=steel` makes `paper` start with the same Versus values as `steel` for every warhead. A warhead then overrides it with `Versus.paper=`.
- `magic=11%` makes every warhead start at 11% against `magic`.

Objects use a new armor through `Armor=paper` in their own type section, as with stock armors. Warheads set damage against it with `Versus.paper=50%`. The original 11 stay on the legacy `Verses=` list (Westwood spelling); custom ones use only `Versus.<name>`.

## Special values: targeting is separate from damage

A Versus value controls two things: the damage multiplier, and whether weapons of that warhead may choose the target. Three low values act as markers:

| Versus value | Force-fire | Retaliation | Passive (auto) acquisition |
|---|---|---|---|
| 0% | blocked | blocked | blocked |
| 1% | allowed | blocked | blocked |
| 2% | allowed | allowed | blocked |
| 3% or more | allowed | allowed | allowed |

Each `.ForceFire`, `.Retaliate` and `.PassiveAcquire` key overrides its column of that table. A mod makes a target immune to damage but still attackable by setting the Versus value to 0% and the three flags to `yes`. A target that takes damage but is never picked automatically uses 2% or `PassiveAcquire=no`.

## Phobos EffectsRequireVerses

Phobos warhead effects (the extra effects it adds, not the damage itself) normally need a nonzero Versus against the target's armor. With `EffectsRequireVerses=no`, those effects still happen when the Versus is 0%. Stock and Ares effects are unchanged.

## What stock YR does

Only the 11 stock armors exist. `Verses=` is read as a comma list; a list shorter than 11 entries crashes the original game. A 0% value already stops weapon selection in some paths (the engine treats `Modifier[armor] == 0` as "cannot hurt").

## Where it hooks in OpenYR

- `WarheadTypeClass` (`code/warhead.h`, `code/warhead.cpp`): holds `double Modifier[ARMOR_COUNT]` and parses `Verses=` in its INI read (the `Verses` `Get_String` call). Custom armors need this array to become dynamic, a parse of `Versus.<name>` per declared armor, and three more flags (force-fire, retaliate, passive) per armor.
- `ArmorType`, `ARMOR_COUNT`, `ArmorName[]` (`code/const.cpp`, `code/globals.h`) and `Armor_From_Name` (`code/weapon.cpp`, also called from `code/ccini.cpp`): the fixed enum and name table. These must read `[ArmorTypes]` and extend the name list before any type section is parsed.
- `ObjectTypeClass::Armor` (`code/objtype.h`, `code/objtype.cpp`): the armor per type, parsed through `Armor_From_Name`.
- `Modify_Damage` (`code/combat.cpp`): applies `warhead->Modifier[armor]`. The damage path needs no behaviour change once the array is dynamic.
- Targeting tests of `Modifier[...] == 0`, all in `code/techno.cpp`: weapon choice (the `first`/`second` weapon check near the start of the weapon selection routine), `Evaluate_Object` and threat scoring (`target_warhead->Modifier[ttype->Armor]`), retaliation (the routine that returns false when `Modifier[source->TClass->Armor] == 0`) and the infantry and unit enemy scans. Each must switch from `== 0` to the matching flag: automatic scan uses PassiveAcquire, retaliation uses Retaliate, manual attack and the force-fire cursor use ForceFire.
- `code/unit.cpp` also reads `warhead->Modifier[techno->TClass->Armor]` for its own damage estimate.
- Save and CRC: `WarheadTypeClass::Serialize` and the type CRC must include the new per-armor data.

## Open questions

1. The Ares page does not say whether the 1% and 2% markers still apply when the value comes from an `[ArmorTypes]` `<name>=<N%>` default.
2. It does not state what happens if a mod declares a custom armor with the same name as a stock one.
3. Percent syntax: stock accepts `50%` and `0.5`. The page does not say whether `Versus.*` accepts both. Assume both, as `Verses=` does.
