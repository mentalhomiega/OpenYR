---
title: Play the drill sergeant and other Red Alert 2 mission voices
category: fix
release: 0.2.0
targets:
- type: format
  id: opents-ini
  effect: changed
credit:
- MentalHomiega
---

A Red Alert 2 mission's announcer line now plays when its sample is in Red Alert 2's `AUDIO.MIX`. Before, the game found the line but not its sample, because it mounts only `AUDIOMD.MIX` when that file exists, so Boot Camp still had no drill sergeant. The game now reads the archive `RA2Audio=` names under `[Files]` in `OPENTS.INI`, `AUDIO.MIX` by default, for a sample that no other archive or loose file holds. Without that archive the lines stay silent.
