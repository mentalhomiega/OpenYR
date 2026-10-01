---
key: SpyPlaneCameraFrames
summary: "How many frames apart a spy plane photographs on its way to the target."
see_also: [SpyPlaneCamera, "system:superweapons"]
when_omitted:
  kind: value
  value: "16"
---

A spy plane flying to its target checks this often whether it is within its weapon's range of the target, and photographs the ground with [`SpyPlaneCamera`](/keys/spyplanecamera/) each time it is. Within three cells of the target it turns for the far map edge and photographs every 3 frames instead. [Spy plane](/systems/superweapons/#spy-plane) covers the flight.

```ini title="rulesmd.ini"
[AudioVisual]
SpyPlaneCameraFrames=16
```
