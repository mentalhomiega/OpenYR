---
title: Give a barrel shot one flight turn when it fires
category: fix
release: 0.2.0
targets:
- type: system
  id: projectile-flight
  effect: changed
- type: system
  id: firing-geometry
  effect: changed
- type: key
  id: IsLaser
  effect: changed
- type: key
  id: Trailer
  effect: changed
credit:
- MentalHomiega
---

A shot from a weapon with a barrel took two extra flight turns while it was fired, so it moved three times in the frame it was fired and could detonate there. Now it takes one turn in that frame, as every other shot does. A close-range shot that used to hit on the firing frame can now hit one frame later. gamemd's `TechnoClass::Fire` (0x6FDD50) creates and places the projectile and returns without running its update, so the extra turns did not match gamemd.
