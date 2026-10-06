---
title: Choose mods on a Mods screen
category: feature
release: 0.2.0
targets:
- type: format
  id: opents-ini
  effect: changed
- type: system
  id: ui-files
  effect: changed
credit:
- MentalHomiega
---

The options menu now opens a [Mods screen](/using/mods/#the-mods-screen) that lists the folders in the `Mods` folder and the mods `Mods=` and `-MOD=` name. Players turn mods on and off and change their order there, and OK writes the list to `Mods=` in `OPENTS.INI` for the next start. Mods named only on the command line are shown but never written to the file.
