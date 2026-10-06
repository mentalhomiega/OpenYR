---
key: AirportBound
summary: Lets an aircraft land only over a structure it is docking with, and makes it crash when no structure can take it.
see_also: ["Dock", "Landable", "Fighter"]
when_omitted:
  kind: value
  value: "no"
---

`AirportBound=yes` limits an aircraft to landing over the structure it is in radio contact with, such as the Allied Air Force Command it was sent to enter. The aircraft never settles on open ground. Without the key, an aircraft told to stop or land anywhere lands there when the cell is clear.

An airport bound aircraft that is airborne and has nowhere to dock crashes. This happens when it goes idle in the air with [`Dock`](/keys/dock/) buildings listed but none able to take it, and when it has no [`Ammo`](/keys/ammo/) left, no radio contact and no structure to rearm at. An airport bound aircraft already on the ground does not crash.

Because the aircraft never lands in the open, a move order ends with the aircraft flying to the cell and then to a docking structure, where it lands. [Landing at a structure](/systems/aircraft-operations/#landing-at-a-structure) gives the details.

```ini title="rules.ini"
[ORCA]
Dock=GAAIRC
AirportBound=yes   ; crashes when the last Air Force Command is gone
```
