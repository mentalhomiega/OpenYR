---
key: Bolt.Color3
summary: "The colour of the third strand of this weapon's electric bolt."
see_also: [IsElectricBolt, IsAlternateColor, Bolt.Color1, Bolt.Color2, Bolt.Color3]
when_omitted:
  kind: context-dependent
  note: "The palette colour: the third strand uses colour 15 of the palette."
---

An [`IsElectricBolt=yes`](/keys/iselectricbolt/) weapon draws the third of its three strands in this colour, written as red, green and blue from `0` to `255`. A strand whose key is not written keeps its palette colour, so the three keys can be set independently. The colour is drawn as written, with no shading by the terrain's light.

```ini title="rulesmd.ini"
[MyCoilZap] ; example WeaponType
IsElectricBolt=yes
Bolt.Color3=255,200,200
```
