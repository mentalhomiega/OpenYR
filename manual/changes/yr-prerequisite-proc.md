---
title: Require a refinery for types that name PROC
category: fix
release: 0.2.0
targets:
- type: key
  id: PrerequisiteProc
  effect: added
- type: key
  id: PrerequisiteProcAlternate
  effect: added
- type: key
  id: Prerequisite
  effect: changed
credit: [MentalHomiega]
---

`PROC` in a `Prerequisite=` list is now met by owning a structure on `PrerequisiteProc=` or a unit on `PrerequisiteProcAlternate=` (the Slave Miner in the standard rules). Before, the entry was dropped, so a player could build a War Factory with no refinery and a computer-controlled Yuri queued its Psychic Sensor and defenses ahead of its first refinery.
