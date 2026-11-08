---
title: Zoom the map to one height in scripted scenes
category: feature
release: 0.2.0
targets:
- type: system
  id: map-zoom
  effect: changed
- type: key
  id: ScriptedCameraZoom
  effect: added
- type: key
  id: ScriptedCameraHeight
  effect: added
credit: [MentalHomiega]
---

A scripted scene that takes the input away or moves the camera now zooms the map to a fixed height, 900 map pixels by default, whatever zoom the player picked. When control returns, the player's zoom comes back. `ScriptedCameraZoom=no` under `[Options]` in `RA2MD.INI` turns the scene zoom off, and the Cinematic zoom for scripted scenes switch on the Accessibility page sets the same key. `ScriptedCameraHeight` under `[General]` in `rules.ini` sets the height.

While the input is taken, the mouse wheel no longer zooms the map.
