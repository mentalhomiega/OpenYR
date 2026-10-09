---
key: JumpjetCrash
summary: "How far below a bridge deck, in leptons, a jumpjet of this type may be and still stand on it."
see_also: [JumpjetClimb, Climb]
when_omitted:
  kind: value
  value: "5"
---

A jumpjet of this type that is within `JumpjetCrash` leptons below a bridge deck stands on the deck, and the deck becomes its ground. A jumpjet that is lower than that stays under the bridge, and the terrain under the bridge is its ground.

```ini title="rulesmd.ini"
[JUMPJET] ; Rocketeer
JumpjetCrash=25
```
