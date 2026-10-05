---
key: AttackCursorOnFriendlies
summary: Shows the attack cursor over allied objects so the player can order an attack on them.
see_also: [AttackFriendlies, "system:target-selection"]
when_omitted:
  kind: value
  value: "no"
---

When the player selects an object of an `AttackCursorOnFriendlies=yes` type, the attack cursor shows over an allied object, and a click orders the attack. The other conditions for the cursor still apply, such as an armed object and a legal target. Without the key, the player needs the force-fire key, left Ctrl by default, to attack an ally.

The key does not change which targets the object picks by itself. [`AttackFriendlies`](/keys/attackfriendlies/) does that and also gives the cursor.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
AttackCursorOnFriendlies=yes
```
