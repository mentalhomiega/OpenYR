---
title: Turn V-Sync on by default
category: feature
release: 0.2.0
targets:
- type: key
  id: VSync
  effect: changed
credit:
- MentalHomiega
---

[`VSync`](/keys/vsync/) under `[Video]` in `RA2MD.INI` now defaults to `yes`, so each present waits for the display's refresh, which removes tearing while the map scrolls. The wait adds a little delay between input and the picture. A file that already sets `VSync` keeps its value; set `VSync=no` to turn it off.
