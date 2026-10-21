---
title: Prism tower support
summary: "How prism towers charge each other before a shot, and how much damage the support adds."
category: combat-targeting
keys:
  - PrismType
  - PrismSupportModifier
  - PrismSupportMax
  - PrismSupportDelay
  - PrismSupportDuration
  - DelayedFireDelay
  - IsAnimDelayedFire
related:
  - type: system
    id: target-selection
---

Structures of the [`PrismType`](/keys/prismtype/) type charge each other before they fire. When one is ready to fire at its target, it first recruits its owner's other towers of that type to beam support to it, and each one that does adds to the damage of its shot.

```ini title="rulesmd.ini"
[General]
PrismType=ATESLA
PrismSupportModifier=150%   ; each supporting tower adds 150% of the shot's damage
PrismSupportMax=8
PrismSupportDelay=45        ; frames a supporting tower rests afterwards
PrismSupportDuration=15     ; frames the support beam shows
```

## Recruiting support

While fewer than [`PrismSupportMax`](/keys/prismsupportmax/) towers support it, a tower that could fire instead recruits one more: the nearest tower of its owner's that is the same type, within the range of the firing tower's `Secondary` weapon, ready to fire, not charging, not attacking anything itself and not [drained](/keys/drainweapon/). Distance is the straight line between the two towers. A tower on low power or stunned can still be recruited. It recruits one tower per attack update. A recruited tower charges for its art's [`DelayedFireDelay`](/keys/delayedfiredelay/) frames and then draws a beam to the firing tower in its owner's house laser color, shown for [`PrismSupportDuration`](/keys/prismsupportduration/) frames and fading from full intensity to none over them. It then rests for [`PrismSupportDelay`](/keys/prismsupportdelay/) frames before it can fire or support again.

## The shot

When no more towers can be recruited, the firing tower charges for its own `DelayedFireDelay` frames and then fires its `Primary` weapon at its target, if it still can. A tower that is charging cannot start another shot or be recruited until the charge ends. While a tower charges, whether to fire or to support, its `ActiveAnim` gives way to its `SpecialAnim`, and the `ActiveAnim` returns when the `SpecialAnim` has played through. Its shot deals [`PrismSupportModifier`](/keys/prismsupportmodifier/) percent of the weapon's damage more for each tower that supported it, rounded down. The count of supporting towers is cleared when a shot spends it or when the tower is left without a target. A tower that cannot fire when its charge ends keeps the count for its next shot. With `PrismSupportModifier=150%`, two supporting towers make the shot deal four times its normal damage.

## Other structures that charge

A structure that is not of the `PrismType` type but sets [`IsAnimDelayedFire=yes`](/keys/isanimdelayedfire/), such as the Tesla coil, charges the same way before each shot, with its `ActiveAnim` and `SpecialAnim` swapping as above. It recruits no support, and it fires whichever of its weapons it chose when the charge began.
