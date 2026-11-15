---
key: Missile.Type
scope: aircrafttype
summary: Read with the other Missile. keys; nothing uses it.
no_effect: true
when_omitted:
  kind: value
  value: none
---

We read this key into the aircraft type's own missile settings with `Missile.PauseFrames` and the other `Missile.` keys, and nothing uses it. Which aircraft type flies the missile is decided by the section the `Missile.Custom=yes` key is set in, never by this value.
