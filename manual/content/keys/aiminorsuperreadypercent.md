---
key: AIMinorSuperReadyPercent
summary: How far charged a computer team's Iron Curtain must be for the team to wait for it.
see_also: ["system:superweapons", "system:ai-team-execution"]
when_omitted:
  kind: value
  value: "0.8"
---

A computer team on the [Iron Curtain me](/scripting/missions/55/) script line waits for its house's Iron Curtain only while the weapon has charged at least this share of its [`RechargeTime`](/keys/rechargetime/). A weapon charged less than that is passed over, and the team takes its next line.

```ini title="rulesmd.ini"
[General]
AIMinorSuperReadyPercent=0.8 ; wait only when the last fifth of the charge remains
```

At `1` or above, a team never waits. At `0` or below, it waits however little the weapon has charged.
