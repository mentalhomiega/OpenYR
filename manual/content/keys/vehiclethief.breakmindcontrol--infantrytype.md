---
key: VehicleThief.BreakMindControl
scope: infantrytype
summary: "Whether a vehicle thief may take a mind-controlled vehicle, freeing it from its controller."
see_also: [VehicleThief, VehicleThief.OneTime, "system:capture"]
when_omitted:
  kind: value
  value: "yes"
---

`VehicleThief.BreakMindControl` is an Ares key for a [`VehicleThief=yes`](/keys/vehiclethief/) soldier.

With `yes`, a thief that takes a vehicle a mind controller holds frees the vehicle first: the controller's link and control ring go, the mind-control release sound plays, and the vehicle returns to the house it was taken from before it changes hands to the thief's house. A vehicle that was taken from the thief's own house therefore stays with that house. The controller no longer holds the vehicle, so its death or a later release does not take the vehicle back.

With `no`, the soldier cannot take a vehicle while a mind controller holds it. A player-controlled thief gets the select cursor over it, a thief that reaches its cell anyway stops and goes idle, and an unarmed thief does not pick it as a target. A controlled crusher that drives over such a thief heading for it crushes it like any other soldier. A vehicle taken for good, such as by a [`MindControl.Permanent=yes`](/keys/mindcontrol.permanent/) warhead, has no controller and can be stolen either way.

```ini title="rulesmd.ini"
[HIJACKER] ; example InfantryType
VehicleThief=yes
VehicleThief.BreakMindControl=no
```
