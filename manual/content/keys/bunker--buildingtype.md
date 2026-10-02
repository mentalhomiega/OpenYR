---
key: Bunker
scope: buildingtype
label: 'Holds a vehicle'
see_also: [Bunkerable, BunkerDamageMultiplier, BunkerROFMultiplier, BunkerWeaponRangeBonus, "system:tank-bunkers"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the structure holds one of its owner's vehicles, which fights from inside it with the bonuses in [Tank bunkers](/systems/tank-bunkers/).

```ini title="rulesmd.ini"
[MYBUNKER] ; example BuildingType
Bunker=yes
```
