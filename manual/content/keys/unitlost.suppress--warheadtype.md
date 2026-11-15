---
key: UnitLost.Suppress
scope: warheadtype
summary: "Stops the unit-lost announcement for a unit this warhead kills."
see_also: ["DieSound.Override", "VoiceDie.Override"]
when_omitted:
  kind: value
  value: "no, so a killed unit is announced"
---

With `yes`, a soldier, vehicle or aircraft that this warhead destroys does not make EVA announce a lost unit to its owner. A unit the warhead only damages is not affected. The warhead's other effects, including the unit's death animation and sounds, are unchanged.

```ini title="rulesmd.ini"
[MyRemoverWH] ; example Warhead
UnitLost.Suppress=yes
```
