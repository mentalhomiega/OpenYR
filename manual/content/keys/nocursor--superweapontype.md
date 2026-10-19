---
key: NoCursor
scope: superweapontype
summary: "Names the cursor shown over cells where the weapon may not be aimed."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "Disallowed"
---

The name of a stock cursor or of an entry in [`[MouseCursors]`](/systems/superweapons/#named-cursors). It shows where [`SW.RequiresTarget=`](/keys/sw.requirestarget/), [`SW.RequiresHouse=`](/keys/sw.requireshouse/), the range limits, the designators, the inhibitors or [`SW.FireIntoShroud=`](/keys/sw.fireintoshroud/) refuse the cell.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
NoCursor=NoCanDo
```

See [Presentation](/systems/superweapons/#presentation).
