---
key: StupidHunt
summary: "Stops an object scanning for targets while it hunts; in a campaign it walks to its house's starting cell."
when_omitted:
  kind: value
  value: "no"
---

With `yes`, an infantryman or vehicle of this type does not scan for a target during its hunt mission, so it does not move toward an enemy on its own. In a campaign game (the `GAME_NORMAL` game type), an object owned by a computer player walks to its house's starting cell, and its slaves are sent to guard first. Any other such object stays where it is. That covers an object owned by a human player, an object in a skirmish or multiplayer game, and an object whose house has no starting cell.

Aircraft ignore the key, because they have their own hunt mission. A vehicle with a `DeploysInto` building runs its deploy routine instead of the hunt when that building is listed in `BuildConst`, when the vehicle has a target, or when a human player owns it; the key has no effect then.

```ini title="rulesmd.ini"
[MYINF] ; an InfantryType registered in [InfantryTypes]
StupidHunt=yes
```
