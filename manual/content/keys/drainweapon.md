---
key: DrainWeapon
summary: "Makes a weapon drain the structure its firer hovers over."
see_also: [Drainable, DrainMoneyAmount, DrainAnimationType]
when_omitted:
  kind: value
  value: "no"
---

A weapon with `DrainWeapon=yes` drains a [`Drainable`](/keys/drainable/) object instead of firing a projectile. Ordered against one, the firer moves over it and starts draining once it is above one of the object's cells. It then stops on that cell and drops the attack order. Draining ends when the firer leaves the object's cells, when either side is destroyed or removed, or when the object's owner becomes the firer's ally.

While a structure is drained:

- if it produces power, its owner's structures produce none at all, so the whole base runs on low power;
- if it is a refinery, its owner pays the firer's owner [`DrainMoneyAmount`](/keys/drainmoneyamount/) credits every [`DrainMoneyFrameDelay`](/keys/drainmoneyframedelay/) frames, never more than the owner has;
- if it has a weapon, it cannot fire;
- it cannot be sold;
- the firer shows [`DrainAnimationType`](/keys/drainanimationtype/).

When a drain starts, the structure lets go of the units it holds through mind control, and a firer on foot leaves its team and gets no Guard mission.

An object drains one thing at a time, and an object being drained cannot be drained by a second one. A unit whose second weapon is a drain weapon uses it against an enemy `Drainable` object while it is draining nothing, and its first weapon otherwise. The weapon's `Damage` and `Warhead` are not used.

```ini title="rulesmd.ini"
[MyDrain] ; example Weapon
DrainWeapon=yes
Range=1.5
ROF=50
Projectile=InvisibleVertical
Warhead=MyDrainWH
```
