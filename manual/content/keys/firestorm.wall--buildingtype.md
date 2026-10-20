---
key: Firestorm.Wall
scope: buildingtype
summary: "Another name for FirestormWall."
see_also: [FirestormWall]
when_omitted:
  kind: value
  value: "the value of FirestormWall"
---

`Firestorm.Wall=yes` makes the structure a firestorm wall section, as [`FirestormWall=yes`](/keys/firestormwall/) does, and `Firestorm.Wall=no` turns the flag off. The structure type reads `FirestormWall` first and `Firestorm.Wall` second, so the second wins when both are present.

```ini title="rulesmd.ini"
[MYWALL] ; example BuildingType
Firestorm.Wall=yes
```
