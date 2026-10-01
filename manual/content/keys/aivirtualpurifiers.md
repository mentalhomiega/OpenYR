---
key: AIVirtualPurifiers
summary: How many ore purifiers a skirmish computer house counts without building them, per difficulty slot.
see_also: [OrePurifier, PurifierBonus, "system:tiberium"]
when_omitted:
  kind: value
  value: none
---

A list of whole numbers, one for each [difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot), counted from `0`. A computer house outside a campaign adds the number for its slot to the [`OrePurifier=yes`](/keys/orepurifier/) structures it owns when it is paid for delivered ore. A computer opponent on Hard normally holds slot 0.

```ini title="rulesmd.ini"
[General]
AIVirtualPurifiers=2,1,0 ; a Hard opponent earns as if it had two purifiers
```

Human players and campaign houses get no virtual purifiers. A slot past the end of the list counts as `0`.
