---
key: MultiplayerScore.Background
summary: The shape drawn as the side's picture on the multiplayer score screen.
see_also: ["system:multiplayer-score-screen", "MultiplayerScore.Palette", "MultiplayerScore.Bars"]
when_omitted:
  kind: computed
  note: '`MPASCRNL.SHP` for the first side in `[Sides]`, `MPSSCRNL.SHP` for the second and `MPYSCRNL.SHP` for any other; the 640x480 pictures end in `S` in place of `L`.'
---

Write the file name with its `.SHP` extension. The shape is drawn into the picture's box whatever the screen's size, so the same file serves the large and the small layout. Pair it with [`MultiplayerScore.Palette`](/keys/multiplayerscore.palette/), because the shape is drawn with the palette the screen loaded.

```ini title="rules.ini"
[FourthSide]
MultiplayerScore.Background=mpfscrnl.shp
MultiplayerScore.Palette=mpsscrnlf.pal
```

The side is the one the local player played. A file that cannot be found leaves the picture's box empty.
