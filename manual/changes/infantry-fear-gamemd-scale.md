---
title: Match gamemd's infantry fear scale
category: fix
release: 0.2.0
targets:
- type: key
  id: Fraidycat
  effect: changed
- type: key
  id: Fearless
  effect: changed
- type: key
  id: Doggie
  effect: changed
- type: key
  id: Sequence
  effect: changed
- type: mission
  id: TMISSION_PANIC
  effect: changed
credit: [MentalHomiega]
---

Infantry fear now runs from 0 to 300, as in gamemd. A soldier goes prone at 50, except a Fraidycat, which scatters instead, and a dog. A hit adds 50; if the soldier's health is then above ConditionRed the amount is 25, and above ConditionYellow it is 12. A soldier at 200 or more runs its Panic sequence whatever its type. A Fraidycat hit while its fear is below 100, or firing its last round, sets fear to 300.
