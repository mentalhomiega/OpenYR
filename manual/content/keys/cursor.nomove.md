---
key: Cursor.NoMove
summary: The cursor shown when a selected object of this type cannot move to the cell under the pointer.
see_also: ["Cursor.Move", "Cursor.NoDeploy", "Cursor.NoEnter"]
when_omitted:
  kind: value
  value: "NoMove"
---

`Cursor.NoMove` names a cursor, either one of the stock cursors or one that `[MouseCursors]` in the rules file defines. It shows over ground that the selected object cannot reach. A name that is neither is ignored, and the stock cursor shows.

The cursor of the first selected object decides when several are selected.

```ini title="rulesmd.ini"
[MouseCursors]
MyCursor=1000,10,5,-1,1,Center,Middle ; frame, count, interval, small frame, small count, hot spot

[MTNK] ; example VehicleType
Cursor.NoMove=MyNoMove
```
