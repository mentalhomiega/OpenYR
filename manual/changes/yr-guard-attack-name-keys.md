---
title: Read friendly-fire, guard, name, death sound and radar color keys
category: fix
release: 0.2.0
targets:
- type: key
  id: AttackFriendlies
  effect: added
- type: key
  id: AttackCursorOnFriendlies
  effect: added
- type: key
  id: DefaultToGuardArea
  effect: added
- type: key
  id: UIName
  effect: added
- type: key
  id: DieSound
  effect: added
- type: key
  id: RadarColor
  effect: added
credit: [MentalHomiega]
---

Yuri's Revenge keys that the engine ignored now work in rulesmd.ini. `AttackFriendlies=yes` lets a type pick allied targets and show the attack cursor over allies, `AttackCursorOnFriendlies=yes` gives only the cursor, and `DefaultToGuardArea=yes` sends an idle soldier or armed vehicle to Area Guard.

Cameos and superweapon countdowns now show the string table text that `UIName=` names, where they showed `Name=`. `DieSound=` plays one of its sounds where an object is destroyed. `RadarColor=` on a TerrainType sets the color its objects show on the radar.
