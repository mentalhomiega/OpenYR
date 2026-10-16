---
key: Range
scope: superweapontype
label: Superweapon target ring
see_also: ["UnitActionLines", "system:superweapons"]
when_omitted:
  kind: value
  value: "0"
---

`Range=` in a superweapon type's section sets the radius, in cells, of the ring the game draws while the player aims that superweapon. A fraction is accepted, but the ring uses only the whole cells of the value, so a value below `1` draws no ring. `Range=0`, the default, draws none.

```ini title="rules.ini"
[MySuperWeapon] ; example SuperWeaponType
Range=12.5
```

The ring is centred on the cell under the pointer. It appears only while the superweapon is being aimed, [`UnitActionLines`](/keys/unitactionlines/) is on, and the pointer is over the tactical view on a cell inside the playable area. A type whose `LineMultiplier=` is at least `1` gets a ring at each whole-cell radius up to the range, in place of the single ring.

Nothing else reads the value. The superweapon's effect comes from its own keys, and a click outside the ring is not refused.
