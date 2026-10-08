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

We no longer rate an ion cannon target by its strength. Before, we kept a candidate above `IonCannonDamage` at its starting rating of 1, or 3 for a structure, whatever its kind, so a healthy construction yard was rated the same as any other structure. Now we give every candidate its kind's figure, as Yuri's Revenge does, and we rate an object outside the playable area 0. The nuke and the lightning storm use this rating.
