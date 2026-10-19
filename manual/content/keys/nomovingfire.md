---
key: NoMovingFire
summary: Stops a vehicle firing while it has somewhere to go and resets its firing delay when it halts.
see_also: ["DeployToFire", "Ammo", "Turret"]
when_omitted:
  kind: value
  value: "no"
---

A vehicle with `NoMovingFire=yes` cannot fire while it has a destination. A vehicle held up mid-route by a blockage still has a destination, so it cannot fire either. It can fire again once it arrives or its move order is cleared.

When the vehicle halts, its firing delay is replaced by a quarter of its secondary weapon's firing delay, rounded down. It halts in this sense when a move step leaves it with no destination and no path. The replacement discards whatever firing delay was still running. A vehicle that fires, moves a cell or two and halts can therefore usually fire again before that shot's [`ROF`](/keys/rof/#scope-weapontype) delay has run out.

The secondary weapon's delay, before it is quartered, is:

- 1 frame with no secondary weapon, so the vehicle can fire at once.
- The weapon's full `ROF` for a sonic secondary weapon, or a spark, fire or railgun secondary whose particle system is already attached to the vehicle. The vehicle then waits a quarter of that `ROF`.
- The weapon's full `ROF`, adjusted as the `ROF` page describes for an ordinary reload, when the vehicle's place in a burst is at or past the secondary weapon's [`Burst`](/keys/burst/) count. Shots from either weapon advance that place. For example, a vehicle whose primary has `Burst=2` and whose secondary has `Burst=1` reaches this case when it halts after the first shot of a primary burst. The vehicle then waits a quarter of that adjusted `ROF`.
- Otherwise, the weapon's [`BurstDelay0`](/keys/burstdelay0/) to `BurstDelay3` value partway through a burst, or a random 3 to 5 frames. The random figure leaves a wait of 0 or 1 frames.

The flag does not change how a vehicle reloads. [`Reload`](/keys/reload/) sets the delay, and [`MobileFire=no`](/keys/mobilefire/) is the setting that stops the delay from counting down while the vehicle moves.

```ini title="rules.ini"
[MYARTILLERY] ; a UnitType registered in [VehicleTypes]
NoMovingFire=yes
Ammo=2
Reload=300
```
