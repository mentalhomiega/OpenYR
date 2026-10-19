---
key: Reload
scope: aircrafttype
label: Shot reload delay
see_also: [EmptyReload, ReloadIncrement, PipWrap, Ammo, ManualReload, MobileFire, ReloadRate]
when_omitted:
  kind: value
  value: "0"
---

A vehicle with an [`Ammo`](/keys/ammo/) limit regains one shot `Reload` frames after the countdown starts. Every shot the vehicle fires while it is short of `Ammo` starts the countdown again, so a vehicle that keeps firing waits `Reload` frames after its last shot. When the countdown ends the vehicle regains one shot, and if it is still short the countdown starts again. A vehicle created short of its limit regains its first shot on the next frame.

```ini title="rules.ini"
[MYARTILLERY] ; a UnitType registered in [VehicleTypes]
Ammo=4
Reload=60 ; a shot comes back 60 frames (4 seconds) after the last one was fired
```

Other settings change the delay or when it runs:

- [`EmptyReload`](/keys/emptyreload/) replaces `Reload` when the vehicle has no shots left.
- [`ReloadIncrement`](/keys/reloadincrement/) lengthens the delay while the vehicle holds many shots.
- A vehicle with [`MobileFire=no`](/keys/mobilefire/) does not count down while it is moving.
- [`ManualReload=yes`](/keys/manualreload/) stops firing from starting the countdown.

Only vehicles reload this way. An aircraft refills at a [`UnitReload=yes`](/keys/unitreload/) structure, a structure refills when it is empty, and infantry follow the rules on the [`Ammo`](/keys/ammo/) page, so `Reload` has no effect on them. The default of `0` returns a shot on the frame after the vehicle fires, which makes the ammunition limit nearly meaningless: set `Reload` on every vehicle that has an `Ammo` limit.
