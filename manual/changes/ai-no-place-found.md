---
title: Skip the site clearing when a computer finds no place
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-base-building
  effect: changed
credit:
- MentalHomiega
---

When the placement search finds no cell, a computer house now abandons the structure at once. Before, it went on to clear and place the structure at cell `0,0` at the map corner, which only failed afterwards. The autotest also gained `buildtrace`, which logs each structure a computer house starts and places.
