---
title: Throw debris from warhead detonations
category: feature
release: 0.2.0
targets:
- type: key
  id: MaxDebris
  effect: added
  scope: warheadtype
- type: key
  id: MinDebris
  effect: added
  scope: warheadtype
- type: key
  id: DebrisTypes
  effect: added
  scope: warheadtype
- type: key
  id: DebrisMaximums
  effect: added
  scope: warheadtype
credit:
- MentalHomiega
---

A warhead can set `MaxDebris=`, `MinDebris=`, `DebrisTypes=`, and `DebrisMaximums=` in its section of the rules. A shot from that warhead throws debris where it detonates, as a destroyed object throws its wreckage. Saves made by earlier builds no longer load, because warheads now save these settings.
