---
title: Terrain objects no longer catch fire
category: fix
release: 0.2.0
targets:
- type: key
  id: Sparky
  effect: changed
- type: key
  id: TreeFire
  effect: changed
- type: key
  id: TreeFlammability
  effect: changed
credit:
- MentalHomiega
---

Terrain objects never catch fire, as in gamemd. A `Sparky=yes` warhead no longer sets a tree alight, a burning tree no longer sets fire to its neighbors, and the `TreeFire` and `TreeFlammability` settings have no effect. A destroyed `SpawnsTiberium` terrain object no longer starts a Tiberium chain reaction of its own. A unit now plans a route through a tree that its weapon's warhead can destroy even when the tree is `Immune=yes`, as gamemd does. The unit still cannot damage the immune tree.
