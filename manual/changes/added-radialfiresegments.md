---
title: Read RadialFireSegments
category: feature
release: 0.2.0
targets:
- type: key
  id: RadialFireSegments
  effect: added
credit:
- MentalHomiega
---

`RadialFireSegments` is now read on object types. A value above zero fires each shot in the next direction of a 180-degree fan centered on the body facing. Stock rules set it to 10 on `AEGIS`, whose missiles now leave in a fan and curve onto the target. A vertical projectile fired by an object without the key, such as the Kirov's bomb, now leaves at speed 1 and accelerates instead of leaving at full speed.
