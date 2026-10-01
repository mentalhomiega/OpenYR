---
key: DestroyAnim
summary: "Animations, one picked at random, that play where a structure or vehicle of this type is destroyed."
see_also: [DebrisAnims, Explosion, "system:destruction-and-debris"]
when_omitted:
  kind: value
  value: none
---

When a structure of this type is destroyed, one of these animations, chosen at random, plays at its center, drawn in its owner's colors as the structure's own animations are. Yuri's Revenge uses it for collapsing landmarks. A vehicle of this type that has an [`Explosion`](/keys/explosion/) list also plays one where it is destroyed.

```ini title="rulesmd.ini"
[MYLANDMARK] ; example BuildingType
DestroyAnim=MYLANDMARKDM
```
