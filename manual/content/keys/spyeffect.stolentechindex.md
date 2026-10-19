---
key: SpyEffect.StolenTechIndex
summary: The kinds of technology infiltrating this structure steals, as a list of numbers from 0 to 31.
see_also: ["SpyEffect.Custom", "SpyEffect.ResetRadar", "SpyEffect.PowerOutageDuration", "SpyEffect.StolenMoneyPercentage", Agent, "system:capture"]
when_omitted:
  kind: value
  value: "none"
---

With [`SpyEffect.Custom=yes`](/keys/spyeffect.custom/), the infiltrating house records each listed kind as stolen. An object that names the kind in [`Prerequisite.StolenTechs`](/keys/prerequisite.stolentechs/) can then be built, once its other prerequisites are met. A number outside 0 to 31 is ignored. The kinds are not tied to a side: the stock rules' three side flags that [`RequiresStolenAlliedTech`](/keys/requiresstolenalliedtech/) and its siblings read are set only by a structure without `SpyEffect.Custom`.

```ini title="rulesmd.ini"
[GATECH] ; example BuildingType
SpyEffect.Custom=yes
SpyEffect.StolenTechIndex=0,3
```
