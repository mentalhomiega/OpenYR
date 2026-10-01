---
key: TerroristExplodeTurretWeapon
summary: "The weapon whose turret TerroristExplodeTurretIndex sets, on the vehicle type named FV."
see_also: [TerroristExplodeTurretIndex, IFVMode, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "-1"
  note: Maps no weapon.
---

Names a weapon of the `FV` vehicle's [numbered list](/systems/gattling-weapons/#numbered-weapon-lists), counting from `0`. While `FV` fires that weapon, it shows the turret [`TerroristExplodeTurretIndex`](/keys/terroristexplodeturretindex/) names. Only the type named `FV` reads this key, and a value outside `0` to `17` maps nothing. [The turret](/systems/gunner-vehicles/#the-turret) covers the order the pairs apply in.

```ini title="rulesmd.ini"
[FV]
TerroristExplodeTurretWeapon=2
TerroristExplodeTurretIndex=1
```
