---
key: WallAbsoluteDestroyer
summary: "Lets this warhead knock down any wall in its blast in one hit."
see_also: [Wall, CellSpread, "system:walls-and-gates"]
when_omitted:
  kind: value
  value: "no"
---

A blast from a `WallAbsoluteDestroyer=yes` warhead removes every wall overlay in its reach outright, whatever the wall's [`Strength`](/keys/strength/#scope-overlaytype) and the blast's damage. [Taking damage](/systems/walls-and-gates/#taking-damage) covers walls.

```ini title="rulesmd.ini"
[MyNukeWH] ; example WarheadType
WallAbsoluteDestroyer=yes
```

The flag does not make objects treat walls as destroyable or give the attack cursor over walls; [`Wall`](/keys/wall/#scope-warheadtype) does that.
