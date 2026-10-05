---
key: AttachEffect.ArmorMultiplier
scope: warheadtype
label: Armor multiplier
when_omitted:
  kind: value
  value: "1.0"
  note: "Damage taken is unchanged."
---

`AttachEffect.ArmorMultiplier` divides the damage taken by each object carrying this warhead's effect: `2` halves it and `0.5` doubles it. The division happens before the attacking warhead's `Verses`, together with the owner's country armor bonus, crate armor and veteran armor, and the result is rounded down to no less than `1`. Healing is not changed, and neither is damage that skips armor, such as an EM pulse destroying an object past its [`EMP.Threshold`](/keys/emp.threshold/). The hit that attaches the effect is not changed by it.

```ini title="rulesmd.ini"
[Hardener] ; example Warhead
AttachEffect.Duration=450
AttachEffect.ArmorMultiplier=2 ; a 100-point hit does 50
```
