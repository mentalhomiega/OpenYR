---
key: YuriBarracks
summary: Gives the structure an extra preferred door cell for leaving objects, two east and one south of its foundation's north-west corner.
see_also: [NODBarracks, GDIBarracks, ExitCoord, "system:production"]
when_omitted:
  kind: value
  value: "no"
---

A `YuriBarracks=yes` structure lets objects out through a door cell when it can: the cell two east and one south of the north-west corner of its foundation. An object that leaves by the door cell is shifted by [`ExitCoord`](/keys/exitcoord/) where it appears, the same as for [`NODBarracks=yes`](/keys/nodbarracks/).

The door cell is tried after the [`GDIBarracks`](/keys/gdibarracks/) and [`NODBarracks`](/keys/nodbarracks/) door cells, and before the ordinary exit cells around the foundation. It is used only when it lies inside the [playfield](/glossary/#playable-area) and the leaving object could enter it. Otherwise the structure falls back to the next candidate.

The flag changes only where objects leave. It does not decide what the structure produces. A repair bay with this flag also prefers the door cell when it sends a repaired vehicle away.

Stock rules set this flag on `YABRCK` and `NACLON`.

```ini title="rules.ini"
[MYBARRACKS] ; a BuildingType registered in [BuildingTypes]
YuriBarracks=yes ; objects try the cell two east and one south first
```
