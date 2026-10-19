---
key: Foundation
scope: buildingtype
label: Structure footprint
see_also: ["Bib", "BibShape", "Adjacent", "system:base-adjacency", "system:production"]
when_omitted:
  kind: value
  value: "1x1"
---

The value is one of the fixed [building foundation](/reference/enums/building-foundation/) names, matched without regard to case, or `Custom` for a footprint of any shape (below). In the Image ID entry, any other value gives the structure a one-cell footprint.

The footprint alone fixes a structure's shape on the map. It sets:

- the cells the structure occupies, which ground units cannot enter, except that a gate's cells are passable while the gate stands open;
- the ring of cells around the structure that a finished object leaves by;
- the structure's width and height;
- the shape the structure covers on the radar map.

```ini title="art.ini"
[MYWEAP] ; example war factory, drawn from its own Image ID
Foundation=4x3
```

The width and height are the two numbers in the name, and `3x3Refinery` counts as three by three. [Base adjacency](/systems/base-adjacency/) covers how far the footprint reaches for placement, and [`Bib=yes`](/keys/bib/) covers why it does not grow when a structure has a bib.

## The irregular sizes

`3x3Refinery` is three cells by three but occupies eight of the nine. The cell two along and one down from its top-left cell stays open. Its exit ring is a single cell: its own top-left cell, inside the footprint.

`0x0` occupies no cells, has no exit ring, and has a width and height of zero. A structure given it marks no cells on the map, and an object that leaves by the exit ring, such as a vehicle from a factory that is not [`WeaponsFactory=yes`](/keys/weaponsfactory/), has no cell to leave by.

`6x4` is the only other name whose block is not a rectangle. It occupies 21 cells: its bottom row is three cells wide and starts one cell along. Its exit ring is a single cell, two along and one above its top-left cell. It is also the only size six cells wide, and a structure that wide is normally drawn without the shared depth shape that decides whether a neighboring object appears in front of it or behind it. [`ZShapePointMove`](/keys/zshapepointmove/) covers the effect.

## The second reading

To give a structure a one-cell footprint, write `Foundation=1x1` in its Image ID entry and write no other size in the entry named after the BuildingType.

## Custom footprints

`Foundation=Custom` follows the Ares format and lists the footprint cell by cell:

| Key | Value |
|---|---|
| `Foundation.X`, `Foundation.Y` | The width and height in cells. Both must be at least 1, or the entry keeps the size it had. |
| `Foundation.0`, `Foundation.1`, ... | The occupied cells as `x,y` offsets from the top-left cell. The list ends at the first missing number or after width times height entries. With none, every cell of the rectangle is occupied. |
| `FoundationOutline.Length` | The number of cells in the exit ring. |
| `FoundationOutline.0`, `FoundationOutline.1`, ... | The exit ring as `x,y` offsets, in the order a finished object tries them. With no ring given, every free cell that touches the footprint, diagonals included, is used, row by row from the top. |

```ini title="art.ini"
[MYPOWR] ; an L-shaped structure: three cells across, one below the left end
Foundation=Custom
Foundation.X=3
Foundation.Y=2
Foundation.0=0,0
Foundation.1=1,0
Foundation.2=2,0
Foundation.3=0,1
```

A war factory leaves by the eleventh cell of its exit ring, as with the fixed sizes. Give a custom war factory at least eleven ring cells; with fewer, its vehicles have no preferred exit cell. The entry named after the BuildingType is read before the Image ID entry, and a fixed size written in either clears a custom footprint read earlier.

