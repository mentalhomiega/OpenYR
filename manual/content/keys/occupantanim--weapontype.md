---
key: OccupantAnim
scope: weapontype
label: 'Garrison muzzle flash'
see_also: [OpenToppedAnim, Anim, "system:garrisons"]
when_omitted:
  kind: value
  value: none
---

Plays at the structure's muzzle point each time an occupant of a garrisoned structure fires this weapon. It replaces the weapon's [`Anim`](/keys/anim/) for those shots. Without `OccupantAnim`, a garrison's shots play no animation, and the weapon's `Anim` is not used for them.

```ini title="rulesmd.ini"
[MyRifle] ; example Weapon
OccupantAnim=MYFLASH ; an AnimType registered in [Animations]
```
