---
title: Draw house colored lasers thick, and fade every laser beam
category: fix
release: 0.2.0
targets:
- type: key
  id: IsHouseColor
  effect: changed
- type: key
  id: IsLaser
  effect: changed
- type: key
  id: LaserDuration
  effect: changed
- type: key
  id: IsBigLaser
  effect: changed
credit: [MentalHomiega]
---

As in Yuri's Revenge, the beam of an `IsHouseColor=yes` laser is now a thick beam of parallel lines in the house color: 2 pairs from a unit, 3 from a prism tower, 5 from a supported one. Every laser beam now fades from full strength to nothing over `LaserDuration`, and a steady beam's glow adds its color to the pixels beneath it. A beam takes its settings from the weapon that fired, no laser makes a screen glow, and `IsBigLaser` has no effect. Only the thick beam looks at the frame rate.
