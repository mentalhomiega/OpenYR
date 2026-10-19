---
key: Cursor.Enter
summary: The cursor shown over an object of this type when a selected object could enter it.
see_also: ["Cursor.NoEnter", "Cursor.Deploy", "Cursor.Move"]
when_omitted:
  kind: value
  value: "Enter"
---

`Cursor.Enter` names a cursor, either one of the stock cursors or one that `[MouseCursors]` in the rules file defines. The cursor comes from the object under the pointer, not from the selected one. It shows over a transport, a tank bunker, a bio reactor, a grinder, a repair depot or another object that a selected object can enter. A name that is neither is ignored, and the stock cursor shows.

The cursor of the first selected object decides when several are selected.

```ini title="rulesmd.ini"
[MouseCursors]
MyCursor=1000,10,5,-1,1,Center,Middle ; frame, count, interval, small frame, small count, hot spot

[GAPILE] ; example BuildingType
Cursor.Enter=MyEnter
```
