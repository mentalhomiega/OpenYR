---
key: ReselectIfLimboed
summary: "Selects this object again when it comes back out of its victim."
see_also: [LimboLaunch, "system:parasites"]
when_omitted:
  kind: value
  value: "no"
---

A [parasite](/systems/parasites/#coming-out) of this type that the player had selected when it leapt is selected again when it comes back out of its victim.

```ini title="rulesmd.ini"
[MYDRONE] ; example VehicleType
ReselectIfLimboed=yes
```
