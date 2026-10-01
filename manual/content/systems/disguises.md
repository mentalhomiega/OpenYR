---
title: Disguises
summary: "How a spy takes on another soldier's look, and who sees through it."
category: combat-targeting
keys:
  - MakesDisguise
  - CanDisguise
  - Disguised
  - FireOnce
related:
  - type: system
    id: capture
---

An object of a [`CanDisguise=yes`](/keys/candisguise/) type that hits a soldier with a [`MakesDisguise=yes`](/keys/makesdisguise/) warhead takes on that soldier's look: its type's artwork and name, in its house's colors. A weapon with `Range=-2` reaches any target, so a spy can copy a soldier anywhere on the map.

```ini title="rulesmd.ini"
[MYSPY] ; example InfantryType
Primary=MyMakeupKit
CanDisguise=yes

[MyMakeupKit]
Range=-2       ; reaches any target
FireOnce=yes   ; one shot, then the spy drops the target
Warhead=MySnapshot

[MySnapshot]
MakesDisguise=yes
```

The disguise shows only to players whose house is not an ally of the disguised object's owner; its owner and allies see it as it is. It lasts until the object copies another soldier, or until the house it imitates leaves the game. The warhead does no damage, whether or not it disguises anything.

An infantry type with [`Disguised=yes`](/keys/disguised/) is a separate, older disguise: it always looks like the rules' `Disguise` type to other houses.

Yuri's Revenge also makes the Mirage tank look like a tree while it stands still, and lets dogs and other detectors see through disguises. Neither is done yet.
