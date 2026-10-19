---
key: Abductor
summary: "With yes, a hit from this weapon pulls its target into the firer's passenger hold instead of damaging it."
see_also: [Abductor.AbductBelowPercent, Abductor.Anim, Abductor.ChangeOwner, ImmuneToAbduction, SizeLimit, Passengers, "system:transports"]
when_omitted:
  kind: value
  value: "no"
---

When the projectile of an `Abductor=yes` weapon detonates on its target, the firer takes the target aboard as a passenger and the weapon deals no damage. The target must be infantry or a vehicle, and the firer must be a vehicle or aircraft with a [`Passengers`](/keys/passengers/) budget above zero. The target is refused, and the hit deals its ordinary damage, when any of these holds:

- The target type sets [`ImmuneToAbduction=yes`](/keys/immunetoabduction/), or the target is under the Iron Curtain.
- The target has more strength left than [`Abductor.AbductBelowPercent`](/keys/abductor.abductbelowpercent/) allows.
- The target does not fit the hold by the tests that admit a passenger. Its [`Size`](/keys/size/) must be no larger than the firer's [`SizeLimit`](/keys/sizelimit/), which is `0` unless the type sets it, and no larger than the space left in the hold. A target under mind control, or a vehicle for a firer with [`IsVehicleTransport=no`](/keys/isvehicletransport/), is refused too.

A target that is taken leaves the map, drops its orders and leaves its team. It plays [`Abductor.Anim`](/keys/abductor.anim/) where it stood and changes owner when [`Abductor.ChangeOwner=yes`](/keys/abductor.changeowner/). It leaves the hold as any passenger does.

```ini title="rulesmd.ini"
[MyAbductGun] ; example WeaponType
Abductor=yes

[MYSAUCER] ; example VehicleType
Passengers=3
SizeLimit=2
Primary=MyAbductGun
```
