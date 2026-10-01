---
key: SuppressionThreshold
summary: "How much damage to its victim from others a parasite of this type survives."
see_also: [Parasite, "system:parasites"]
when_omitted:
  kind: value
  value: "0"
---

A hit on this [parasite's](/systems/parasites/#coming-out) victim from anyone else that does more than this much damage dooms the parasite for twice the damage less this value, in frames.

```ini title="rulesmd.ini"
[MYDRONE] ; example VehicleType
SuppressionThreshold=5
```
