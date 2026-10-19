---
key: StupidHunt
summary: Stops an object searching for a target during its hunt mission.
when_omitted:
  kind: value
  value: "no"
---

With `yes`, an infantryman or vehicle of this type does not search for a target during its hunt mission, so it does not move toward an enemy on its own. A vehicle that deploys while it hunts ignores the key. Aircraft have their own hunt mission and ignore it.

The original game also walks an object owned by a computer player toward a starting cell. This engine does not do that yet, so such an object stays where it is.

```ini title="rules.ini"
[MYINF] ; an InfantryType registered in [InfantryTypes]
StupidHunt=yes
```
