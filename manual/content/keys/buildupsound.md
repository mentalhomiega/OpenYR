---
key: BuildupSound
summary: "The sound a structure makes as its construction begins."
see_also: [Construction, DeploySound]
when_omitted:
  kind: value
  value: none
---

A structure plays this sound at its position when its construction animation begins, whichever house owns it. A type that sets none plays the sound named by [`Construction`](/keys/construction/).

```ini title="rulesmd.ini"
[MYSILO] ; example BuildingType
BuildupSound=MYSILO_Buildup
```
