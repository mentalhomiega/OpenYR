---
key: SpawnsNumber
scope: aircrafttype
label: 'Carried aircraft count'
see_also: [Spawns, "system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "0"
---

How many of its [`Spawns`](/keys/spawns/#scope-aircrafttype) type the object carries. With `0` or below it carries none, and its `Spawner` weapons do nothing.

A selected object with this key above `0` shows one pip for each carried aircraft in its pip row, lit while that aircraft is docked, whatever its [`PipScale`](/keys/pipscale/).
