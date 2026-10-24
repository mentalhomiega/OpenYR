---
title: Play the nuclear missile siren when a nuke launches
category: fix
release: 0.2.0
targets:
- type: key
  id: DigSound
  effect: changed
credit:
- MentalHomiega
- Killerbote
---

A nuclear missile launch now plays the `DigSound` sound at its target cell, as Yuri's Revenge does. The stock rules set `DigSound` to `NukeSiren`, so the air-raid siren plays when a nuke is launched from a silo or from the map edge. Before, the launch played only the announcer line.
