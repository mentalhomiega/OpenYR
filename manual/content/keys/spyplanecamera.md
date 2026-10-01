---
key: SpyPlaneCamera
summary: "The camera sound a spy plane plays as it photographs on its way to the target."
see_also: [SpyPlaneCameraFrames, "system:superweapons"]
when_omitted:
  kind: value
  value: none
---

A spy plane on its way to its target plays this sound at its position each time it photographs the ground. It photographs every [`SpyPlaneCameraFrames`](/keys/spyplanecameraframes/) frames once within its weapon's range of the target. After turning away from the target it photographs without the sound. [Spy plane](/systems/superweapons/#spy-plane) covers the flight.

```ini title="rulesmd.ini"
[AudioVisual]
SpyPlaneCamera=MySpyCamera ; a sound ID registered in SOUNDMD.INI
```
