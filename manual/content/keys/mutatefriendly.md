---
key: MutateFriendly
summary: "Whether the genetic mutator also mutates infantry of the firing house and its allies."
see_also: [MutateExplosion, MutateWarhead, "system:superweapons"]
when_omitted:
  kind: value
  value: "yes"
---

With `MutateFriendly=yes`, the [genetic mutator](/systems/superweapons/#genetic-mutator) affects infantry of the firing house and of its allies the same as enemy infantry. With `MutateFriendly=no`, it spares infantry owned by the firing house or by an ally, and only enemy infantry is mutated.

The key applies to both forms of the strike, the [`MutateExplosion=yes`](/keys/mutateexplosion/) blast and the cell damage from [`MutateExplosion=no`](/keys/mutateexplosion/). Vehicles and structures are not affected by it.

```ini title="rulesmd.ini"
[General]
MutateFriendly=no
```

The value is read from the rules files and is not stored in a save game.
