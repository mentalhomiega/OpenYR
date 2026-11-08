---
title: Make an animation's start sound follow the animation
category: fix
release: 0.2.0
targets:
- type: key
  id: StartSound
  scope: animtype
  effect: changed
- type: key
  id: Report
  scope: animtype
  effect: changed
credit:
- MentalHomiega
---

An animation's [`StartSound`](/keys/startsound/#scope-animtype), or its `Report` sound when it has no `StartSound`, now moves with the animation. The sound is re-aimed at the animation's place on every game update. It starts again when it ends while the animation is still playing. An endless loop stops when the animation is removed, and a one-shot sound plays to its end. Before, the sound was placed once where the animation began and never moved.
