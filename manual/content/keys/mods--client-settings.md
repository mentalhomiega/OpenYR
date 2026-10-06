---
key: Mods
scope: client-settings
label: Mods
summary: The mods this player starts the game with, in the order they are read.
see_also: [MenuStyle]
when_omitted:
  kind: context-dependent
  note: The list in `Mods=` under `[Paths]` in `OPENTS.INI`, or no mods when that file gives none.
---

```ini title="RA2MD.INI"
[Options]
Mods=HighTech,MapPack
```

`Mods=` under `[Options]` lists the [mods](/using/mods/) this player starts the game with, in the order they are read, so a later mod overrides an earlier one. Separate the names with commas; spaces around a name are ignored. A name is a folder in the `Mods` folder of the game data directory, and a path that starts with a drive letter or a backslash is used as written.

When the key is present, its list replaces the one in `Mods=` under `[Paths]` in [`OPENTS.INI`](/formats/opents-ini/#the-mods-it-starts-with). A key with nothing after the equals sign is ignored, so write a single comma, `Mods=,`, to start with no mods; it names no mod and overrides the deployment's list. The Mods screen writes it when every mod is turned off. [`-MOD=`](/using/command-line/mod/) adds its mods after the ones this key lists.

The game reads the key when it starts, so a change takes effect at the next start. The [Mods screen](/using/mods/#the-mods-screen) writes it, and the file's other settings are kept.
