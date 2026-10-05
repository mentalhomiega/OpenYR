---
key: AttachEffect.DiscardOnEntry
scope: aircrafttype
label: Remove own effect on entry
when_omitted:
  kind: value
  value: "no"
  note: "The effect stays on an object that leaves the map, with its duration paused until the object returns."
---

`AttachEffect.DiscardOnEntry=yes` removes an object's [own effect](/systems/attach-effects/#an-object-types-own-effect) when the object leaves the map. The effect is attached again [`AttachEffect.Delay`](/keys/attacheffect.delay/) frames after it was removed, counted while the object is on the map.
