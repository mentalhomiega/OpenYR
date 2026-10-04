---
title: Choose rank insignia per object type
category: feature
release: 0.2.0
targets:
- type: key
  id: Insignia
  effect: added
- type: key
  id: InsigniaFrame
  effect: added
- type: key
  id: InsigniaFrames
  effect: added
- type: key
  id: Insignia.ShowEnemy
  effect: added
- type: key
  id: EnemyInsignia
  effect: added
credit:
- MentalHomiega
---

An object type's rulesmd.ini section can name its own insignia shape file with `Insignia` and the frame for each rank with `InsigniaFrame` or `InsigniaFrames`; the `.Rookie`, `.Veteran` and `.Elite` forms of each set one rank. A rookie shows an insignia when a frame is set for it. `Insignia.ShowEnemy`, or `EnemyInsignia` in `[General]`, hides the insignia from players not allied with the owner. The keys follow the Ares and Phobos documentation.
