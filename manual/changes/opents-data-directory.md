---
title: Name the data and user directories in OPENTS.INI
category: feature
release: 0.2.0
targets:
- type: format
  id: opents-ini
  effect: changed
credit:
- MentalHomiega
---

`OPENTS.INI` can name the game data directory and the user directory under `[Paths]`, with `DataDirectory=` and `UserDirectory=`. The game reads them from the file beside `Game.exe`, so a copy installed apart from its data finds that data without a command line option. A `-DATADIR=` or `-USERDIR=` option given on the command line takes its place.
