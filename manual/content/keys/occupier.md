---
key: Occupier
summary: Lets this soldier move into garrisonable structures.
see_also: [OccupyWeapon, EliteOccupyWeapon, OccupyPip, CanBeOccupied, "system:garrisons"]
when_omitted:
  kind: value
  value: "no"
---

An `Occupier=yes` soldier can move into a [`CanBeOccupied=yes`](/keys/canbeoccupied/) structure that has room for it. A player who points the soldier at such a structure gets the enter cursor. [Moving in](/systems/garrisons/#moving-in) lists when a structure can take the soldier.

```ini title="rulesmd.ini"
[MYRIFLEMAN] ; example InfantryType
Occupier=yes
OccupyWeapon=MyGarrisonGun
```

Inside a structure that sets [`CanOccupyFire=yes`](/keys/canoccupyfire/), the soldier fires its [`OccupyWeapon`](/keys/occupyweapon/), or its primary weapon when that is unset.
