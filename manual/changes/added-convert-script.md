---
title: Convert infantry with the Convert type script line
category: feature
release: 0.2.0
targets:
- type: key
  id: Convert.Script
  effect: added
  scope: infantrytype
credit:
- MentalHomiega
---

An infantry type now reads `Convert.Script=` as Ares does: when its team runs the Convert type script line, the soldier becomes the named infantry type where it stands, with the same share of its health and its rank, mission and team. Infantry types that leave the key out behave as before.
