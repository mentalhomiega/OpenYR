---
key: MaxPips
summary: How many pips a selected object's pip row holds.
see_also: [PipScale, Passengers, Ammo, Storage, MaxCharge, "system:transports"]
when_omitted:
  kind: context-dependent
  note: "The length the object's pip scale gives. That is five for `Tiberium` (three for a soldier), ten for `Power` and eight for `Charge`, for `Ammo` it is the type's `Ammo` value, and for `Passengers` it is the type's `Passengers` value. A structure using `Power` gets six pips per cell of footprint width instead."
---

```ini title="rules.ini"
[MYAPC]           ; a UnitType registered in [VehicleTypes]
Passengers=12
PipScale=Passengers
MaxPips=6
```

`MaxPips` sets how many pips the row has, replacing the default length of the type's [`PipScale`](/keys/pipscale/). The example transport shows six passenger pips instead of its twelve. A type with no `PipScale` has no row, so the key does nothing for it.

Some rows stay capped by another value:

- Under `PipScale=Ammo` or `PipScale=Passengers`, the row holds at most the type's [`Ammo`](/keys/ammo/) or [`Passengers`](/keys/passengers/) value. `MaxPips=20` on a transport with `Passengers=6` draws six pips.
- Under `PipScale=Ammo` with [`PipWrap`](/keys/pipwrap/) set, `MaxPips` is ignored and the pips number the type's `Ammo`.

On a structure under `PipScale=Power`, the key replaces the default of six pips per cell of footprint width.

Under `PipScale=Power`, no pips are drawn unless the type also sets `Passengers` above `0`. Such a type shows passenger pips across the length `PipScale=Power` gives.

A negative value counts as `0`, which leaves the row with no pips.

:::caution[Keep a long row within its object]
A vehicle, infantry soldier or aircraft draws its row to the right, four pixels per pip, and the row does not stop at the object's edge. A row much longer than the object is wide is drawn over whatever stands beside it. A structure's default length fits its footprint, but a larger `MaxPips` can run the row past the structure's edge too.
:::
