---
key: CanBeOccupied
summary: Lets soldiers move into this structure and take it over as a garrison.
see_also: [MaxNumberOccupants, CanOccupyFire, ShowOccupantPips, Occupier, "system:garrisons"]
when_omitted:
  kind: value
  value: "no"
---

`CanBeOccupied=yes` makes the structure a garrison. Soldiers whose type sets [`Occupier=yes`](/keys/occupier/) can move in, up to [`MaxNumberOccupants`](/keys/maxnumberoccupants/) of them, and the structure's owner follows its occupants. [Garrisons](/systems/garrisons/) gives the conditions for moving in and out.

```ini title="rulesmd.ini"
[MYOFFICES] ; example BuildingType
CanBeOccupied=yes
MaxNumberOccupants=6
CanOccupyFire=yes
```

The structure fires only when [`CanOccupyFire=yes`](/keys/canoccupyfire/) is also set. With `MaxNumberOccupants=0`, no soldier can move in.

A garrisonable structure shows its occupant row to every player, whoever owns it. [`ShowOccupantPips=no`](/keys/showoccupantpips/) hides the row.
