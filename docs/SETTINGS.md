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
| `.ZeroText` | The text a slider shows in place of its number and unit at 0, such as `Auto`. |
| `.BeforeRenderer` | Also reads the key at startup before the renderer starts, for a setting the renderer needs at its start. |

The row kind comes from the entry: a `.Flag`, or a `.Names` setting with `.SwitchOn`, is a switch, and any other
`.Number` with a label is a slider. Drop-down rows are not built yet.

A row whose `.Page` names a page in `Pages[]` appears on that page with no other change. A row on `graphics` appears on the Graphics page.

Add the key to the manual with `python manual/tools/manage.py scaffold key <Key>`. The catalog reads the table, so
the key is listed without any change to the manual tooling.

## Adding a page

A page made only of declared rows is one entry in `Pages[]` in `code/optionsettings.cpp`:

```cpp
constexpr SettingPage Pages[] = {
	{ .Id = PERFORMANCE, .Label = "Performance" },
};
```

`.Id` is the name the rows give in `.Page`, and `.Label` is the text of the page's tab and of its button in the
options menu. Then give each row `.Page` set to that id. The table order is the order of the tabs.

The page needs no screen code and no markup. In the modern menu style it is a tab of the Settings screen, after the
built-in tabs; in the classic style's options menu (`ui/mainopt.rml`) it is a button above Main Menu. Both open the
one generic page, `code/ui/screens/settingspage/` with `ui/settingspage.rml`, which shows:

- the page's rows under their group headings, from the shared row markup in `ui/setting-rows.rml`;
- the description panel, which shows the tip of the row the pointer or the focus is on;
- Reset to defaults, which puts back the defaults of the page on show; Back, which drops every change; and OK, which
  writes the changed rows of every declared page to `Options`. Nothing reaches `Options` before OK, and the settings
  are saved when the options close.

All declared pages share one presenter and one document. Each page keeps its pending values while the player moves
between tabs.

The classic style normally shows its own one-page options (`Classic_Options_Page` in `code/mainopt.cpp`) in place of
the options menu, and that page has no button for the declared pages or the Graphics page. The declared pages are
therefore reached from the modern menu style.

Some parts stay written by hand:

- The Graphics page is its own screen (`code/ui/screens/display/`), and the rows on `graphics` are listed among its
  hand-written rows. It is not in `Pages[]`.
- The built-in tabs (Game, Display, Graphics, Audio, Keyboard, Accessibility, Mods) keep their `UISettingsTab` values,
  their entries in `UI_Settings_Tab_Bar` (`code/ui/uisettingshost.cpp`) and their presenters in `Build_Session`
  (`code/ui/uisettings.cpp`). The tab bar is built from that list: the menu style's frame holds an empty `#tabs`, and
  no tab button is written in markup.
- A tab number is held in a 32-bit mask, so at most 25 declared pages get a tab; a page past that is left out.
- The classic options menu sizes its panel by its button count in `ui/mainopt.rcss` (`.count-7` for one declared
  page). A second declared page needs a `.count-8` rule there; the modern style already has rules up to eight.

## What the table order decides

A save writes each entry in table order, and `INIClass` moves a key it writes to the end of its section. The table
order is therefore the order of those keys in a saved file, and keys the table does not declare stay above them. To
keep a saved file the same as before, add a converted key at the place its old `Put_` call had.

A page shows its rows grouped: each group under its `.Group` heading, in the order the table first names the group.
The row markup is one `data-for` block in the template `ui/setting-rows.rml`, which `ui/graphics.rml` and `ui/settingspage.rml` both insert; it needs no edit for a new row.

## Reading and greying

A key the file does not hold keeps the member's value, as the old code did. Unknown keys and sections in a settings
file are never touched: a save only rewrites declared keys.

`UI_Setting_Row_Execute` ignores an intent for a greyed row, the same check the row's markup shows. On the Graphics page, `Set_Declared_Settings`
writes every row back when any value changed, then calls `Apply_HDR_Highlights`.

## Converted so far

The `[Video]` GPU group (`GpuRender` to `WeatherStrength`, with the five GPU voxel keys), `SteadyVoxelPlacement`, `ShadowStacking`, `RenderThread` and `PerfOverlay`, and the two `[Options]` path search keys `ParallelPaths` and `PathThreads`. `RenderThread` and the path search keys are the rows of the Performance page. `.Names` and `.Number`
cover the rest of `[Video]` and `[Options]` once each is moved beside the code that uses it.
