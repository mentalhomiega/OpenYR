---
key: HarvesterDumpRate
summary: Minutes a docked harvester waits before each pass that hands over one ore type.
see_also: ["system:tiberium", "HarvesterLoadRate", "Storage"]
when_omitted:
  kind: value
  value: ".016"
---

A docked harvester waits this many minutes of game time, then hands its house everything it holds of one Tiberium type. A minute is 900 frames, and the wait is rounded up to whole frames: the default `.016` gives 14.4, so a pass every 15 frames. A load of one type is therefore paid out 15 frames after the harvester docks, and each further type in a mixed load takes another 15 frames. One more pass finds the harvester empty, and the refinery's production animation starts after it. Weeders unload at the same rate.

```ini title="rules.ini"
[General]
HarvesterDumpRate=.03   ; a pass every 27 frames
```

The same rate sets how long a returning harvester expects to wait at a busy refinery. It compares that wait, a pass for each type the loads there hold plus the final pass, with the extra drive to a free one when it [chooses where to unload](/systems/tiberium/#unloading).
