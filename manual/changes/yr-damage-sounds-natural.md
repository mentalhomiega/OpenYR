---
title: Add damage sounds and Natural targeting
category: feature
release: 0.2.0
targets:
- type: key
  id: DamageSound
  effect: added
- type: key
  id: BuildingDamageSound
  effect: added
- type: key
  id: Natural
  effect: added
- type: key
  id: Unnatural
  effect: added
- type: key
  id: BlowupSound
  effect: changed
credit: [Lucas]
---

Objects now play `DamageSound` on light hits and structures `BuildingDamageSound` as they lose condition, in place of the Tiberian Sun blow-up sound, and `Natural=yes` objects such as dogs no longer fire at `Unnatural=yes` ones, as in Yuri's Revenge.
