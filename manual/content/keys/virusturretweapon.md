---
key: VirusTurretWeapon
summary: "The weapon whose turret VirusTurretIndex sets, on the vehicle type named FV."
see_also: [VirusTurretIndex, IFVMode, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "-1"
  note: Maps no weapon.
---

Names a weapon of the `FV` vehicle's [numbered list](/systems/gattling-weapons/#numbered-weapon-lists), counting from `0`. While `FV` fires that weapon, it shows the turret [`VirusTurretIndex`](/keys/virusturretindex/) names. Only the type named `FV` reads this key, and a value outside `0` to `17` maps nothing. [The turret](/systems/gunner-vehicles/#the-turret) covers the order the pairs apply in.

```ini title="rulesmd.ini"
[FV]
VirusTurretWeapon=2
VirusTurretIndex=1
```
