---
key: ClickRepairable
summary: "Lets the player repair this structure with the repair cursor."
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: "yes"
---

With `ClickRepairable=no`, the structure cannot be [repaired](/systems/repair/) with the repair cursor, whoever owns it.

```ini title="rulesmd.ini"
[MYCIVBUILDING] ; example BuildingType
ClickRepairable=no
```
