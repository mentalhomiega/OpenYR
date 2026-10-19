---
key: Cursor.Attack
scope: weapontype
summary: "The cursor shown when the weapon can attack a target in range."
see_also: ["Cursor.AttackOutOfRange", "Cursor.Move"]
when_omitted:
  kind: value
  value: "the stock attack cursor"
---

The cursor shows over an enemy when one object is selected, the object's target is within the weapon's range and the weapon is the one the object would use against it. The value is a cursor name, as `Cursor.Move=` takes, either a stock cursor or one that `[MouseCursors]` defines. A name that is neither leaves the stock cursor.

```ini title="rulesmd.ini"
[MyGun] ; example WeaponType
Cursor.Attack=MyAttack
```
