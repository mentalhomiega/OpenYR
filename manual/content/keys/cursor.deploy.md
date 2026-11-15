---
key: Cursor.Deploy
summary: The cursor shown over a selected object of this type when a click would deploy it.
see_also: ["Cursor.NoDeploy", "Cursor.Move", "Cursor.Enter"]
when_omitted:
  kind: value
  value: "Deploy"
---

`Cursor.Deploy` names a cursor, either one of the stock cursors or one that `[MouseCursors]` in the rules file defines. It shows where the selected object is the one under the pointer, which is a click that deploys a structure, deploys a soldier or a vehicle that deploys, or empties a transport. A name that is neither is ignored, and the stock cursor shows.

The cursor of the first selected object decides when several are selected.

```ini title="rulesmd.ini"
[MouseCursors]
MyCursor=1000,10,5,-1,1,Center,Middle ; frame, count, interval, small frame, small count, hot spot

[GGI] ; example InfantryType
Cursor.Deploy=MyCursor
```
