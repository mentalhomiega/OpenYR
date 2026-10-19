---
key: SonarPulse.Delay
scope: superweapontype
summary: "Sets how long a sonar pulse keeps decloaked objects visible."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "60"
---

The number of frames before an object the pulse decloaked may cloak again.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SonarPulse.Delay=120
```

See [Sonar pulse](/systems/superweapons/#sonar-pulse).
