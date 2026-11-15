---
key: SW.RequiresTarget
scope: superweapontype
summary: "Limits where this superweapon may be aimed by what is on the cell."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "No limit"
---

A list of `land`, `water`, `empty`, `infantry`, `units` and `buildings`. `land` and `water` test the cell; if neither is listed, both pass. The other names test what stands on the cell, and the cell must hold one of the listed kinds; if none is listed, any content passes. `all` and `none` lift every limit. `empty` means no object on the cell.

Where the weapon cannot be aimed, the cursor shows [`NoCursor=`](/keys/nocursor/), a click fires nothing, and the computer skips the cell. Every machine repeats the test when the shot launches.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.RequiresTarget=land
```

See [Aiming rules](/systems/superweapons/#aiming-rules).
