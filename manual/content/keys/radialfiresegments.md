---
key: RadialFireSegments
summary: Fires each shot of the object's weapons at the next angle of a 180-degree fan centered on its body facing, and launches vertical projectiles at full speed.
see_also: [ROT, Burst, OmniFire]
when_omitted:
  kind: value
  value: "0"
---

A `RadialFireSegments` value above zero splits the half circle in front of the object into that many directions. Each projectile the object fires leaves along the next direction in turn, starting 90 degrees to the right of the body facing and ending one step short of 90 degrees to the left. The object keeps one count for all its weapons. After the last direction the count starts again from the first.

The directions are spaced evenly. With `RadialFireSegments=10`, the shots step through 18-degree intervals. A homing projectile curves to its target from its launch direction. A projectile with no rate of turn flies along the launch direction and does not turn toward the target.

A vertical projectile, such as the Kirov's bomb, leaves an object with `RadialFireSegments` above zero at its weapon's `Speed`, or at half the distance to the target when that is less. Without the key it leaves at speed 1 and accelerates. Homing projectiles always leave at speed 1.

Stock rules set `RadialFireSegments=10` on `AEGIS`. Its missiles fan out from the cruiser and then curve onto the target.

```ini title="rules.ini"
[MYSHIP] ; a UnitType registered in [VehicleTypes]
RadialFireSegments=8 ; shots step through eight directions
```
