---
key: Insignia
summary: The shape file that supplies the rank insignia frames of an object type, for all three ranks at once.
see_also: [Insignia.ShowEnemy, InsigniaFrame, InsigniaFrames, EnemyInsignia, "system:veterancy"]
when_omitted:
  kind: value
  value: PIPS.SHP
  note: The stock insignia frames come from PIPS.SHP.
---

`Insignia` names the shape file that the [rank insignia](/systems/veterancy/#rank-display) of this type are drawn from. Write the name without an extension; the engine adds `.SHP`. The file is found among the loaded archives like `PIPS.SHP`. If it is not found, the type keeps drawing from `PIPS.SHP`.

The key applies to infantry, vehicles, aircraft and structures. The frame drawn is the one [`InsigniaFrame`](/keys/insigniaframe/) or [`InsigniaFrames`](/keys/insigniaframes/) picks for the rank, or frame 0 of the file when they pick none. A rookie whose rank has a file shows that frame too.

Three keys set the file for one rank only: `Insignia.Rookie`, `Insignia.Veteran` and `Insignia.Elite`. A rank's key replaces the `Insignia` file for that rank, and a rank without a key keeps the `Insignia` file.

A below-rookie object keeps its stock insignia from `PIPS.SHP`. None of these keys changes it.

```ini title="rulesmd.ini"
[HTNK] ; example VehicleType
Insignia=MYRANKS ; reads MYRANKS.SHP for every rank
Insignia.Elite=MYELITE ; elite ones read MYELITE.SHP instead
InsigniaFrame.Rookie=0 ; shows frame 0 of MYRANKS.SHP on a rookie
```

In the example, a veteran shows frame 0 of `MYRANKS.SHP` and an elite object shows frame 0 of `MYELITE.SHP`. A rookie shows frame 0 of `MYRANKS.SHP`, because `InsigniaFrame.Rookie` is set to 0, and would show it without the frame key too.
