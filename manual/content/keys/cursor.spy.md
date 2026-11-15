---
key: Cursor.Spy
summary: The cursor shown over a structure of this type when a spy could infiltrate it.
see_also: ["Cursor.Enter", "Cursor.Deploy"]
when_omitted:
  kind: value
  value: "Enter"
---

`Cursor.Spy` names a cursor, either one of the stock cursors or one that `[MouseCursors]` in the rules file defines. The cursor comes from the structure under the pointer. It shows when the selected object is a soldier with `Agent=yes` and the structure is one it can infiltrate. A name that is neither is ignored, and the stock cursor shows.

The cursor of the first selected object decides when several are selected.

```ini title="rulesmd.ini"
[MouseCursors]
MyCursor=1000,10,5,-1,1,Center,Middle ; frame, count, interval, small frame, small count, hot spot

[GAPOWR] ; example BuildingType
Cursor.Spy=MySpy
```
