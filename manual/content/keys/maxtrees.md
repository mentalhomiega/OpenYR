---
key: MaxTrees
summary: How many trees a generated map's woods may plant at the largest width and full vegetation.
see_also: [Vegetation, "system:map-generation"]
when_omitted:
  kind: value
  value: "500"
---

`MaxTrees` limits how many trees the woods of a generated map plant. The limit is `MaxTrees` times 0.7 plus 0.1 for each step of the map seed's [`Width`](/keys/width/#scope-random-map-generation) from 0 to 3, times [`Vegetation`](/keys/vegetation/) as a percentage. Each product rounds toward zero, so `MaxTrees=600` allows 599 trees at `Width=3` and `Vegetation=100`, and 419 at `Width=0`.

```ini title="RMGMD.INI"
[General]
MaxTrees=600
```

Once the woods have planted that many trees, the map's remaining woods still spread but plant no trees. A limit of zero or less plants none. Tiberium trees do not count against the limit and are not stopped by it. Woods rarely reach the stock limit: one temperate map with `Width=1` and `Vegetation=80` planted 65 trees against a limit of 383.
