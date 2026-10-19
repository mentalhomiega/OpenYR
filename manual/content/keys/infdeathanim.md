---
key: InfDeathAnim
summary: "The animation an infantryman dies with when this warhead kills him, in place of InfDeath."
see_also: [InfDeath, NotHuman, "system:warheads"]
when_omitted:
  kind: value
  value: ""
  note: The infantryman dies as InfDeath says.
---

An infantryman killed by this warhead plays the named animation where he fell, then is removed. The animation replaces the death that [`InfDeath`](/keys/infdeath/) picks, and the warhead's `InfDeath` is not read for him. It does not apply to a type with [`NotHuman=yes`](/keys/nothuman/), which dies with its own death sequence. A name that matches no animation type leaves the key unset.

```ini title="rulesmd.ini"
[MyFreezeWH] ; example Warhead
InfDeathAnim=FREEZE
```

An animation whose art sets `MakeInfantry` turns the dead soldier into the infantry type that `MakeInfantry` selects, for the killer's house, as the mutation death of `InfDeath=9` does. The soldier mutates only on ground infantry can walk on, with a free spot in the cell and no structure there. Otherwise he dies with the explosion death.
