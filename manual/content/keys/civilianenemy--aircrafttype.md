---
key: CivilianEnemy
scope: aircrafttype
summary: "With yes, other houses' units pick this object as a target although a neutral house owns it."
see_also: [MultiplayPassive, Insignificant]
when_omitted:
  kind: value
  value: "no"
---

In a skirmish or multiplayer game, an object owned by a house with `MultiplayPassive=yes`, such as the Civilian house, is not picked as a target on its own unless the game option to attack neutral units is on. A `CivilianEnemy=yes` object is picked whatever that option says. An `Insignificant=yes` object is still never picked, and a campaign is not changed.

```ini title="rulesmd.ini"
[CAMILIT] ; example BuildingType
CivilianEnemy=yes
```
