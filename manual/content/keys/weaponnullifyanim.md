---
key: WeaponNullifyAnim
summary: "The animation a shot shows when an Iron Curtain turns it away."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: none
---

When a shot explodes within 85 leptons of an Iron Curtained object, the object takes no damage and the shot shows this animation instead of its own explosion. Only a shot whose warhead has a `CellSpread` of `0.5` or less is turned away like this, and it then harms nothing else either. A wider blast explodes as usual and still hurts the objects around it.

```ini title="rulesmd.ini"
[General]
WeaponNullifyAnim=IRONFX
```
