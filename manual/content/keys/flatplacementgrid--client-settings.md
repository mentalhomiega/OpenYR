---
key: FlatPlacementGrid
scope: client-settings
label: Flat building placement grid
summary: "Whether the squares that show where a building can go are drawn flat in the modern interface style."
when_omitted:
  kind: value
  value: "yes"
---

`FlatPlacementGrid=yes` draws the squares of a pending building's footprint flat, all at the height of the square under the pointer. Without it each square follows its own ground: a square on a ramp takes the slope shape, and one on a raised cell is drawn at that cell's height, as Yuri's Revenge draws them. Only the drawing changes. Whether a square is green or red does not.

The Game page of the settings sets it in the `[Options]` section of `RA2MD.INI`. It has an effect only in the modern interface style; the classic style always draws the squares as Yuri's Revenge does. The previews of a wall, fence or firestorm run that joins a structure follow the same setting.
