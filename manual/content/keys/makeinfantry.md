---
key: MakeInfantry
summary: "The infantry type this animation turns into when it ends."
see_also: [AnimToInfantry, InfantryMutate]
when_omitted:
  kind: value
  value: "-1"
---

When the animation ends, it becomes an infantryman of the type at this position in [`AnimToInfantry`](/keys/animtoinfantry/), counting from 0, owned by the animation's house. A computer house's new infantryman hunts. When the cell is too crowded to place him, the animation holds its last frame and tries again on the next frame.

```ini title="artmd.ini"
[MYMUTATE] ; example AnimType
MakeInfantry=0 ; the first AnimToInfantry entry
```

An animation with no house, or whose house has been defeated, gives the infantryman to the Civilian side's house. At `-1`, or past the end of the list, the animation ends normally.
