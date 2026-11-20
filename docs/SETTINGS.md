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
| `.Fold` | The name of a fold the group sits in. See [Folds and presets](#folds-and-presets). |
| `.Needs` | The key of another row on the same page. This row is greyed while that one is off or greyed. |
| `.Condition` | A reason for greying that is not a setting, such as `SettingCondition::MODERN_INTERFACE`. The page answers it. |
| `.NeedsRestart` | Adds "Applies at the next start." to the tip. |
| `.SwitchOn` | Shows a `.Names` setting as a switch: ticking it picks this value, unless the page opened on another non-off value. |
| `.Unit` | The text after a slider's number, such as `%`. |
| `.ZeroText` | The text a slider shows in place of its number and unit at 0, such as `Auto`. |
| `.BeforeRenderer` | Also reads the key at startup before the renderer starts, for a setting the renderer needs at its start. |

The row kind comes from the entry: a `.Flag`, or a `.Names` setting with `.SwitchOn`, is a switch, and any other
`.Number` with a label is a slider. A declaration has no drop-down row; a preset choice is one (see below), and the `CHOICE` row kind (a drop-down over
`Items`) and a locked row with a `Note` are built by the skirmish lobby for a mod's game options (LOBBY_MODES.md).

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
  hand-written rows, below the preset choice and the Advanced fold. It is not in `Pages[]`.
- The built-in tabs (Game, Display, Graphics, Audio, Keyboard, Accessibility, Mods) keep their `UISettingsTab` values,
  their entries in `UI_Settings_Tab_Bar` (`code/ui/uisettingshost.cpp`) and their presenters in `Build_Session`
  (`code/ui/uisettings.cpp`). The tab bar is built from that list: the menu style's frame holds an empty `#tabs`, and
  no tab button is written in markup.
- A tab number is held in a 32-bit mask, so at most 25 declared pages get a tab; a page past that is left out.
- The classic options menu sizes its panel by its button count in `ui/mainopt.rcss` (`.count-7` for one declared
  page). A second declared page needs a `.count-8` rule there; the modern style already has rules up to eight.

## Folds and presets

A page can fold groups of rows under a heading that opens and closes them. Give each row of those groups the same
`.Fold` text, such as `.Fold = ADVANCED`. The page shows the fold's groups together, at the place its first group
would be, under a button reading "[+] Show Advanced" while they are hidden and "[-] Hide Advanced" while they show.
A fold is closed each time the page opens, a hidden row cannot be tabbed to, and opening or closing a fold is not a
setting change: Reset to defaults leaves the fold as it is, and Back and OK ignore it.

A preset choice is a drop-down at the top of a page that sets several rows at once. It is declared in `Presets[]` in
`code/optionsettings.cpp` as a `SettingPresetChoice`: the page, a key, the group heading, the label and tip, and a list
of `SettingPreset`, each a label and the value it gives each of its rows. The choice is never stored and has no key in
`RA2MD.INI`. The page reads it from the rows on every change: it shows the first preset whose values every named row
holds, and "Custom" when none is. A settings file therefore reads as the right preset whatever build wrote it, and
changing any covered row by hand switches the choice to Custom. Choosing Custom changes nothing. Choosing a preset
sets its rows on the page like a hand change, so they reach `Options` on OK through the usual apply path, and Cancel
drops them. The choice greys with the condition the declaration names.

The Graphics page has one preset choice, Preset under the heading Graphics quality, over the rows of the Advanced fold
and the Weather row. Each preset names the same ten rows. Switches no preset names, such as the sharpness slider,
**Steady vehicle placement** and **Darker overlapping shadows**, keep their value and do not change the reading.

| Row | Classic | Enhanced | Full |
|---|---|---|---|
| Draw with the graphics card (`GpuRender`) | on | on | on |
| Soft shadows (`SoftShadows`) | off | on | on |
| Draw vehicles and aircraft in 3D (`GpuVoxels`) | off | on | on |
| Smooth turning (`GpuVoxelSmooth`) | off | on | on |
| Smooth movement when zoomed in (`GpuVoxelSubpixel`) | off | on | on |
| Shockwaves, Glow (`Bloom`), Water ripples, Cloud shadows | off | off | on |
| Weather (`Weather`) | None | None | Map |

Classic is the default of every row it names, so a settings file with none of these keys reads as Classic, and the
picture is the one the game drew before the presets existed. Enhanced changes how the game draws what it already
shows: it draws with the graphics card, softens shadows, and turns and moves vehicles smoothly. Full adds effects that
put new things in the picture. None of the rows changes the game's state, so every preset plays the same game and is
allowed in multiplayer. A preset sets no renderer and no strength slider.

## What the table order decides

A save writes each entry in table order, and `INIClass` moves a key it writes to the end of its section. The table
order is therefore the order of those keys in a saved file, and keys the table does not declare stay above them. To
keep a saved file the same as before, add a converted key at the place its old `Put_` call had.

A page shows its rows grouped: each group under its `.Group` heading, in the order the table first names the group. A preset choice comes first, and the groups of a fold follow its button at the place its first group would be.
The row markup is one `data-for` block in the template `ui/setting-rows.rml`, which `ui/graphics.rml` and `ui/settingspage.rml` both insert; it needs no edit for a new row.

## Reading and greying

A key the file does not hold keeps the member's value, as the old code did. Unknown keys and sections in a settings
file are never touched: a save only rewrites declared keys.

`UI_Setting_Row_Execute` ignores an intent for a greyed row, the same check the row's markup shows. On the Graphics page, `Set_Declared_Settings`
writes every row back when any value changed, then calls `Apply_HDR_Highlights`.

## Converted so far

The `[Video]` GPU group (`GpuRender` to `WeatherStrength`, with the five GPU voxel keys), `SteadyVoxelPlacement`, `ShadowStacking`, `RenderThread`, `FastestGpu`, `PerfOverlay` and `ZoomOutView`, and the two `[Options]` path search keys `ParallelPaths` and `PathThreads`. `ClassicGameSpeeds` is declared with no row: the game controls dialog shows its switch beside the speed slider, and it changes only the frame length of a solo game. `RenderThread`, `FastestGpu` and the path search keys are the rows of the Performance page. `ZoomOutView` (`Original` or `4K`) is the Zoom out to a 4K view switch on the Graphics page: it sets the most map the player's zoom shows, 2560x1440 or 3840x2160 map pixels, and a value the file holds that is not one of the two reads as `Original`, so no setting widens the zoom past 4K. `DebugMode=yes` lifts the limit in a single-player game; scripted scenes are not held to it (docs/PERFORMANCE.md, Zoom changes). `.Names` and `.Number`
cover the rest of `[Video]` and `[Options]` once each is moved beside the code that uses it.
