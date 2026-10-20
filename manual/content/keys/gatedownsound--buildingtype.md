---
key: GateDownSound
scope: buildingtype
summary: "The sound a gate plays as it opens."
see_also: [GateUpSound, GateDown, Gate]
when_omitted:
  kind: computed
  note: "the GateDown sound of the AudioVisual section"
---

A sound name. A [`Gate=yes`](/keys/gate/) structure plays it where it stands as it starts to open. `none` makes the gate open in silence, and without the key the gate plays the [`GateDown`](/keys/gatedown/) sound. The names follow Tiberian Sun, where down is open and up is closed.

```ini title="rulesmd.ini"
[MYGATE] ; example BuildingType
Gate=yes
GateDownSound=GateEpsilonDown
```
