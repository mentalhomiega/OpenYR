---
title: Skip insignificant objects in automatic target selection
category: fix
release: 0.2.0
targets:
- type: key
  id: Insignificant
  effect: changed
credit:
- MentalHomiega
---

An object whose type sets `Insignificant=yes` is no longer picked by automatic target selection. That covers units of every house, and buildings of a house whose country sets `MultiplayPassive=yes`, such as Neutral. Before this change, a computer unit in a campaign mission attacked the Neutral civilian buildings the map placed near it. Buildings that are not insignificant, such as a Neutral monument with a weapon, are still picked.
