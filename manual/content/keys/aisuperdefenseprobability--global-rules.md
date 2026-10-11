---
key: AISuperDefenseProbability
scope: global-rules
label: Computer superweapon defense chance
summary: "The chance in percent, by difficulty slot, that a computer house raises its force shield against a weapon aimed near its base."
see_also: ["system:superweapons", AIDefendAgainst, AISuperDefenseDistance, AISuperDefenseFrames]
when_omitted:
  kind: value
  value: none
---

A comma-separated list with one percentage for each [difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot). A computer house rolls a number from 0 to 99 when a weapon with [`AIDefendAgainst=yes`](/keys/aidefendagainst/) lands within [`AISuperDefenseDistance`](/keys/aisuperdefensedistance/) of its base. It defends when the roll is no higher than the entry for its own slot, so `0` still defends one time in a hundred and `99` always does.

```ini title="rulesmd.ini"
[General]
AISuperDefenseProbability=90,50,10 ; the first slot defends nine times in ten
```

Without entries no computer house defends, since there is no entry for its slot.
