---
key: DebrisMaximums
summary: The largest number of pieces each debris type may contribute, one figure per entry.
see_also: [DebrisTypes, MaxDebris, MetallicDebris]
---

```ini title="rules.ini"
[MYTANK] ; a UnitType registered in [VehicleTypes]
MaxDebris=6
DebrisTypes=MYSCRAP,MYTIRE ; VoxelAnimTypes registered in [VoxelAnims]
DebrisMaximums=4,2 ; at most four MYSCRAP, then at most two MYTIRE
```

Each figure caps the debris type in the same position of [`DebrisTypes`](/keys/debristypes/): the first figure caps the first type, the second caps the second, and so on. The figure is a maximum, and the count thrown is drawn at random; [`MaxDebris`](/keys/maxdebris/) describes the draw. A figure of `0` never throws a piece of its type and leaves the whole remaining budget to the next entry.

The list does nothing on a type that names no `DebrisTypes`.

:::danger[Give every debris type a figure of 0 or more]
Yuri's Revenge does not check that this list is as long as `DebrisTypes`. With fewer figures than debris types, each extra type reads its figure from past the end of the list, so the piece count is unpredictable and the read can crash the game. A figure of `-1` divides by zero and crashes the game when the object is destroyed.

OpenYR reads a missing figure and a figure below `0` as `0`, so that debris type throws none.
:::

A warhead's `DebrisMaximums` caps the pieces its shot throws. The [warhead page](/keys/debrismaximums/#scope-warheadtype) gives its default and details.
