---
key: SW.TimerVisibility
scope: superweapontype
label: Who sees the superweapon timer
summary: "Chooses which players see the countdown of a ShowTimer=yes superweapon."
see_also: [ShowTimer, SW.AffectsHouse, "system:superweapons"]
when_omitted:
  kind: value
  value: "all"
---

`SW.TimerVisibility` is an Ares key. It takes one of `none`, `owner`, `allies`, `team`, `enemies` or `all`, relative to the house that holds the weapon. `team` is the owner and its allies; `allies` leaves the owner out. A player outside the named houses does not see the weapon's line in the timer list. An observer counts as allied to every house, so `allies`, `team` and `all` show an observer every timer.

The key only matters for a weapon with [`ShowTimer=yes`](/keys/showtimer/). The weapon charges and fires the same way whoever sees its timer.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
ShowTimer=yes
SW.TimerVisibility=team
```
