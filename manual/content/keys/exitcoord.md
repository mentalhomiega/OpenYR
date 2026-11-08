---
key: ExitCoord
summary: The offset at which a barracks or weapons factory places a produced object.
see_also: [GDIBarracks, NODBarracks, WeaponsFactory, "system:production"]
when_omitted:
  kind: value
  value: 0,0,0
  note: A weapons factory uses the foundation offset described under Weapons factories instead.
---

`ExitCoord` moves the point where a barracks puts down an object it releases. Only a [`GDIBarracks=yes`](/keys/gdibarracks/) or [`NODBarracks=yes`](/keys/nodbarracks/) structure uses it for a barracks exit, and only when the object leaves by the barracks' preferred exit cell. A [`WeaponsFactory=yes`](/keys/weaponsfactory/) structure uses it for every object it releases; see [Weapons factories](#weapons-factories).

```ini title="rules.ini"
[GAPILE] ; the stock GDI barracks
GDIBarracks=yes
ExitCoord=-64,64,0
```

The three numbers are X, Y and height, in leptons; 256 leptons make one cell. The stock value above moves the soldier a quarter of a cell toward lower X and a quarter of a cell toward higher Y.

The height is absolute, not measured from the ground. An object whose height would put it below the ground is raised onto it. A soldier is moved to one of the cell's standard infantry positions, chosen by which part of the cell the point lies in, only when its height equals the ground level at that point. With the stock height of `0`, that happens only on ground at level 0. On higher ground the soldier is lifted onto the ground and stays at the offset X and Y.

## Where the offset is measured from

A `GDIBarracks=yes` structure prefers to release its object toward the cell one along and two down from the top-left cell of its footprint. A `NODBarracks=yes` structure prefers the cell two along and two down. The object does not appear on that cell. It appears on the cell inside the footprint next to it: the preferred cell moved one step back on each axis where it lies outside the footprint. On a footprint two cells deep, which both stock barracks have, that is a cell on the bottom row of the barracks itself. The offset is measured from the center of that cell.

When the preferred cell is blocked, the object leaves by another cell around the footprint. It then appears at the center of the matching cell inside the footprint, with no offset. [Leaving the factory](/systems/production/#leaving-the-factory) covers how the exit cell is picked.

## Weapons factories

A [`WeaponsFactory=yes`](/keys/weaponsfactory/) structure releases every object it builds, soldiers included, at its `ExitCoord`, measured from the structure's location. This holds even when the structure is also a barracks.

When the key is omitted, a weapons factory uses `(width - 1) * 128, (height - 1) * 128, 0` leptons, where width and height are the structure's foundation in cells. For a `5x3` foundation that is `512,256,0`, the value the stock war factories write.
