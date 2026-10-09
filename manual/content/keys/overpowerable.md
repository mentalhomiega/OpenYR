---
key: Overpowerable
summary: "Lets soldiers with an ElectricAssault weapon charge this structure."
see_also: ["system:power"]
when_omitted:
  kind: value
  value: "no"
---

Soldiers with an [`ElectricAssault=yes`](/keys/electricassault/) second weapon can [charge](/systems/power/#overpowered-defenses) an `Overpowerable=yes` structure of their own or an allied house. Two chargers keep it running through a shortfall in power, and an overpowered structure fires its `Secondary` weapon.

```ini title="rulesmd.ini"
[MYCOIL] ; example BuildingType
Overpowerable=yes
Secondary=MyOverchargedBolt
```
