---
title: Launch the silo's missile on the frame the door starts to open
category: fix
release: 0.2.0
targets:
- type: key
  id: AnimAux1
  effect: changed
- type: key
  id: AnimActive
  effect: changed
- type: key
  id: AnimAux2
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit: [MentalHomiega]
---

A missile silo now launches its missile on the frame its door starts to open, as Yuri's Revenge does. Before, the silo waited for the door to finish opening, held it for 14 frames, and then launched. The silo no longer plays `AnimAux1` for a launch, and its missile takes off with the -100 Z adjustment that Yuri's Revenge gives [`NukeTakeOff`](/keys/nuketakeoff/).
