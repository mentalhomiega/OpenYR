---
key: HoverAttack
summary: Makes a jumpjet take off when it has something to attack, so that it fights from the air.
see_also: ["Locomotor", "Fighter"]
when_omitted:
  kind: value
  value: "no"
---

`HoverAttack=yes` makes a unit that is standing on the ground take off when it is ordered to attack, or when it is on guard and has acquired a target. It heads for a cell beside itself, which for a jumpjet means it rises and attacks from the air instead of from the ground.

```ini title="rules.ini"
[JUMPJET]
HoverAttack=yes   ; takes off to attack
```
