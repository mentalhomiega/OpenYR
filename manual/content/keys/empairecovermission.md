---
key: EMPAIRecoverMission
summary: "The mission a computer player's object takes up again when an EMP wears off."
see_also: [HarvesterUnit, "system:emp-pulse"]
when_omitted:
  kind: value
  value: "no mission, so the object keeps the one it had"
---

`EMPAIRecoverMission` is an Ares key, read from `[CombatDamage]`. It names a mission, such as `Guard`, `Hunt` or `Sticky`. When the EMP on an infantryman or vehicle ends, the object is given this mission if all of these hold:

- the key names a mission;
- the object's house is a computer player, not a human;
- the object is not a harvester, which goes back to harvesting as before;
- the object is not unloading.

A name that matches no mission leaves the key unset. Structures and human players are never affected, so a mission set here changes nothing in a match with only human players. The key exists so that computer players do not stand idle after an EMP.

```ini title="rulesmd.ini"
[CombatDamage]
EMPAIRecoverMission=Hunt
```
