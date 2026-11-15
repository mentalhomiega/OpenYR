---
title: Let a mod name its own base files, archives and included INI files
category: feature
release: 0.2.0
targets:
- type: format
  id: mod-ini
  effect: changed
- type: format
  id: opents-ini
  effect: changed
- type: format
  id: ini-syntax
  effect: changed
credit:
- MentalHomiega
---

A `mod.ini` can now name the rules, art, AI, sound, music, announcer and campaign files the game reads, and the prefixes of the expansion and cache archives it mounts, so a total conversion can bring its own `RULESCV.INI` and `EXPANDCV99.MIX`. While a mod is active its archives are searched ahead of the language archives, so the mod can supply `RA2MD.CSF`.

An INI file can list further files to read after it in an `[#include]` section, as Ares allows. Files without such a section read as before.
