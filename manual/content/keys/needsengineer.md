---
key: NeedsEngineer
summary: Marks the structure as one only an engineer captures, such as a neutral tech building.
see_also: [Capturable, "system:ai-team-execution"]
when_omitted:
  kind: value
  value: "no"
---

`NeedsEngineer=yes` marks a structure as a tech structure for the Attack lines of computer team Scripts. A team whose [Attack...](/mapping/missions/tmission-attack/) line names [quarry](/reference/enums/quarry/) `11` looks for structures with this flag.

The structure makes no [`ProduceCashAmount`](/keys/producecashamount/) payment until its owner has changed at least once. A neutral structure that an engineer captures starts paying on capture. A structure that the map places under a player's house does not pay until something changes its owner.

```ini title="rulesmd.ini"
[CAOILD]
NeedsEngineer=yes
```
