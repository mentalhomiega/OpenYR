---
key: MultiplayerScore.Palette
summary: The palette the multiplayer score screen's picture is drawn with.
see_also: ["system:multiplayer-score-screen", "MultiplayerScore.Background"]
when_omitted:
  kind: computed
  note: '`MPASCRN.PAL` for the first side in `[Sides]`, `MPSSCRN.PAL` for the second and `MPYSCRN.PAL` for any other.'
---

Write the file name with its `.PAL` extension. The palette is loaded before the picture is drawn and stays in force for the screen's text. Each band file carries its own palette. See [`MultiplayerScore.Background`](/keys/multiplayerscore.background/).
