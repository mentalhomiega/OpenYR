---
key: SabotageCursor
summary: Shows the demolition cursor over what an infantryman would attack with this weapon.
see_also: [MigAttackCursor, C4, CanC4]
when_omitted:
  kind: value
  value: "no"
---

With `SabotageCursor=yes`, a selected infantryman shows the demolition cursor over any target it could attack, when this weapon is the one it would select for that target. A click on the target is an ordinary attack.

A vehicle inside a [tank bunker](/systems/tank-bunkers/) is the exception. It gets the select action instead.

The key takes precedence over [`MigAttackCursor`](/keys/migattackcursor/) on the same weapon. It is not read when a soldier that can plant C4 points at a structure, because [`C4`](/keys/c4/) settles the cursor there first.

The key changes the cursor and the order a click reports. It does not change which weapon fires or what the weapon can hit.

```ini title="rulesmd.ini"
[MYSAPPER] ; a WeaponType
SabotageCursor=yes
```
