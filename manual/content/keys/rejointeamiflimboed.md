---
key: RejoinTeamIfLimboed
summary: "Adds a parasite back to its team when it comes out of its victim."
see_also: [LimboLaunch, ReselectIfLimboed, "system:parasites"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, an object of this type remembers its team when it fires a [LimboLaunch](/keys/limbolaunch/) weapon at an infantryman while it is on a team. When it comes out of its victim, it is added back to that team, unless the team refuses it. A shot at any other kind of target keeps the team remembered from an earlier shot. The remembered team is forgotten when the team is deleted.

```ini title="rulesmd.ini"
[MYPARASITE] ; a VehicleType or InfantryType with a LimboLaunch weapon
RejoinTeamIfLimboed=yes
```
