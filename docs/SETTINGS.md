# Settings registry

Each converted client setting is one table entry in `code/optionsettings.cpp`. From that entry the
engine reads and writes the `RA2MD.INI` key, builds its row on a settings page and greys the row.
The registry code is in `code/settingregistry.h`, and the page rows are in `code/ui/uisettingrows.h`.

Settings that are not converted stay in `OptionsClass::Load_Settings` and `Save_Settings`. Settings
that affect simulation stay out of the registry: its load path is for presentation settings, and a
converted setting must keep its key, default and value format.

## Adding a setting

1. Add the member to `OptionsClass` in `code/options.h`.
2. Copy the nearest entry in `Settings[]` in `code/optionsettings.cpp` and edit it.

The fields are:

| Field | Meaning |
|---|---|
| `.Section`, `.Key` | The section and key in `RA2MD.INI`. |
| `.Flag` or `.Number` | The `bool` or `int` member that holds the value. Exactly one is set. |
| `.Default` | The value before the file is read. The constructor applies it, so `OptionsClass` needs no initializer. |
| `.Min`, `.Max` | The range a `.Number` is held to when read and on its slider. Equal values mean no range. |
| `.Names` | A table of file names for a `.Number`, saved as text. An unknown name reads as `.Default`. |
| `.Page`, `.Group`, `.Label`, `.Tip` | The settings page, the group heading, and the row's text. Without `.Label` the setting has no row. |
| `.Needs` | The key of another row on the same page. This row is greyed while that one is off or greyed. |
| `.Condition` | A reason for greying that is not a setting, such as `SettingCondition::MODERN_INTERFACE`. The page answers it. |
| `.NeedsRestart` | Adds "Applies at the next start." to the tip. |
| `.SwitchOn` | Shows a `.Names` setting as a switch: ticking it picks this value, unless the page opened on another non-off value. |
| `.Unit` | The text after a slider's number, such as `%`. |
| `.BeforeRenderer` | Also reads the key at startup before the renderer starts, for a setting the renderer needs at its start. |

The row kind comes from the entry: a `.Flag`, or a `.Names` setting with `.SwitchOn`, is a switch, and any other
`.Number` with a label is a slider. Drop-down rows are not built yet.

Add the key to the manual with `python manual/tools/manage.py scaffold key <Key>`. The catalog reads the table, so
the key is listed without any change to the manual tooling.

## What the table order decides

A save writes each entry in table order, and `INIClass` moves a key it writes to the end of its section. The table
order is therefore the order of those keys in a saved file, and keys the table does not declare stay above them. To
keep a saved file the same as before, add a converted key at the place its old `Put_` call had.

A page shows its rows grouped: each group under its `.Group` heading, in the order the table first names the group.
The page markup lists the rows in one `data-for` block (`ui/graphics.rml`) and needs no edit for a new row.

## Reading and greying

A key the file does not hold keeps the member's value, as the old code did. Unknown keys and sections in a settings
file are never touched: a save only rewrites declared keys.

`UI_Setting_Row_Execute` ignores an intent for a greyed row, the same check the row's markup shows. `Set_Declared_Settings`
writes every row back when any value changed, then calls `Apply_HDR_Highlights`.

## Converted so far

The `[Video]` GPU group (`GpuRender` to `WeatherStrength`, with the three GPU voxel keys), `SteadyVoxelPlacement` and `RenderThread`. `.Names` and `.Number`
cover the rest of `[Video]` and `[Options]` once each is moved beside the code that uses it.
