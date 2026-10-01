---
key: ShowOccupantPips
summary: Whether a garrisonable structure shows a figure for each of its occupant slots.
see_also: [CanBeOccupied, MaxNumberOccupants, OccupyPip, "system:garrisons"]
when_omitted:
  kind: value
  value: "yes"
---

A [`CanBeOccupied=yes`](/keys/canbeoccupied/) structure that is selected or under the mouse shows a row of figures, one for each [`MaxNumberOccupants`](/keys/maxnumberoccupants/) slot. A slot with a soldier inside shows that soldier's [`OccupyPip`](/keys/occupypip/) figure, and an empty slot shows an empty figure. `ShowOccupantPips=no` hides the row.

```ini title="rulesmd.ini"
[MYBUNKER] ; example BuildingType
CanBeOccupied=yes
MaxNumberOccupants=5
ShowOccupantPips=no
```

Every player sees the row, whoever owns the structure. Other pips the structure draws, such as its [`PipScale`](/keys/pipscale/) row, follow after it.
