---
key: Enslaves
scope: aircrafttype
label: 'Slave infantry type'
see_also: [SlavesNumber, SlaveRegenRate, SlaveReloadRate, "system:slave-miners"]
when_omitted:
  kind: value
  value: none
---

The InfantryType an object keeps as slaves. It has [`SlavesNumber`](/keys/slavesnumber/#scope-aircrafttype) of them from the moment it is placed. The slaves gather ore only while their owner is a structure. A vehicle's slaves start work once it deploys, and a structure placed without a vehicle uses its own `Enslaves`.

```ini title="rulesmd.ini"
[MYMINERBASE] ; example BuildingType
Enslaves=MYSLAVE
SlavesNumber=5
SlaveRegenRate=500
SlaveReloadRate=25
```

A vehicle hands its slaves to the structure it deploys into, and the structure hands them back when it packs up; see [Slave miners](/systems/slave-miners/).
