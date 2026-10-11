---
key: AISuperDefenseFrames
scope: global-rules
label: Computer force shield wait
summary: "How many frames a computer house keeps wanting its force shield after a weapon was aimed near its base."
see_also: ["system:superweapons", AIDefendAgainst, AISuperDefenseDistance, AISuperDefenseProbability]
when_omitted:
  kind: value
  value: "25"
---

A computer house fires its charged force shield over the cell it chose when it was threatened, for this many frames after the frame it chose it. After that the cell is ignored until another weapon or a [Set Defensive Target Cell](/mapping/actions/taction-set-defensive-target-cell/) trigger action sets one.

```ini title="rulesmd.ini"
[General]
AISuperDefenseFrames=50 ; the shield is wanted for 50 frames
```

The computer checks its weapons every few frames, so a window shorter than that interval can pass without a shot.
