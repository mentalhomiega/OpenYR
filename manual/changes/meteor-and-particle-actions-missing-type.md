---
title: Meteor, particle and special weapon actions skip a missing type
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_METEOR_SHOWER
  effect: changed
credit: [MentalHomiega]
---

Meteor Shower no longer crashes in rules without `METLARGE` and `METSMALL` animations, and Meteor Impact, Particle Anim and the two super weapon grants skip a number the rules do not have.
