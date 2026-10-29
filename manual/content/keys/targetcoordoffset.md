---
key: TargetCoordOffset
summary: "Moves the point on a structure where a bolt or radiation beam aimed at it ends."
see_also: [IsElectricBolt, IsRadBeam]
when_omitted:
  kind: value
  value: 0,0,0
---

`TargetCoordOffset` is added to the center of a structure to give the point that [`IsElectricBolt=yes`](/keys/iselectricbolt/) bolts, [`IsRadBeam=yes`](/keys/isradbeam/) beams, laser beams and sonic waves aimed at it end on. The three numbers are X, Y and height, in leptons; 256 leptons make one cell. Projectiles are not affected: they aim at the structure's center.

```ini title="rules.ini"
[NAYARD] ; the stock Soviet naval yard
TargetCoordOffset=256,256,0
```

The stock naval yards of all three sides set it, so that a beam aimed at one ends on its dock rather than on the middle of the foundation.
