# OpenYR

OpenYR is an open-source engine for *Command & Conquer: Red Alert 2 — Yuri's Revenge*. It
builds on [OpenTS](https://github.com/OpenTS-Developers/OpenTS), the community rebuild of the
Tiberian Sun engine, and extends it until it runs Yuri's Revenge (version 1.001) from the game's
own data files, behaving as the original does.

The goal is the same as OpenTS's: a playable engine that people can read, fix and extend,
without patching the original executable.

OpenYR is an independent community project. It is not affiliated with or endorsed by Electronic
Arts, and it contains no game assets: you need your own copy of Yuri's Revenge to play.

> **Status: early development.** OpenYR runs skirmish games against the computer on Yuri's
> Revenge maps, but it is not yet a full replacement for the original game. Expect missing
> features and bugs.

## What works

Skirmish games start, play and end with the original rules, art, sounds and maps. Among the
parts already rebuilt to match Yuri's Revenge:

- **Interface:** the tabbed sidebar, command bar, radar layout, cursors, fonts, rank insignia,
  health bars, string tables and the announcer.
- **Factions and economy:** countries and their bonuses, ore and gems, Slave Miners, Chrono
  Miners, ore purifiers, cloning vats, grinders and Bio Reactors.
- **Combat:** Yuri's Revenge armor and weapon rules, mind control, Ivan bombs, temporal weapons,
  radiation, parasites (terror drones and giant squids), magnetrons, gattling weapons, prism
  towers, garrisons, Tank Bunkers, open-topped transports, ship sinking and the Iron Curtain.
- **Superweapons:** nuclear missile, lightning storm, Chronosphere, Iron Curtain, Genetic
  Mutator, Psychic Dominator, paradrops and spy planes.
- **Units:** naval units and shipyards, carriers, V3 and Dreadnought missiles, jumpjets such as
  the Rocketeer, Siege Choppers, spies and their disguises, and deployable units.
- **Computer players:** base planning by side and country, team scripts, and use of bunkers,
  Battle Bunkers and Bio Reactors.

About 1,290 of the 1,585 rules keys that Yuri's Revenge reads are supported.

## Running it

OpenYR needs the data files of an installed copy of Yuri's Revenge, for example from
*Command & Conquer: The Ultimate Collection* on Steam or the EA App.

1. Build OpenYR as described below. There are no prebuilt releases yet.
2. Start `Game.exe`, naming the Yuri's Revenge folder with `-DATADIR` and a folder for your
   settings and saves with `-USERDIR`:

   ```
   Game.exe -DATADIR="C:\Games\Yuri's Revenge" -USERDIR="C:\Games\OpenYR-user" -SPAWN -WIN
   ```

The tested way to play is a skirmish launched with `-SPAWN`, which reads the game setup from
`SPAWN.INI` in the user folder:

```ini
[Settings]
Name=Commander
Scenario=XMP08T2.MAP
Side=0
Color=0
AIPlayers=1
Credits=10000

[HouseCountries]
Multi2=8

[HouseColors]
Multi2=1
```

`Side` and the `HouseCountries` entries are country numbers: 0 America, 1 Korea, 2 France,
3 Germany, 4 Great Britain, 5 Libya, 6 Iraq, 7 Cuba, 8 Russia and 9 Yuri. The campaigns and the
in-game menus have not been tested.

## Building

OpenYR builds like OpenTS: for Windows with Visual Studio 2022 and CMake.
[Building OpenTS](docs/BUILDING.md) lists the exact requirements, commands and outputs.

## Documentation

The [manual](manual/) documents runtime behavior, INI configuration and engine internals. Every
Yuri's Revenge feature added in OpenYR is described there, and `manual/changes/` lists each
change.

## Contributing

OpenYR follows the OpenTS rules for code, comments and documentation: see
[CONTRIBUTING.md](CONTRIBUTING.md), [AGENTS.md](AGENTS.md) and [Style](docs/STYLE.md).
Never commit game assets, original binaries or decompiled code.

## Credits and license

OpenYR is built on [OpenTS](https://github.com/OpenTS-Developers/OpenTS) and the
[TibSun](https://github.com/OpenTS-Developers/TibSun) reconstruction, which in turn rest on
Electronic Arts' GPL-released source for related Command & Conquer games.
[ACKNOWLEDGEMENTS.md](ACKNOWLEDGEMENTS.md) thanks the people and projects behind that work.

OpenYR is licensed under the GNU General Public License, version 3 or later. Material derived
from Electronic Arts source remains subject to the additional GPL Section 7 terms in
[LICENSE.md](LICENSE.md). [Third-party notices](THIRD_PARTY_NOTICES.md) list bundled
dependencies and their licenses.

*Command & Conquer*, *Red Alert* and *Yuri's Revenge* are trademarks of Electronic Arts.
