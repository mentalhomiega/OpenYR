---
title: Read a map's preview from wherever its sections stand in the file
category: fix
release: 0.2.0
targets:
- type: format
  id: scenario-terrain
  effect: changed
credit:
- MentalHomiega
---

The multiplayer map list read a map's `[Preview]` and `[PreviewPack]` only from the part of the file before `[Map]`, so a map saved with those sections after `[Map]` showed an empty picture. It now reads them from anywhere in the file.
