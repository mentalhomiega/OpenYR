---
title: Read FireWhileMoving
category: feature
release: 0.2.0
targets:
- type: key
  id: FireWhileMoving
  effect: added
credit:
- MentalHomiega
---

`FireWhileMoving=no` is now read on weapons. A unit with a destination cannot fire that weapon, except a BalloonHover unit. The default is `yes`. Stock rules set it to `no` only on the `DiskDrain` weapon, so the `DISK` unit changes behavior.
