---
title: Match gamemd's flak and ballistic scatter
category: fix
release: 0.2.0
targets:
- type: key
  id: FlakScatter
  effect: added
- type: key
  id: Inaccurate
  effect: changed
credit:
- MentalHomiega
---

Flak shells now scatter as gamemd scatters them: a visible flak shell's aim moves by an amount that grows with its distance to the target, and an invisible flak shell's target moves by up to twice `BallisticScatter`, through the new `FlakScatter` setting. An arcing `Inaccurate` shell now draws its direction and distance in gamemd's order, so its direction changes. Saves from earlier builds are refused, because the save revision rose to 34. Multiplayer games with an earlier build diverge at the first inaccurate shot.
