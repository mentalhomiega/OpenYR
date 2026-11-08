---
key: LaserDuration
summary: How many frames a laser beam stays drawn for.
see_also: ["IsLaser", "IsHouseColor"]
when_omitted:
  kind: value
  value: "10"
---

`LaserDuration` sets how many game frames the colored beam of an [`IsLaser=yes`](/keys/islaser/) weapon stays on screen. The beam dims evenly from full strength to nothing over that time, and is gone when the time is up. A weapon with [`IsHouseColor=yes`](/keys/ishousecolor/) fades its thick beam the same way.

```ini title="rules.ini"
[MyObeliskRay] ; example WeaponType
IsLaser=yes
LaserDuration=15
```

:::caution[Keep the value between 1 and 127]
The value is stored in a single signed byte, so `127` is the longest beam you can set. Larger values wrap around: `128` through `255` become negative, and `256` becomes `0`. A value of `0` or less shows the beam for a single frame.
:::
