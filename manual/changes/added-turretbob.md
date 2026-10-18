---
title: Add the turret bob key
category: feature
release: 0.2.0
targets:
- type: key
  id: TurretBob
  effect: added
credit:
- MentalHomiega
---

`TurretBob=` in a vehicle's section sets whether its turret and barrel shift up with the hull's frame. The default, `no`, matches Yuri's Revenge, where the turret stays on the hull's pixel. `TurretBob=yes` restores the bob that earlier builds drew for every voxel vehicle with a turret.

The key ships with two draw changes. A voxel vehicle now steps its hull through its frames once per walk step; earlier builds stepped once per two, so the hull animation runs twice as fast. An aircraft now draws its body from the frame its walk counter selects, so an aircraft whose body art has more than one frame animates when the counter steps. Saves load unchanged, because the key is read from the rules and is not saved.
