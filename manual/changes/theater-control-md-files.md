---
title: Read the Yuri's Revenge theater control files
category: fix
release: 0.2.0
targets:
- type: format
  id: theater-control
  effect: changed
- type: key
  id: Root
  scope: theater
  effect: changed
credit:
- MentalHomiega
---

A theater's tile sets now come from `<Root>MD.INI`, such as `URBANNMD.INI` for the new urban theater, and from `<Root>.INI` only when there is no such file. The game used to read `<Root>.INI` alone. The desert, new urban and lunar theaters have no such file, so their maps crashed while loading, the first Allied mission among them. Snow and urban maps were read without the tile sets Yuri's Revenge adds to those theaters.
