---
key: AirstrikeTeam
summary: "How many planes an airstrike call brings."
see_also: [EliteAirstrikeTeam, AirstrikeTeamType, Airstrike]
when_omitted:
  kind: value
  value: "0"
---

The number of [`AirstrikeTeamType`](/keys/airstriketeamtype/) planes the object calls with an [`Airstrike`](/keys/airstrike/) weapon. With `0` it calls none. An elite object uses [`EliteAirstrikeTeam`](/keys/eliteairstriketeam/) instead.

```ini title="rulesmd.ini"
[MYSPOTTER] ; example InfantryType
AirstrikeTeam=2
EliteAirstrikeTeam=4
AirstrikeTeamType=BPLN
AirstrikeRechargeTime=100
```
