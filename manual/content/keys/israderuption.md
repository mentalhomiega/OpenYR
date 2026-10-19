---
key: IsRadEruption
summary: "Makes a weapon throw eight arching beams to the cells around its firer each time it fires."
see_also: [IsRadBeam, RadColor, CellSpread]
when_omitted:
  kind: value
  value: "no"
---

Each shot starts eight beams from the firer's center, one to each of the eight cells around the firer's cell, as many cells out as the whole-number part of the warhead's [`CellSpread`](/keys/cellspread/). A spread of `10` puts the beams on the cells ten cells away to the four sides and the four corners. A spread below `1` puts all eight beams on the firer's own cell, where each is a short arch.

Each beam ends at a random point within 128 leptons of its cell's center on both axes, lasts 5 to 20 frames, and arches up to a random height of 100 to 500 leptons. The arch rises over the first half of the beam's life and falls over the second half. The beam is drawn in [`RadColor`](/keys/radcolor/), brighter toward the top of its arch. A beam is drawn only; the shot's projectile still carries the damage.

The key has no effect on a weapon that also sets [`IsLaser=yes`](/keys/islaser/), [`IsElectricBolt=yes`](/keys/iselectricbolt/) or [`IsRadBeam=yes`](/keys/isradbeam/).

While the game runs below its desired frame rate, each beam that is still drawn can be dropped on any frame, and more of them are dropped the lower the rate is. This changes only what is drawn.

```ini title="rulesmd.ini"
[MYERUPTION] ; a WeaponType
IsRadEruption=yes
Warhead=MYERUPTIONWH ; its CellSpread=10 sets how far out the beams land
```
