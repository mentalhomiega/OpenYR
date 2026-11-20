---
title: Add CombatSelectionFirst so a selection box takes combat units first
category: feature
release: 0.2.0
targets:
- type: key
  id: CombatSelectionFirst
  effect: added
credit:
- MentalHomiega
---

[`CombatSelectionFirst`](/keys/combatselectionfirst/) makes a selection box or Select View that holds combat units leave out ore miners, engineers and other unarmed units. It is on by default. The Select Combatants command now picks the units whose type is `Trainable=yes`, as Yuri's Revenge does, in place of the armed ones.
