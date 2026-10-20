---
key: GateUpSound
scope: buildingtype
summary: "The sound a gate plays as it closes."
see_also: [GateDownSound, GateUp, Gate]
when_omitted:
  kind: computed
  note: "the GateUp sound of the AudioVisual section"
---

A sound name. A [`Gate=yes`](/keys/gate/) structure plays it where it stands as it starts to close. `none` makes the gate close in silence, and without the key the gate plays the [`GateUp`](/keys/gateup/) sound.

```ini title="rulesmd.ini"
[MYGATE] ; example BuildingType
Gate=yes
GateUpSound=GateEpsilonUp
GateDownSound=GateEpsilonDown
```
