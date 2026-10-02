---
key: Airstrike
summary: "Makes a weapon call its firer's airstrike planes onto the target."
see_also: [AirstrikeTeam, AirstrikeTeamType, AirstrikeRechargeTime, CanC4]
when_omitted:
  kind: value
  value: "no"
---

A weapon whose warhead sets `Airstrike=yes` does not hit anything when fired at an object. Instead its firer calls [`AirstrikeTeam`](/keys/airstriketeam/) planes of its [`AirstrikeTeamType`](/keys/airstriketeamtype/) in from its owner's map edge to attack that object, and plays `AirstrikeAttackVoice` for the player. While those planes are still out, firing again sends the ones with ammunition left at the new target instead of calling more. A new call also waits until [`AirstrikeRechargeTime`](/keys/airstrikerechargetime/) frames have passed since the last one. Planes that run out of ammunition fly off the map.

A unit whose second weapon has an airstrike warhead uses it against structures that allow C4 ([`CanC4`](/keys/canc4/)), and its first weapon otherwise.

```ini title="rulesmd.ini"
[MyFlare] ; example Weapon
Warhead=MyAirstrikeWH

[MyAirstrikeWH] ; example Warhead
Airstrike=yes
```
