---
title: Play an animation's StartSound when it starts
category: fix
release: 0.2.0
targets:
- type: key
  id: StartSound
  scope: animtype
  effect: changed
- type: key
  id: Report
  scope: animtype
  effect: changed
credit: [MentalHomiega]
---

An animation with [`StartSound`](/keys/startsound/#scope-animtype) now plays that sound when it starts, as Yuri's Revenge does. Before, the key was read and never played, so building fires, superweapon open and ready loops, the nuke launch, the waterfalls and the other stock animations that name a `StartSound` were silent. Such an animation no longer plays its `Report` sound.
