---
title: Read five Yuri's Revenge keys that had no effect
category: fix
release: 0.2.0
targets:
- type: key
  id: MaximumCheerRate
  effect: added
- type: key
  id: AllyChangeAllowed
  effect: added
- type: key
  id: ForceShieldColor
  effect: added
- type: key
  id: CaptureEvaEvent
  effect: added
- type: key
  id: ConcentricRadialIndicator
  effect: added
- type: key
  id: HasRadialIndicator
  effect: changed
- type: key
  id: RadialColor
  effect: changed
credit:
- MentalHomiega
---

The cheer command now waits `MaximumCheerRate` frames between uses, and `AllyChangeAllowed=no` stops alliance changes during a match. A structure under a Force Shield takes the `ForceShieldColor` tint. Taking a tech structure plays its `CaptureEvaEvent` line to the capturing player and `EVA_TechBuildingLost` to the player who lost it.

Radius rings now follow Yuri's Revenge. Gap generators, psychic sensors and armed structures with `HasRadialIndicator=yes` draw one, in the owner's color, and `ConcentricRadialIndicator=yes` makes it pulse. Every range ring, including the placement and superweapon rings, is about a quarter larger than before and two pixels thick.
