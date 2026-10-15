---
title: Place no overlay for an overlay index the rules do not define
category: fix
release: 0.2.0
targets:
- type: format
  id: scenario-terrain
  effect: changed
credit:
- MentalHomiega
---

A scenario whose `[OverlayPack]` holds an index past the last `[OverlayTypes]` entry the loaded rules define used to read a type that does not exist. The cell now gets no overlay, and the rest of the pack loads as before.
