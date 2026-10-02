---
key: PsychicDetectionRadius
scope: buildingtype
label: 'Shows enemy targets'
when_omitted:
  kind: value
  value: "0"
---

The structure shows its owner where enemies plan to attack. While it is powered, the owner sees a line from each enemy vehicle, infantry unit and aircraft to its current target whenever that target lies within this many cells of the structure. With `0`, the structure shows nothing.

```ini title="rulesmd.ini"
[NAPSIS] ; Psychic Sensor
PsychicDetectionRadius=15
```

Distance is measured in a straight line from the structure's center to the target's center. Units of allied houses get no lines.
