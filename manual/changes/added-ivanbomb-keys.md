---
title: Give each bomb weapon its own bomb settings with IvanBomb keys
category: feature
release: 0.2.0
targets:
- type: key
  id: IvanBomb.Delay
  effect: added
- type: key
  id: IvanBomb.Damage
  effect: added
- type: key
  id: IvanBomb.Warhead
  effect: added
- type: key
  id: IvanBomb.AttachSound
  effect: added
- type: key
  id: IvanBomb.TickingSound
  effect: added
- type: key
  id: IvanBomb.Image
  effect: added
- type: key
  id: IvanBomb.Detachable
  effect: added
- type: key
  id: IvanBomb.DetonateOnSell
  effect: added
- type: key
  id: IvanBomb.DestroysBridges
  effect: added
- type: key
  id: IvanBomb.CanDetonateTimeBomb
  effect: added
credit:
- MentalHomiega
---

A weapon with an `IvanBomb=yes` warhead can set the delay, damage, warhead, sounds and icon file of the bomb it plants with `IvanBomb.Delay=`, `IvanBomb.Damage=`, `IvanBomb.Warhead=`, `IvanBomb.AttachSound=`, `IvanBomb.TickingSound=` and `IvanBomb.Image=`. `IvanBomb.Detachable=`, `IvanBomb.DetonateOnSell=`, `IvanBomb.DestroysBridges=` and `IvanBomb.CanDetonateTimeBomb=` set how the bomb is defused, sold, used on bridge repair huts and set off by hand. A weapon that writes none of them plants the bomb the rules describe.
