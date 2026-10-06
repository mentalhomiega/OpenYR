---
key: AttachEffect.DiscardOnEntry
scope: warheadtype
label: Remove on entry
when_omitted:
  kind: value
  value: "no"
  note: "The effect stays on an object that leaves the map, with its duration paused until the object returns."
---

`AttachEffect.DiscardOnEntry=yes` removes this warhead's effect from an object that leaves the map, for example by entering a transport or garrisoning a structure.
