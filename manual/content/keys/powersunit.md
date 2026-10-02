---
key: PowersUnit
summary: "The unit type this structure keeps running while it works."
see_also: [PoweredUnit]
when_omitted:
  kind: value
  value: none
---

While this structure is powered, finished and not being sold, its owner's units of the named type keep running if that type sets [`PoweredUnit=yes`](/keys/poweredunit/). When the owner's last such structure stops working, is destroyed, sold or captured, those units shut down.

```ini title="rulesmd.ini"
[MYCONTROLCENTER] ; example BuildingType
PowersUnit=MYROBOT
Powered=yes
```
