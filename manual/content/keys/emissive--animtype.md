---
key: Emissive
scope: animtype
label: Animation emissive strength
see_also: ["Translucent", "UseNormalLight", "IsAnimatedTiberium", "HDRHighlights", "EmissiveDebugView"]
when_omitted:
  kind: computed
  note: "The largest default in the table below whose condition the animation meets, or `0` when it meets none."
---

`Emissive` sets how strongly the animation's picture can rise above paper white on HDR output. `yes` gives `255`, and `no` gives `0` even where a default would apply. A whole number from `0` to `255` sets the strength directly. A larger number clamps to `255`, and a negative number clamps to `0`. `yes` and `no` are read in any letter case. Any other text gives the omitted default.

The omitted default is the largest default in the table below whose condition the animation meets. The defaults do not add up: an animation with both `Translucent=yes` and `UseNormalLight=yes` gets `200`.

| Name in code | Strength | Applies to |
| --- | --- | --- |
| `AnimTranslucent` | 96 | Animations with [`Translucent=yes`](/keys/translucent/#scope-animtype) |
| `AnimSelfLit` | 200 | Animations with [`UseNormalLight=yes`](/keys/usenormallight/) |
| `AnimTiberium` | 96 | Animations with [`IsAnimatedTiberium=yes`](/keys/isanimatedtiberium/) |
| `AnimTranslucency` | 96 | Animations with [`Translucency`](/keys/translucency/#scope-animtype) set to `25`, `50` or `75` |
| `AnimLightPost` | 160 | Animations named `NAPOST_A`, `NAPOST_AD` and `NAPOST_B`, the lamp glow of the light posts |
| `VoxelSpecular` | 128 | Voxel shade rows above 16, scaled up to this strength at row 31. Row 16 is neutral. The table does not separate specular rows from diffuse rows above 16. |

The strength takes effect only while the emissive masks are kept. The masks are kept when [`HDRHighlights`](/keys/hdrhighlights/) is `Enhanced` on HDR output, or when [`EmissiveDebugView`](/keys/emissivedebugview/) is `yes`.

The strength applies to the animation's picture. Its shadow, its bounce pass and any darkened draw get no emissive value. A translucent picture keeps only its share of the strength: 25 percent translucency keeps three quarters, and 75 percent keeps a quarter.
