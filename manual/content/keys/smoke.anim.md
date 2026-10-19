---
key: Smoke.Anim
summary: The animation a badly damaged or crashing aircraft of this type leaves behind it.
see_also: ["Smoke.ChanceRed", "Smoke.ChanceDead"]
when_omitted:
  kind: value
  value: "SGRYSMK1"
---

`Smoke.Anim` names the smoke animation an aircraft creates at its position while it is in the air and its health is below [`ConditionRed`](/keys/conditionred/) or it is crashing. [`Smoke.ChanceRed`](/keys/smoke.chancered/) and [`Smoke.ChanceDead`](/keys/smoke.chancedead/) set how often.

```ini title="rulesmd.ini"
[ORCA] ; example AircraftType
Smoke.Anim=SGRYSMK1
Smoke.ChanceRed=20
```
