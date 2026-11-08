---
key: Slaved
summary: "Stops a slave infantryman from taking player orders while it works for an Enslaves object."
see_also: [Enslaves, "system:slave-miners"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the player cannot give the infantry movement orders while it works for an [`Enslaves`](/keys/enslaves/#scope-aircrafttype) object. A slave freed when its miner is destroyed takes orders like any other infantry.

The key is read on every object type, but only infantry use it.
