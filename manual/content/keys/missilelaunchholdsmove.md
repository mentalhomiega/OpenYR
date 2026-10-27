---
key: MissileLaunchHoldsMove
summary: "Holds a spawner's move while the missile it launched is still on its pad or taking off."
when_omitted:
  kind: value
  value: "yes"
---

A spawner such as the V3 Launcher does not move while a missile it launched is still on its pad or taking off. The spawner moves again once the missile has left the pad. A missile therefore leaves along the body facing the spawner had when it fired, even when the spawner has been ordered to move since.

With `MissileLaunchHoldsMove=no`, the spawner moves and turns at once, and a missile launched afterwards leaves along its new facing.

```ini title="rulesmd.ini"
[General]
MissileLaunchHoldsMove=no
```

The default is `yes`, which matches the vanilla behavior the owner observed in play. The exact gamemd check that stops the drive is not fully traced; set the key to `no` to keep the earlier movement.
