---
key: Cursor.AttackOutOfRange
scope: weapontype
summary: "The cursor shown when the weapon can attack a target it has to move to reach."
see_also: ["Cursor.Attack", "Cursor.Move"]
when_omitted:
  kind: value
  value: "the stock out-of-range attack cursor"
---

The cursor shows over an enemy that the first selected object would attack with the weapon but is not in its range yet. It applies only when one object is selected. A name that is neither a stock cursor nor a `[MouseCursors]` entry leaves the stock cursor.

```ini title="rulesmd.ini"
[MyGun] ; example WeaponType
Cursor.AttackOutOfRange=MyAdvance
```
