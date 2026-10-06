---
key: CanHideThings
scope: buildingtype
label: 'Hides objects behind it'
see_also: [OccupyHeight, CanBeHidden, Behind, ShowHidden]
when_omitted:
  kind: value
  value: "yes"
---

`CanHideThings=yes` makes a placed structure cover the cells behind it, so that an object standing there is marked as hidden. The marker is the [`Behind`](/keys/behind/#scope-global-rules) animation, or blinking corner brackets when no `Behind` animation is set. It does not change what objects can see, target or reach.

The game reads this key from the art section named after the BuildingType's ID, not from the section its `Image=` points to. The other keys on this page are read from the image's art section.

## Covered cells

When the structure is placed, each cell of its foundation and the cells diagonally behind it, toward the top of the screen, gain one cover. Each line holds [`OccupyHeight`](/keys/occupyheight/#scope-buildingtype) minus one cells, counting the foundation cell, and at least one. A cell reached from two foundation cells gains one cover, not two. With `OccupyHeight` of 2 or less, only the foundation itself is covered, so nothing standing beside the structure is hidden.

`AddOccupy1` to `AddOccupy8` and `RemoveOccupy1` to `RemoveOccupy8` adjust the cover by hand. Each value is an `X,Y` cell offset from the structure's origin cell. When the structure is placed, each `AddOccupy` cell gains one cover and each `RemoveOccupy` cell loses one, never going below zero. An entry left out does nothing.

When the structure is removed, the cover it added to its foundation, the cells behind it and its `AddOccupy` cells is taken away again. Cover that a `RemoveOccupy` cell lost is not given back, so if another structure covered that cell, the cell stays one cover short after this structure is gone.

## Hidden objects

An object is marked as hidden while its cell has at least one cover, with one exception: a cell that holds a structure and has exactly one cover counts as clear, since that cover is normally the structure's own. Aircraft are never marked, nor are objects of a type with [`CanBeHidden=no`](/keys/canbehidden/#scope-aircrafttype), cloaked objects and disguised objects. The marker is drawn only over objects the player can currently see.

```ini title="artmd.ini"
[MYTOWER] ; example art section, named after the BuildingType's ID
CanHideThings=yes
OccupyHeight=5 ; covers three cells behind each foundation cell, plus the foundation
AddOccupy1=-4,-4 ; one more cell further back
RemoveOccupy1=0,-1 ; this cell behind a low part of the building stays visible
```
