---
title: Read CourseLockDuration
category: feature
release: 0.2.0
targets:
- type: key
  id: CourseLockDuration
  effect: added
credit:
- MentalHomiega
---

`CourseLockDuration` is now read on projectiles. A homing projectile with a value above `0` flies straight for that many frames, then steers. Stock rules set it to 15 on `MedusaProjectile`, so the Aegis cruiser's missiles fly 15 frames on their launch heading before they curve onto the target.
