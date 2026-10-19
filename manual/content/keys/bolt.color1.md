---
key: Bolt.Color1
summary: "The colour of the first strand of this weapon's electric bolt."
see_also: [IsElectricBolt, IsAlternateColor, Bolt.Color1, Bolt.Color2, Bolt.Color3]
when_omitted:
  kind: context-dependent
  note: "The palette colour: the first strand uses colour 10 of the palette, or colour 5 with `IsAlternateColor=yes`."
---

An [`IsElectricBolt=yes`](/keys/iselectricbolt/) weapon draws the first of its three strands in this colour, written as red, green and blue from `0` to `255`. A strand whose key is not written keeps its palette colour, so the three keys can be set independently. The colour is drawn as written, with no shading by the terrain's light.

```ini title="rulesmd.ini"
[MyCoilZap] ; example WeaponType
IsElectricBolt=yes
Bolt.Color1=255,200,200
```
