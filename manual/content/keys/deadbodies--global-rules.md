---
key: DeadBodies
scope: global-rules
label: Shared corpse animations
summary: The corpses an infantryman leaves when its own type names none.
see_also: [NotHuman, InfDeath, InfantryExplode, FlamingInfantry]
when_omitted:
  kind: value
  value: ""
---

An infantryman leaves a corpse when he finishes a death sequence that keeps him on the map, such as the gun and explosion deaths that a warhead's [`InfDeath`](/keys/infdeath/) starts. When the sequence reaches its last frame, one animation appears at his center and he is removed. The animation comes from his InfantryType's own [`DeadBodies`](/keys/deadbodies/#scope-infantrytype) when that list has entries. Otherwise it comes from this `[General]` list, unless the type sets [`NotHuman=yes`](/keys/nothuman/). Every entry is equally likely.

```ini title="rulesmd.ini"
[General]
DeadBodies=MYDEATH_A,MYDEATH_B ; AnimTypes registered in [Animations]
```

With this list empty and no list on the type, the infantryman leaves no corpse.

Many deaths remove the infantryman at once, without a death sequence, so they leave no corpse whatever the warhead's `InfDeath` is. These include:

- an infantryman who was already falling and comes down in water;
- a prone [`Cyborg=yes`](/keys/cyborg/) infantryman;
- a `Cyborg=yes` infantryman killed by forced damage, such as being caught in a raised firestorm wall;
- a [`JumpJet=yes`](/keys/jumpjet/) infantryman.
