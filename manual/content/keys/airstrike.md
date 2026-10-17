---
key: Airstrike
summary: "Makes a weapon call its firer's airstrike planes onto the target."
see_also: [AirstrikeTeam, AirstrikeTeamType, AirstrikeRechargeTime, CanC4]
when_omitted:
  kind: value
  value: "no"
---

A weapon whose warhead sets `Airstrike=yes` does no damage. Its shot flies to the object it was fired at, and when the shot detonates on an object, its firer calls [`AirstrikeTeam`](/keys/airstriketeam/) planes of its [`AirstrikeTeamType`](/keys/airstriketeamtype/) in from its owner's map edge to attack that object, and plays `AirstrikeAttackVoice` for the player. While those planes are still out, firing again sends the ones with ammunition left at the new target instead of calling more. A new call also waits until [`AirstrikeRechargeTime`](/keys/airstrikerechargetime/) frames have passed since the last of those planes left the map or was destroyed. Planes that run out of ammunition fly off the map.

A unit whose second weapon has an airstrike warhead uses it against structures that allow C4 ([`CanC4`](/keys/canc4/)), except a structure that is both a [`ResourceGatherer`](/keys/resourcegatherer/) and a [`ResourceDestination`](/keys/resourcedestination/), such as a deployed Slave Miner. It uses its first weapon against everything else.

```ini title="rulesmd.ini"
[MyFlare] ; example Weapon
Warhead=MyAirstrikeWH

[MyAirstrikeWH] ; example Warhead
Airstrike=yes
```
