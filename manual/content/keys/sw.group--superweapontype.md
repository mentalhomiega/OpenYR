---
key: SW.Group
scope: superweapontype
summary: "Gives the weapon a group number that the Iron Curtain me team script line matches."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "0"
---

The [Iron Curtain me](/mapping/missions/tmission-iron-curtain-me/) team script line fires only the Iron Curtain weapons whose group equals the line's parameter, so a house with several can send each team to its own. Nothing else uses the number.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
SW.Group=3
```
