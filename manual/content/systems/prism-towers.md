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

While fewer than [`PrismSupportMax`](/keys/prismsupportmax/) towers support it, a tower that could fire instead recruits one more: the nearest tower of its owner's that is the same type, within the range of the firing tower's `Secondary` weapon, ready to fire, not charging, not attacking anything itself, not stunned and [operational](/systems/power/#defenses). It recruits one tower per attack update. A recruited tower charges for its art's [`DelayedFireDelay`](/keys/delayedfiredelay/) frames and then draws a beam to the firing tower in its owner's color, shown for [`PrismSupportDuration`](/keys/prismsupportduration/) frames. It then rests for [`PrismSupportDelay`](/keys/prismsupportdelay/) frames before it can fire or support again.

## The shot

When no more towers can be recruited, the firing tower charges for its own `DelayedFireDelay` frames and then fires its `Primary` weapon at its target, if it still can. Its shot deals [`PrismSupportModifier`](/keys/prismsupportmodifier/) percent of the weapon's damage more for each tower that supported it. With `PrismSupportModifier=150%`, two supporting towers make the shot deal four times its normal damage.
