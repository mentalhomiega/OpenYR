---
key: BodyFrameFromIdle
summary: Whether a structure whose body animation reaches the shadow frames draws its idle body frame instead.
see_also: [ConditionYellow, AnimIdle, "system:building-animations"]
when_omitted:
  kind: value
  value: "yes"
---

```ini title="rulesmd.ini"
[AudioVisual]
BodyFrameFromIdle=yes
```

A structure's body is drawn from the first half of its image, and the second half holds its shadow frames. A structure drawn from its animation stage can reach a stage at or above that half. At `yes`, the body then draws the idle body frame: the first frame of the idle sequence, plus one when the structure's health is at or below [`ConditionYellow`](/keys/conditionyellow/). The idle sequence's current frame is not used, only its first frame. If that frame is itself in the shadow half, the body draws the first shadow frame.

At `no`, the body draws the first shadow frame. That is the frame Yuri's Revenge's draw code selects, so `no` gives its clamp.

The key changes only the body of a structure drawn from its animation stage. Garrisoned structures, gates, laser fences, firestorm walls and construction frames are drawn the same at both settings. Overlay animations are unaffected.

The default is `yes`. The Allied Construction Yard is the case that shows the difference: its active sequence runs through stages past the body half, so at `no` its hangar disappears for as long as the sequence runs, and at `yes` it stays drawn. The reason the shipped game does not show this is not known, so the default follows the look of the shipped game rather than a known mechanism.

The key is read for drawing only. It is neither saved nor part of the checksum.
