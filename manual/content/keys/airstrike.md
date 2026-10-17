---
key: Airstrike
summary: "Makes a weapon call its firer's airstrike planes onto the target."
see_also: [AirstrikeTeam, AirstrikeTeamType, AirstrikeRechargeTime, CanC4]
when_omitted:
  kind: value
  value: "no"
---

A weapon whose warhead sets `Airstrike=yes` does no damage. Its shot flies to the object it was fired at, and when the shot detonates on an object, its firer calls [`AirstrikeTeam`](/keys/airstriketeam/) planes of its [`AirstrikeTeamType`](/keys/airstriketeamtype/) in from its owner's map edge to attack that object, and plays `AirstrikeAttackVoice` for the player. While those planes are still out, firing again sends the ones with ammunition left at the new target instead of calling more. A new call also waits until [`AirstrikeRechargeTime`](/keys/airstrikerechargetime/) frames have passed since the last of those planes left the map or was destroyed. Planes that run out of ammunition fly off the map.

While the planes of a strike are out, the structure they attack is drawn tinted. The tint adds the entry of the `[ColorAdd]` table that `LaserTargetColor=` in `[AudioVisual]` numbers to each colour of the structure. Entry 0 adds nothing, so it is the default and no structure is tinted. The tint lasts until the firer changes its target, or the firer or the structure leaves the map. It covers only the structure's own image: its animations draw untinted, and a structure drawn with the terrain palette is not tinted.

A unit whose second weapon has an airstrike warhead uses it against structures that allow C4 ([`CanC4`](/keys/canc4/)), except a structure that is both a [`ResourceGatherer`](/keys/resourcegatherer/) and a [`ResourceDestination`](/keys/resourcedestination/), such as a deployed Slave Miner. It uses its first weapon against everything else.

```ini title="rulesmd.ini"
[MyFlare] ; example Weapon
Warhead=MyAirstrikeWH

[MyAirstrikeWH] ; example Warhead
Airstrike=yes
```
