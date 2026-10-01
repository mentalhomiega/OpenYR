---
key: Cloning
summary: Makes this structure release a free copy of every infantryman its owner's barracks release.
see_also: [Factory, "system:production"]
when_omitted:
  kind: value
  value: "no"
---

Each time a barracks lets out a trained infantryman, every `Cloning=yes` structure its owner has on the map lets out another infantryman of the same type for free. [Leaving the factory](/systems/production/#leaving-the-factory) describes the exit.

```ini title="rulesmd.ini"
[MYVATS] ; example BuildingType
Cloning=yes
```

The structure copies from the moment it is placed until it is sold or destroyed, whether or not its owner has power. A copy that cannot get out at once, because the structure is still letting out the previous copy or has no free exit cell, is discarded. A cloning structure that is itself a barracks copies nothing it trains.
