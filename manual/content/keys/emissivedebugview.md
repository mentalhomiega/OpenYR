---
key: EmissiveDebugView
summary: Shows the emissive masks in place of the art, even without HDR output.
when_omitted:
  kind: value
  value: "no"
---

`EmissiveDebugView=yes` keeps the emissive masks even without HDR output, and the presented picture shows the masks in place of the art. The red channel of the presented picture carries each mask value, from 0 to 255. Use it to see which pixels of an animation carry an [`Emissive`](/keys/emissive/#scope-animtype) value, and how strong that value is.

Without this key, the masks are kept only when HDR output is on and [`HDRHighlights`](/keys/hdrhighlights/) is `Enhanced`. This build has no HDR output.

This is a developer key. It has no entry in the settings, so set it by editing `RA2MD.INI`.
