---
key: HDRHighlights
summary: Whether light sources may be drawn brighter than paper white on HDR output.
see_also: ["Emissive", "EmissiveDebugView"]
when_omitted:
  kind: value
  value: Enhanced
---

`HDRHighlights` sets whether light sources may be drawn brighter than paper white on HDR output. `Enhanced` allows it, and `Classic` keeps every picture at paper white. The value is read in any letter case. Any other text, or no value, selects `Enhanced`.

On HDR output with `Enhanced`, explosions, flames, lasers and electric bolts can be drawn brighter than paper white. [`Emissive`](/keys/emissive/#scope-animtype) on each animation or weapon sets how far above paper white it can go.

The **Highlights** list on the Display page of the settings writes this key. The list is grayed out, with the tooltip "Needs HDR output", unless the game is showing HDR output. When the settings are saved, the game writes the value back to `RA2MD.INI` as `Classic` or `Enhanced`.

Highlights only apply while the game is showing HDR output, so `Enhanced` has no visible effect with [`HDROutput`](/keys/hdroutput/) set to `Off`, or with `Automatic` while Windows HDR is off for the display.
