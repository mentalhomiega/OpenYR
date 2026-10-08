---
key: Paralyzes
summary: "How many frames a parasite's bite holds its victim still."
see_also: [Parasite, "system:parasites"]
when_omitted:
  kind: value
  value: "0"
---

A parasite whose warhead has `Paralyzes` set [holds its victim](/systems/parasites/#holding-the-victim) for this many frames after each bite. While held, a vehicle or ship that drives or sails cannot move, and the victim launches no spawned aircraft or missiles. With `0`, a bite does not paralyze and ends any paralysis the victim has.

```ini title="rulesmd.ini"
[ParasitePlus] ; giant squid's warhead
Parasite=yes
Paralyzes=32767
```
