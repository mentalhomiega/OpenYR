---
key: OrePurifier
summary: Makes this structure add PurifierBonus to the ore unloaded at any structure its owner owns.
see_also: [PurifierBonus, AIVirtualPurifiers, Refinery, "system:tiberium"]
when_omitted:
  kind: value
  value: "no"
---

Every `OrePurifier=yes` structure a house has on the map raises the payment for each unit of ore unloaded at a structure that house owns by [`PurifierBonus`](/keys/purifierbonus/), 25% by default. Two purifiers add the bonus twice. [Credits and storage](/systems/tiberium/#credits-and-storage) gives the payment in full.

```ini title="rulesmd.ini"
[MYPURIFIER] ; example BuildingType
OrePurifier=yes
```

A purifier counts from the moment it is placed, while it builds up, whether or not its owner has power. It stops counting when it is sold, or when it leaves the map after being destroyed. A destroyed purifier still on the map keeps counting while its destruction runs.
