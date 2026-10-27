---
title: Use the generic art for theater overlays the theater lacks
category: fix
release: 0.2.0
targets:
- type: key
  id: NewTheater
  effect: changed
- type: key
  id: Theater
  effect: changed
credit: [MentalHomiega]
---

An overlay type with `NewTheater=yes` or `Theater=yes` whose art has no file for the map's theater now loads the generic art, with `G` as the second letter of the file name, as Yuri's Revenge does. Before, such an overlay had no image, so the map loader dropped every one of its cells. The Yuri citadel wall `GAFWLL` was lost on New Urban, Urban, Desert and Lunar maps: Allied 2 to 6 and Soviet 3, 4 and 6. Snow maps, whose theater has its own file, were not affected. Each of those maps now loads all of its walls.
