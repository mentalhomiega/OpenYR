---
key: Cursor
scope: superweapontype
summary: "Names the cursor shown over cells where the weapon may be aimed."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "the cursor for the Action"
---

The name of a stock cursor or of an entry in [`[MouseCursors]`](/systems/superweapons/#named-cursors). A weapon with `Action=Custom` and no `Cursor=` shows the attack cursor. A weapon with another `Action=` replaces that action's cursor only when it sets this key.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Cursor=Paradrop
```

See [Presentation](/systems/superweapons/#presentation).
