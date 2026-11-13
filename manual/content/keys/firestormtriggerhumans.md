---
key: FirestormTriggerHumans
summary: Whether the Activate and Deactivate Firestorm Defense trigger actions work on a house a player controls.
when_omitted:
  kind: value
  value: "no"
---

`FirestormTriggerHumans` is an OpenTS key, read from `[General]`. Ares supports the [Activate Firestorm Defense](/mapping/actions/taction-activate-firestorm/) and [Deactivate Firestorm Defense](/mapping/actions/taction-deactivate-firestorm/) actions for houses a computer controls only, and says nothing about what they do to a player's house.

With `FirestormTriggerHumans=no` the two actions do nothing for a house a player controls. With `yes` they raise and lower that house's wall the way they do for a computer house, with no charge needed or spent.

```ini title="rulesmd.ini"
[General]
FirestormTriggerHumans=yes
```
