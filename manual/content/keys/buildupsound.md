---
key: BuildupSound
summary: "The sound a structure makes as its construction begins."
see_also: [DeploySound]
when_omitted:
  kind: value
  value: none
---

A structure plays this sound at its position when its construction animation begins, whichever house owns it.

```ini title="rulesmd.ini"
[MYSILO] ; example BuildingType
BuildupSound=MYSILO_Buildup
```
