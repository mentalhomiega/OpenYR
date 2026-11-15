---
key: Cursor.Move
summary: The cursor shown when a click would order a selected object of this type to move.
see_also: ["Cursor.NoMove", "Cursor.Deploy", "Cursor.Enter"]
when_omitted:
  kind: value
  value: "Move"
---

`Cursor.Move` names a cursor, either one of the stock cursors or one that `[MouseCursors]` in the rules file defines. It shows over open ground that the selected object can reach. A name that is neither is ignored, and the stock cursor shows.

The cursor of the first selected object decides when several are selected.

```ini title="rulesmd.ini"
[MouseCursors]
MyCursor=1000,10,5,-1,1,Center,Middle ; frame, count, interval, small frame, small count, hot spot

[MTNK] ; example VehicleType
Cursor.Move=MyMove
```
