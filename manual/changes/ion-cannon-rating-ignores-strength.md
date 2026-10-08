---
title: Rate ion cannon targets without the strength test
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
- type: key
  id: IonCannonDamage
  effect: changed
credit:
- MentalHomiega
---

The computer's ion cannon rating no longer depends on a target's strength. Before, a candidate above `IonCannonDamage` kept its starting rating of 1, or 3 for a structure, whatever its kind. A healthy construction yard was therefore rated the same as any other structure. Now every candidate takes its kind's figure, as Yuri's Revenge does, and an object outside the playable area is rated 0. The nuke and the lightning storm use this rating.
