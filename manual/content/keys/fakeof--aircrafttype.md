---
key: FakeOf
scope: aircrafttype
summary: "The type whose price a decoy of this type shows to the selection commands."
see_also: [Trainable]
when_omitted:
  kind: value
  value: "none"
---

The key names the type that this one imitates. The health and veterancy selection commands narrow a selection by condition and rank, never by price, so a decoy cannot be told from its original by them and the key changes nothing yet.

```ini title="rulesmd.ini"
[D_KNIGHT] ; example InfantryType
FakeOf=KNIGHT
```
