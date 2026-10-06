---
key: SpawnDelay
summary: The number of frames between one trail puff and the next for an aircraft.
see_also: ["Trailer"]
when_omitted:
  kind: value
  value: "3"
---

An aircraft with a [`Trailer`](/keys/trailer/#scope-aircrafttype) drops a puff on every game frame that is a multiple of this number. A value of `0` or less disables the trail.

The setting is read from the art section the aircraft's [`Image`](/keys/image/) names, not from its rules section.
