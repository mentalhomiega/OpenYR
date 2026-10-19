---
key: Cursor.NoDeploy
summary: The cursor shown over a selected object of this type when a deploy click is refused.
see_also: ["Cursor.Deploy", "Cursor.NoMove", "Cursor.NoEnter"]
when_omitted:
  kind: value
  value: "NoDeploy"
---

`Cursor.NoDeploy` names a cursor, either one of the stock cursors or one that `[MouseCursors]` in the rules file defines. It shows where a deploy click on the selected object would be refused. A name that is neither is ignored, and the stock cursor shows.

The cursor of the first selected object decides when several are selected.

```ini title="rulesmd.ini"
[MouseCursors]
MyCursor=1000,10,5,-1,1,Center,Middle ; frame, count, interval, small frame, small count, hot spot

[GGI] ; example InfantryType
Cursor.NoDeploy=MyNoCursor
```
