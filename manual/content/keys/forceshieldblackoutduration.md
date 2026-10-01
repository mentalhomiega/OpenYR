---
key: ForceShieldBlackoutDuration
summary: "How many frames the force shield cuts its house's power."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "800"
---

After raising the force shield, the firing house's structures make no power for this many frames. Its structures then behave as for any shortage of power.

```ini title="rulesmd.ini"
[General]
ForceShieldBlackoutDuration=1000
```

The outage replaces any spy blackout already running.
