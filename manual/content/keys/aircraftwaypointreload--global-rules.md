---
key: AircraftWaypointReload
scope: global-rules
label: Aircraft take planned orders and rearm
summary: Lets an armed aircraft with limited ammunition take planned orders in planning mode, and resume them after it rearms at a dock.
when_omitted:
  kind: value
  value: "yes"
---

`AircraftWaypointReload=yes` lets an aircraft that rearms at a dock take planned orders in planning mode. An aircraft qualifies when its type has a limited [`Ammo`](/keys/ammo/) and carries a weapon, so a Harrier or Black Eagle qualifies, and a transport with no ammunition limit does not. Planning mode accepts such an aircraft, and it refuses every other aircraft as before.

A qualifying aircraft with less than its full ammunition does not take its next planned order. It goes to a dock to rearm, as an idle aircraft does. If it ran out while it was attacking a target that is still alive, that attack goes back to the front of the plan, so the aircraft attacks again after it rearms. It takes the next order once its ammunition is full and it is on guard.

`AircraftWaypointReload=no` restores the refusal of gamemd. Planning mode refuses every aircraft, and the refused order shows "You cannot use waypoints with aircraft." and plays the scold sound.

The value is read from the rules, so every player in a multiplayer game must use the same rules. A plan is not saved, so a game saved while a plan is running loads without it, as it does for any plan.

Set it in the `[General]` section.

```ini title="rulesmd.ini"
[General]
AircraftWaypointReload=yes
```
