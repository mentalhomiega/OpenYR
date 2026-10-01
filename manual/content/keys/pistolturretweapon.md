---
key: PistolTurretWeapon
summary: "The weapon whose turret PistolTurretIndex sets, on the vehicle type named FV."
see_also: [PistolTurretIndex, IFVMode, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "-1"
  note: Maps no weapon.
---

Names a weapon of the `FV` vehicle's [numbered list](/systems/gattling-weapons/#numbered-weapon-lists), counting from `0`. While `FV` fires that weapon, it shows the turret [`PistolTurretIndex`](/keys/pistolturretindex/) names. Only the type named `FV` reads this key, and a value outside `0` to `17` maps nothing. [The turret](/systems/gunner-vehicles/#the-turret) covers the order the pairs apply in.

```ini title="rulesmd.ini"
[FV]
PistolTurretWeapon=2
PistolTurretIndex=1
```
