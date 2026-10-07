---
key: NeedsEngineer
summary: Marks the structure as one only an engineer captures, such as a neutral tech building.
see_also: [Capturable, "system:ai-team-execution"]
when_omitted:
  kind: value
  value: "no"
---

`NeedsEngineer=yes` marks a structure as a tech structure for the Attack lines of computer team Scripts. A team whose [Attack...](/scripting/missions/0/) line names [quarry](/reference/enums/quarry/) `11` looks for structures with this flag, and the engine does not read the key for any other purpose.

```ini title="rulesmd.ini"
[CAOILD]
NeedsEngineer=yes
```
