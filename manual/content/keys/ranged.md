---
key: Ranged
summary: Makes the projectile check its fuse every frame, as a steered projectile does.
see_also: [Arm, ROT]
when_omitted:
  kind: value
  value: "no"
---

A `Ranged=yes` projectile checks its fuse every frame, whatever its [`ROT`](/keys/rot/#scope-bullettype). A projectile with `ROT=0` that is not `Ranged=yes` does not check its fuse at all.

A flak shell is the usual case. It bursts near its aim point, at about the height it was aimed at, instead of falling to the ground under that point.

The fuse works as it does on a steered projectile: [`Arm`](/keys/arm/) holds it shut for a delay, and once armed it trips within 64 leptons of the fuse point. Nothing else limits how far a `Ranged=yes` projectile flies. Its flight ends by the rules in [what ends a flight](/systems/projectile-flight/#what-ends-a-flight).

```ini title="rules.ini"
[MYROCKET] ; a BulletType, registered by a weapon naming it as its Projectile
Image=MISSILE
ROT=5
Ranged=yes
```
