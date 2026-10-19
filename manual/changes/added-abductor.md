---
title: Take targets aboard with Abductor weapons
category: feature
release: 0.2.0
targets:
- type: key
  id: Abductor
  effect: added
- type: key
  id: Abductor.AbductBelowPercent
  effect: added
- type: key
  id: Abductor.Anim
  effect: added
- type: key
  id: Abductor.ChangeOwner
  effect: added
- type: key
  id: ImmuneToAbduction
  effect: changed
credit:
- MentalHomiega
---

A weapon with `Abductor=yes` pulls the infantry or vehicle it hits into the passenger hold of its vehicle or aircraft instead of damaging it, when the target fits the hold and is not `ImmuneToAbduction=yes`. `Abductor.AbductBelowPercent=` limits it to weakened targets, `Abductor.Anim=` plays an animation where the target stood, and `Abductor.ChangeOwner=yes` gives the target to the firer's house.
