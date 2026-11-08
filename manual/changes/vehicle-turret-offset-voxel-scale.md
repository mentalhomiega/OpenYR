---
title: Place a vehicle's turret by TurretOffset at gamemd's scale
category: fix
release: 0.2.0
targets:
- type: key
  id: TurretOffset
  effect: changed
credit:
- MentalHomiega
---

A voxel vehicle's turret is now drawn [`TurretOffset`](/keys/turretoffset/) leptons along its hull at gamemd's scale of about 0.166 voxel units per lepton. Before, the draw used a scale of 0.125 rounded down to a whole number, which drew the turret 2 to 5 voxel units too near the hull centre. The Half-track, Magnetron, War Miner and Slave Miner turrets move back to where gamemd draws them, which is also where their shots start. A turretless vehicle's barrel mounting uses the same scale.
