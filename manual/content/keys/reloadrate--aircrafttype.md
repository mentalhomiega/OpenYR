---
key: ReloadRate
scope: aircrafttype
label: Aircraft reload interval
see_also: [Ammo, UnitReload]
when_omitted:
  kind: value
  value: "unset, so the aircraft reloads on the pad's [General] ReloadRate passes"
---

`ReloadRate` on an aircraft type is an Ares key: the minutes between the rounds of [`Ammo`](/keys/ammo/#scope-aircrafttype) a [`UnitReload=yes`](/keys/unitreload/) building gives a docked aircraft of this type. The game multiplies it by 900 frames and truncates, with at least one frame. Each such aircraft runs its own timer, which starts when the aircraft settles on its dock, so its first round arrives one interval later.

Once the aircraft is full, the pad repairs it on its usual [`[General] ReloadRate`](#scope-global-rules) passes. Aircraft whose type leaves the key out reload on those passes as before, also when they share a pad with one that sets it.

```ini title="rulesmd.ini"
[ORCA]
ReloadRate=.05   ; a rocket every 45 frames
```
