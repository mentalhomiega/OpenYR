---
key: Cursor.NoEnter
summary: The cursor shown over an object of this type when a selected object cannot enter it.
see_also: ["Cursor.Enter", "Cursor.NoDeploy", "Cursor.NoMove"]
when_omitted:
  kind: value
  value: "NoEnter"
---

`Cursor.NoEnter` names a cursor, either one of the stock cursors or one that `[MouseCursors]` in the rules file defines. The cursor comes from the object under the pointer, not from the selected one. It shows over a full transport or any other object that refuses the selected one. A name that is neither is ignored, and the stock cursor shows.

The cursor of the first selected object decides when several are selected.

```ini title="rulesmd.ini"
[MouseCursors]
MyCursor=1000,10,5,-1,1,Center,Middle ; frame, count, interval, small frame, small count, hot spot

[GAPILE] ; example BuildingType
Cursor.NoEnter=MyNoEnter
```
