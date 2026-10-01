---
key: ShakeYhi
summary: "The largest vertical screen shake, in pixels, the warhead's detonation starts."
see_also: [ShakeXlo, ShakeXhi, ShakeYlo, "system:warheads"]
when_omitted:
  kind: value
  value: "0"
---

The highest value of the random vertical [screen shake](/systems/warheads/#what-one-blast-does-besides-damage) a projectile with this warhead starts when it detonates. Positive values move the view right or down first, negative ones left or up.

```ini title="rulesmd.ini"
[MyBigBang] ; example Warhead
ShakeYlo=10
ShakeYhi=20
```
