---
key: AttackFriendlies
summary: Lets an object of this type choose allied objects as targets and offers the attack cursor over them.
see_also: [AttackCursorOnFriendlies, "system:target-selection"]
when_omitted:
  kind: value
  value: "no"
---

An object of an `AttackFriendlies=yes` type no longer rejects a candidate for being an ally when it [looks for a target](/systems/target-selection/#why-a-candidate-is-rejected). Allied objects, including those of its own house, are scored like enemies and can be picked. The setting also skips the special treatment healers and engineers give damaged allies, so a healer with it can pick an ally of any health.

When the player selects such an object, the attack cursor shows over an allied object, and a click orders the attack. The other conditions for the cursor still apply, such as an armed object and a legal target. [`AttackCursorOnFriendlies`](/keys/attackcursoronfriendlies/) gives the cursor alone.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
AttackFriendlies=yes
```
