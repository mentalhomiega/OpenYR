---
key: IFVMode
summary: "The weapon a gunner vehicle fires while this object is its first passenger."
see_also: [Gunner, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "0"
---

A [`Gunner=yes`](/keys/gunner/) vehicle that this object boards while empty fires this weapon of its [numbered list](/systems/gattling-weapons/#numbered-weapon-lists), counting from `0`: `0` is `Weapon1`, `2` is `Weapon3`. A value outside `0` to `17` selects `Weapon1`. [Gunner vehicles](/systems/gunner-vehicles/#the-weapon) covers the rest.

```ini title="rulesmd.ini"
[MYSOLDIER] ; example InfantryType
IFVMode=2 ; a gunner vehicle carrying this soldier fires its Weapon3
```
