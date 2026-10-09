---
title: Spare iron-curtained and falling objects from a crush, and rock after a vehicle
category: fix
release: 0.2.0
targets:
- type: key
  id: Crushable
  effect: changed
credit: [MentalHomiega]
---

A crusher no longer flattens an ordinary `Crushable=yes` object while an iron curtain protects it, and it never flattens an object that is falling, such as a parachuting infantryman, even when the crusher is an `OmniCrusher`. After a crusher flattens a vehicle, it rocks once. The kill itself is unchanged: the crusher is credited with it and the victim goes without a death animation.
