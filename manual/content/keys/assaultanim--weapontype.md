---
key: AssaultAnim
scope: weapontype
label: 'Assault animation'
see_also: [Assaulter, OccupantAnim, "system:garrisons"]
when_omitted:
  kind: value
  value: none
---

Plays at the structure's muzzle point of each occupant that an [`Assaulter=yes`](/keys/assaulter/) soldier kills, when this is the soldier's primary weapon. Without `AssaultAnim`, the kills play no animation.

```ini title="rulesmd.ini"
[MyAssaultBolt] ; example Weapon
AssaultAnim=MYCLEAR ; an AnimType registered in [Animations]
```
