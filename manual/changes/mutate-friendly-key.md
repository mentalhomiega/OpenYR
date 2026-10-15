---
title: Add MutateFriendly to choose whether the genetic mutator spares friendly infantry
category: feature
release: 0.2.0
targets:
- type: key
  id: MutateFriendly
  effect: added
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

[`MutateFriendly=`](/keys/mutatefriendly/) sets whether the genetic mutator mutates infantry of its firing house and that house's allies. It defaults to `yes`, so the mutator behaves as before. `MutateFriendly=no` spares those infantry, and only enemy infantry turns into brutes.
