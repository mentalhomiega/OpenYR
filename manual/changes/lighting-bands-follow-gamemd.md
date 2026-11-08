---
title: Light objects through as many brightness bands as Yuri's Revenge
category: fix
release: 0.2.0
targets:
- type: system
  id: particle-systems
  effect: changed
- type: system
  id: building-animations
  effect: changed
credit: [MentalHomiega]
---

Lit drawing now uses 53 brightness bands, as Yuri's Revenge does, instead of 63. Animations, smoke and other particles drawn at normal light are about 3 percent brighter than before, now exactly as bright as their palette. Terrain, buildings and units under map lighting can each come out one shade lighter or darker, as Yuri's Revenge rounds their light.
