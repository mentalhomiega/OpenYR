---
key: InsigniaFrame
summary: The insignia frame an object type shows for all three ranks, or for one rank through the rank keys.
see_also: [InsigniaFrames, Insignia, "system:veterancy"]
when_omitted:
  kind: value
  value: "-1"
  note: A value below 0 selects the stock frame of the rank.
---

`InsigniaFrame` is the frame number shown for a rookie, a veteran and an elite object of this type. Frames are counted from `0` in the file that [`Insignia`](/keys/insignia/) names, or in `PIPS.SHP` when the type names none. Keep the number inside the file; a frame the file lacks is not checked.

The stock frames are `14` for a veteran and `15` for an elite object. A rookie has no stock frame and shows no insignia, so the key is what gives a rookie one. A value below `0` selects the stock frame for each rank.

Three keys set one rank only: `InsigniaFrame.Rookie`, `InsigniaFrame.Veteran` and `InsigniaFrame.Elite`. They take the same values and override `InsigniaFrame` and [`InsigniaFrames`](/keys/insigniaframes/) for their rank. The keys apply to infantry, vehicles, aircraft and structures.

A below-rookie object keeps its stock insignia whatever the keys say.

```ini title="rulesmd.ini"
[E1] ; example InfantryType
InsigniaFrame=3 ; frame 3 for rookie, veteran and elite
InsigniaFrame.Elite=4 ; elite soldiers show frame 4
```
