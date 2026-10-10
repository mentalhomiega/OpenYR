---
key: FireWhileMoving
summary: "Set to no, stops a unit with a destination from firing this weapon until it stops moving."
see_also: [MobileFire, NoMovingFire, BalloonHover, "system:target-selection"]
when_omitted:
  kind: value
  value: "yes"
---

`FireWhileMoving=no` makes a unit with a destination unable to fire this weapon. The unit can fire it again once its move order completes or is cleared. Its other weapons are not affected.

A [`BalloonHover=yes`](/keys/balloonhover/) unit ignores the flag, so its weapons fire while it moves.

The check is separate from [`MobileFire=no`](/keys/mobilefire/) and [`NoMovingFire=yes`](/keys/nomovingfire/). Either of those flags also blocks every weapon of the unit while it moves, and the unit's halt delay follows them.

Stock rules set this flag only on the `DiskDrain` weapon, which the `DISK` unit carries as its secondary weapon. The `DISK` unit cannot fire that weapon while it has a destination.

```ini title="rules.ini"
[MYWEAPON] ; a WeaponType registered in [Weapons]
FireWhileMoving=no ; units with a destination cannot fire MYWEAPON
```
