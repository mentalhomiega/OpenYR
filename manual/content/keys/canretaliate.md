---
key: CanRetaliate
summary: "With no, an object of this type never fires back when it is hit."
see_also: ["system:target-selection"]
when_omitted:
  kind: value
  value: "yes"
---

A `CanRetaliate=no` object does not [retaliate](/systems/target-selection/#retaliation) against an attacker, whoever owns it.

```ini title="rulesmd.ini"
[MYDEMOTRUCK] ; example VehicleType
CanRetaliate=no
```
