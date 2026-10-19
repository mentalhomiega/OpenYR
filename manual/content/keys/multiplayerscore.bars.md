---
key: MultiplayerScore.Bars
summary: The PCX files drawn behind the title, the headings and each player row of the multiplayer score screen.
see_also: ["system:multiplayer-score-screen"]
when_omitted:
  kind: computed
  note: '`MPASCRNLBAR~~.PCX` for the first side in `[Sides]`, `MPSSCRNLBAR~~.PCX` for the second and `MPYSCRNLBAR~~.PCX` for any other, with `~~` standing for `01` to `10`.'
---

The screen draws ten bands, numbered from `01`: the title, the headings and the eight player rows. Each `~~` in the value is replaced by the band's two-digit number. A value without `~~` names one file that every band uses. A file that cannot be found leaves its band empty.

```ini title="rules.ini"
[GDI]
MultiplayerScore.Bars=mpnone.pcx
```
