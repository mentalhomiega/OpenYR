---
title: Apply TurretOffset along the body before the turret turns
category: fix
release: 0.2.0
targets:
- type: key
  id: TurretOffset
  effect: changed
- type: system
  id: firing-geometry
  effect: changed
credit:
- MentalHomiega
---

A vehicle's shot now starts [`TurretOffset`](/keys/turretoffset/) along its hull before the turret turns, as gamemd's `TechnoClass::GetFLH` does. Before, the offset turned with the turret, so the shot left from a point that drifted away from the drawn turret when the turret faced away from the hull. A structure's shot also starts the offset along its unturned axis now, not along its turret's heading. Its drawn turret is unchanged, so the two now separate as the turret turns, as they do in gamemd.
