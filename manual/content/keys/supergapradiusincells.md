---
key: SuperGapRadiusInCells
summary: How far a gap generator shrouds the ground while its owner has charged it, in cells.
see_also: [GapRadiusInCells, GapGenerator, ExtraPower, "system:map-visibility"]
when_omitted:
  kind: value
  value: "0"
---

A [`GapGenerator=yes`](/keys/gapgenerator/) structure with this key above `0` can be charged by its owner. Charging replaces the shroud of [`GapRadiusInCells`](/keys/gapradiusincells/) cells with one of this many cells, and adds the drain that a negative [`ExtraPower`](/keys/extrapower/) sets. Selecting the structure and clicking it charges it, and clicking again discharges it. [Gap generators](/systems/map-visibility/#gap-generators) covers the effects on the map.

The cursor appears only for the structure's owner, and only while the owner's power output exceeds its drain by more than the charge's drain, or the structure is already charged. A structure that is off cannot be charged, and one that loses power is discharged. A value of `0` keeps the click from changing anything.

Stock rules set `SuperGapRadiusInCells=10` on `GAGAP`, the same as its normal radius, with an `ExtraPower` of `-9000`.

```ini title="rules.ini"
[MYGAP] ; a BuildingType registered in [BuildingTypes]
GapGenerator=yes
GapRadiusInCells=6
SuperGapRadiusInCells=12 ; the charged shroud
ExtraPower=-200 ; the charge drains 200 more
```
