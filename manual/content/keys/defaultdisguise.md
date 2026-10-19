---
key: DefaultDisguise
summary: The InfantryType that an object with PermaDisguise shows for a house of the side when it has no other disguise.
see_also: ["system:disguises", PermaDisguise, AlliedDisguise, SovietDisguise, ThirdDisguise]
when_omitted:
  kind: computed
  note: '`AlliedDisguise`, `SovietDisguise` or `ThirdDisguise`, for the first, second or third side in `[Sides]`; a side after the third shows no disguise.'
---

An object that can disguise itself and has [`PermaDisguise=yes`](/keys/permadisguise/) takes this type as its disguise while it has none.

```ini title="rules.ini"
[FourthSide]
DefaultDisguise=KNIGHT
```
